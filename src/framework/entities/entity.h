#pragma once

#include "framework/includes.h"
#include "framework/framework.h"
#include "graphics/mesh.h"
#include "graphics/material.h"
#include "graphics/shader.h"
#include "graphics/texture.h"

class Camera;

class Entity {

public:

	Entity() {}; 			// Constructor
	virtual ~Entity() {

	}; 	// Destructor

	std::string name;

	Matrix44 model;

	Entity* parent = nullptr;
	std::vector<Entity*> children;

	void addChild(Entity* child);
	void removeChild(Entity* child);

	// Methods that should be overwritten
	// by derived classes 
	virtual void render(Camera* camera);
	virtual void update(float delta_time);

	// Some useful methods
	Matrix44 getGlobalMatrix();
	float distance(Entity* e);
};


class EntityMesh : public Entity {

public:
	EntityMesh() {}; 			// Constructor

	EntityMesh(Mesh *m,Material mat) {
		mesh = m;
		material = new Material(mat);

	}; 			// Constructor
	virtual ~EntityMesh() {}; 	// Destructor
	// Attributes of the derived class  
	std::vector<Matrix44> getArrayofGlobalMatrix();
	Mesh* mesh = nullptr;

	Texture* texture = nullptr;
	bool isInstanced = false;
	bool culling = true;
	// Check your framework material class!
	Material* material = nullptr;
	std::vector<Matrix44> models; // For instanced rendering

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);
};
