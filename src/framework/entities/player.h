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
class Inventory {
public:
	int food;
	int keys;
};
class Player :public EntityMesh {
public:
	Player() {};
	Player(Mesh* m, Material mat);
		
	EntityCollider* collider;
	Inventory inventory;
	float yaw = 0.0;
	float pitch = 0.0;
	float walk_speed = 0.50f;
	float camera_speed = 0.3f;
	bool transport_to_spawn = true;
	Vector3 velocity;
	Material LampMat;
	float time = 0.0f;


	// UI variables
	std::vector<EntityUI*> playerUI;
	bool canmove = true;
	float actual_stun = 0.0f;
	float max_stun = 1.0;
	bool is_blackoff = false;
	~Player(){}
	void onColisionEnter(Entity* e, sCollisionData collisiondata, eCollisionFilter Type);
	void update(float elapsed_time,Camera& camera);
	void render(Camera* camera);

	void onResize(Vector2 prev,Vector2 next);

	void MoveTo(const vec3& pos)
	{
		model.setIdentity();
		model.translate(pos.x, pos.y, pos.z);
		model.scale(0.1f, 0.1f, 0.1f);
	}
};