
#ifndef ENTITY_COLLIDER_H
#define ENTITY_COLLIDER_H
#pragma once

#include "framework/includes.h"
#include "framework/framework.h"
#include "graphics/mesh.h"
#include "graphics/material.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "framework/entities/entity.h"
#include "framework/world.h"
#include "framework/input.h"
#include "framework/collision.h"


class EntityCollider : public EntityMesh {
	
public:
	EntityCollider() {}; 			// Constructor

	EntityCollider(Mesh* m, Material mat) {
		mesh = m;
		material = new Material(mat);
	};
	eCollisionFilter layer = NONE;
	bool is_dynamic = false;
	void render(Camera* camera){}
	void update(float elapsed_time);
};
#endif 