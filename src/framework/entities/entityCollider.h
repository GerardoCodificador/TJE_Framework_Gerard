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
enum {
	NONE = 0,
	FLOOR = 1 << 0,
	WALL = 1 << 1,
	PLAYER = 1 << 2,
	ENEMY = 1 << 3,
	// Both WALL and FLOOR are considered SCENARIO
	// using OR operator
	SCENARIO = WALL | FLOOR,
	ALL = 0xFF
};


class EntityCollider : public EntityMesh {
	bool is_dynamic = false;
public:
	int layer = NONE;

};