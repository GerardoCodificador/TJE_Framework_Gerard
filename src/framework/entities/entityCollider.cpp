#include "framework/entities/entityCollider.h"

void EntityCollider::update(float elapsed_time,Camera& camera) {
	if (is_dynamic) {
		if (!isInstanced) {
			collider = transformBoundingBox(parent->model, static_cast<EntityMesh*>( parent)->mesh->box);
		}
		else {
			colliders.clear();
			for (int i = 0; i < models.size(); i++) {
				colliders.push_back(transformBoundingBox(static_cast<EntityMesh*>(parent)->model, static_cast<EntityMesh*>(parent)-> mesh->box));
			}
		}
	}
	if (!isInstanced) {

		model.setScale(collider.halfsize.x*2, collider.halfsize.z*2, collider.halfsize.y*2);
		model.translate(collider.center+Vector3(0,1,0));
	}
	else {
		for (int i = 0; i < models.size(); i++) {
			models[i].setScale(colliders[i].halfsize.x * 2, colliders[i].halfsize.z * 2, colliders[i].halfsize.y * 2);
			models[i].translate(colliders[i].center);
		}
	}
	
	Entity::update(elapsed_time,camera);
}
void EntityCollider::render(Camera* camera) {
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

		
	}
	else {
		if (layer == WALL) {
			if (!material->shader) {
				material->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
			}
			Shader* shader = material->shader;
			// Set OpenGL flags
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			shader->enable();


			// Enable shader and pass uniforms 
			shader->setUniform("u_model", getGlobalMatrix());
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
	}
	// Disable shader after finishing rendering

	Entity::render(camera);
};
