#pragma once

#include "framework/includes.h"
#include "framework/framework.h"
#include "graphics/mesh.h"
#include "graphics/material.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "framework/entities/entity.h"

#include "framework/entities/entityCollider.h"
#include "framework/world.h"
#include "framework/input.h"
#include "game/game.h"
#include "framework/entities/EntityUI.h"
class EntityMesh;
class EntityCollider;
class EntityUI;
class Player :public EntityMesh {
public:
	Player() {};
	Player(Mesh* m, Material mat);
		
	EntityCollider* collider;
	int point = 0;
	float yaw = 0.0;
	float pitch = 0.0;
	float walk_speed = 0.50f;
	float camera_speed = 0.3f;
	Vector3 velocity;
	Material LampMat;
	float time = 0.0f;
	EntityUI* playerUI;
	bool canmove = true;
	~Player(){}
	void onColisionEnter(Entity* e, sCollisionData collisiondata, eCollisionFilter Type);
	void update(float elapsed_time,Camera& camera);
	void render(Camera* camera);
};