#pragma once
#include <condition_variable>
#include <memory>
#include <queue>

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
	StateManager(LoadingScreen* loading_screen);
	void Update(InputActionManager* iam, float frame_time);
	void Render(gef::Renderer3D* renderer_3d);
	void Render(gef::Renderer3D* renderer_3d, gef::SpriteRenderer* sprite_renderer, gef::Font* font);
	void PushScene(Scene* scene);
	void PushLevel(Level* level, const char* file_name);
	Scene* NextScene();
private:
	Scene* current_scene_ = nullptr;
	std::queue<Scene*> scenes_;
	
	std::atomic<bool> is_loading_ = false;
	std::thread loading_thread_;
	LoadingScreen* loading_screen_;
};
