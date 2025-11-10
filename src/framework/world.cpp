#include "framework/world.h"

void World::Init(const char* rootname) {
	SceneParser parser;

	root = new Entity();
	parser.parse(rootname, root);
	Texture* cubetexture = new Texture();
	{
		cubetexture->loadCubemap("landscape", { "data/shaders/skybox/px.png","data/shaders/skybox/nx.png","data/shaders/skybox/ny.png","data/shaders/skybox/py.png","data/shaders/skybox/pz.png","data/shaders/skybox/nz.png" });
	}
	Material cubeMat;
	cubeMat.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/boxtexture.fs");
	cubeMat.diffuse = cubetexture;
	skybox = new EntityMesh(Mesh::Get("data/meshes/box.ASE"), cubeMat);
	skybox->culling = false;
	
	Mesh* mesh = Mesh::Get("data/Rat/Cube.001/Cube.001.obj");
	Material mat;
	mat.color = Vector4(1, 1, 1, 1);
	mat.diffuse = Texture::Get("data/textures/rat.tga");
	player = new Player(mesh,mat);
	Matrix44 model;
	model.setTranslation(0.0f, 0.0f, 0.0f);   // posición inicial
	model.scale(0.1f, 0.1f, 0.1f);
	player->model = model;
	player->canrender = false;
	root->addChild(player);
}
void World::Update(float deltaTime, Camera& camera) {
	player->update(deltaTime, camera);
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