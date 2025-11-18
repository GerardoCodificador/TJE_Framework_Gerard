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
class EntityCollider;
class Item :public EntityMesh {
public:
	Item() {};
	Item(Mesh* m, Material mat);
		
	EntityCollider* collider;
	std::vector<bool> active;
	float time = 0.0f;
	~Item(){}
	void update(float elapsed_time,Camera& camera);
	void render(Camera* camera);
};