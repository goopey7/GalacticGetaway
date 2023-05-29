﻿#pragma once
#include <condition_variable>
#include <memory>
#include <queue>

#include "obj_mesh_loader.h"

namespace gef
{
	class AudioManager;
}

class Menu;
class LoadingScreen;
class Level;

namespace gef
{
	class Font;
	class SpriteRenderer;
	class Renderer3D;
}

class InputActionManager;
class Scene;

class StateManager
{
public:
	StateManager(LoadingScreen* loading_screen, bool* should_run, gef::AudioManager* audio_manager,
				gef::Platform* platform);
	void SetMainMenu(Menu* main_menu) {main_menu_ = main_menu;}
	void Update(InputActionManager* iam, float frame_time);
	void Render(gef::Renderer3D* renderer_3d);
	void Render(gef::Renderer3D* renderer_3d, gef::SpriteRenderer* sprite_renderer, gef::Font* font);
	void PushScene(Scene* scene);
	void PushLevel(Level* level, const char* file_name, OBJMeshLoader& mesh_loader);
	void Pause();
	void Unpause();
	
	Scene* NextScene();
	void SwitchToMainMenu();
	void SetPauseMenu(Menu* pause_menu);
	void SetShouldRun(bool should_run) { *should_run_ = should_run; }

private:
	Scene* current_scene_ = nullptr;
	std::queue<Scene*> scenes_;
	
	std::atomic<bool> is_loading_ = false;
	std::thread loading_thread_;
	LoadingScreen* loading_screen_;
	Menu* main_menu_;
	bool on_main_menu_ = true;
	Menu* pause_menu_ = nullptr;
	bool* should_run_;

	gef::AudioManager* audio_manager_ = nullptr;
	gef::Platform* platform_ = nullptr;
};
