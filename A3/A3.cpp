#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"
//#include "GeometryCommand.hpp"
#include <map>

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

#define PI 				3.14159265359
#define FACTOR 			(double) 2 / m_windowWidth

enum modes {
	POSITION,
	JOINT
};

enum trans_types {
	TRANSLATE,
	ROTATE_SCALE
};

struct command {
	unsigned int id;
	int type;
	mat4 T;
};

vec4 prevMousePos, onClickMousePos;
bool mouseLeftClicked = false;
bool mouseMiddleClicked = false;
bool mouseRightClicked = false;

mat4 TMP;    // current matrix transformation
int ttype;	// current transformation type

vector<command> commands;
unsigned int last_command = 0;

mat4 vAxisRotMatrix(float fVecX, float fVecY, float fVecZ);
vec4 vCalcRotVec(float fNewX, float fNewY, float fOldX, float fOldY, float fDiameter);

void A3::updateNode(unsigned int id, mat4 T, int type) {

	if (m_rootNode->m_nodeId == id) {
		if (type == TRANSLATE) m_rootNode->set_transform(T * m_rootNode->get_transform());
		else m_rootNode->set_transform(m_rootNode->get_transform() * T);
	} else
	for (SceneNode *node : m_rootNode->children) {
		if (node->m_nodeId == id) {
			if (type == TRANSLATE) node->set_transform(T * node->get_transform());
			else node->set_transform(node->get_transform() * T);
			break;
		}
	}
}

void A3::add_command(unsigned int id, mat4 T, int type) {
	struct command cmd = {id, type, T};
	commands.resize(last_command);
	commands.push_back(cmd);
	last_command++;
}

void A3::undo() {
	if (commands.empty()) return;
	last_command--;
	updateNode(commands[last_command].id, inverse(commands[last_command].T), commands[last_command].type);
}

void A3::redo() {
	if (commands.empty() || last_command+1 > commands.size()) return;
	updateNode(commands[last_command].id, commands[last_command].T, commands[last_command].type);
	last_command++;
}

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0),
	  mode(0)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.35, 0.35, 0.35, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	add_command(m_rootNode->m_nodeId, mat4(), TRANSLATE);

	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	std::string assetFilePath = getAssetFilePath(filename.c_str());
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	//m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could not open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(-2.0f, 5.0f, 0.5f);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;


		//-- Set LightSource uniform for the scene:
		{
			location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}

		//-- Set background light ambient intensity
		{
			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.05f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui elements here here ...


		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * node.trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;


		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		vec3 kd = node.material.kd;
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.ks");
		vec3 ks = node.material.ks;
		glUniform3fv(location, 1, value_ptr(ks));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.shininess");
		glUniform1f(location, node.material.shininess);
		CHECK_GL_ERRORS;

	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

	glEnable( GL_DEPTH_TEST );
	renderSceneGraph(*m_rootNode);
	glDisable( GL_DEPTH_TEST );
	renderArcCircle();
}

void A3::traverse(SceneNode *node, const mat4 P) {

	mat4 O = node->get_transform(); 		// old matrix
	node->set_transform(P * O);

	for (SceneNode *child : node->children) {
		traverse(child, node->get_transform());
	}

	if (node->m_nodeType == NodeType::GeometryNode) {
		const GeometryNode *geometryNode = static_cast<const GeometryNode *>(node);

		updateShaderUniforms(m_shader, *geometryNode, m_view);

		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

		//-- Now render the mesh:
		m_shader.enable();
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		m_shader.disable();
	}

	node->set_transform(O);
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.
	traverse(&root, glm::mat4());
	// for (const SceneNode * node : root.children) {
	//
	// 	if (node->m_nodeType != NodeType::GeometryNode)
	// 		continue;
	//
	// 	const GeometryNode * geometryNode = static_cast<const GeometryNode *>(node);
	//
	// 	updateShaderUniforms(m_shader, *geometryNode, m_view);
	//
	//
	// 	// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
	// 	BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];
	//
	// 	//-- Now render the mesh:
	// 	m_shader.enable();
	// 	glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
	// 	m_shader.disable();
	// }

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {

		if (mouseLeftClicked || mouseMiddleClicked || mouseRightClicked) {
			int width, height;
			glfwGetWindowSize(m_window, &width, &height);
			vec4 center = vec4((float)width / 2.0f, (float)height / 2.0f, 0.0f, 1.0f);
			vec4 curMousePos = vec4((float)xPos, (float)yPos, 0.0f, 1.0f);
			double theta = -(curMousePos.x - prevMousePos.x) * PI / m_windowWidth;
			mat4 R, T, S;

			if (mouseLeftClicked) {
				switch (mode) {
					case POSITION:
						T = translate(mat4(), vec3((curMousePos.x - prevMousePos.x) * FACTOR, -(curMousePos.y - prevMousePos.y) * FACTOR, 0));
						ttype = TRANSLATE;
						updateNode(m_rootNode->m_nodeId, T, ttype);
						break;
					case JOINT:
						break;
				}
			}

			if (mouseMiddleClicked) {
				switch (mode) {
					case POSITION:
						T = translate(mat4(), vec3(0, 0, -(curMousePos.y - prevMousePos.y) * FACTOR));
						ttype = TRANSLATE;
						updateNode(m_rootNode->m_nodeId, T, ttype);
						break;
					case JOINT:break;
				}
			}

			if (mouseRightClicked) {
				switch (mode) {
					case POSITION:
						vec4 center = vec4(m_windowWidth / 2, m_windowHeight / 2, 0, 1);
						vec4 newPos = curMousePos - center;
						vec4 oldPos = prevMousePos - center;
						float diameter = m_framebufferWidth / 2 < m_framebufferHeight / 2 ? m_framebufferWidth / 2: m_framebufferHeight / 2;
						vec4 newAxis;
						newAxis = vCalcRotVec(newPos.x, newPos.y, oldPos.x, oldPos.y, diameter);
						R = vAxisRotMatrix(newAxis.x, newAxis.y, newAxis.z);
						ttype = ROTATE_SCALE;
						updateNode(m_rootNode->m_nodeId, R, ttype);
						break;
				}
			}

			TMP *= T;
			prevMousePos = curMousePos;
		}
	}

	eventHandled = true;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if (actions == GLFW_PRESS) {
			double xpos, ypos;
			glfwGetCursorPos(m_window, &xpos, &ypos);
			prevMousePos = vec4((float) xpos, (float) ypos, 0.0f, 1.0f);
			if (button == GLFW_MOUSE_BUTTON_LEFT) mouseLeftClicked = true;
			if (button == GLFW_MOUSE_BUTTON_MIDDLE) mouseMiddleClicked = true;
			if (button == GLFW_MOUSE_BUTTON_RIGHT) mouseRightClicked = true;
		}
		if (actions == GLFW_RELEASE) {
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				mouseLeftClicked = false;
				add_command(m_rootNode->m_nodeId, TMP, ttype);
			}
			if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
				mouseMiddleClicked = false;
				add_command(m_rootNode->m_nodeId, TMP, ttype);
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT) mouseRightClicked = false;
		}
	}

	eventHandled = true;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
	}
	// Fill in with event handling code...

	return eventHandled;
}

vec4 vCalcRotVec(float fNewX, float fNewY,
                 float fOldX, float fOldY,
                 float fDiameter) {
   long  nXOrigin, nYOrigin;
   float fNewVecX, fNewVecY, fNewVecZ,        /* Vector corresponding to new mouse location */
         fOldVecX, fOldVecY, fOldVecZ,        /* Vector corresponding to old mouse location */
         fLength;

   /* Vector pointing from center of virtual trackball to
    * new mouse position
    */
   fNewVecX    = fNewX * 2.0 / fDiameter;
   fNewVecY    = fNewY * 2.0 / fDiameter;
   fNewVecZ    = (1.0 - fNewVecX * fNewVecX - fNewVecY * fNewVecY);

   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (fNewVecZ < 0.0) {
      fLength = sqrt(1.0 - fNewVecZ);
      fNewVecZ  = 0.0;
      fNewVecX /= fLength;
      fNewVecY /= fLength;
   } else {
      fNewVecZ = sqrt(fNewVecZ);
   }

   /* Vector pointing from center of virtual trackball to
    * old mouse position
    */
   fOldVecX    = fOldX * 2.0 / fDiameter;
   fOldVecY    = fOldY * 2.0 / fDiameter;
   fOldVecZ    = (1.0 - fOldVecX * fOldVecX - fOldVecY * fOldVecY);

   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (fOldVecZ < 0.0) {
      fLength = sqrt(1.0 - fOldVecZ);
      fOldVecZ  = 0.0;
      fOldVecX /= fLength;
      fOldVecY /= fLength;
   } else {
      fOldVecZ = sqrt(fOldVecZ);
   }

   /* Generate rotation vector by calculating cross product:
    *
    * fOldVec x fNewVec.
    *
    * The rotation vector is the axis of rotation
    * and is non-unit length since the length of a crossproduct
    * is related to the angle between fOldVec and fNewVec which we need
    * in order to perform the rotation.
    */
   // *fVecX = fOldVecY * fNewVecZ - fNewVecY * fOldVecZ;
   // *fVecY = fOldVecZ * fNewVecX - fNewVecZ * fOldVecX;
   // *fVecZ = fOldVecX * fNewVecY - fNewVecX * fOldVecY;
   return vec4(
	   fOldVecY * fNewVecZ - fNewVecY * fOldVecZ,
	   fOldVecZ * fNewVecX - fNewVecZ * fOldVecX,
	   fOldVecX * fNewVecY - fNewVecX * fOldVecY,
	   0
   );
}

/*******************************************************
 * void vAxisRotMatrix(float fVecX, float fVecY, float fVecZ, Matrix mNewMat)
 *
 *    Calculate the rotation matrix for rotation about an arbitrary axis.
 *
 *    The axis of rotation is specified by (fVecX,fVecY,fVecZ). The length
 *    of the vector is the amount to rotate by.
 *
 * Parameters: fVecX,fVecY,fVecZ - Axis of rotation
 *             mNewMat - Output matrix of rotation in column-major format
 *                       (ie, translation components are in column 3 on rows
 *                       0,1, and 2).
 *
 *******************************************************/
mat4 vAxisRotMatrix(float fVecX, float fVecY, float fVecZ) {
    float fRadians, fInvLength, fNewVecX, fNewVecY, fNewVecZ;

    /* Find the length of the vector which is the angle of rotation
     * (in radians)
     */
    fRadians = sqrt(fVecX * fVecX + fVecY * fVecY + fVecZ * fVecZ);

    /* If the vector has zero length - return the identity matrix */
    if (fRadians > -0.000001 && fRadians < 0.000001) {
        return mat4();
    }

    /* Normalize the rotation vector now in preparation for making
     * rotation matrix.
     */
    fInvLength = 1 / fRadians;
    fNewVecX   = fVecX * fInvLength;
    fNewVecY   = fVecY * fInvLength;
    fNewVecZ   = fVecZ * fInvLength;

    /* Create the arbitrary axis rotation matrix */
    double dSinAlpha = sin(fRadians);
    double dCosAlpha = cos(fRadians);
    double dT = 1 - dCosAlpha;

	return mat4(
		vec4(dCosAlpha + fNewVecX*fNewVecX*dT, fNewVecX*fNewVecY*dT - dSinAlpha*fNewVecZ, fNewVecZ*fNewVecX*dT + dSinAlpha*fNewVecY, 0),
		vec4(fNewVecX*fNewVecY*dT + fNewVecZ*dSinAlpha, dCosAlpha + fNewVecY*fNewVecY*dT, fNewVecZ*fNewVecY*dT - dSinAlpha*fNewVecX, 0),
		vec4(fNewVecX*fNewVecZ*dT - fNewVecY*dSinAlpha, fNewVecY*fNewVecZ*dT + dSinAlpha*fNewVecX, dCosAlpha + fNewVecZ*fNewVecZ*dT, 0),
		vec4(0, 0, 0, 1)
	);

    // mNewMat[0][0] = dCosAlpha + fNewVecX*fNewVecX*dT;
    // mNewMat[0][1] = fNewVecX*fNewVecY*dT + fNewVecZ*dSinAlpha;
    // mNewMat[0][2] = fNewVecX*fNewVecZ*dT - fNewVecY*dSinAlpha;
    // mNewMat[0][3] = 0;
	//
    // mNewMat[1][0] = fNewVecX*fNewVecY*dT - dSinAlpha*fNewVecZ;
    // mNewMat[1][1] = dCosAlpha + fNewVecY*fNewVecY*dT;
    // mNewMat[1][2] = fNewVecY*fNewVecZ*dT + dSinAlpha*fNewVecX;
    // mNewMat[1][3] = 0;
	//
    // mNewMat[2][0] = fNewVecZ*fNewVecX*dT + dSinAlpha*fNewVecY;
    // mNewMat[2][1] = fNewVecZ*fNewVecY*dT - dSinAlpha*fNewVecX;
    // mNewMat[2][2] = dCosAlpha + fNewVecZ*fNewVecZ*dT;
    // mNewMat[2][3] = 0;
	//
    // mNewMat[3][0] = 0;
    // mNewMat[3][1] = 0;
    // mNewMat[3][2] = 0;
    // mNewMat[3][3] = 1;
}
