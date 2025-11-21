#include "framework/entities/entityCollider.h"
#include <iostream>

void EntityCollider::update(float elapsed_time,Camera& camera) {
	if (!parent) return;
	if(is_dynamic||!is_initialized){
		EntityMesh* parentMesh = static_cast<EntityMesh*>(parent);
		if (!parentMesh->mesh) return;
		if (!isInstanced) {
			BoundingBox worldBox = transformBoundingBox(parentMesh->model, parentMesh->mesh->box);
			collider = worldBox;
		}
		else {
			colliders.clear();
			const std::vector<Matrix44> globalMats = getArrayofGlobalMatrix();
			for (size_t i = 0; i < globalMats.size(); ++i) {
				BoundingBox b = transformBoundingBox(parentMesh->models[i], parentMesh->mesh->box);
				colliders.push_back(b);
			}
		}
		if (!isInstanced) {
			model = parentMesh->model;
			position = collider.center;
			sphereRadius = collider.halfsize.length();
		}
		else {
			positions.clear();
			sphereRadiusses.clear();
			for (int i = 0; i < (int)models.size() && i < (int)colliders.size(); i++) {
				models[i] = parentMesh->models[i];
				positions.push_back(colliders[i].center);
				sphereRadiusses.push_back(colliders[i].halfsize.length());
			}
		}
		is_initialized = true;
	}
	Entity::update(elapsed_time, camera);
}
bool EntityCollider::TestCollisionSphere(float radius, const Vector3& center, std::vector<sCollisionData>& collisions, eCollisionFilter filter) {
	if (filter == layer)return false;
	if (type == MESH){
		bool output= Collision::TestEntitySphere(this, radius, center, collisions, filter);
		vec3 dirtoobj;
		if (isInstanced) {
			for (int j = 0; j < models.size(); j++) {

				dirtoobj = (models[j].getTranslation() - center);
				if (dirtoobj.length() < (models[closest].getTranslation() - center).length()) {
					closest = j;
				}
			}
		}
		return output;
	}
	if (type == BOX) {
		BoundingBox boxused = collider;
		closest = 0;
		if (isInstanced) {
			vec3 dirtoobj;
			for (int j = 0; j < models.size(); j++) {

				dirtoobj = (models[j].getTranslation() - center);
				if (dirtoobj.length() < (models[closest].getTranslation() - center).length()) {
					closest = j;
				}
			}
			boxused = colliders[closest];
		}
		Vector3 min = boxused.center - boxused.halfsize;
		Vector3 max = boxused.center + boxused.halfsize;

		Vector3 closestp;

		closestp.x = clamp(center.x, min.x, max.x);
		closestp.y = clamp(center.y, min.y, max.y);
		closestp.z = clamp(center.z, min.z, max.z);

		Vector3 v = center - closestp;
		float dist2 = pow(v.length(),2);

		
		if (dist2 > radius * radius)
			return false;


		sCollisionData data = {
		.col_point = closestp,
		.col_normal = v.normalize(),
		.distance = center.distance(closestp),
		.collided = true,
		.collider = this
		};
		collisions.push_back(data);
		return true;
	}
	if(type==SPHERE){
		Vector3 usedPos = position;
		
		vec3 dirtoobj;
		closest = 0;
		if (isInstanced) {
			for (int j = 0; j < positions.size(); j++) {

				dirtoobj = (positions[j] - center);
				if (dirtoobj.length() < (positions[closest] - center).length()) {
					closest = j;
				}
			}
			usedPos = positions[closest];
			sphereRadius = sphereRadiusses[closest];
		}
		dirtoobj = (usedPos - center);
		if (dirtoobj.length() < (sphereRadius + radius)) {
			sCollisionData data = {
			.col_point = usedPos+dirtoobj/2*sphereRadius/radius,
			.col_normal = dirtoobj.normalize(),
			.distance = center.distance(usedPos),
			.collided = true,
			.collider = this
			};

			collisions.push_back(data);
			return true;
		}
		return false;
	}
	return false;
	
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

	if (isInstanced) {
		if (type == SPHERE) {
			Mesh* sphereMesh = Mesh::Get("data/meshes/sphere.obj");
			Shader* sphereShader= Shader::Get("data/shaders/instanced.vs", "data/shaders/flat.fs");
			sphereShader->enable();
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			sphereShader->setUniform("u_viewprojection", camera->viewprojection_matrix);

			sphereShader->setUniform("u_color", material->color);
			
			mesh->renderInstanced(GL_LINES, MatstoRender.data(), MatstoRender.size());
			sphereShader->disable();
		}
		if (type == BOX) {
			int i = 0;
			BoundingBox box=mesh->box;
			for (Matrix44 actualModel : MatstoRender) {
				mesh->box = colliders[i];
				mesh->renderBounding(actualModel,false);
				i++;
			}
			mesh->box = box;
		}
		else{
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
			mesh->renderInstanced(GL_LINES, MatstoRender.data(), MatstoRender.size());
			shader->disable();
		}
	}
	else {
			if (type == SPHERE) {
				Mesh* sphereMesh = Mesh::Get("data/meshes/sphere.obj");
				Shader* sphereShader = Shader::Get("data/shaders/instanced.vs", "data/shaders/flat.fs");
				sphereShader->enable();
				glDisable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
				sphereShader->setUniform("u_viewprojection", camera->viewprojection_matrix);

				sphereShader->setUniform("u_color", material->color);
				Matrix44 mat;
				mat.setTranslation(position);
				mat.scale(sphereRadius);
				sphereShader->setUniform("u_model", mat);
				sphereMesh->renderInstanced(GL_LINES, MatstoRender.data(), MatstoRender.size());
				sphereShader->disable();
			}
			if (type == BOX) {
			
				mesh->renderBounding(model,true);
			}
			
			else{	
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
				mesh->render(GL_LINES);

				shader->disable();
			}
		
	}
	// Disable shader after finishing rendering

	Entity::render(camera);
};
