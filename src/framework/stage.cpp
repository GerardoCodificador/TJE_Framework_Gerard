
#include "framework/stage.h"
#include <fstream>
#include <cmath>
#include <algorithm>
#include "graphics/render_to_texture.h"
#include "framework/input.h"
#include "framework/pulse.h"
#include "game/game.h"
#include "extra/bass.h"
HSAMPLE hSample;	// Handler to store one sample	
HCHANNEL hSampleChannel; // Handler to store one channel
float angle = 0;
Mesh* mesh = NULL;
Texture* texture = NULL;
Shader* shader = NULL;
bool RENDERCOLISIONS = false;
float mouse_speed = 1.0f;
EntityMesh* skybox = nullptr;
bool stagechange = false;
int night = 0;
void GameDayStage::Init() {
	
	font.loadTGA("data/bitmap-font-black.tga"); //load bitmap-font image
	minifont.loadTGA("data/mini-font-black-4x6.tga"); //load bitmap-font image
	// Create our camera
	World::DayMap = new World();
	World::DayMap->Init("data/sceneDay.scene");
	mouse_locked = false;
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
	// OpenGL flags
	glEnable(GL_CULL_FACE); //render both sides of every triangle
	glEnable(GL_DEPTH_TEST); //check the occlusions using the Z buffer
	// Hide the cursor
	Texture* tex = Texture::Get("data/textures/personaje.tga");
	NPC *npc = new NPC(tex, "Julia", vec3(1.5, 0.25, 1.5));

	World::DayMap->root->addChild(npc);


}
void GameDayStage::OnEnter(Stage* last_stage) {
	//init game stage

	World::player->canrender = true;
	Game::instance->setMouseLocked(true);

	World::player->MoveTo(vec3(0.0));
}
void GameDayStage::OnExit(Stage* last_stage) {
	//cleanup game stage

	World::player->canrender = false;
}



void GameDayStage::Update(float seconds_elapsed,Camera& camera) {
	if(!Game::instance->ask_to_change){
		World::DayMap->Update(seconds_elapsed, camera);

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
		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) World::DayMap->player->walk_speed *= 10; //move faster with left shift
	
	}
	else {
		if (Input::isKeyPressed(SDL_SCANCODE_Y)) {
			Game::instance->answer = 1;

			Game::instance->ask_to_change = false;
		}
		if (Input::isKeyPressed(SDL_SCANCODE_N)) {
			Game::instance->answer = 0;

			Game::instance->ask_to_change = false;
		}

	}
	//update game stage
	

}
bool show_map=false;
void GameDayStage::Render(Camera& camera) {
	World::DayMap->Render(camera);
	
	//render game stage
}

bool GameDayStage::onKeyDown(SDL_KeyboardEvent event) {
	World::DayMap->onKeyDown(event);
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: Game::instance->setStage(eStage::STAGE_MENU); break; //ESC key, kill the app
	case SDLK_F1: Shader::ReloadAll(); break;

	case SDLK_F2: RENDERCOLISIONS = !RENDERCOLISIONS; break;
	case SDLK_SPACE:
		pulse.active = !pulse.active;
		break;
	case SDLK_f: stagechange = !stagechange; break;

	case SDLK_RSHIFT:Game::instance->mouse_glocked = !Game::instance->mouse_glocked; Game::instance->setMouseLocked(Game::instance->mouse_glocked); break;
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
	if (BASS_Init(-1, 44100, 0, 0, NULL) == false) {
		// Error with sound device
	}

	hSample = BASS_SampleLoad(
		false,  			// From internal memory
		"data/sounds/breathing.wav", 	// Filepath
		0,				// Offset
		0,				// Length
		3,				// Max playbacks
		BASS_SAMPLE_LOOP    				// Flags
	);

	hSampleChannel = BASS_SampleGetChannel(hSample, false);
	
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
	Mesh* mesh = Mesh::Get("data/Item/item.obj");
	Material mat;
	mat.color = Vector4(0.8, 0.6, 0.0, 1);
	items = new Item(mesh, mat);
	items->isInstanced = true;
	Matrix44 matrix;
	std::vector<BoundingBox> colliders;
	for (int i = 0; i < 1; i++) {
		matrix.setTranslation(vec3(1, 0.1, 1));
		matrix.scale(vec3(0.2));
		items->models.push_back(matrix);
		items->active.push_back(true);
		colliders.push_back(transformBoundingBox(matrix, mesh->box));
	}
	items->collider->colliders = colliders;
	World::NightMap->root->addChild(items);
	Mesh* mesh2 = Mesh::Get("data/Rat/Cube.001/Cube.001.obj");
	mat.color = Vector4(1, 1, 1, 1);
	mat.diffuse = Texture::Get("data/textures/rat.tga");
	World::player = new Player(mesh2, mat);
	Matrix44 model;
	model.setTranslation(0.0f, 0.0f, 0.0f);   // posición inicial
	model.scale(0.1f, 0.1f, 0.1f);
	World::player->model = model;
	World::player->canrender = false;
	
}
void GameNightStage::OnEnter(Stage* last_stage) {
	//init menu stage

	World::player->canrender = true;
	BASS_ChannelPlay(hSampleChannel, false);

	Game::instance->setMouseLocked(true);
	World::player->MoveTo(vec3(0.0));
	World::player->transport_to_spawn=true;
	night++;
	std::cout<< std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "Night " <<night<< std::endl;
	if (night == 1) {
		std::vector<Inventory> loot;
		loot.push_back({ 0,0 });

		loot.push_back({ 0,1 });
		for (int i = 0; i < World::NightMap->root->children.size(); i++) {
			if (World::isItem(World::NightMap->root->children[i])) {
				Item *entity = static_cast<Item*>(World::NightMap->root->children[i]);
				entity->active.clear();
				entity->loot.clear();
				int actual = -1;
				for (int i = 0; i < entity->models.size()-1; i++) {
					entity->active.push_back(true);
					actual = (actual < 0) ? (static_cast<int>(Game::instance->time) * 20) % 2 : (actual + 1) % 2;
					entity->loot.push_back(loot[actual]);
				}
				entity->active.push_back(true);
				entity->loot.push_back({ 1,0 });
			}
		}
	}
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

	std::cout << "I need to explore this house for food" << std::endl;
}
void GameNightStage::OnExit(Stage* last_stage) {

	//cleanup menu stage
	World::player->canrender = false;
}
void GameNightStage::Update(float deltaTime,Camera& camera) {
	//update menu stage
	if(!Game::instance->ask_to_change){
		World::NightMap->Update(deltaTime, camera);
	
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
		// Play channel
	}
	else {
		if (Input::isKeyPressed(SDL_SCANCODE_Y)) {
			std::cout << "Returning Home..." << std::endl;
			World::player->max_stun = 2.0;
			World::player->actual_stun = 2.0;
			World::player->is_blackoff = true;
			Game::instance->setStage(STAGE_GAMEDAY);
			World::player->canmove = false;
		}
		if (Input::isKeyPressed(SDL_SCANCODE_N)) {

			std::cout << "I will explore a little more" << std::endl;
			Game::instance->ask_to_change = false;
		}
	}

}
void GameNightStage::Render(Camera& camera) {
	
	
	World::NightMap->Render(camera);

	

}
bool GameNightStage::onKeyDown(SDL_KeyboardEvent event) {
	World::NightMap->onKeyDown(event);
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: Game::instance->setStage(eStage::STAGE_MENU); break; //ESC key, kill the app
	case SDLK_F1: Shader::ReloadAll(); break;
	case SDLK_F2: RENDERCOLISIONS=!RENDERCOLISIONS; break;
	case SDLK_SPACE:
		pulse.active = !pulse.active;
		break;
	case SDLK_f: stagechange = !stagechange; break;

	case SDLK_RSHIFT:Game::instance->mouse_glocked = !Game::instance->mouse_glocked; Game::instance->setMouseLocked(Game::instance->mouse_glocked); break;
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
	Material mat = Material();
	mat.diffuse = Texture::Get("data/textures/play.tga");
	mat.shader = Shader::Get("data/shaders/HUD.vs", "data/shaders/HUD.fs");
	Vector2 size = Vector2(0.2 * Game::instance->window_width * Game::instance->window_width/ Game::instance->window_height, 0.2 * Game::instance->window_height* Game::instance->window_height/ Game::instance->window_width);
	play = new Button(Vector2(
		Game::instance->window_width/2 ,
		Game::instance->window_height/2 - size.y / 2
	), size, mat);
	mat.diffuse = Texture::Get("data/textures/portada.tga");
	size = Vector2( Game::instance->window_width ,Game::instance->window_height );
	Portada = new Button(Vector2(
		Game::instance->window_width / 2,
		Game::instance->window_height / 2 
	), size, mat);

}
void MenuStage::OnEnter(Stage* last_stage) {
	//init menu stage

	Game::instance->setMouseLocked(false);
}
void MenuStage::OnExit(Stage* last_stage) {

	//cleanup menu stage
}
void MenuStage::Update(float deltaTime,Camera &Camera) {
	//update menu stage
	play->update(deltaTime, Camera);
	if (play->state == Clicked)Game::instance->setStage(STAGE_GAMENIGHT);
}
void MenuStage::Render(Camera& camera) {
	Portada->render(&camera);
	play->render(&camera);
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
void GameOver::Init() {
	//init menu stage
	background.loadTGA("data/assets/mountains.tga");

	font.loadTGA("data/bitmap-font-black.tga"); //load bitmap-font image
	minifontb.loadTGA("data/mini-font-black-4x6.tga"); //load bitmap-font image
	minifontw.loadTGA("data/mini-font-white-4x6.tga");
	Material mat =  Material();
	mat.diffuse = Texture::Get("data/textures/play.tga");
	Vector2 size = Vector2(0.5 * Game::instance->window_width * mat.diffuse->width / mat.diffuse->height, 0.5 * Game::instance->window_height * mat.diffuse->height / mat.diffuse->width);
	play=new Button(Vector2(
		Game::instance->window_width - size.x / 2,
		Game::instance->window_height - size.y / 2
	), size, mat);
	
}
void GameOver::OnEnter(Stage* last_stage) {
	//init menu stage
	BASS_ChannelStop(hSampleChannel);
	Game::instance->setMouseLocked(false);
}
void GameOver::OnExit(Stage* last_stage) {

	//cleanup menu stage
}
void GameOver::Update(float deltaTime, Camera& Camera) {
	//update menu stage
	play->update(deltaTime, Camera);
}
void GameOver::Render(Camera& camera) {
	int text_size = 5; 
	int text_width = strlen("Game Over") * text_size*6;
	play->render(&camera);
	drawText(
		Game::instance->window_width / 2 - text_width / 2,
		Game::instance->window_height / 2,
		"Game Over",
		Vector3(1, 1, 1),
		text_size
	);
}
bool GameOver::onKeyDown(SDL_KeyboardEvent event) {
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
		Game::instance->setStage(MenuOption == 0 ? eStage::STAGE_GAMEDAY : eStage::STAGE_GAMENIGHT);
	}
	if (event.keysym.scancode == SDL_SCANCODE_ESCAPE) {
		Game::instance->must_exit = true;
	}
	return true;
}