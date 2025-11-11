
#pragma once
#include "framework/includes.h"
#include "framework/camera.h"
#include "framework/utils.h"
#include "framework/pulse.h"
#include "framework/includes.h"
#include "framework/framework.h"
#include "graphics/mesh.h"
#include "graphics/material.h"
#include "graphics/shader.h"
#include "graphics/texture.h"

#include "framework/entities/entity.h"
#include "framework/scene_parser.h"
#include "framework/entities/player.h"

#ifndef WORLD_H
#define WORLD_H



struct MapArea
{
    BoundingBox limits;
    std::vector<Entity*> entities;
};




class Player;
class World {
public:
    static World* DayMap;
    static World* NightMap;
	Entity* root;
    EntityMesh* skybox;
    Player* player;
    std::vector<MapArea>Areas;
	void Init(const char* rootname);
	void Update(float deltaTime, Camera& camera);
	void Render(Camera& camera);
    bool onKeyDown(SDL_KeyboardEvent event);
    bool onKeyUp(SDL_KeyboardEvent event) { return false; }
    bool onMouseButtonDown(SDL_MouseButtonEvent event);
    bool onMouseButtonUp(SDL_MouseButtonEvent event) { return false; }
    bool onMouseMove(SDL_MouseMotionEvent event) { return false; }
    bool onMouseWheel(SDL_MouseWheelEvent event) { return false; }/*
    virtual bool onGamepadButtonDown(SDL_JoyButtonEvent event) { return false; }
    virtual bool onGamepadButtonUp(SDL_JoyButtonEvent event) { return false; }
    virtual bool onResize(int width, int height) { return false; }*/
};

#endif