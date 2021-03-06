#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <set>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

#define PI 				3.14159265359
#define FACTOR 			(double) 2 / m_windowWidth

static int HEAD_JOINT_ID = 3;
set<unsigned int> xRotaters;
set<unsigned int> yRotaters;
set<unsigned int> zRotaters;

enum modes {
	POSITION,
	JOINT
};

bool CIRCLE = false;
bool Z_BUFFER = true;
bool BACKFACE_CULLING = false;
bool FRONTFACE_CULLING = false;

enum trans_types {
	TRANSLATE,
	ROTATE_SCALE
};

struct command {
	vector<unsigned int> ids;
	int type;
	mat4 T;
};

vec4 prevMousePos, onClickMousePos;
bool mouseLeftClicked = false;
bool mouseMiddleClicked = false;
bool mouseRightClicked = false;

mat4 TMPX;
mat4 TMPY;
mat4 TMPZ;
mat4 TMP;    // current matrix transformation
mat4 TPOSITION;
mat4 ORIENTATION;

int ttype;   // current trans type

vector<command> commands;
int last_command = -1;
vector<unsigned int> selected_nodes;

mat4 vAxisRotMatrix(float fVecX, float fVecY, float fVecZ);
vec4 vCalcRotVec(float fNewX, float fNewY, float fOldX, float fOldY, float fDiameter);

// get current node
SceneNode* A3::getNode(SceneNode *node, unsigned int id) {
	if (node->m_nodeId == id) return node;

	for (SceneNode *child : node->children) {
		SceneNode* tmp = getNode(child, id);
		if (tmp) return tmp;
	}

	return NULL;
}

// get parent node
SceneNode* A3::getPNode(SceneNode *node, unsigned int id) {
	for (SceneNode *child : node->children) {
		if (child->m_nodeId == id) return node;
		SceneNode *tmp = getPNode(child, id);
		if (tmp) return tmp;
	}
	return NULL;
}

void A3::add_command(vector<unsigned int> ids, mat4 T, int type) {
	struct command cmd = {ids, type, T};
	commands.resize(last_command+1);
	commands.push_back(cmd);
	last_command++;
}

void A3::undo() {
	if (commands.empty() || last_command < 0) return;
	for (unsigned int i = 0; i < commands[last_command].ids.size(); i++) {
		SceneNode *node = getNode(m_rootNode, commands[last_command].ids[i]);
		if (!node) return;
		node->set_transform(node->get_transform() * inverse(commands[last_command].T));
	}
	last_command--;
}

void A3::redo() {
	if (commands.empty() || last_command+1 == commands.size()) return;
	last_command++;
	for (unsigned int i = 0; i < commands[last_command].ids.size(); i++) {
		SceneNode *node = getNode(m_rootNode, commands[last_command].ids[i]);
		if (!node) return;
		node->set_transform(node->get_transform() * commands[last_command].T);
	}
}

void A3::set_picking_mode(int mode) {
	m_shader.enable();
	GLint picking = m_shader.getUniformLocation("picking");
	glUniform1i(picking, mode);
	CHECK_GL_ERRORS;
	m_shader.disable();
}

void A3::resetPosition() {
	m_rootNode->set_transform(inverse(TPOSITION) * m_rootNode->get_transform());
	TPOSITION = mat4();
}

void A3::resetOrientation() {
	m_rootNode->set_transform(m_rootNode->get_transform() * inverse(ORIENTATION));
	ORIENTATION = mat4();
}

void A3::resetJoints() {
	for (int i = 0; i <= last_command; i++) {
		for (int j = 0; j < commands[i].ids.size(); j++) {
			SceneNode *node = getNode(m_rootNode, commands[i].ids[j]);
			if (node->m_nodeType == NodeType::JointNode) {
				node->set_transform(node->get_transform() * inverse(commands[i].T));
			}
		}
	}

	commands.clear();
	last_command = -1;
}

void A3::resetAll() {
	resetPosition();
	resetOrientation();
	resetJoints();
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
	  mode(POSITION)
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

	set<unsigned int>::iterator xit = xRotaters.begin();
	set<unsigned int>::iterator yit = yRotaters.begin();
	set<unsigned int>::iterator zit = zRotaters.begin();

	xRotaters.insert(xit, 49);
	xRotaters.insert(xit, 56);
	xRotaters.insert(xit, 61);
	xRotaters.insert(xit, 52);
	xRotaters.insert(xit, 58);
	xRotaters.insert(xit, 64);

	yRotaters.insert(yit, 3);

	zRotaters.insert(zit, 22);
	zRotaters.insert(zit, 25);
	zRotaters.insert(zit, 28);
	zRotaters.insert(zit, 31);
	zRotaters.insert(zit, 34);
	zRotaters.insert(zit, 35);
	zRotaters.insert(zit, 36);
	zRotaters.insert(zit, 41);
	zRotaters.insert(zit, 42);
	zRotaters.insert(zit, 43);
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
	//m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));
	m_rootNode = (SceneNode *)(import_lua(assetFilePath));
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
	m_light.position = vec3(0.0f, 0.0f, 0.0f);
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

			if (ImGui::BeginMenu("Application")) {
				if (ImGui::Button("Reset Position [I]")) {
					resetPosition();
				}
				if (ImGui::Button("Reset Orientation [O]")) {
					resetOrientation();
				}
				if (ImGui::Button("Reset Joints [N]")) {
					resetJoints();
				}
				if (ImGui::Button("Reset All [A]")) {
					resetAll();
				}
				if (ImGui::Button("Quit [Q]")) {
					glfwSetWindowShouldClose(m_window, GL_TRUE);
				}
				ImGui::EndMenu();
			}
			//ImGui::SameLine();

			if (ImGui::BeginMenu("Edit")) {
				if (ImGui::Button("Undo [U]")) {
					undo();
				}
				if (ImGui::Button("Redo [R]")) {
					redo();
				}
				ImGui::EndMenu();
			}
			//ImGui::SameLine();

			if (ImGui::BeginMenu("Options")) {
				if (ImGui::Checkbox("Circle [C]", &CIRCLE)) {

				}
				if (ImGui::Checkbox("Z-buffer [Z]", &Z_BUFFER)) {

				}
				if (ImGui::Checkbox("Backface culling [B]", &BACKFACE_CULLING)) {

				}
				if (ImGui::Checkbox("Frontface culling [F]", &FRONTFACE_CULLING)) {

				}
				ImGui::EndMenu();
			}

			ImGui::PushID(0);
			if (ImGui::RadioButton("##Position", &mode, 0)) {

			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::Text("Position/Orientation [P]");

			ImGui::PushID(1);
			if (ImGui::RadioButton("##Joint", &mode, 1)) {

			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::Text("Joints [J]");
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

		// -- Set node id
		GLint id = shader.getUniformLocation("id");
		glUniform1f(id, node.m_nodeId);
		CHECK_GL_ERRORS;

	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

	if (Z_BUFFER) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);

	if (BACKFACE_CULLING || FRONTFACE_CULLING) {
		glEnable(GL_CULL_FACE);
		if (BACKFACE_CULLING && FRONTFACE_CULLING) glCullFace(GL_FRONT_AND_BACK);
		else if (BACKFACE_CULLING) glCullFace(GL_BACK);
		else if (FRONTFACE_CULLING) glCullFace(GL_FRONT);
	} else glDisable(GL_CULL_FACE);

	renderSceneGraph(*m_rootNode);

	if (CIRCLE) renderArcCircle();
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
			double theta = (curMousePos.y - prevMousePos.y) * PI / 180;
			vec4 newPos = curMousePos - center;
			vec4 oldPos = prevMousePos - center;
			float diameter = m_framebufferWidth / 2 < m_framebufferHeight / 2 ? m_framebufferWidth / 2: m_framebufferHeight / 2;
			vec4 newAxis;
			mat4 T;
			vec3 rVec = vec3(0.0f, 0.0f, 1.0f);

			if (mouseLeftClicked) {
				switch (mode) {
					case POSITION:
						T = translate(mat4(), vec3((curMousePos.x - prevMousePos.x) * FACTOR, -(curMousePos.y - prevMousePos.y) * FACTOR, 0));
						TPOSITION *= T;
						m_rootNode->set_transform(T * m_rootNode->get_transform());
						break;
					case JOINT:
						break;
				}
			}

			if (mouseMiddleClicked) {
				switch (mode) {
					case POSITION:
						T = translate(mat4(), vec3(0, 0, -(curMousePos.y - prevMousePos.y) * FACTOR));
						TPOSITION *= T;
						m_rootNode->set_transform(T * m_rootNode->get_transform());
						break;
					case JOINT:
						for (int i = 0; i < selected_nodes.size(); i++) {
							JointNode *jnode = static_cast<JointNode *>(getNode(m_rootNode, selected_nodes[i]));
							if (jnode->currentX + theta > jnode->m_joint_x.min && jnode->currentX + theta < jnode->m_joint_x.max) {
								if (xRotaters.count(jnode->m_nodeId)) {
									rVec = vec3(1.0f, 0.0f, 0.0f);
								}
								if (yRotaters.count(jnode->m_nodeId)) {
									rVec = vec3(0.0f, 1.0f, 0.0f);
								}
								if (zRotaters.count(jnode->m_nodeId)) {
									rVec = vec3(0.0f, 0.0f, 1.0f);
								}
								T = rotate(mat4(), (float) theta, rVec);
								jnode->set_transform(jnode->get_transform() * T);
								jnode->currentX += theta;
							}
						}
						TMP *= T;
						break;
				}
			}

			if (mouseRightClicked) {
				switch (mode) {
					case POSITION:
						newAxis = vCalcRotVec(newPos.x, newPos.y, oldPos.x, oldPos.y, diameter);
						T = vAxisRotMatrix(newAxis.x, newAxis.y, newAxis.z);
						ORIENTATION *= T;
						m_rootNode->set_transform(m_rootNode->get_transform() * T);
						break;
					case JOINT:
						T = rotate(mat4(), (float) theta, vec3(0.0f, 1.0f, 0.0f));
						TMP *= T;
						JointNode *jnode = static_cast<JointNode *>(getNode(m_rootNode, HEAD_JOINT_ID));
						if (jnode->isSelected) {
							if (jnode->currentY + theta > jnode->m_joint_y.min && jnode->currentY + theta < jnode->m_joint_y.max) {
								jnode->set_transform(jnode->get_transform() * T);
								jnode->currentY += theta;
							}
						}
						break;
				}
			}
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
			int width, height;
			glfwGetWindowSize(m_window, &width, &height);
			double xpos, ypos;
			glfwGetCursorPos(m_window, &xpos, &ypos);
			prevMousePos = vec4((float) xpos, (float) ypos, 0.0f, 1.0f);

			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				mouseLeftClicked = true;
				if (mode == JOINT) {
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					set_picking_mode(1);

					glEnable(GL_DEPTH_TEST);
					renderSceneGraph(*m_rootNode);
					glDisable(GL_DEPTH_TEST);

					unsigned char *color = new unsigned char;
					glReadPixels(xpos, m_windowHeight - ypos, 1, 1, GL_BLUE, GL_UNSIGNED_BYTE, (void *)color);
					unsigned int nodeId = (unsigned int) *color;

					set_picking_mode(0);

					SceneNode *node = getNode(m_rootNode, nodeId);
					if (!node) return false;
					SceneNode *parent = getPNode(m_rootNode, nodeId);

					if (parent->m_nodeType == NodeType::JointNode) {
						GeometryNode* gnode = static_cast<GeometryNode *>(node);
						if (parent->isSelected) {
							parent->isSelected = false;
							gnode->material = gnode->original;

							for (int i = 0; i < selected_nodes.size(); i++) {
								if (parent->m_nodeId == selected_nodes[i]) {
									selected_nodes.erase(selected_nodes.begin()+i);
									break;
								}
							}
						} else {
							parent->isSelected = true;
							gnode->original = gnode->material;
							gnode->material.kd = vec3(0.0f, 0.0f, 0.0f);
							if (parent->m_nodeId != HEAD_JOINT_ID) selected_nodes.push_back(parent->m_nodeId);
						}
					}
				}
			}
			if (button == GLFW_MOUSE_BUTTON_MIDDLE) mouseMiddleClicked = true;
			if (button == GLFW_MOUSE_BUTTON_RIGHT) mouseRightClicked = true;
		}

		if (actions == GLFW_RELEASE) {
			vector<unsigned int> ids;

			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				mouseLeftClicked = false;
			}

			if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
				mouseMiddleClicked = false;

				if (mode == JOINT) {
					for (int i = 0; i < selected_nodes.size(); i++) {
						ids.push_back(selected_nodes[i]);
					}
					add_command(ids, TMP, ttype);
				}
			}

			if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				mouseRightClicked = false;

				if (mode == JOINT) {
					JointNode *jnode = static_cast<JointNode *>(getNode(m_rootNode, HEAD_JOINT_ID));
					if (jnode->isSelected) {
						ids.push_back(HEAD_JOINT_ID);
						add_command(ids, TMP, ttype);
					}
				}
			}
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

	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_Q:
				glfwSetWindowShouldClose(m_window, GL_TRUE);
				break;
			case GLFW_KEY_A:
				resetAll();
				break;
			case GLFW_KEY_O:
				resetOrientation();
				break;
			case GLFW_KEY_N:
				resetJoints();
				break;
			case GLFW_KEY_I:
				resetPosition();
				break;
			case GLFW_KEY_U:
				undo();
				break;
			case GLFW_KEY_R:
				redo();
				break;
			case GLFW_KEY_C:
				CIRCLE = CIRCLE ? false : true;
				break;
			case GLFW_KEY_Z:
				Z_BUFFER = Z_BUFFER ? false : true;
				break;
			case GLFW_KEY_B:
				BACKFACE_CULLING = BACKFACE_CULLING ? false : true;
				break;
			case GLFW_KEY_F:
				FRONTFACE_CULLING = FRONTFACE_CULLING ? false : true;
				break;
			case GLFW_KEY_P:
				mode = POSITION;
				break;
			case GLFW_KEY_J:
				mode = JOINT;
				break;
			case GLFW_KEY_M:
				show_gui = !show_gui;
				eventHandled = true;
				break;
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
}
