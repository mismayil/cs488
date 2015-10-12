#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <math.h>

using namespace glm;

#define PI 3.1415926535

// modes
enum modes {
	ROTATE_VIEW,
	TRANSLATE_VIEW,
	PERSPECTIVE,
	ROTATE_MODEL,
	TRANSLATE_MODEL,
	SCALE_MODEL,
	VIEWPORT
};

// #define ROTATE_VIEW 	0
// #define TRANSLATE_VIEW 	1
// #define PERSPECTIVE 	2
// #define ROTATE_MODEL	3
// #define TRANSLATE_MODEL 4
// #define SCALE_MODEL		5
// #define VIEWPORT		6

#define SCALE_MIN_LIMIT = 0.1;
#define SCALE_MAX_LIMIT = 10;
#define SCALE_UP = 1.005f;
#define SCALE_DOWN = (double) 1 / SCALE_UP;
// #define FACTOR =

point prevMousePos;
bool mouseLeftClicked = false;
bool mouseMiddleClicked = false;
bool mouseRightClicked = false;

static double scales = 1;

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.reserve(kMaxVertices);
	colours.reserve(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f))
{

}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);


	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();

	mode = 0;
	FAR_PLANE = -1;
	NEAR_PLANE = 1;
	ASPECT = m_windowHeight / m_windowWidth;

	float fov = PI / 4;
	float p[16];

	p[0] = (1.0f / tan(fov / 2)) / ASPECT;
	p[1] = 0;
	p[2] = 0;
	p[3] = 0;
	p[4] = 0;
	p[5] = 1.0f / tan(fov / 2);
	p[6] = 0;
	p[7] = 0;
	p[8] = 0;
	p[9] = 0;
	p[10] = -(FAR_PLANE + NEAR_PLANE) / (FAR_PLANE - NEAR_PLANE);
	p[11] = -1;
	p[12] = 0;
	p[13] = 0;
	p[14] = (-2 * FAR_PLANE * NEAR_PLANE) / (FAR_PLANE - NEAR_PLANE);
	p[15] = 0;

	MODEL = glm::mat4();
	VIEW = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 10.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	PROJ = glm::make_mat4(p);
}

void A2::reset() {}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//----------------------------------------------------------------------------------------
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec2 & v0,   // Line Start (NDC coordinate)
		const glm::vec2 & v1    // Line End (NDC coordinate)
) {

	m_vertexData.positions[m_vertexData.index] = v0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = v1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

void A2::drawCube() {
	glm::vec4 cube[8];

	int n = 0;
	for (int i = -1; i < 2; i += 2) {
		for (int j = -1; j < 2; j += 2) {
			for (int k = -1; k < 2; k += 2) {
				cube[n++] = vec4(i, j, k, 1);
			}
		}
	}

	setLineColour(vec3(1.0f, 1.0f, 1.0f));

	for (int i = 0; i < 8; i++) {
		cube[i] = PROJ * VIEW * MODEL * cube[i];
	}

	drawLine(vec2(cube[0].x / cube[0].w, cube[0].y / cube[0].w), vec2(cube[4].x / cube[4].w, cube[4].y / cube[4].w));
	drawLine(vec2(cube[4].x / cube[4].w, cube[4].y / cube[4].w), vec2(cube[5].x / cube[5].w, cube[5].y / cube[5].w));
	drawLine(vec2(cube[5].x / cube[5].w, cube[5].y / cube[5].w), vec2(cube[1].x / cube[1].w, cube[1].y / cube[1].w));
	drawLine(vec2(cube[1].x / cube[1].w, cube[1].y / cube[1].w), vec2(cube[0].x / cube[0].w, cube[0].y / cube[0].w));
	drawLine(vec2(cube[2].x / cube[2].w, cube[2].y / cube[2].w), vec2(cube[6].x / cube[6].w, cube[6].y / cube[6].w));
	drawLine(vec2(cube[6].x / cube[6].w, cube[6].y / cube[6].w), vec2(cube[7].x / cube[7].w, cube[7].y / cube[7].w));
	drawLine(vec2(cube[7].x / cube[7].w, cube[7].y / cube[7].w), vec2(cube[3].x / cube[3].w, cube[3].y / cube[3].w));
	drawLine(vec2(cube[3].x / cube[3].w, cube[3].y / cube[3].w), vec2(cube[2].x / cube[2].w, cube[2].y / cube[2].w));
	drawLine(vec2(cube[1].x / cube[1].w, cube[1].y / cube[1].w), vec2(cube[3].x / cube[3].w, cube[3].y / cube[3].w));
	drawLine(vec2(cube[0].x / cube[0].w, cube[0].y / cube[0].w), vec2(cube[2].x / cube[2].w, cube[2].y / cube[2].w));
	drawLine(vec2(cube[4].x / cube[4].w, cube[4].y / cube[4].w), vec2(cube[6].x / cube[6].w, cube[6].y / cube[6].w));
	drawLine(vec2(cube[5].x / cube[5].w, cube[5].y / cube[5].w), vec2(cube[7].x / cube[7].w, cube[7].y / cube[7].w));
}

// draw model coordinate system
void A2::drawModelCoord() {
	glm::vec4 xaxis = vec4(0.5f, 0.0f, 0.0f, 1);
	glm::vec4 yaxis = vec4(0.0f, 0.5f, 0.0f, 1);
	glm::vec4 zaxis = vec4(0.0f, 0.0f, 0.5f, 1);
	glm::vec4 center = vec4(0.0f, 0.0f, 0.0f, 1);

	xaxis = PROJ * VIEW * MODEL * xaxis;
	yaxis = PROJ * VIEW * MODEL * yaxis;
	zaxis = PROJ * VIEW * MODEL * zaxis;
	center = PROJ * VIEW * MODEL * center;

	setLineColour(vec3(0.0f, 0.0f, 1.0f));
	drawLine(vec2(center.x / center.w, center.y / center.w), vec2(xaxis.x / xaxis.w, xaxis.y / xaxis.w));
	drawLine(vec2(center.x / center.w, center.y / center.w), vec2(yaxis.x / yaxis.w, yaxis.y / yaxis.w));
	drawLine(vec2(center.x / center.w, center.y / center.w), vec2(zaxis.x / zaxis.w, zaxis.y / zaxis.w));
}

// draw world coordinate system
void A2::drawWorldCoord() {
	glm::vec4 xaxis = vec4(2.0f, 0.0f, 0.0f, 1);
	glm::vec4 yaxis = vec4(0.0f, 2.0f, 0.0f, 1);
	glm::vec4 zaxis = vec4(0.0f, 0.0f, 2.0f, 1);
	glm::vec4 center = vec4(0.0f, 0.0f, 0.0f, 1);

	xaxis = PROJ * VIEW * xaxis;
	yaxis = PROJ * VIEW * yaxis;
	zaxis = PROJ * VIEW * zaxis;
	center = PROJ * VIEW * center;

	setLineColour(vec3(1.0f, 0.0f, 0.0f));
	drawLine(vec2(center.x / center.w, center.y / center.w), vec2(xaxis.x / xaxis.w, xaxis.y / xaxis.w));
	drawLine(vec2(center.x / center.w, center.y / center.w), vec2(yaxis.x / yaxis.w, yaxis.y / yaxis.w));
	drawLine(vec2(center.x / center.w, center.y / center.w), vec2(zaxis.x / zaxis.w, zaxis.y / zaxis.w));
}

/*----------------------------------------------------------------------------------------
*
* Called once per frame, before guiLogic().
*/
void A2::appLogic() {
	// Place per frame, application logic here ...

	// Call at the beginning of frame, before drawing lines:
	initLineData();
	drawCube();
	drawModelCoord();
	drawWorldCoord();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
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

		ImGui::PushID(0);
		if (ImGui::RadioButton("##RotateView", &mode, 0)) {

		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::Text("Rotate View");

		ImGui::PushID(1);
		if (ImGui::RadioButton("##TranslateView", &mode, 1)) {

		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::Text("Translate View");

		ImGui::PushID(2);
		if (ImGui::RadioButton("##Perspective", &mode, 2)) {

		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::Text("Perspective");

		ImGui::PushID(3);
		if (ImGui::RadioButton("##RotateModel", &mode, 3)) {

		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::Text("Rotate Model");

		ImGui::PushID(4);
		if (ImGui::RadioButton("##TranslateModel", &mode, 4)) {

		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::Text("Translate Model");

		ImGui::PushID(5);
		if (ImGui::RadioButton("##ScaleModel", &mode, 5)) {

		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::Text("Scale Model");

		ImGui::PushID(6);
		if (ImGui::RadioButton("##Viewport", &mode, 6)) {

		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::Text("Viewport");

		if (ImGui::Button("Reset")) {
			reset();
		}
		ImGui::SameLine();

		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
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
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {

		if (mouseLeftClicked || mouseMiddleClicked || mouseRightClicked) {
			int width, height;
			glfwGetWindowSize(m_window, &width, &height);
			point center = {(float)width / 2.0f, (float)height / 2.0f, 0.0f};
			point curMousePos = {(float)xPos, (float)yPos, 0.0f};
			double theta = -(curMousePos.x - prevMousePos.x) * PI / m_windowWidth;
			glm::mat4 R, T, S;

			if (mouseLeftClicked) {
				switch (mode) {
					case ROTATE_MODEL:
						R = glm::rotate(mat4(), (float) theta, vec3(1, 0, 0));
						MODEL = MODEL * R * glm::inverse(MODEL) * MODEL;
						break;
					case TRANSLATE_MODEL:
						T = glm::translate(mat4(), vec3((curMousePos.x - prevMousePos.x) * FACTOR, 0, 0));
						MODEL = T * MODEL;
						break;
					case SCALE_MODEL:
						if (curMousePos.x > prevMousePos.x) S = glm::scale(mat4(), vec3(SCALE_UP, 1, 1));
						else S = glm::scale(mat4(), vec3(SCALE_DOWN, 1, 1));
						MODEL = MODEL * S * glm::inverse(MODEL) * MODEL;
						break;
					default:break;
				}
			} else

			if (mouseMiddleClicked) {
				switch (mode) {
					case ROTATE_MODEL:
						R = glm::rotate(mat4(), (float) theta, vec3(0, 1, 0));
						MODEL = MODEL * R * glm::inverse(MODEL) * MODEL;
						break;
					case TRANSLATE_MODEL:
						T = glm::translate(mat4(), vec3(0, (curMousePos.x - prevMousePos.x) * FACTOR, 0));
						MODEL = T * MODEL;
						break;
					case SCALE_MODEL:
						if (curMousePos.x > prevMousePos.x) S = glm::scale(mat4(), vec3(1, SCALE_UP, 1));
						else S = glm::scale(mat4(), vec3(1, SCALE_DOWN, 1));
						MODEL = MODEL * S * glm::inverse(MODEL) * MODEL;
						break;
					default:break;
				}
			} else

			if (mouseRightClicked) {
				switch (mode) {
					case ROTATE_MODEL:
						R = glm::rotate(mat4(), (float) theta, vec3(0, 0, 1));
						MODEL = MODEL * R * glm::inverse(MODEL) * MODEL;
						break;
					case TRANSLATE_MODEL:
						T = glm::translate(mat4(), vec3(0, 0, (curMousePos.x - prevMousePos.x) * FACTOR));
						MODEL = T * MODEL;
						break;
					case SCALE_MODEL:
						if (curMousePos.x > prevMousePos.x) S = glm::scale(mat4(), vec3(1, 1, SCALE_UP));
						else S = glm::scale(mat4(), vec3(1, 1, SCALE_DOWN));
						MODEL = MODEL * S * glm::inverse(MODEL) * MODEL;
						break;
					default:break;
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
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if (actions == GLFW_PRESS) {
			double xpos, ypos;
			glfwGetCursorPos(m_window, &xpos, &ypos);
			prevMousePos = {(float) xpos, (float) ypos, 0.0f};
			if (button == GLFW_MOUSE_BUTTON_LEFT) mouseLeftClicked = true;
			if (button == GLFW_MOUSE_BUTTON_MIDDLE) mouseMiddleClicked = true;
			if (button == GLFW_MOUSE_BUTTON_RIGHT) mouseRightClicked = true;
		}
		if (actions == GLFW_RELEASE) {
			if (button == GLFW_MOUSE_BUTTON_LEFT) mouseLeftClicked = false;
			if (button == GLFW_MOUSE_BUTTON_MIDDLE) mouseMiddleClicked = false;
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
bool A2::mouseScrollEvent (
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
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}
