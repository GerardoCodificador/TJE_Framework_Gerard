
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

enum eColliderDetection {
	SPHERE,
	BOX,
	MESH
};
class EntityCollider : public EntityMesh {
	
public:
	EntityCollider() {}; 			// Constructor

	EntityCollider(Mesh* m, Material mat) {
		mesh = m;
		material = new Material(mat);
	};
	eCollisionFilter layer = NONE;
	bool is_dynamic = false;
	bool is_initialized = false;
	bool is_collided=false;
	eColliderDetection type = MESH;
	BoundingBox collider;
	std::vector<BoundingBox> colliders;
	int closest = -1;

	Vector3 position;
	std::vector<Vector3> positions;
	float sphereRadius;
	std::vector<float> sphereRadiusses;
	bool TestCollisionSphere( float radius, const Vector3& center, std::vector<sCollisionData>& collisions, eCollisionFilter filter);
	void render(Camera* camera);
	void update(float elapsed_time,Camera& camera);
};
#endif 