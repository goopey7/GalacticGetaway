#include "StateManager.h"

#include "Level.h"
#include "LoadingScreen.h"
#include "Scene.h"

StateManager::StateManager(LoadingScreen* loading_screen)
	: loading_screen_(loading_screen)
{
}

void StateManager::Update(InputActionManager* iam, float frame_time)
{
	if(is_loading_)
	{
		loading_screen_->Update(iam, frame_time);
	}
	else
	{
		current_scene_->Update(iam, frame_time);
	}
}

void StateManager::Render(gef::Renderer3D* renderer_3d)
{
	if(is_loading_)
	{
		loading_screen_->Render(renderer_3d);
	}
	else
	{
		current_scene_->Render(renderer_3d);
	}
}

void StateManager::Render(gef::Renderer3D* renderer_3d, gef::SpriteRenderer* sprite_renderer, gef::Font* font)
{
	if(is_loading_)
	{
		loading_screen_->Render(renderer_3d, sprite_renderer, font);
	}
	else
	{
		current_scene_->Render(renderer_3d, sprite_renderer, font);
	}
}

void StateManager::PushScene(Scene* scene)
{
	scenes_.push(scene);
	current_scene_ = scenes_.front();
}

void StateManager::PushLevel(Level* level, const char* file_name)
{
	loading_thread_ = std::thread([this, level, file_name]
	{
		is_loading_ = true;
		level->LoadFromFile(file_name, loading_screen_);
		is_loading_ = false;
	});
	scenes_.push(level);
	current_scene_ = scenes_.front();
}

Scene* StateManager::NextScene()
{
	Scene* oldScene = current_scene_;
	scenes_.pop();
	current_scene_ = scenes_.front();
	return oldScene;
}
