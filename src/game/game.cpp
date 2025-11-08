#include "game.h"
#include "framework/utils.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/fbo.h"
#include "graphics/shader.h"
#include "framework/input.h"
#include "framework/entities/entity.h"
#include "framework/scene_parser.h"

#include <cmath>
#include "framework/pulse.h"
#include "framework/stage.h"

//some globals

float mouse_gspeed = 1.0f;
bool mouse_glocked;
eStage currentStage = eStage::STAGE_MENU;
std::map<eStage, Stage*> stages;
Game* Game::instance = NULL;
EntityMesh* ent=nullptr;
Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	
	camera = new Camera();
	camera->lookAt(Vector3(0.f, 1.f, 1.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective
	stages[eStage::STAGE_MENU] = new MenuStage();

	stages[eStage::STAGE_GAMEDAY] = new GameDayStage();

	stages[eStage::STAGE_GAMENIGHT] = new GameNightStage();
	stages[eStage::STAGE_GAMEDAY]->Init();
	stages[eStage::STAGE_MENU]->Init();
	stages[eStage::STAGE_GAMENIGHT]->Init();
	mouse_glocked = false;
	SDL_ShowCursor(!mouse_glocked);

	
}

//what to do when the image has to be draw
void Game::render(void)
{
	// Set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the camera as default
	camera->enable();
	// Set flags


	// Create model matrix for cube
	stages[currentStage]->Render(*camera);

	// Draw the floor grid
	drawGrid();

	// Render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	// Swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	stages[currentStage]->mouse_locked = mouse_glocked;
	stages[currentStage]->Update(seconds_elapsed, *camera);

}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	stages[currentStage]->onKeyDown(event);
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{

}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_glocked = !mouse_glocked;
		SDL_ShowCursor(!mouse_glocked);
		SDL_SetRelativeMouseMode((SDL_bool)(mouse_glocked));
	}
}
void Game::setStage(eStage nextstage) {

	stages[currentStage]->OnExit(stages[nextstage]);
	stages[nextstage]->OnEnter(stages[currentStage]);

	currentStage = nextstage;
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{

}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{

	stages[currentStage]->onMouseWheel(event);
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

void Game::setMouseLocked(bool must_lock)
{
	SDL_ShowCursor(!must_lock);

	SDL_SetRelativeMouseMode((SDL_bool)must_lock);

	mouse_glocked = must_lock;
}