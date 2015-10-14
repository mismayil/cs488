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

// scale constants
#define SCALE_MIN_LIMIT 0.1
#define SCALE_MAX_LIMIT 10
#define SCALE_UP 		1.05f
#define SCALE_DOWN 		(double) 1 / SCALE_UP

// balance factor
#define FACTOR 			(double) 2 / m_windowWidth

// field of view constants
#define FOV_MIN 		5 * PI / 180
#define FOV_MAX			160 * PI / 180

// near-far plane constants
#define NPC 0.1f
#define FPC 0.1f

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

glm::vec4 prevMousePos, onClickMousePos;
bool mouseLeftClicked = false;
bool mouseMiddleClicked = false;
bool mouseRightClicked = false;

static double scalex = 1;
static double scaley = 1;
static double scalez = 1;

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
	ASPECT = m_windowHeight / m_windowWidth;
	FOV = PI / 6;
	NP = -5.0f;
	FP = -20.0f;

	vpStart = glm::vec4(-0.9f, -0.9f, 0, 1);
	vpWidth = 1.8f;
	vpHeight = 1.8f;
	VP = getViewport(vpStart, vpWidth, vpHeight, FP, NP);
	MODEL = glm::mat4();
	CMODEL = glm::mat4();
	VIEW = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 10.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	PROJ = getProj(FOV, ASPECT, FP, NP);
}

glm::mat4 A2::getProj(float fov, float aspect, float f, float n) {
	glm::mat4 proj = glm::mat4(
		vec4((1.0f / tan(fov / 2)) / aspect, 0, 0, 0),
		vec4(0, 1.0f / tan(fov / 2), 0, 0),
		vec4(0, 0, -(f + n) / (f - n), -1),
		vec4(0, 0, (-2 * f * n) / (f - n), 0)
	);
	return proj;
}

glm::mat4 A2::getViewport(glm::vec4 vp, float width, float height, float f, float n) {
	glm::mat4 viewport = mat4();
	viewport = glm::scale(mat4(), vec3(width/2, height / 2, (f - n) / 2)) * viewport;
	viewport = glm::translate(mat4(), vec3(vp.x + width / 2, vp.y + height / 2, (f + n) / 2)) * viewport;
	return viewport;
}

void A2::reset() {
	mode = 0;
	FOV = PI / 6;
	NP = -5.0f;
	FP = -20.0f;
	vpStart = glm::vec4(-0.9f, -0.9f, 0, 1);
	vpWidth = 1.8f;
	vpHeight = 1.8f;
	VP = getViewport(vpStart, vpWidth, vpHeight, FP, NP);
	MODEL = glm::mat4();
	CMODEL = glm::mat4();
	VIEW = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 10.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	PROJ = getProj(FOV, ASPECT, FP, NP);
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

// draw viewport
void A2::drawViewport() {
	setLineColour(vec3(0.0f, 0.0f, 0.0f));
	glm::vec4 LB = glm::vec4(-0.9f, -0.9f, 0, 1);
	glm::vec4 LT = glm::vec4(-0.9f, 0.9f, 0, 1);
	glm::vec4 RB = glm::vec4(0.9f, -0.9f, 0, 1);
	glm::vec4 RT = glm::vec4(0.9f, 0.9f, 0, 1);

	LB = VPMODEL * LB;
	LT = VPMODEL * LT;
	RB = VPMODEL * RB;
	RT = VPMODEL * RT;

	// LB = LB / LB.w;
	// LT = LT / LT.w;
	// RB = RB / RB.w;
	// RT = RT / RT.w;

	drawLine(vec2(LB.x, LB.y), vec2(RB.x, RB.y));
	drawLine(vec2(LB.x, LB.y), vec2(LT.x, LT.y));
	drawLine(vec2(LT.x, LT.y), vec2(RT.x, RT.y));
	drawLine(vec2(RB.x, RB.y), vec2(RT.x, RT.y));
}

// draw cube
void A2::drawCube() {
	glm::vec4 cube[8];
	vector<plane> planes;
	vector<line> lines;
	glm::vec4 A, B, P, n;

	// near plane
	P = glm::vec4(0.0f, 0.0f, NP, 1.0f);
	n = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	plane nearPlane = {P, n};
	planes.push_back(nearPlane);

	//far plane
	P = glm::vec4(0.0f, 0.0f, FP, 1.0f);
	n = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	plane farPlane = {P, n};
	planes.push_back(farPlane);

	// left plane
	P = glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f);
	n = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	planes.push_back({P, n});

	// right plane
	P = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	n = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	planes.push_back({P, n});

	// top plane
	P = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	n = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
	planes.push_back({P, n});

	// bottom plane
	P = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	n = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	planes.push_back({P, n});

	int l = 0;
	for (int i = -1; i < 2; i += 2) {
		for (int j = -1; j < 2; j += 2) {
			for (int k = -1; k < 2; k += 2) {
				cube[l++] = vec4(i, j, k, 1);
			}
		}
	}

	setLineColour(vec3(1.0f, 1.0f, 1.0f));

	for (int i = 0; i < 8; i++) {
		cube[i] = VIEW * MODEL * cube[i];
	}

	lines.push_back({cube[0], cube[4]});
	lines.push_back({cube[4], cube[5]});
	lines.push_back({cube[5], cube[1]});
	lines.push_back({cube[1], cube[0]});
	lines.push_back({cube[2], cube[6]});
	lines.push_back({cube[6], cube[7]});
	lines.push_back({cube[7], cube[3]});
	lines.push_back({cube[3], cube[2]});
	lines.push_back({cube[1], cube[3]});
	lines.push_back({cube[0], cube[2]});
	lines.push_back({cube[4], cube[6]});
	lines.push_back({cube[5], cube[7]});

	for (int i = 0; i < lines.size(); i++) {
		clip(lines[i], planes);
	}

}

// draw model coordinate system
void A2::drawModelCoord() {
	glm::vec4 xaxis = vec4(0.5f, 0.0f, 0.0f, 1);
	glm::vec4 yaxis = vec4(0.0f, 0.5f, 0.0f, 1);
	glm::vec4 zaxis = vec4(0.0f, 0.0f, 0.5f, 1);
	glm::vec4 center = vec4(0.0f, 0.0f, 0.0f, 1);

	xaxis = PROJ * VIEW * CMODEL * xaxis;
	xaxis = xaxis / xaxis.w;
	yaxis = PROJ * VIEW * CMODEL * yaxis;
	yaxis = yaxis / yaxis.w;
	zaxis = PROJ * VIEW * CMODEL * zaxis;
	zaxis = zaxis / zaxis.w;
	center = PROJ * VIEW * CMODEL * center;
	center = center / center.w;

	setLineColour(vec3(0.0f, 0.0f, 1.0f));
	drawLine(vec2(center.x, center.y), vec2(xaxis.x, xaxis.y));
	setLineColour(vec3(1.0f, 0.0f, 1.0f));
	drawLine(vec2(center.x, center.y), vec2(yaxis.x, yaxis.y));
	setLineColour(vec3(0.5f, 0.5f, 0.5f));
	drawLine(vec2(center.x, center.y), vec2(zaxis.x, zaxis.y));
}

// draw world coordinate system
void A2::drawWorldCoord() {
	glm::vec4 xaxis = vec4(2.0f, 0.0f, 0.0f, 1);
	glm::vec4 yaxis = vec4(0.0f, 2.0f, 0.0f, 1);
	glm::vec4 zaxis = vec4(0.0f, 0.0f, 2.0f, 1);
	glm::vec4 center = vec4(0.0f, 0.0f, 0.0f, 1);

	xaxis = PROJ * VIEW * xaxis;
	xaxis = xaxis / xaxis.w;
	yaxis = PROJ * VIEW * yaxis;
	yaxis = yaxis / yaxis.w;
	zaxis = PROJ * VIEW * zaxis;
	zaxis = zaxis / zaxis.w;
	center = PROJ * VIEW * center;
	center = center / center.w;

	setLineColour(vec3(1.0f, 0.0f, 0.0f));
	drawLine(vec2(center.x, center.y), vec2(xaxis.x, xaxis.y));
	setLineColour(vec3(0.3f, 0.2f, 0.4f));
	drawLine(vec2(center.x, center.y), vec2(yaxis.x, yaxis.y));
	setLineColour(vec3(0.5f, 0.2f, 0.0f));
	drawLine(vec2(center.x, center.y), vec2(zaxis.x, zaxis.y));
}

void A2::clip(line line, vector<plane> planes) {
	float dotA, dotB, t;
	glm::vec4 A, B;
	A = line.A;
	B = line.B;

	for (int i = 0; i < 2; i++) {
		dotA = glm::dot(A - planes[i].P, planes[i].n);
		dotB = glm::dot(B - planes[i].P, planes[i].n);
		t = dotA / (dotA - dotB);

		if (dotA < 0 && dotB < 0) return;
		if (dotA >=0 && dotB >= 0) continue;

		if (dotA < 0) A = A + t * (B - A);
		else B = A + t * (B - A);
	}

	A = PROJ * A;
	B = PROJ * B;
	A = A / A.w;
	B = B / B.w;

	for (int i = 2; i < planes.size(); i++) {
		dotA = glm::dot(A - planes[i].P, planes[i].n);
		dotB = glm::dot(B - planes[i].P, planes[i].n);
		t = dotA / (dotA - dotB);

		if (dotA < 0 && dotB < 0) return;
		if (dotA >=0 && dotB >= 0) continue;

		if (dotA < 0) A = A + t * (B - A);
		else B = A + t * (B - A);
	}

	drawLine(vec2(A.x, A.y), vec2(B.x, B.y));
}

/*----------------------------------------------------------------------------------------
*
* Called once per frame, before guiLogic().
*/
void A2::appLogic() {
	// Place per frame, application logic here ...

	// Call at the beginning of frame, before drawing lines:
	initLineData();
	drawViewport();
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
		ImGui::Text("Rotate View [O]");

		ImGui::PushID(1);
		if (ImGui::RadioButton("##TranslateView", &mode, 1)) {

		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::Text("Translate View [N]");

		ImGui::PushID(2);
		if (ImGui::RadioButton("##Perspective", &mode, 2)) {

		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::Text("Perspective [P]");

		ImGui::PushID(3);
		if (ImGui::RadioButton("##RotateModel", &mode, 3)) {

		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::Text("Rotate Model [R]");

		ImGui::PushID(4);
		if (ImGui::RadioButton("##TranslateModel", &mode, 4)) {

		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::Text("Translate Model [T]");

		ImGui::PushID(5);
		if (ImGui::RadioButton("##ScaleModel", &mode, 5)) {

		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::Text("Scale Model [S]");

		ImGui::PushID(6);
		if (ImGui::RadioButton("##Viewport", &mode, 6)) {

		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::Text("Viewport [V]");

		ImGui::Text("FAR PLANE: %f", FP);
		ImGui::Text("NEAR PLANE: %f", NP);

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
			glm::vec4 center = glm::vec4((float)width / 2.0f, (float)height / 2.0f, 0.0f, 1.0f);
			glm::vec4 curMousePos = glm::vec4((float)xPos, (float)yPos, 0.0f, 1.0f);
			double theta = -(curMousePos.x - prevMousePos.x) * PI / m_windowWidth;
			glm::mat4 R, T, S;

			if (mouseLeftClicked) {
				switch (mode) {
					case ROTATE_VIEW:
						R = glm::rotate(mat4(), (float) -theta, vec3(1, 0, 0));
						VIEW = VIEW * R * glm::inverse(VIEW) * VIEW;
						break;
					case TRANSLATE_VIEW:
						T = glm::translate(mat4(), vec3(-(curMousePos.x - prevMousePos.x) * FACTOR, 0, 0));
						VIEW = T * VIEW;
						break;
					case PERSPECTIVE:
						FOV += theta;
						FOV = FOV < FOV_MIN ? FOV_MIN : FOV;
						FOV = FOV > FOV_MAX ? FOV_MAX : FOV;
						PROJ = getProj(FOV, ASPECT, FP, NP);
						break;
					case ROTATE_MODEL:
						R = glm::rotate(mat4(), (float) theta, vec3(1, 0, 0));
						MODEL = MODEL * R * glm::inverse(MODEL) * MODEL;
						CMODEL = CMODEL * R * glm::inverse(CMODEL) * CMODEL;
						break;
					case TRANSLATE_MODEL:
						T = glm::translate(mat4(), vec3((curMousePos.x - prevMousePos.x) * FACTOR, 0, 0));
						MODEL = T * MODEL;
						CMODEL = T * CMODEL;
						break;
					case SCALE_MODEL:
						if (curMousePos.x > prevMousePos.x && scalex * SCALE_UP < SCALE_MAX_LIMIT) {
							S = glm::scale(mat4(), vec3(SCALE_UP, 1, 1));
							scalex *= SCALE_UP;
						}
						if (curMousePos.x < prevMousePos.x && scalex * SCALE_DOWN > SCALE_MIN_LIMIT) {
							S = glm::scale(mat4(), vec3(SCALE_DOWN, 1, 1));
							scalex *= SCALE_DOWN;
						}
						MODEL = MODEL * S * glm::inverse(MODEL) * MODEL;
						break;
					case VIEWPORT:
						vpWidth = curMousePos.x - vpStart.x;
						vpHeight = curMousePos.y - vpStart.y;
						break;
					default:break;
				}
			}

			if (mouseMiddleClicked) {
				switch (mode) {
					case ROTATE_VIEW:
						R = glm::rotate(mat4(), (float) -theta, vec3(0, 1, 0));
						VIEW = VIEW * R * glm::inverse(VIEW) * VIEW;
						break;
					case TRANSLATE_VIEW:
						T = glm::translate(mat4(), vec3(0, -(curMousePos.x - prevMousePos.x) * FACTOR, 0));
						VIEW = T * VIEW;
						break;
					case PERSPECTIVE:
						NP -= (curMousePos.x - prevMousePos.x) * FACTOR;
						PROJ = getProj(FOV, ASPECT, FP, NP);
						break;
					case ROTATE_MODEL:
						R = glm::rotate(mat4(), (float) theta, vec3(0, 1, 0));
						MODEL = MODEL * R * glm::inverse(MODEL) * MODEL;
						CMODEL = CMODEL * R * glm::inverse(CMODEL) * CMODEL;
						break;
					case TRANSLATE_MODEL:
						T = glm::translate(mat4(), vec3(0, (curMousePos.x - prevMousePos.x) * FACTOR, 0));
						MODEL = T * MODEL;
						CMODEL = T * CMODEL;
						break;
					case SCALE_MODEL:
						if (curMousePos.x > prevMousePos.x && scaley * SCALE_UP < SCALE_MAX_LIMIT) {
							S = glm::scale(mat4(), vec3(1, SCALE_UP, 1));
							scaley *= SCALE_UP;
						}
						if (curMousePos.x < prevMousePos.x && scaley * SCALE_DOWN > SCALE_MIN_LIMIT) {
							S = glm::scale(mat4(), vec3(1, SCALE_DOWN, 1));
							scaley *= SCALE_DOWN;
						}
						MODEL = MODEL * S * glm::inverse(MODEL) * MODEL;
						break;
					default:break;
				}
			}

			if (mouseRightClicked) {
				switch (mode) {
					case ROTATE_VIEW:
						R = glm::rotate(mat4(), (float) -theta, vec3(0, 0, 1));
						VIEW = VIEW * R * glm::inverse(VIEW) * VIEW;
						break;
					case TRANSLATE_VIEW:
						T = glm::translate(mat4(), vec3(0, 0, -(curMousePos.x - prevMousePos.x) * FACTOR));
						VIEW = T * VIEW;
						break;
					case PERSPECTIVE:
						FP -= (curMousePos.x - prevMousePos.x) * FACTOR;
						PROJ = getProj(FOV, ASPECT, FP, NP);
						break;
					case ROTATE_MODEL:
						R = glm::rotate(mat4(), (float) theta, vec3(0, 0, 1));
						MODEL = MODEL * R * glm::inverse(MODEL) * MODEL;
						CMODEL = CMODEL * R * glm::inverse(CMODEL) * CMODEL;
						break;
					case TRANSLATE_MODEL:
						T = glm::translate(mat4(), vec3(0, 0, (curMousePos.x - prevMousePos.x) * FACTOR));
						MODEL = T * MODEL;
						CMODEL = T * CMODEL;
						break;
					case SCALE_MODEL:
						if (curMousePos.x > prevMousePos.x && scalez * SCALE_UP < SCALE_MAX_LIMIT) {
							S = glm::scale(mat4(), vec3(1, 1, SCALE_UP));
							scalez *= SCALE_UP;
						}
						if (curMousePos.x < prevMousePos.x && scalez * SCALE_DOWN > SCALE_MIN_LIMIT) {
							S = glm::scale(mat4(), vec3(1, 1, SCALE_DOWN));
							scalez *= SCALE_DOWN;
						}
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
			prevMousePos = glm::vec4((float) xpos, (float) ypos, 0.0f, 1.0f);
			vpStart = glm::vec4((float) xpos, (float) ypos, 0.0f, 1.0f);
			if (button == GLFW_MOUSE_BUTTON_LEFT) mouseLeftClicked = true;
			if (button == GLFW_MOUSE_BUTTON_MIDDLE) mouseMiddleClicked = true;
			if (button == GLFW_MOUSE_BUTTON_RIGHT) mouseRightClicked = true;
		}
		if (actions == GLFW_RELEASE) {
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				mouseLeftClicked = false;
				if (mode == VIEWPORT) VP = getViewport(vpStart, vpWidth, vpHeight, FP, NP);
			}
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

	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_Q:
				glfwSetWindowShouldClose(m_window, GL_TRUE);
				break;
			case GLFW_KEY_A:
				reset();
				break;
			case GLFW_KEY_O:
				mode = ROTATE_VIEW;
				break;
			case GLFW_KEY_N:
				mode = TRANSLATE_VIEW;
				break;
			case GLFW_KEY_P:
				mode = PERSPECTIVE;
				break;
			case GLFW_KEY_R:
				mode = ROTATE_MODEL;
				break;
			case GLFW_KEY_T:
				mode = TRANSLATE_MODEL;
				break;
			case GLFW_KEY_S:
				mode = SCALE_MODEL;
				break;
			case GLFW_KEY_V:
				mode = VIEWPORT;
				break;
		}
	}

	return eventHandled;
}
