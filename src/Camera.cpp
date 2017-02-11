#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>

#include "Camera.hpp"


using namespace glm;


glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 Camera::getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 Camera::getProjectionMatrix(){
	return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 5 ); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 60.0f;

float speed = 0.005f; // 0.005 units / second
float mouseSpeed = 0.005f;
glm::vec2 mousePosition;

int lastTime = 0;

void Camera::computeMatricesFromInputs(){

	// Compute time difference between current and last frame
	const int currentTime = SDL_GetTicks();
	float deltaTime = float(currentTime - lastTime);
	lastTime = currentTime;
	// Get mouse position
	int xpos, ypos;
	if (SDL_GetMouseState(&xpos, &ypos) & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
		int diffX = mousePosition.x - xpos;
		int diffY = mousePosition.y - ypos;
		horizontalAngle    += diffX *.005f;
		verticalAngle  += diffY *.005f;
	}
	mousePosition = glm::vec2(xpos, ypos);


	// Reset mouse position for next frame
	//glfwSetCursorPos(window, 1024/2, 768/2);


	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );


	const Uint8 *state = SDL_GetKeyboardState(NULL);

    // Move forward
	if (state[SDL_SCANCODE_W]){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (state[SDL_SCANCODE_S]){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (state[SDL_SCANCODE_D]){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (state[SDL_SCANCODE_A]){
		position -= right * deltaTime * speed;
	}





	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(75.f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

