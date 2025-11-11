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
class EntityMesh;
class Player :public EntityMesh {
public:
	Player() {};
	Player(Mesh* m, Material mat) {
		mesh = m;
		material = new Material(mat);


	};
	float yaw = 0.0;
	float pitch = 0.0;
	float walk_speed = 1.0f;
	Vector3 velocity;
	float time = 0.0f;
	~Player(){}
	void update(float elapsed_time,Camera& camera);
};