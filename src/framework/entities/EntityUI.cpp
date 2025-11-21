#include "framework/entities/EntityUI.h"

EntityUI::EntityUI(Vector2 pos, Vector2 new_size, const Material mat) {
	material = new Material(mat);
	position = pos;
	size = new_size;
	mesh = new Mesh();
	mesh->createQuad(pos.x, pos.y,size.x,size.y, false);
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
	Shader* shader = material->shader;
	shader->enable();
	shader->setUniform("u_model", model);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_color", material->color);
	shader->setUniform("u_pulse_color", pulse.color);
	shader->setUniform("u_pulse_width", pulse.width);
	shader->setUniform3("u_pulse_center", pulse.center);
	shader->setUniform("u_pulse_radius", pulse.radius);
	shader->setUniform("u_pulse_active", pulse.active);
	if (material->diffuse)shader->setTexture("u_texture", material->diffuse, 0);
	shader->disable();
}