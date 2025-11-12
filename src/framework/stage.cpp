
#include "framework/stage.h"
#include <fstream>
#include <cmath>
#include <algorithm>

#include "framework/input.h"
#include "framework/pulse.h"
#include "game/game.h"
float angle = 0;
Mesh* mesh = NULL;
Texture* texture = NULL;
Shader* shader = NULL;

float mouse_speed = 1.0f;
EntityMesh* skybox = nullptr;
float timer = 10;
float inittime = 10;
bool stagechange = false;
void GameDayStage::Init() {
	
	font.loadTGA("data/bitmap-font-black.tga"); //load bitmap-font image
	minifont.loadTGA("data/mini-font-black-4x6.tga"); //load bitmap-font image
	// Create our camera
	World::DayMap = new World();
	World::DayMap->Init("data / myscene.scene");
	mouse_locked = false;
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
	// OpenGL flags
	glEnable(GL_CULL_FACE); //render both sides of every triangle
	glEnable(GL_DEPTH_TEST); //check the occlusions using the Z buffer
	// Hide the cursor
	
}
void GameDayStage::OnEnter(Stage* last_stage) {
	//init game stage
	
	timer = inittime;
}
void GameDayStage::OnExit(Stage* last_stage) {
	//cleanup game stage
}



void GameDayStage::Update(float seconds_elapsed,Camera& camera) {
	World::DayMap->Update(seconds_elapsed, camera);
	if(stagechange)timer -= seconds_elapsed;
	if (timer <= 0) {
		Game::instance->setStage(eStage::STAGE_GAMENIGHT);
	}
	float speed = seconds_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

	// Example
	angle += (float)seconds_elapsed * 10.0f;

	// Mouse input to rotate the cam
	if (Input::isMousePressed(SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
	{
		camera.rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
		camera.rotate(Input::mouse_delta.y * 0.005f, camera.getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
	}

	// Async input to move the camera around
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
	
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE)) {
		pulse.active = !pulse.active;

		pulse.center = camera.eye;
	}
	//update game stage
}
bool show_map=false;
void GameDayStage::Render(Camera& camera) {
	World::DayMap->Render(camera);
	drawText(2, 20, std::to_string(timer), Vector3(1, 1, 1), 2);
	//render game stage
}

bool GameDayStage::onKeyDown(SDL_KeyboardEvent event) {
	World::DayMap->onKeyDown(event);
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: Game::instance->setStage(eStage::STAGE_MENU); break; //ESC key, kill the app
	case SDLK_F1: Shader::ReloadAll(); break;
	case SDLK_SPACE:
		pulse.active = !pulse.active;
		break;
	case SDLK_e: stagechange = !stagechange; break;
	//case SDLK_LSHIFT:Game::instance->mouse_glocked = !Game::instance->mouse_glocked;
		//break;
	}
	return true;
}

bool GameDayStage::onMouseWheel(SDL_MouseWheelEvent event) {
	World::DayMap->onMouseWheel(event);
	mouse_speed *= event.y > 0 ? 1.1f : 0.9f;
	return true;
}
int MenuOption = 0;
void GameNightStage::Init() {
	//init menu stage

	font.loadTGA("data/bitmap-font-black.tga"); //load bitmap-font image
	minifont.loadTGA("data/mini-font-black-4x6.tga"); //load bitmap-font image
	// Create our camera
	
	World::NightMap = new World();
	World::NightMap->Init("data/sceneNight.scene");
	
	mouse_locked = false;
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
	// OpenGL flags
	glEnable(GL_CULL_FACE); //render both sides of every triangle
	glEnable(GL_DEPTH_TEST); //check the occlusions using the Z buffer
	// Hide the cursor
}
void GameNightStage::OnEnter(Stage* last_stage) {
	//init menu stage
	timer = inittime;
}
void GameNightStage::OnExit(Stage* last_stage) {

	//cleanup menu stage
}
void GameNightStage::Update(float deltaTime,Camera& camera) {
	//update menu stage
	World::NightMap->Update(deltaTime, camera);
	if (stagechange)timer -= deltaTime;
	if (timer <= 0) {
		Game::instance->setStage(eStage::STAGE_GAMEDAY);
	}
	float speed = deltaTime * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

	// Example
	angle += (float)deltaTime * 10.0f;

	// Mouse input to rotate the cam
	if (Input::isMousePressed(SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
	{
		camera.rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
		camera.rotate(Input::mouse_delta.y * 0.005f, camera.getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
	}

	// Async input to move the camera around
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift

}
void GameNightStage::Render(Camera& camera) {
	World::NightMap->Render(camera);
	drawText(2, 20, std::to_string(timer), Vector3(1, 1, 1), 2);
}
bool GameNightStage::onKeyDown(SDL_KeyboardEvent event) {
	World::NightMap->onKeyDown(event);
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: Game::instance->setStage(eStage::STAGE_MENU); break; //ESC key, kill the app
	case SDLK_F1: Shader::ReloadAll(); break;
	case SDLK_SPACE:
		pulse.active = !pulse.active;
		break;
	case SDLK_e: stagechange = !stagechange; break;
	}
	
	return true;
}
bool GameNightStage::onMouseWheel(SDL_MouseWheelEvent event) {
	World::NightMap->onMouseWheel(event);
	mouse_speed *= event.y > 0 ? 1.1f : 0.9f;
	return true;
}

void MenuStage::Init() {
	//init menu stage
	background.loadTGA("data/assets/mountains.tga");

	font.loadTGA("data/bitmap-font-black.tga"); //load bitmap-font image
	minifontb.loadTGA("data/mini-font-black-4x6.tga"); //load bitmap-font image
	minifontw.loadTGA("data/mini-font-white-4x6.tga");
}
void MenuStage::OnEnter(Stage* last_stage) {
	//init menu stage
}
void MenuStage::OnExit(Stage* last_stage) {

	//cleanup menu stage
}
void MenuStage::Update(float deltaTime,Camera &Camera) {
	//update menu stage
}
void MenuStage::Render(Camera& camera) {
	drawText(20, 30,"FROZEN LAND", Vector3(1,1,1),4.0);
	drawText( 60, 80, "Day Game", MenuOption == 0 ?Vector3(1,0,0) : Vector3(1,  1,1), 2);
	drawText( 60, 100, "Night Game", MenuOption == 1 ? Vector3(1, 0, 0) : Vector3(1, 1, 1), 2);
}
bool MenuStage::onKeyDown(SDL_KeyboardEvent event) {
	const float DEAD_ZONE = 0.2f;
	GamepadState& pad = Input::gamepads[0];
	if (Input::isKeyPressed(SDL_SCANCODE_UP) || pad.axis[LEFT_ANALOG_Y] < -DEAD_ZONE ||
		(pad.hat & PAD_UP)) //if key up
	{
		MenuOption = (MenuOption + 1) % 2;
	}
	else if (Input::isKeyPressed(SDL_SCANCODE_DOWN) || pad.axis[LEFT_ANALOG_Y] > DEAD_ZONE ||
		(pad.hat & PAD_DOWN)) //if key up
	{
		MenuOption = (MenuOption + 1) % 2;
	}
	else if (Input::wasKeyPressed(SDL_SCANCODE_Z) || Input::wasButtonPressed(A_BUTTON)) //if key Z was pressed
	{
		Game::instance->setStage(MenuOption==0?eStage::STAGE_GAMEDAY:eStage::STAGE_GAMENIGHT);
	}
	if (event.keysym.scancode == SDL_SCANCODE_ESCAPE) {
		Game::instance->must_exit = true;
	}
	return true;
}