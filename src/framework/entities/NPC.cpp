
#include "framework/entities/NPC.h"




NPC::NPC(Texture* tex, std::string inname,vec3 pos) {
	texture = tex;
	name = inname;
	model.setTranslation(pos);

	Mesh* mmesh = Mesh::Get("data/meshes/sphere.obj");
	Material* mmat = new Material();
	EntityCollider* collider = new EntityCollider(mmesh, *mmat);
	collider->layer = TYPENPC;
	collider->model = model;
	collider->model.scale(0.2f);
	collider->is_dynamic = true;
	collider->collider = transformBoundingBox(model, mmesh->box);
	this->addChild(collider);

};
void NPC::update(float deltatime,Camera& camera) {
	Vector3 PlayerPos;
	if (Game::instance->currentStage == eStage::STAGE_GAMEDAY) {
		PlayerPos = World::DayMap->player->model.getTranslation();
	}
	else if (Game::instance->currentStage == eStage::STAGE_GAMENIGHT) {

		PlayerPos = World::NightMap->player->model.getTranslation();
	}
	Vector3 myPos = model.getTranslation();

	Vector3 target = PlayerPos;
	// mantener la misma altura
	target.y = myPos.y;

	// matriz lookAt (view matrix)
	Matrix44 view;
	Vector3 up(0, 1, 0);
	view.lookAt(myPos, target, up);

	// convertirla a matriz de modelo invirtiéndola
	Matrix44 modelRot = view;
	modelRot.inverse();

	// conservar la posición
	model = modelRot;
	Entity::update(deltatime, camera);
}
void NPC::render(Camera* camera) {
	if (!canrender)return;
	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/pulse.fs");
	Mesh* hudQuad=new Mesh();
	hudQuad->createQuad(0, 0, 0.5f, 0.5f, false);
	hudQuad->uploadToVRAM();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shader->enable();

	shader->setUniform("u_model", model);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_color", vec4(1,1,1,1));
	shader->setUniform("u_pulse_color", pulse.color);
	shader->setUniform("u_pulse_width", pulse.width);
	shader->setUniform3("u_pulse_center", pulse.center);
	shader->setUniform("u_pulse_radius", pulse.radius);
	shader->setUniform("u_pulse_active",false);
	shader->setTexture("u_texture", texture, 0);

	// Renderizamos el quad en espacio de pantalla
	hudQuad->render(GL_TRIANGLES);

	shader->disable();

	glDisable(GL_DEPTH_TEST);
	

	Entity::render(camera);
}