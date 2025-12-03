#include "framework/entities/player.h"
int i_saved;
float actualy;



Player::Player(Mesh* m, Material mat) {
	mesh = m;
	material = new Material(mat);
	collider = new EntityCollider(mesh, *material);
	collider->layer = PLAYER;
	collider->model = model;
	collider->is_dynamic = true;
	collider->collider = transformBoundingBox(model, mesh->box);
	collider->type = SPHERE;
	inventory = { 0,0 };
	this->addChild(collider);
	Material ma;
	ma.color = Vector4(1, 1, 1, 1);
	ma.diffuse = Texture::Get("data/textures/HandLamp.tga");
	ma.shader = Shader::Get("data/shaders/HUD.vs", "data/shaders/HUD.fs"); // ✔ PRIMERO shader

	LampMat = ma;

	Vector2 size = Vector2(1* Game::instance->window_width*ma.diffuse->width/ ma.diffuse->height, 0.5 * Game::instance->window_height * ma.diffuse->height / ma.diffuse->width);
	playerUI.push_back(new EntityUI(Vector2(
		Game::instance->window_width - size.x/2,
		size.y/2
	), size, ma));
	ma.color = Vector4(1, 1, 1, 1);

	ma.diffuse = Texture::Get("data/textures/tecla_E.tga");
	ma.shader = Shader::Get("data/shaders/HUD.vs", "data/shaders/HUD.fs"); // ✔ PRIMERO shader

	size = Vector2(0.1 * Game::instance->window_width * ma.diffuse->width / ma.diffuse->height, 0.1 * Game::instance->window_height * ma.diffuse->height / ma.diffuse->width);
	playerUI.push_back(new EntityUI(Vector2(
		Game::instance->window_width*2/3 - size.x / 2,
		Game::instance->window_height * 2 / 3-size.y / 2
	), size, ma));
	ma.color = Vector4(0,1);
	ma.diffuse = Texture::Get("data/textures/black.tga");
	ma.shader = Shader::Get("data/shaders/HUD.vs", "data/shaders/HUD.fs"); // ✔ PRIMERO shader


	size = Vector2(3000,3000);
	playerUI.push_back(new EntityUI(Vector2(
		0,0
	), size, ma));
	ma.color = Vector4(0,1,1, 1);
	ma.shader = Shader::Get("data/shaders/HUD.vs", "data/shaders/completioncircle.fs"); // ✔ PRIMERO shader


	size = Vector2(0.2 * Game::instance->window_width , 0.2 * Game::instance->window_height );
	playerUI.push_back(new EntityUI(Vector2(
		Game::instance->window_width * 2 / 3 - size.x / 2,
		Game::instance->window_height * 2 / 3 - size.y / 2
	), size, ma));
}


void Player::onResize(Vector2 prev,Vector2 next) {
	for (EntityUI* UI : playerUI) {

	}
}
void Player::onColisionEnter(Entity* e, sCollisionData collisiondata, eCollisionFilter Type) {
	if (Type == WALL)canmove = false;
	if (Type == ITEM){
		Item* collided = static_cast<Item*>(e);
		int collided_i = static_cast<EntityCollider*>(e->children[0])->closest;
		
	}
	if (Type & INTERACTIVE) {		
		playerUI[1]->canrender=true;
		playerUI[1]->linkedesh=e;
		if (collisiondata.collider->type==BOX) 
			canmove = false;
	}
}
void Player::update(float deltatime,Camera& camera) {

	Vector3 position =
		model.getTranslation();
	vec3 dir = vec3(0.0);
	yaw -= Input::mouse_delta.x * deltatime * camera_speed;
	pitch -= Input::mouse_delta.y * deltatime * camera_speed;
	time = time>M_PI*2?0:time;

	// Restrict pitch (Rads)
	float limitAngle = M_PI * 0.4;
	pitch = clamp(pitch, -limitAngle, limitAngle);

	if (actual_stun > 0) {
		actual_stun -= deltatime;
		canmove = false;
	}
	else {
		is_blackoff = false; 
		actual_stun = 0;
	}
	playerUI[2]->canrender = is_blackoff;

	// Create matrices individually and join them
	Matrix44 mYaw;
	mYaw.setRotation(yaw, Vector3(0, 1, 0));
	Matrix44 mPitch;
	mPitch.setRotation(pitch, Vector3(-1, 0, 0));
	Matrix44 mRotation = mPitch * mYaw;
	pulse.color = vec3(0.0f);
	Vector3 front = mYaw.frontVector().normalize();
	Vector3 right = mYaw.rightVector().normalize();
	bool ismoving = false;
	walk_speed = 0.5f;

	if(Input::isKeyPressed(SDL_SCANCODE_LSHIFT))walk_speed = 1.5f;
	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)){
		position += front * walk_speed * deltatime;
		ismoving = true;
		dir += front;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)){
		position -= front * walk_speed * deltatime;
		ismoving = true;
		dir -= front;
}
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)){
		position += right * walk_speed * deltatime;
		ismoving = true;
		dir += right;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)){
		position -= right * walk_speed * deltatime;
		ismoving = true;
		dir -= right;
	}

	dir=dir.normalize();
	if (ismoving){
		time += 2*M_PI*deltatime*walk_speed;

	}

	else 
		time *= 0.99f;
	actualy = time;

	Vector2 move = Vector2(0, 0.02 * Game::instance->window_height * std::sin(actualy));

	playerUI[0]->Updateposition(move);
	Matrix44 mewModel;
	
	std::vector<sCollisionData> collisions;
	eCollisionFilter filter=WALL;
	


	if (Game::instance->currentStage == eStage::STAGE_GAMEDAY) {

		// Check if collides with wall using sphere (radius = 1)
		EntityCollider* entity;
		vec3 dirtoobj;
		Vector3 objectcenter = position;
		int closest = 0;
		for (int i = 0; i < World::DayMap->root->children.size(); i++) {
			
			for(int j=0;j< World::DayMap->root->children[i]->children.size();j++){
				collisions.clear();
				entity = static_cast<EntityCollider*>(World::DayMap->root->children[i]->children[j]);


				if (entity->TestCollisionSphere(0.2, objectcenter, collisions, PLAYER)) {
					onColisionEnter(entity->parent, collisions[0], entity->layer);

					static_cast<EntityMesh*>(entity->parent)->onColisionEnter(this, collisions[0], PLAYER);


				}
			}
		}
		

	}	else if (Game::instance->currentStage == eStage::STAGE_GAMENIGHT) {


		// Check if collides with wall using sphere (radius = 1)
		EntityCollider* entity;
		vec3 dirtoobj;
		Vector3 objectcenter = position;
		int closest=0;
		for (int i = 0; i < World::NightMap->root->children.size(); i++) {
			for (int j = 0; j < World::NightMap->root->children[i]->children.size(); j++) {

				collisions.clear();
				entity = static_cast<EntityCollider*>(World::NightMap->root->children[i]->children[j]);


				if (entity->TestCollisionSphere(0.2, objectcenter, collisions, PLAYER)) {
					onColisionEnter(entity->parent, collisions[0], entity->layer);

					static_cast<EntityMesh*>(entity->parent)->onColisionEnter(this, collisions[0], PLAYER);


				}
			}
		}
	}
	if
		(canmove) {
		model.setIdentity();
		model.translate(position.x, position.y, position.z);
		model = mRotation * model;
		model.scale(0.1f, 0.1f, 0.1f);
			
	}
	canmove = true;
		
	
	// Update the camera with new eye & center
	//Vector3 eye = model.getTranslation()+Vector3(0,0.3f,0) + Vector3(0, 0.05f, 0) * std::cos(time) ;

	Vector3 eye = model.getTranslation() + Vector3(0, 0.5, 0) ;
	Vector3 center = eye + mRotation.frontVector().normalize() ;
	Vector3 up = Vector3(0, 1, 0);
	camera.lookAt(eye, center, up);
	Entity::update(deltatime, camera);
	pulse.color = vec3(1.0, 0.78, 0.45);
	pulse.radius = 4 ;
	pulse.center = eye + 0.1 * mRotation.frontVector().normalize()-right * 0.1 -Vector3(0,0.05- std::sin(time) * 0.01f,0);
	for (EntityUI* UI : playerUI) {
		UI->Update(deltatime, camera);

		UI->UpdateAspectRatio();
	}
}
void Player::render(Camera* camera) {
	if (!canrender) return;
	//Shader* shader;
	//shader = Shader::Get("data/shaders/HUD.vs", "data/shaders/HUD.fs");
	//Mesh* hudQuad=new Mesh();
	//hudQuad->createQuad(0.6f, 0.5f+0.02* std::sin(actualy), 0.8f, 1.0f, false);
	//hudQuad->uploadToVRAM();
	//glDisable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//shader->enable();

	//// Uniform: resolución de la pantalla
	//shader->setUniform("u_resolution", Vector2((float)Game::instance->window_width, (float)Game::instance->window_height));

	//// Textura del HUD
	//if (LampMat.diffuse)
	//	shader->setTexture("u_texture", LampMat.diffuse, 0);

	//// Renderizamos el quad en espacio de pantalla
	//hudQuad->render(GL_TRIANGLES);

	//shader->disable();

	//glDisable(GL_DEPTH_TEST);
	/*
	if (RENDERCOLISIONS){
			shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
		Mesh* meshs = Mesh::Get("data/meshes/sphere.obj");
		shader->enable();

		Matrix44 m;
		m.setIdentity();
		m.translate(model.getTranslation());
		m.scale(vec3(1));
		shader->setUniform("u_color", Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_model", m);

		meshs->render(GL_LINES);
		shader->disable();
	}*/
	/*Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
	Mesh * mesh = Mesh::Get("data/meshes/sphere.obj");

	shader->enable();

	Matrix44 m;
	EntityMesh* entity;
	BoundingBox collider;
	Vector3 objectcenter;
	m.setTranslation(model.getTranslation() + vec3(0, 0.5f, 0));
	m.scale(vec3(0.5f));
	shader->setUniform("u_color",  Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_model", m);

	mesh->render(GL_LINES);
	for (int i = 0; i < World::NightMap->root->children.size(); i++) {
		entity = static_cast<EntityMesh*>(World::NightMap->root->children[i]);
		collider = static_cast<EntityCollider*>(entity->children[0])->collider;
		objectcenter = collider.center;
		m.setTranslation(objectcenter);


	}

	shader->disable();
	*/
	playerUI[0]->render(camera);
	Entity::render(camera);
}
void Player::renderUI(Camera* camera) {
	if (!canrender)return;
	for (EntityUI* UI : playerUI) {
		if (UI != playerUI[0])
		UI->render(camera);
	}

	playerUI[1]->canrender = false;
}