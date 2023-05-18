#include "StateManager.h"

#include "Level.h"
#include "Scene.h"

void StateManager::Update(InputActionManager* iam, float frame_time)
{
	current_scene_->Update(iam, frame_time);
}

void StateManager::Render(gef::Renderer3D* renderer_3d)
{
	current_scene_->Render(renderer_3d);
}

void StateManager::Render(gef::Renderer3D* renderer_3d, gef::SpriteRenderer* sprite_renderer, gef::Font* font)
{
	current_scene_->Render(renderer_3d, sprite_renderer, font);
}

void StateManager::PushScene(Scene* scene)
{
	scenes_.push(scene);
	current_scene_ = scenes_.front();
}

void StateManager::PushLevel(Level* level, const char* file_name)
{
	level->LoadFromFile(file_name);
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
