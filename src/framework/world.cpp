#include "framework/world.h"
World* World::DayMap = nullptr;
World* World::NightMap = nullptr;
Player* World::player = nullptr;

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
	Door* lastdoor = nullptr;
	for (int i = 0; i < root->children.size(); i++) {
		entity = static_cast<EntityMesh*>(root->children[i]);
		
		if (entity == nullptr) {
			continue;
		}
		collider = new EntityCollider(entity->mesh, *entity->material);
		collider->model = entity->model;
		collider->models = entity->models;
		collider->isInstanced = entity->isInstanced;
		collider->layer = isFloor(entity) ? FLOOR : isWall(entity) ? WALL :isSpawner(entity)? SPAWNER:isDoor(entity)?DOOR:isItem(entity)?ITEM:NONE;
		entity->material->color = isFloor(entity) ? Vector4(0.9) : isWall(entity) ? Vector4(1) : isSpawner(entity) ? Vector4(0.25, 0, 0.8, 1) : isDoor(entity) ? Vector4(0.102469, 0.040966, 0.023005,1) : isItem(entity) ? Vector4(0, 1, 1, 1) : Vector4(0, 0, 0, 1);
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
		if (isDoorSpawn(entity)) {
			lastdoor->DoorSpawns = collider->colliders;
			root->children[i-1]->addChild(collider);
		}
		collider->type = BOX;
		if (collider->layer == DOOR)collider->type = SPHERE;
		if (collider->layer == ITEM) {

		}
	
		if (isDoor(entity)||isKeyDoor(entity)) {

			lastdoor = static_cast<Door*>(entity);
			lastdoor->isInstanced = true;
			if (isDoor(lastdoor))lastdoor->MyType = NORMAL;
			else lastdoor->MyType = KEYED;
		}
		if (isFloor(entity)) {
			entity->material->diffuse = Texture::Get("data/textures/wood.tga");
		}
		if (isSpawner(entity)) {
			entity->canrender = false;
		}
		if (isItem(entity)) {
			entity->material->diffuse = Texture::Get("data/textures/Desk.tga");
		}
		
		root->children[i]->addChild(collider);
	}
	MapArea area;
	area.entities = root->children;
	Areas.push_back(area);
}
void World::Update(float deltaTime, Camera& camera) {
	root->update(deltaTime,camera);

	player->update(deltaTime, camera);
}
void World::Render(Camera& camera) {
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
		// Enable the shader

	skybox->render(&camera);

	root->render(&camera);
	player->render(&camera);
	

}
bool World::onKeyDown(SDL_KeyboardEvent event) {
	return true;
}
bool World::onMouseButtonDown(SDL_MouseButtonEvent event) {

	return true;
}