#include "framework/entities/item.h"


Item::Item(Mesh* m, Material mat) {
	mesh = m;
	material = new Material(mat);
	collider = new EntityCollider(mesh, *material);
	collider->layer = ITEM;


	collider->mesh = Mesh::Get("data/meshes/sphere.obj");


	collider->type = SPHERE;
	this->addChild(collider);
	isInstanced = true;
	if (BASS_Init(-1, 44100, 0, 0, NULL) == false) {
		// Error with sound device
	}

	hSample = BASS_SampleLoad(
		false,  			// From internal memory
		"data/sounds/search.wav", 	// Filepath
		0,				// Offset
		0,				// Length
		3,				// Max playbacks
		BASS_SAMPLE_MONO    				// Flags
	);

	hSampleChannel = BASS_SampleGetChannel(hSample, false); // Handler to store one channel

};

void Item::onColisionEnter(Entity* e, sCollisionData collisiondata, eCollisionFilter Type) {
	if (Input::isKeyPressed(SDL_SCANCODE_E)) {
		int collided_i = static_cast<EntityCollider*>(children[0])->closest;
		if (World::player->actual_stun <= 0.0) {
			World::player->max_stun = block_time;
			World::player->actual_stun = block_time;
			World::player->is_blackoff = blackofff;
			World::player->canmove = false;
			activate_action = collided_i;
			BASS_ChannelPlay(hSampleChannel, true);
		}
	}
}
void Item::update(float deltatime,Camera& camera) {

	if (activate_action >= 0) {
		if (active[activate_action] && World::player->actual_stun <= 0) {
			
			if(loot[activate_action].food>0){
				World::player->inventory.food += loot[activate_action].food;
				std::cout << "I've found some food!!" << std::endl;
			}
			else if (loot[activate_action].keys > 0) {
				World::player->inventory.keys += loot[activate_action].keys;
				std::cout << "I've found a key!! I wonder what will open" << std::endl;
			}
			else std::cout << "Is empty" << std::endl;
			active[activate_action] = false;

			activate_action = -1;
		}
		else if (World::player->actual_stun <= 0) {
			//looteado
			std::cout << "Is empty" << std::endl;
			activate_action = -1;
		}
	}
	collider->models = models;
	Entity::update(deltatime,camera);
	
}
void Item::render(Camera* camera) {
	if (!canrender)return;
	if (!mesh) {
		Entity::render(camera);
		return;
	}

	std::vector<Matrix44> MatstoRender;
	if (isInstanced) {
		const std::vector<Matrix44> globalMats = getArrayofGlobalMatrix();
		float distance = 10.0f;
		BoundingBox box;
		bool skip = false;
		for (int i = 0; i < models.size(); i++) {
			box = transformBoundingBox(globalMats[i], mesh->box);
			if (camera->eye.distance(box.center) > distance + box.halfsize.length()) {
				skip = true;
			}

			//Frustum Culling
			if (camera->testSphereInFrustum(box.center, box.halfsize.length()) != CLIP_INSIDE) {
				skip = true;
			}
			if (!skip) {
				MatstoRender.push_back(models[i]);
			}
			skip = false;

		}
		if (MatstoRender.size() < 1) {
			Entity::render(camera);
			return;
		}
	}
	else {
		const Matrix44& globalMat = getGlobalMatrix();
		float distance = 10.0f;
		BoundingBox box = transformBoundingBox(globalMat, mesh->box);

		//Distance CUlling
		if (camera->eye.distance(box.center) > distance + box.halfsize.length()) {
			Entity::render(camera);
			return;
		}

		//Frustum Culling
		if (camera->testSphereInFrustum(box.center, box.halfsize.length()) != CLIP_INSIDE) {
			Entity::render(camera);
			return;
		}

	}


	if (isInstanced) {

		if (!material->shader) {
			material->shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/pulse.fs");
		}
		// Set OpenGL flags
		Shader* shader = material->shader;
		shader->enable();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);

		shader->setUniform("u_color", material->color);
		shader->setUniform("u_pulse_color", pulse.color);
		shader->setUniform("u_pulse_width", pulse.width);
		shader->setUniform3("u_pulse_center", pulse.center);
		shader->setUniform("u_specular_strength", (float)0.05);

		shader->setUniform("u_shininess", (float)10);
		shader->setUniform("u_pulse_radius", pulse.radius);
		shader->setUniform("u_pulse_active", pulse.active);
		if (material->diffuse) {
			shader->setTexture("u_texture", material->diffuse, 0);
			shader->setUniform("u_is_texture", 0);
		}
		else shader->setUniform("u_is_texture", 1);
		mesh->renderInstanced(GL_TRIANGLES, MatstoRender.data(), MatstoRender.size());
		shader->disable();
	}
	else {
		if (!material->shader) {
			material->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/pulse.fs");
		}
		Shader* shader = material->shader;
		// Set OpenGL flags
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		shader->enable();


		// Enable shader and pass uniforms 
		shader->setUniform("u_model", model);
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_color", material->color);
		shader->setUniform("u_pulse_color", pulse.color);
		shader->setUniform("u_pulse_width", pulse.width);
		shader->setUniform("u_specular_strength", (float)0.05);

		shader->setUniform("u_shininess", (float)10);
		shader->setUniform3("u_pulse_center", pulse.center);
		shader->setUniform("u_pulse_radius", pulse.radius);
		shader->setUniform("u_pulse_active", pulse.active);
		if (material->diffuse) {
			shader->setTexture("u_texture", material->diffuse, 0);
			shader->setUniform("u_is_texture", 0);
		}
		else shader->setUniform("u_is_texture", 1);

		shader->setUniform("u_isinstanced", isInstanced);

		// Render the mesh using the active shader
		mesh->render(GL_TRIANGLES);

		shader->disable();
	}
	// Disable shader after finishing rendering

	Entity::render(camera);
}

Door::Door(Mesh* m, Material mat) {
	mesh = m;
	material = new Material(mat);
	MyType = MAIN;
	collider = new EntityCollider(mesh, *material);
	collider->layer = DOOR;


	collider->mesh = Mesh::Get("data/meshes/sphere.obj");
	if (BASS_Init(-1, 44100, 0, 0, NULL) == false) {
		// Error with sound device
	}

	hSample = BASS_SampleLoad(
		false,  			// From internal memory
		"data/sounds/door.mp3", 	// Filepath
		0,				// Offset
		0,				// Length
		3,				// Max playbacks
		BASS_SAMPLE_MONO    				// Flags
	);

	hSampleChannel = BASS_SampleGetChannel(hSample, false); // Handler to store one channel
	collider->type = SPHERE;
	this->addChild(collider);

}
void Door::update(float elapsed_time, Camera& camera) {
	collider->models = models;
	Entity::update(elapsed_time, camera);
}
void Door::onColisionEnter(Entity* e, sCollisionData collisiondata, eCollisionFilter Type) {
	if (Input::isKeyPressed(SDL_SCANCODE_E)) {
		if(MyType ==NORMAL){
			int collided_i = static_cast<EntityCollider*>(children[0])->closest;
			if (World::player->actual_stun == 0.0) {
				World::player->max_stun = block_time;
				World::player->actual_stun = block_time;
				World::player->is_blackoff = blackofff;
				World::player->model.setTranslation(DoorSpawns[(collided_i + 1) % 2].center + Vector3(0, DoorSpawns[(collided_i + 1) % 2].halfsize.y, 0));
				World::player->canmove = false;
				BASS_ChannelPlay(hSampleChannel, true);
			}
		}
		else if (MyType == KEYED) {
			if (World::player->inventory.keys > 0) {
				std::cout << "Nice, the key fits in" << std::endl;
				int collided_i = static_cast<EntityCollider*>(children[0])->closest;
				if (World::player->actual_stun == 0.0) {
					World::player->max_stun = block_time;
					World::player->actual_stun = block_time;
					World::player->is_blackoff = blackofff;
					World::player->model.setTranslation(DoorSpawns[(collided_i ) % 2].center + Vector3(0, DoorSpawns[(collided_i ) % 2].halfsize.y, 0));
					World::player->canmove = false;
					BASS_ChannelPlay(hSampleChannel, true);
				}
			}
			else {
				std::cout << "The Door is closed" << std::endl;
			}
		}
		else if (MyType == MAIN) {
			std::cout << "Return home? Y/N:" << std::endl;
			Game::instance->ask_to_change = true;
		}
	}
}
void Door::render(Camera* camera){
	if (!canrender)return;
	if (!mesh) {
		Entity::render(camera);
		return;
	}

	std::vector<Matrix44> MatstoRender;
	if (isInstanced) {
		const std::vector<Matrix44> globalMats = getArrayofGlobalMatrix();
		float distance = 10.0f;
		BoundingBox box;
		bool skip = false;
		for (int i = 0; i < models.size(); i++) {
			box = transformBoundingBox(globalMats[i], mesh->box);
			if (camera->eye.distance(box.center) > distance + box.halfsize.length()) {
				skip = true;
			}

			//Frustum Culling
			if (camera->testSphereInFrustum(box.center, box.halfsize.length()) != CLIP_INSIDE) {
				skip = true;
			}
			if (!skip) {
				MatstoRender.push_back(models[i]);
			}
			skip = false;

		}
		if (MatstoRender.size() < 1) {
			Entity::render(camera);
			return;
		}
	}
	else {
		const Matrix44& globalMat = getGlobalMatrix();
		float distance = 10.0f;
		BoundingBox box = transformBoundingBox(globalMat, mesh->box);

		//Distance CUlling
		if (camera->eye.distance(box.center) > distance + box.halfsize.length()) {
			Entity::render(camera);
			return;
		}

		//Frustum Culling
		if (camera->testSphereInFrustum(box.center, box.halfsize.length()) != CLIP_INSIDE) {
			Entity::render(camera);
			return;
		}

	}

	if (isInstanced) {

		if (!material->shader) {
			material->shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/pulse.fs");
		}
		// Set OpenGL flags
		Shader* shader = material->shader;
		shader->enable();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);

		shader->setUniform("u_color", material->color);
		shader->setUniform("u_pulse_color", pulse.color);
		shader->setUniform("u_pulse_width", pulse.width);
		shader->setUniform3("u_pulse_center", pulse.center);
		shader->setUniform("u_pulse_radius", pulse.radius);
		shader->setUniform("u_specular_strength", (float)0.05);

		shader->setUniform("u_shininess", (float)10);
		shader->setUniform("u_pulse_active", pulse.active);
		if (material->diffuse) {
			shader->setTexture("u_texture", material->diffuse, 0);
			shader->setUniform("u_is_texture", 0);
		}
		else shader->setUniform("u_is_texture", 1);
		mesh->renderInstanced(GL_TRIANGLES, MatstoRender.data(), MatstoRender.size());
		shader->disable();
	}
	else {
		if (!material->shader) {
			material->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/pulse.fs");
		}
		Shader* shader = material->shader;
		// Set OpenGL flags
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		shader->enable();


		// Enable shader and pass uniforms 
		shader->setUniform("u_model", model);
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_color", material->color);
		shader->setUniform("u_pulse_color", pulse.color);
		shader->setUniform("u_pulse_width", pulse.width);
		shader->setUniform("u_specular_strength", (float)0.05);

		shader->setUniform("u_shininess", (float)10);
		shader->setUniform3("u_pulse_center", pulse.center);
		shader->setUniform("u_pulse_radius", pulse.radius);
		shader->setUniform("u_pulse_active", pulse.active);
		if (material->diffuse) {
			shader->setTexture("u_texture", material->diffuse, 0);
			shader->setUniform("u_is_texture", 0);
		}
		else shader->setUniform("u_is_texture", 1);

		shader->setUniform("u_isinstanced", isInstanced);

		// Render the mesh using the active shader
		mesh->render(GL_TRIANGLES);

		shader->disable();
	}
	// Disable shader after finishing rendering

	Entity::render(camera);
}