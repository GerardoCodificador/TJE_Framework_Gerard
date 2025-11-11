
#pragma once

#ifndef STAGE_H
#define STAGE_H
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
#include "framework/world.h"
enum eStage {
	STAGE_MENU,
	STAGE_GAMEDAY,
	STAGE_GAMENIGHT
};
class Stage{

public:
    bool mouse_locked;
    virtual void Init() {}
	virtual ~Stage() {}
    virtual void OnEnter(Stage* last_stage){}
    virtual void OnExit(Stage* last_stage){}
    virtual  void Update(float deltaTime, Camera& camera) {}
    // virtual void Render(Image& framebuffer){
    virtual void Render(Camera& camera){}


    virtual bool onKeyDown(SDL_KeyboardEvent event) { return false; }
    virtual bool onKeyUp(SDL_KeyboardEvent event) { return false; }
    virtual bool onMouseButtonDown(SDL_MouseButtonEvent event) { return false; }
    virtual bool onMouseButtonUp(SDL_MouseButtonEvent event) { return false; }
    virtual bool onMouseMove(SDL_MouseMotionEvent event) { return false; }
    virtual bool onMouseWheel(SDL_MouseWheelEvent event) { return false; }/*
    virtual bool onGamepadButtonDown(SDL_JoyButtonEvent event) { return false; }
    virtual bool onGamepadButtonUp(SDL_JoyButtonEvent event) { return false; }
    virtual bool onResize(int width, int height) { return false; }*/


};

class MenuStage : public Stage{
public:

    Image background;

    Image font;
    Image minifontb,minifontw;
   
    void Init() override;
    void OnEnter(Stage* last_stage) override;
    void OnExit(Stage* last_stage) override;
    void Update(float deltaTime, Camera& camera) override;
    void Render(Camera& camera) override;

    
    bool onKeyDown(SDL_KeyboardEvent event) override;
    
   /* bool onKeyUp(SDL_KeyboardEvent event) override;
    bool onMouseButtonDown(SDL_MouseButtonEvent event) override;
    bool onMouseButtonUp(SDL_MouseButtonEvent event) override;
    bool onMouseMove(SDL_MouseMotionEvent event) override;
    void onMouseWheel(SDL_MouseWheelEvent event) override;
    void onGamepadButtonDown(SDL_JoyButtonEvent event) override;
    void onGamepadButtonUp(SDL_JoyButtonEvent event) override;
    void onResize(int width, int height) override;*/
};
class GameDayStage : public Stage {
public:


    Image font;
    Image minifont;

    bool mouse_locked; //tells if the mouse is locked (not seen)

    void Init() override;
    void OnEnter(Stage* last_stage) override;
    void OnExit(Stage* last_stage) override;
    void Update(float deltaTime,Camera& camera) override;
    void Render(Camera& camera) override;
    bool onKeyDown(SDL_KeyboardEvent event) override;
    bool onMouseWheel(SDL_MouseWheelEvent event) override;
};

class GameNightStage : public Stage{
public:
    




    Image font;
    Image minifont;

    bool mouse_locked; //tells if the mouse is locked (not seen)


    void Init() override;
    void OnEnter(Stage* last_stage) override;
    void OnExit(Stage* last_stage) override;
    void Update(float deltaTime, Camera& camera) override;
    void Render(Camera& camera) override;

    bool CheckColisions(Vector2 pos, Vector2 size);
	
    
    bool onKeyDown(SDL_KeyboardEvent event) override;
    bool onMouseWheel(SDL_MouseWheelEvent event) override;
    /*bool onKeyUp(SDL_KeyboardEvent event) override;
    bool onMouseButtonDown(SDL_MouseButtonEvent event) override;
    bool onMouseButtonUp(SDL_MouseButtonEvent event) override;
    bool onMouseMove(SDL_MouseMotionEvent event) override;/*
    void onMouseWheel(SDL_MouseWheelEvent event) override;
    void onGamepadButtonDown(SDL_JoyButtonEvent event) override;
    void onGamepadButtonUp(SDL_JoyButtonEvent event) override;
    void onResize(int width, int height) override;*/
}   ;

#endif