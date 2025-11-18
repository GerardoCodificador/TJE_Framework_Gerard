#include "framework/world.h"
World* World::DayMap = nullptr;
World* World::NightMap = nullptr;

void World::Init(const char* rootname) {
	SceneParser parser;

	root = new Entity();
	parser.parse(rootname, root);
	Texture* cubetexture = new Texture();
	{
		cubetexture->loadCubemap("landscape", { "data/shaders/skybox/night/px.png","data/shaders/skybox/night/nx.png","data/shaders/skybox/night/ny.png","data/shaders/skybox/night/py.png","data/shaders/skybox/night/pz.png","data/shaders/skybox/night/nz.png" });
	}
	Material cubeMat;
	cubeMat.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/boxtexture.fs");
	cubeMat.diffuse = cubetexture;
	skybox = new EntityMesh(Mesh::Get("data/meshes/box.ASE"), cubeMat);
	skybox->culling = false;
	


	EntityCollider* collider;
	EntityMesh *entity;
	for (int i = 0; i < root->children.size(); i++) {
		entity = static_cast<EntityMesh*>(root->children[i]);
		if (entity == nullptr) {
			continue;
		}
		collider = new EntityCollider(entity->mesh, *entity->material);
		collider->model = entity->model;
		collider->models = entity->models;
		collider->isInstanced = entity->isInstanced;
		collider->layer = isFloor(entity) ? FLOOR : isWall(entity) ? WALL : NONE;
		if (entity->mesh) {
			if (!collider->isInstanced)collider->collider = transformBoundingBox(entity->model, entity->mesh->box);
			if (collider->isInstanced) {
				for (int i = 0; i < entity->models.size(); i++) {
					collider->colliders.push_back(transformBoundingBox(entity->models[i], entity->mesh->box));
				}
			}
		}
		else {
			collider->collider = BoundingBox(vec3(0),vec3(0));
		}
		
		
		root->children[i]->addChild(collider);
	}
	MapArea area;
	area.entities = root->children;
	Areas.push_back(area);
}
void World::Update(float deltaTime, Camera& camera) {
	root->update(deltaTime,camera);
}
void World::Render(Camera& camera) {
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
		// Enable the shader

	skybox->render(&camera);

	root->render(&camera);
	

}
bool World::onKeyDown(SDL_KeyboardEvent event) {
	return true;
}
bool World::onMouseButtonDown(SDL_MouseButtonEvent event) {

	return true;
}