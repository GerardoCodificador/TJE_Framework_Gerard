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

#include "framework/extra/bass.h"
class EntityMesh;
class EntityCollider;
class Inventory;
class Item :public EntityMesh {
public:
	Item() {};
	Item(Mesh* m, Material mat);
	HSAMPLE hSample;	// Handler to store one sample	
	HCHANNEL hSampleChannel; // Handler to store one channel
	EntityCollider* collider;
	std::vector<bool> active;

	std::vector<Inventory> loot;
	int activate_action = -1;
	float time = 0.0f;
	float block_time = 2.0f;
	bool blackofff = false;
	~Item(){}
	void update(float elapsed_time,Camera& camera);
	void render(Camera* camera);
	void onColisionEnter(Entity* e, sCollisionData collisiondata, eCollisionFilter Type);
};
enum DoorType {
	NORMAL,
	KEYED,
	MAIN
};
class Door :public EntityMesh {
public:
	Door() {};
	Door(Mesh* m, Material mat);
	HSAMPLE hSample;	// Handler to store one sample	
	HCHANNEL hSampleChannel; // Handler to store one channel
	float block_time = 3.0f;
	std::vector<BoundingBox> DoorSpawns;
	bool blackofff = true;
	DoorType MyType;
	void update(float elapsed_time, Camera& camera);
	void render(Camera* camera);
	void onColisionEnter(Entity* e, sCollisionData collisiondata, eCollisionFilter Type);
	EntityCollider* collider;

};