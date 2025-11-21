#include "framework/entities/entityCollider.h"
#include <iostream>

void EntityCollider::update(float elapsed_time,Camera& camera) {
	if (!parent) return;
	if(is_dynamic||!is_initialized){
		EntityMesh* parentMesh = static_cast<EntityMesh*>(parent);
		if (!parentMesh->mesh) return;
		if (!isInstanced) {
			BoundingBox worldBox = transformBoundingBox(getGlobalMatrix(), parentMesh->mesh->box);
			collider = worldBox;
		}
		else {
			colliders.clear();
			const std::vector<Matrix44> globalMats = getArrayofGlobalMatrix();
			for (size_t i = 0; i < globalMats.size(); ++i) {
				BoundingBox b = transformBoundingBox(globalMats[i], parentMesh->mesh->box);
				colliders.push_back(b);
			}
		}
		if (!isInstanced) {
			model.setIdentity();
			model.translate(collider.center);
			model.scale(collider.halfsize.x * 2.0f, collider.halfsize.z * 2.0f, collider.halfsize.y * 2.0f);
		}
		else {
			for (int i = 0; i < (int)models.size() && i < (int)colliders.size(); i++) {
				models[i].setIdentity();
				models[i].translate(colliders[i].center);
				models[i].scale(colliders[i].halfsize.x * 2.0f, colliders[i].halfsize.z * 2.0f, colliders[i].halfsize.y * 2.0f);
			}
		}
		is_initialized = true;
	}
	Entity::update(elapsed_time, camera);
}
void EntityCollider::render(Camera* camera) {
	if (!RENDERCOLISIONS) return;
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

	if(layer!=FLOOR){
	if (isInstanced) {
		if (!material->shader) {
			material->shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/flat.fs");
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
				material->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
			}
			Shader* shader = material->shader;
			// Set OpenGL flags
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			shader->enable();


			// Enable shader and pass uniforms 
			shader->setUniform("u_model",model);
			std::cout << model.getTranslation().x<< ","<<model.getTranslation().y << "," << model.getTranslation().z << "," << std::endl;
			std::cout << model._11 << "," << model._22 << "," << model._33 << "," << std::endl;
			shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
			shader->setUniform("u_color", layer==TYPENPC?vec4(1,0,0,1): material->color);
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
