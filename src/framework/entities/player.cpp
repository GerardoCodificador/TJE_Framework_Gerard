#include "framework/entities/player.h"

void Player::update(float deltatime,Camera& camera) {

	Vector3 position =
		model.getTranslation();
	yaw -= Input::mouse_delta.x * deltatime * walk_speed;
	pitch -= Input::mouse_delta.y * deltatime * walk_speed;
	time = time >100?0:time;

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
	}
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)){
		position -= front * walk_speed * deltatime;
		ismoving = true;
}
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)){
		position += right * walk_speed * deltatime;
		ismoving = true;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)){
		position -= right * walk_speed * deltatime;
		ismoving = true;
	}


	if (ismoving)time += deltatime * 8;
	else 
		time *= 0.99f;
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
			for (int i = 0; i < World::DayMap->root->children.size(); i++) {
				entity = World::DayMap->root->children[i];
				if(Collision::TestEntitySphere(entity, 1.0,character_center,collisions,filter))
					entercollision = true;
				
			}

		}	else if (Game::instance->currentStage == eStage::STAGE_GAMENIGHT) {

			Vector3 character_center = mewModel.getTranslation() + Vector3(0, 1, 0);

			// Check if collides with wall using sphere (radius = 1)
			EntityMesh* entity;
			EntityCollider* collider;
			for (int i = 0; i < World::NightMap->root->children.size(); i++) {
				collisions.clear();
				entity = static_cast<EntityMesh*>( World::NightMap->root->children[i]);
				collider = new EntityCollider(entity->mesh, *entity->material);
				collider->layer = WALL;
				collider->model = entity->model;
				if (Collision::TestEntitySphere(collider, .50, character_center, collisions, filter)&&!(entity->isInstanced))
					entercollision = true;
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
}