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
	this->addChild(collider);
	Material ma;
	ma.color= Vector4(1, 1, 1, 1);
	ma.diffuse = Texture::Get("data/textures/HandLamp.tga");
	LampMat = ma;


};
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
	pulse.radius = 2 + std::sin(time) * 0.3f;
	pulse.center = position;
	Matrix44 mewModel;
	
	std::vector<sCollisionData> collisions;
	eCollisionFilter filter=WALL;
	bool entercollision = false;
	if(ismoving){
		if (Game::instance->currentStage == eStage::STAGE_GAMEDAY) {

			Vector3 character_center = mewModel.getTranslation() + Vector3(0, 2, 0);

			// Check if collides with wall using sphere (radius = 1)
			Entity* entity;
			vec3 dirtoobj;
			for (int i = 0; i < World::DayMap->root->children.size(); i++) {
				entity = World::DayMap->root->children[i];
				if (Collision::TestEntitySphere(entity, 1.0, character_center, collisions, filter)) {
					entercollision = true;
				}
				
			}

		}	else if (Game::instance->currentStage == eStage::STAGE_GAMENIGHT) {


			// Check if collides with wall using sphere (radius = 1)
			EntityMesh* entity;
			vec3 dirtoobj;
			Vector3 objectcenter = position + Vector3(0, 0.5f, 0);
			int closest=0;
			for (int i = 0; i < World::NightMap->root->children.size(); i++) {
				collisions.clear();
				entity = static_cast<EntityMesh*>( World::NightMap->root->children[i]);
				
				if (Collision::TestEntitySphere(entity->children[0], 2.0, objectcenter, collisions, WALL) && dynamic_cast<Player*>(entity) == nullptr) {
				dirtoobj = ( entity->model.getTranslation() - position).normalize();
				if((dirtoobj.x/std::abs(dirtoobj.x)== dir.x / std::abs(dir.x)||(dir.x==0&&dirtoobj.x==0))&&( dirtoobj.y / std::abs(dirtoobj.y) == dir.y/ std::abs(dir.y)) || (dir.y == 0 && dirtoobj.y == 0) && (dirtoobj.z / std::abs(dirtoobj.z) == dir.z / std::abs(dir.z)) || (dir.z == 0 && dirtoobj.z == 0)){
					entercollision = true;
					i_saved = i;
					}
				}

				if (Collision::TestEntitySphere(entity->children[0], 0.2, objectcenter, collisions, ITEM) && dynamic_cast<Player*>(entity) == nullptr) {
					for(int j=0;j<entity->models.size();j++){

						dirtoobj = (entity->models[j].getTranslation() - position);
						if (dirtoobj.length() < (entity->models[closest].getTranslation() - position).length()) {
							closest = j;
						}
					}
					static_cast<Item*>(entity)->active[closest] = false;
				}
			}
		}
		if
			(!entercollision) {
			model.setIdentity();
			model.translate(position.x, position.y, position.z);
			model = mRotation * model;
			model.scale(0.1f, 0.1f, 0.1f);
		}
			
	}
	// Update the camera with new eye & center
	//Vector3 eye = model.getTranslation()+Vector3(0,0.3f,0) + Vector3(0, 0.05f, 0) * std::cos(time) ;

	Vector3 eye = model.getTranslation() + Vector3(0, 0.3f, 0) ;
	Vector3 center = eye + mRotation.frontVector().normalize() ;
	Vector3 up = Vector3(0, 1, 0);
	camera.lookAt(eye, center, up);
	Entity::update(deltatime, camera);
}
void Player::render(Camera* camera) {
	Shader* shader = Shader::Get("data/shaders/HUD.vs", "data/shaders/HUD.fs");
	Mesh* hudQuad=new Mesh();
	hudQuad->createQuad(0.6f, 0.5f+0.02* std::sin(actualy), 0.8f, 1.0f, false);
	hudQuad->uploadToVRAM();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shader->enable();

	// Uniform: resolución de la pantalla
	shader->setUniform("u_resolution", Vector2((float)Game::instance->window_width, (float)Game::instance->window_height));

	// Textura del HUD
	if (LampMat.diffuse)
		shader->setTexture("u_texture", LampMat.diffuse, 0);

	// Renderizamos el quad en espacio de pantalla
	hudQuad->render(GL_TRIANGLES);

	shader->disable();
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
}