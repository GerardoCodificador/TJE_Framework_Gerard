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
struct eUITYPE {};

class EntityUI : public EntityMesh {
public:
	EntityUI() {};
	EntityUI(Vector2 pos, Vector2 new_size, const Material mat);
	Vector2 position;
	Vector2 size;
	Entity* linkedesh;
	bool check3D();
	
	void Update(float elapsed_time, Camera& camera);
	void Updateposition(Vector2 deltamove);
	void UpdateAspectRatio();
	void render(Camera* camera);

};
enum eMouseState {
	Out,
	Hoover,
	Active,
	Clicked
};
class Button : public EntityUI {
public:
	Button() {};
	Button(Vector2 pos, Vector2 new_size, const Material mat);
	Vector2 position;
	Vector2 size;
	eMouseState state;
	void update(float elapsed_time, Camera& camera);
	void Updateposition(Vector2 deltamove);
	void render(Camera* camera);

};
class CodeMachine :public EntityUI {
public:
	CodeMachine() {}
	CodeMachine(Vector2 pos, Vector2 new_size);
	Vector2 position;
	Vector2 size;
	eMouseState state;
	/*void update(float elapsed_time, Camera& camera);
	void Updateposition(Vector2 deltamove);
	void render(Camera* camera);
	*/
};