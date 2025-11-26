#include "framework/entities/EntityUI.h"

EntityUI::EntityUI(Vector2 pos, Vector2 new_size, const Material mat) {
	material = new Material();
	material->shader = mat.shader;       // si quieres reusar
	material->diffuse = mat.diffuse;
	position = Vector2(pos.x / Game::instance->window_width, pos.y / Game::instance->window_height);
	size =Vector2(new_size.x / Game::instance->window_width, new_size.y / Game::instance->window_height);
	mesh = new Mesh();

	mesh->createQuad(position.x,position.y,size.x,size.y , false);
	
	mesh->uploadToVRAM();
}
void EntityUI::Updateposition(Vector2 deltamove) {
	Vector2 newpos= Vector2(position.x + deltamove.x / Game::instance->window_width, position.y + deltamove.y / Game::instance->window_height);

	mesh->createQuad(newpos.x, newpos.y, size.x, size.y, false);

	mesh->uploadToVRAM();
}
void EntityUI::render(Camera* camera) {
	if (!canrender)return;
	if (!mesh) {
		Entity::render(camera);
		return;
	}
	if (!material->shader) {
		material->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/pulse.fs");
	}
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader* shader = material->shader;
	shader->enable();
	shader->setUniform("u_center", position);
	shader->setUniform("u_size", size);
	shader->setUniform("u_percent", World::player->actual_stun/World::player->max_stun);

	shader->setUniform("u_color", material->color);

	shader->setUniform("u_colorB",Vector3(0.5));
	shader->setUniform("u_model", model); Camera camera2D;
	camera2D.view_matrix = Matrix44(); // Set View to identity
	camera2D.setOrthographic(0, Game::instance->window_width,Game::instance-> window_height, 0, -1, 1);
	shader->setUniform("u_viewprojection", camera2D.viewprojection_matrix);
	shader->setUniform("u_resolution", Vector2((float)Game::instance->window_width, (float)Game::instance->window_height));
	if (material->diffuse)shader->setTexture("u_texture", material->diffuse, 0);
	mesh->render(GL_TRIANGLES);
	shader->disable();
}

Button::Button(Vector2 pos, Vector2 new_size, const Material mat) {
	material = new Material();
	material->shader = mat.shader;       // si quieres reusar
	material->diffuse = mat.diffuse;
	position = Vector2(pos.x / Game::instance->window_width, pos.y / Game::instance->window_height);
	size = Vector2(new_size.x / Game::instance->window_width, new_size.y / Game::instance->window_height);
	mesh = new Mesh();

	mesh->createQuad(position.x, position.y, size.x, size.y, false);

	mesh->uploadToVRAM();
}

void Button::update(float elapsed_time, Camera& camera) {
	Vector2 transformedpos = Vector2(position.x * Game::instance->window_width, (1-position.y) * Game::instance->window_height);
	Vector2 transformedsize = Vector2(size.x * Game::instance->window_width, size.y * Game::instance->window_height);
	if (transformedpos.x-transformedsize.x/2 < Input::mouse_position.x && Input::mouse_position.x < transformedpos.x + transformedsize.x/2) {
		if (transformedpos.y - transformedsize.y/2 < Input::mouse_position.y && Input::mouse_position.y < transformedpos.y + transformedsize.y/2) {
			state = Hoover;
			if (Input::isMousePressed(SDL_BUTTON_LEFT)) {
				state = Active;
			}
			if (Input::wasMouseReleased(SDL_BUTTON_LEFT)) {
				state = Clicked;
			}
		}
	}

	else state = Out;
}

void Button::render(Camera* camera) {
	if (!canrender)return;
	if (!mesh) {
		Entity::render(camera);
		return;
	}
	if (!material->shader) {
		material->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/pulse.fs");
	}
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader* shader = material->shader;
	shader->enable();
	shader->setUniform("u_center", position);
	shader->setUniform("u_size", size);
	shader->setUniform("u_percent", World::player->actual_stun / World::player->max_stun);

	shader->setUniform("u_color", state==Clicked?Vector4(1.2,1.2,1.2,1):state==Active? Vector4(1.1, 1.1, 1.1, 1): state==Hoover?Vector4(0.9, 0.9, 0.9, 1):Vector4(1,1,1,1));

	shader->setUniform("u_colorB", Vector3(0.5));
	shader->setUniform("u_model", model); Camera camera2D;
	camera2D.view_matrix = Matrix44(); // Set View to identity
	camera2D.setOrthographic(0, Game::instance->window_width, Game::instance->window_height, 0, -1, 1);
	shader->setUniform("u_viewprojection", camera2D.viewprojection_matrix);
	shader->setUniform("u_resolution", Vector2((float)Game::instance->window_width, (float)Game::instance->window_height));
	if (material->diffuse)shader->setTexture("u_texture", material->diffuse, 0);
	mesh->render(GL_TRIANGLES);
	shader->disable();
}