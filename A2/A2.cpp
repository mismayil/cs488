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

#define PI 3.14

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
	//glEnable(GL_LINE_SMOOTH);


	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();

	float angle = PI / 4;
	ASPECT = m_windowHeight / m_windowWidth;
	float a[16];

	a[0] = (1.0f / tan(angle / 2)) / ASPECT;
	a[1] = 0;
	a[2] = 0;
	a[3] = 0;
	a[4] = 0;
	a[5] = 1.0f / tan(angle / 2);
	a[6] = 0;
	a[7] = 0;
	a[8] = 0;
	a[9] = 0;
	a[10] = -(FAR_PLANE + NEAR_PLANE) / (FAR_PLANE - NEAR_PLANE);
	a[11] = -1;
	a[12] = 0;
	a[13] = 0;
	a[14] = (-2 * FAR_PLANE * NEAR_PLANE) / (FAR_PLANE - NEAR_PLANE);
	a[15] = 0;

	MODEL = glm::mat4();
	MODEL = glm::rotate(mat4(), (float) PI/8, vec3(0.0f, 1.0f, 0.0f)) * MODEL;
	VIEW = glm::lookAt(
		glm::vec3(4.0f, 0.0f, 10.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(3.0f, 1.0f, 0.0f));
	PROJ = make_mat4(a);
}

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

void A2::drawCube(float dx, float dy, float dz) {
	glm::vec4 cube_verts[8];
	int n = 0;

	for (int i = -1; i < 2; i += 2) {
		for (int j = -1; j < 2; j += 2) {
			for (int k = -1; k < 2; k += 2) {
				cube_verts[n++] = vec4(i+dx, j+dy, k+dz, 1);
			}
		}
	}

	setLineColour(vec3(1.0f, 1.0f, 1.0f));

	for (int i = 0; i < 8; i++) {
		cube_verts[i] = PROJ * VIEW * MODEL * cube_verts[i];
	}

	drawLine(vec2(cube_verts[0].x / cube_verts[0].w, cube_verts[0].y / cube_verts[0].w), vec2(cube_verts[4].x / cube_verts[4].w, cube_verts[4].y / cube_verts[4].w));
	drawLine(vec2(cube_verts[4].x / cube_verts[4].w, cube_verts[4].y / cube_verts[4].w), vec2(cube_verts[5].x / cube_verts[5].w, cube_verts[5].y / cube_verts[5].w));
	drawLine(vec2(cube_verts[5].x / cube_verts[5].w, cube_verts[5].y / cube_verts[5].w), vec2(cube_verts[1].x / cube_verts[1].w, cube_verts[1].y / cube_verts[1].w));
	drawLine(vec2(cube_verts[1].x / cube_verts[1].w, cube_verts[1].y / cube_verts[1].w), vec2(cube_verts[0].x / cube_verts[0].w, cube_verts[0].y / cube_verts[0].w));
	drawLine(vec2(cube_verts[2].x / cube_verts[2].w, cube_verts[2].y / cube_verts[2].w), vec2(cube_verts[6].x / cube_verts[6].w, cube_verts[6].y / cube_verts[6].w));
	drawLine(vec2(cube_verts[6].x / cube_verts[6].w, cube_verts[6].y / cube_verts[6].w), vec2(cube_verts[7].x / cube_verts[7].w, cube_verts[7].y / cube_verts[7].w));
	drawLine(vec2(cube_verts[7].x / cube_verts[7].w, cube_verts[7].y / cube_verts[7].w), vec2(cube_verts[3].x / cube_verts[3].w, cube_verts[3].y / cube_verts[3].w));
	drawLine(vec2(cube_verts[3].x / cube_verts[3].w, cube_verts[3].y / cube_verts[3].w), vec2(cube_verts[2].x / cube_verts[2].w, cube_verts[2].y / cube_verts[2].w));
	drawLine(vec2(cube_verts[1].x / cube_verts[1].w, cube_verts[1].y / cube_verts[1].w), vec2(cube_verts[3].x / cube_verts[3].w, cube_verts[3].y / cube_verts[3].w));
	drawLine(vec2(cube_verts[0].x / cube_verts[0].w, cube_verts[0].y / cube_verts[0].w), vec2(cube_verts[2].x / cube_verts[2].w, cube_verts[2].y / cube_verts[2].w));
	drawLine(vec2(cube_verts[4].x / cube_verts[4].w, cube_verts[4].y / cube_verts[4].w), vec2(cube_verts[6].x / cube_verts[6].w, cube_verts[6].y / cube_verts[6].w));
	drawLine(vec2(cube_verts[5].x / cube_verts[5].w, cube_verts[5].y / cube_verts[5].w), vec2(cube_verts[7].x / cube_verts[7].w, cube_verts[7].y / cube_verts[7].w));
}

void A2::drawModelCoord(float dx, float dy, float dz) {
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

void A2::drawWorldCoord(float dx, float dy, float dz) {
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
	drawCube(0, 0, 0);
	drawModelCoord(0, 0, 0);
	drawWorldCoord(0, 0, 0);
	// // Draw outer square:
	// setLineColour(vec3(1.0f, 0.7f, 0.8f));
	// drawLine(vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f));
	// drawLine(vec2(0.5f, -0.5f), vec2(0.5f, 0.5f));
	// drawLine(vec2(0.5f, 0.5f), vec2(-0.5f, 0.5f));
	// drawLine(vec2(-0.5f, 0.5f), vec2(-0.5f, -0.5f));


	// // Draw inner square:
	// setLineColour(vec3(0.2f, 1.0f, 1.0f));
	// drawLine(vec2(-0.25f, -0.25f), vec2(0.25f, -0.25f));
	// drawLine(vec2(0.25f, -0.25f), vec2(0.25f, 0.25f));
	// drawLine(vec2(0.25f, 0.25f), vec2(-0.25f, 0.25f));
	// drawLine(vec2(-0.25f, 0.25f), vec2(-0.25f, -0.25f));
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


		// Add more gui elements here here ...


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

	// Fill in with event handling code...

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

	// Fill in with event handling code...

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
