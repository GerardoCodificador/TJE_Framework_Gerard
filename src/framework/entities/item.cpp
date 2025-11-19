#include "framework/entities/item.h"


Item::Item(Mesh* m, Material mat) {
	mesh = m;
	material = new Material(mat);
	collider = new EntityCollider(mesh, *material);
	collider->layer = ITEM;


	collider->mesh = Mesh::Get("data/meshes/sphere.obj");
	collider->models = models;
	collider->model.translate(0, 0.5, 0);
	collider->isInstanced = true;
	this->addChild(collider);
	isInstanced = true;

};
void Item::update(float deltatime,Camera& camera) {
	Matrix44 position;
	Matrix44 rotation;
	rotation.setIdentity();
	rotation.rotate(M_PI*0.5 * deltatime, vec3(0, 1, 0));

	for (int i = 0; i < models.size(); i++) {
		if (!active[i])continue;
		vec3 center = models[i].getTranslation();

		Matrix44 T;   T.setTranslation(center);
		Matrix44 Ti;  Ti.setTranslation(-center);

		models[i] = rotation *models[i];
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
			if (!active[i])skip = true;
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
		shader->setUniform("u_pulse_active", pulse.active);
		if (material->diffuse)
			shader->setUniform("u_texture", material->diffuse);
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
		shader->setUniform3("u_pulse_center", pulse.center);
		shader->setUniform("u_pulse_radius", pulse.radius);
		shader->setUniform("u_pulse_active", pulse.active);
		if (material->diffuse)shader->setTexture("u_texture", material->diffuse, 0);


		// Render the mesh using the active shader
		mesh->render(GL_TRIANGLES);

		shader->disable();
	}
	// Disable shader after finishing rendering

	Entity::render(camera);
}