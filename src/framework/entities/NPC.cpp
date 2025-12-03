
#include "framework/entities/NPC.h"

#include <thread>
#include <chrono>


NPC::NPC(Texture* tex, std::string inname,vec3 pos) {
	texture = tex;
	name = inname;
	model.setTranslation(pos);

	Mesh* mmesh = Mesh::Get("data/meshes/sphere.obj");
	mesh = mmesh;
	Material* mmat = new Material();
	EntityCollider* collider = new EntityCollider(mmesh, *mmat);
	collider->layer = TYPENPC;
	collider->is_initialized = true;
	collider->type = SPHERE;
	collider->model = model;
	collider->model.scale(0.1f);
	collider->position = pos;
	collider->sphereRadius = 1.0;
	model = collider->model;
	
	collider->collider = transformBoundingBox(model, mmesh->box);
	this->addChild(collider);

};
void NPC::onColisionEnter(Entity* e, sCollisionData collisiondata, eCollisionFilter Type) {
	if (Input::isKeyPressed(SDL_SCANCODE_E)) {
		std::cout << "Sister: 'Brother I am very Hungry" << std::endl;
		if (World::player->inventory.food > 0) {
			std::cout << "You have only one portion of food you wan give it to her or eat the food? Y(Give)/N(Eat)" << std::endl;
			Game::instance->ask_to_change=true;
			wait_for_answer = true;
		}
		else {
			std::cout << "You have no food for none of you" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(2));
			std::cout << "*Your sister died*" << std::endl;
			this->~NPC();

			std::this_thread::sleep_for(std::chrono::seconds(2));
			std::cout << "You are starving" << std::endl;
			Game::instance->setStage(GAMEOVER);
		}
	}
}
	 
void NPC::update(float deltatime,Camera& camera) {
	Vector3 PlayerPos = World::player->model.getTranslation();
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
	if (wait_for_answer && !Game::instance->ask_to_change) {
		if (Game::instance->answer == 1) {
			std::cout << "You are starving" << std::endl;
			//Game::instance->setStage(GAMEOVER);
		}
		else if (Game::instance->answer == 0) {
			std::cout << "*Your sister died*" << std::endl;
			this->~NPC();
		}
		wait_for_answer = false;

	}
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
	shader->setUniform("u_specular_strength", (float)0.05);

	shader->setUniform("u_shininess", 10);
	shader->setUniform3("u_pulse_center", pulse.center);
	shader->setUniform("u_pulse_radius", pulse.radius);
	shader->setUniform("u_pulse_active",false);
	shader->setTexture("u_texture", texture, 0); 
	shader->setUniform("u_is_texture", 0);
	
	// Renderizamos el quad en espacio de pantalla
	hudQuad->render(GL_TRIANGLES);

	shader->disable();

	glDisable(GL_DEPTH_TEST);
	

	Entity::render(camera);
}