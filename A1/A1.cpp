#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <cmath>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

static const size_t DIM = 16;
static const double SCALE_MIN_LIMIT = 0.1;
static double SCALE_MAX_LIMIT = 10;
static double SCALE_UP = 1.5f;
static double SCALE_DOWN = (double) 1 / SCALE_UP;
static double scales = 1;

void A1::init_colours() {
	// black
	colours[0][0] = 0.0f;
	colours[0][1] = 0.0f;
	colours[0][2] = 0.0f;

	// red
	colours[1][0] = 1.0f;
	colours[1][1] = 0.0f;
	colours[1][2] = 0.0f;

	// green
	colours[2][0] = 0.0f;
	colours[2][1] = 1.0f;
	colours[2][2] = 0.0f;

	// blue
	colours[3][0] = 0.0f;
	colours[3][1] = 0.0f;
	colours[3][2] = 1.0f;

	// yellow
	colours[4][0] = 1.0f;
	colours[4][1] = 1.0f;
	colours[4][2] = 0.0f;

	// 
	colours[5][0] = 1.0f;
	colours[5][1] = 0.0f;
	colours[5][2] = 1.0f;

	// 
	colours[6][0] = 0.0f;
	colours[6][1] = 1.0f;
	colours[6][2] = 1.0f;

	// white
	colours[7][0] = 1.0f;
	colours[7][1] = 1.0f;
	colours[7][2] = 1.0f;	
}

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 )
{
	init_colours();
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1(){
	delete grid;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );

	grid = new Grid(DIM);
	active_cell = {0, 0, 0};
	mouse_left_clicked = false;
	mouse_right_clicked = false;
	T = glm::translate(mat4(), vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );

	initGrid();

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt( 
		glm::vec3( 0.0f, float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );
	proj = glm::perspective( 
		glm::radians( 45.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A1::reset() {
	T = glm::translate(mat4(), vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );
	grid->reset();
	active_cell = {0, 0, 0};
	mouse_left_clicked = false;
	init_colours();
	scales = 1;
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.
		ImGui::SameLine();
		if (ImGui::Button("Reset")) {
			reset();
		}

		ImGui::PushID( 0 );
		ImGui::ColorEdit3( "##Colour", colours[0]);
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 0 ) ) {
			grid->setColour(active_cell.x, active_cell.z, current_col);
		}
		ImGui::PopID();

		ImGui::PushID( 1 );
		ImGui::ColorEdit3( "##Colour", colours[1]);
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 1 ) ) {
			grid->setColour(active_cell.x, active_cell.z, current_col);
		}
		ImGui::PopID();

		ImGui::PushID( 2 );
		ImGui::ColorEdit3( "##Colour", colours[2]);
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 2 ) ) {
			grid->setColour(active_cell.x, active_cell.z, current_col);
		}
		ImGui::PopID();

		ImGui::PushID( 3 );
		ImGui::ColorEdit3( "##Colour", colours[3]);
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 3 ) ) {
			grid->setColour(active_cell.x, active_cell.z, current_col);
		}
		ImGui::PopID();		

		ImGui::PushID( 4 );
		ImGui::ColorEdit3( "##Colour", colours[4]);
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 4 ) ) {
			grid->setColour(active_cell.x, active_cell.z, current_col);
		}
		ImGui::PopID();

		ImGui::PushID( 5 );
		ImGui::ColorEdit3( "##Colour", colours[5]);
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 5 ) ) {
			grid->setColour(active_cell.x, active_cell.z, current_col);
		}
		ImGui::PopID();

		ImGui::PushID( 6 );
		ImGui::ColorEdit3( "##Colour", colours[6]);
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 6 ) ) {
			grid->setColour(active_cell.x, active_cell.z, current_col);
		}
		ImGui::PopID();

		ImGui::PushID( 7 );
		ImGui::ColorEdit3( "##Colour", colours[7]);
		ImGui::SameLine();
		if( ImGui::RadioButton( "##Col", &current_col, 7 ) ) {
			grid->setColour(active_cell.x, active_cell.z, current_col);
		}
		ImGui::PopID();
/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}
*/

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

void A1::drawCube(float dx, float dy, float dz, int colour) {
	int CUBE_SIZE = 24;

	float *cube_verts = new float[CUBE_SIZE];
	int n = 0;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				cube_verts[n++] = i + dx;
				cube_verts[n++] = j + dy;
				cube_verts[n++] = k + dz;
			}
		}
	}

	GLuint elements[] = {
		0, 4, 5,
		5, 1, 0,
		2, 6, 7,
		7, 3, 2,
		3, 2, 0,
		0, 1, 3,
		7, 6, 4,
		4, 5, 7,
		2, 6, 4,
		4, 0, 2,
		3, 7, 5,
		5, 1, 3
	};

	glGenVertexArrays(1, &m_cube_vao);
	glUniform3f(col_uni, colours[colour][0], colours[colour][1], colours[colour][2]);
	glBindVertexArray(m_cube_vao);

	glGenBuffers(1, &m_cube_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, CUBE_SIZE*sizeof(float), cube_verts, GL_STATIC_DRAW);

	glGenBuffers(1, &m_cube_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cube_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	GLint posAttrib = m_shader.getAttribLocation("position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// Cleanup
	glDeleteBuffers(1, &m_cube_vbo);
	glDeleteBuffers(1, &m_cube_ebo);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete [] cube_verts;
}

void A1::drawRomb(float dx, float dy, float dz) {
	glDisable(GL_DEPTH_TEST);
	float x_verts[] = {
		0.5f+dx, 0.0f+dy, 0.0f+dz,
		1.0f+dx, 0.0f+dy, 0.5f+dz,
		0.5f+dx, 0.0f+dy, 1.0f+dz,
		0.0f+dx, 0.0f+dy, 0.5f+dz
	};

	GLuint xelements[] = {
		0, 1, 2,
		2, 3, 0
	};

	glGenVertexArrays(1, &m_cube_vao);
	glUniform3f(col_uni, 0.3, 0.3, 0.3);
	glBindVertexArray(m_cube_vao);

	glGenBuffers(1, &m_cube_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(x_verts), x_verts, GL_STATIC_DRAW);

	glGenBuffers(1, &m_cube_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cube_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(xelements), xelements, GL_STATIC_DRAW);

	GLint posAttrib = m_shader.getAttribLocation("position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glEnable(GL_DEPTH_TEST);

    // Cleanup
	glDeleteBuffers(1, &m_cube_vbo);
	glDeleteBuffers(1, &m_cube_ebo);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw() {

	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( T ) );

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// Draw the cubes
		// Highlight the active square.
		for (int i = 0; i < grid->getDim(); i++) {
			for (int k = 0; k < grid->getDim(); k++) {
				for (int h = 0; h < grid->getHeight(i, k); h++) {
					drawCube(i, h, k, grid->getColour(i, k));
				}
			}
		}

		drawRomb(active_cell.x, active_cell.y, active_cell.z);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}

// finds the angle between two vectors from origin
double findTheta(point center, point p1, point p2) {
    
    if (p1.x - center.x == 0 || p2.x - center.x == 0) return 0;

    double s1 = (p1.z - center.z) / (p1.x - center.x);
    double s2 = (p2.z - center.z) / (p2.x - center.x);

    double theta = atan((s2-s1)/(1+s2*s1));
    return -theta;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
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
bool A1::mouseMoveEvent(double xPos, double yPos) 
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so 
		// that you can rotate relative to the *change* in X.
		if (mouse_left_clicked || mouse_right_clicked) {
			int width, height;
			glfwGetWindowSize(m_window, &width, &height);
			point center = {(float)width / 2.0f, 0, (float)height / 2.0f};
			point cur_mouse_pos = {(float)xPos, 0, (float)yPos};
			double theta = findTheta(center, old_mouse_pos, cur_mouse_pos);
			old_mouse_pos = cur_mouse_pos;
			glm::mat4 R;
			if (mouse_left_clicked) R = glm::rotate(mat4(), (float) theta, vec3(0, 1, 0));
			if (mouse_right_clicked) R = glm::rotate(mat4(), (float) theta, vec3(1, 0, 0));
			T = R * T;
		}
	}
	eventHandled = true;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if (actions == GLFW_PRESS) {
			// The user clicked in the window.  If it's the left
			// mouse button, initiate a rotation.
			double xpos, ypos;
			glfwGetCursorPos(m_window, &xpos, &ypos);
			old_mouse_pos = {(float) xpos, 0, (float) ypos};
			if (button == GLFW_MOUSE_BUTTON_LEFT) mouse_left_clicked = true;
			if (button == GLFW_MOUSE_BUTTON_RIGHT) mouse_right_clicked = true;
		}
		if (actions == GLFW_RELEASE) {
			mouse_left_clicked = false;
			mouse_right_clicked = false;
		}
	}
	eventHandled = true;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);
	glm::mat4 S;
	if (yOffSet == 1 && (scales * SCALE_UP < SCALE_MAX_LIMIT)) {
		scales *= SCALE_UP;
		S = glm::scale(mat4(), vec3(SCALE_UP));
	}

	if (yOffSet == -1 && (scales * SCALE_DOWN > SCALE_MIN_LIMIT)) {
		scales *= SCALE_DOWN;
		S = glm::scale(mat4(), vec3(SCALE_DOWN));
	}
	T = S * T;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		if (key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			
		}

		if (key == GLFW_KEY_R) {
			reset();
		}

		if (key == GLFW_KEY_RIGHT) {

			if (active_cell.x + 1 < DIM) {
				if (mods == GLFW_MOD_SHIFT) {
					grid->setHeight(active_cell.x + 1, active_cell.z, grid->getHeight(active_cell.x, active_cell.z));
					grid->setColour(active_cell.x + 1, active_cell.z, grid->getColour(active_cell.x, active_cell.z));
				}
				active_cell.x += 1;
				active_cell.y = grid->getHeight(active_cell.x, active_cell.z);
			}
		}

		if (key == GLFW_KEY_LEFT) {
			if (active_cell.x - 1 >= 0) {
				if (mods == GLFW_MOD_SHIFT) {
					grid->setHeight(active_cell.x - 1, active_cell.z, grid->getHeight(active_cell.x, active_cell.z));
					grid->setColour(active_cell.x - 1, active_cell.z, grid->getColour(active_cell.x, active_cell.z));
				}
				active_cell.x -= 1;
				active_cell.y = grid->getHeight(active_cell.x, active_cell.z);
			}
		}

		if (key == GLFW_KEY_UP) {
			if (active_cell.z - 1 >= 0) {
				if (mods == GLFW_MOD_SHIFT) {
					grid->setHeight(active_cell.x, active_cell.z - 1, grid->getHeight(active_cell.x, active_cell.z));
					grid->setColour(active_cell.x, active_cell.z - 1, grid->getColour(active_cell.x, active_cell.z));
				}
				active_cell.z -= 1;
				active_cell.y = grid->getHeight(active_cell.x, active_cell.z);
			}
		}

		if (key == GLFW_KEY_DOWN) {
			if (active_cell.z + 1 < DIM) {
				if (mods == GLFW_MOD_SHIFT) {
					grid->setHeight(active_cell.x, active_cell.z + 1, grid->getHeight(active_cell.x, active_cell.z));
					grid->setColour(active_cell.x, active_cell.z + 1, grid->getColour(active_cell.x, active_cell.z));
				}
				active_cell.z += 1;
				active_cell.y = grid->getHeight(active_cell.x, active_cell.z);
			}
		}

		if (key == GLFW_KEY_SPACE) {
			grid->setHeight(active_cell.x, active_cell.z, grid->getHeight(active_cell.x, active_cell.z) + 1);
			grid->setColour(active_cell.x, active_cell.z, current_col);
			active_cell.y = grid->getHeight(active_cell.x, active_cell.z);
		}

		if (key == GLFW_KEY_BACKSPACE) {
			int height = grid->getHeight(active_cell.x, active_cell.z);
			grid->setHeight(active_cell.x, active_cell.z, height > 0 ? height - 1 : 0);
			active_cell.y = grid->getHeight(active_cell.x, active_cell.z);
		}

		eventHandled = true;
	}

	return eventHandled;
}
