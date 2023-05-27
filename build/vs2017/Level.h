#pragma once
#include <map>

#include "CollisionManager.h"
#include "Player.h"
#include "Scene.h"
#include "SpriteAnimator3D.h"
#include "Camera.h"
#include "graphics/scene.h"
#include "Door.h"

class Text;
class Enemy;
class GameObject;

namespace gef
{
	class Platform;
	class Renderer3D;
	class Font;
	class SpriteRenderer;
}

class b2World;
class PrimitiveBuilder;
class Gun;

class Level : public Scene
{
public:
	Level(gef::Platform& platform, StateManager& state_manager) : Scene(platform, state_manager) {}
	void LoadFromFile(const char* filename, LoadingScreen* loading_screen);
	void Update(InputActionManager* iam_,float frame_time) override;
	void Render(gef::Renderer3D* renderer_3d) override;
	void Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font) override;
	void Render(gef::Renderer3D* renderer_3d, gef::SpriteRenderer* sprite_renderer, gef::Font* font) override;
	gef::Vector2 getPlayerPosition() const;
	const Player* getPlayer() const;
	const Gun* getGun() const;
	std::vector<GameObject*>& getBodiesToDestroy() {return objects_to_destroy_;}
	
private:
	enum HudElement
	{
		Ammo,
	};
	void Init();
	b2World* b2_world_;
	PrimitiveBuilder* primitive_builder_;
	
	std::vector<GameObject*> static_game_objects_;
	std::vector<GameObject*> dynamic_game_objects_;
	std::vector<gef::MeshInstance*> background_objects_;
	std::vector<Door*> door_objects_;
	Player player_;
	std::vector<Enemy*> enemies_;
	SpriteAnimator3D* sprite_animator3D_;
	gef::Scene scene_loader_;

	std::vector<GameObject*> objects_to_destroy_;
	
	CollisionManager collision_manager_;

	Camera camera_;

	//HUD
	std::map<HudElement, Text*> hud_text_;
};
