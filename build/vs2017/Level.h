#pragma once
#include <map>
#include <vector>
#include "CollisionManager.h"
#include "Player.h"
#include "Scene.h"
#include "SpriteAnimator3D.h"
#include "Camera.h"
#include "graphics/scene.h"
#include "Door.h"
#include "Image.h"
#include "obj_mesh_loader.h"

class Menu;
class Text;
class Enemy;
class GameObject;

enum EndState { NONE, WIN, LOSE };

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
	Level(gef::Platform& platform, gef::SpriteRenderer* sr, gef::Font* font, StateManager& state_manager, gef::AudioManager* am) : Scene(platform, state_manager), audio_manager_(am), sprite_renderer_(sr), font_(font) {}
	~Level();
	void LoadFromFile(const char* filename, LoadingScreen* loading_screen, OBJMeshLoader& obj_loader);
	void CleanUp();
	void Update(InputActionManager* iam_,float frame_time) override;
	void Render(gef::Renderer3D* renderer_3d) override;
	void Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font) override;
	void Render(gef::Renderer3D* renderer_3d, gef::SpriteRenderer* sprite_renderer, gef::Font* font) override;
	void SetPauseMenu(Menu* pause_menu) {pause_menu_ = pause_menu;}
	void Pause() {is_paused_ = true;}
	void Unpause() {is_paused_ = false;}
	gef::Vector2 getPlayerPosition() const;
	const Player* getPlayer() const;
	const Gun* getGun() const;
	std::vector<GameObject*>& getBodiesToDestroy() {return objects_to_destroy_;}
	void SetEndState(EndState end_state) { end_state_ = end_state; }
	const char* GetFileName() const;

private:
	void LoadObject(auto obj, MeshResource mr, OBJMeshLoader& obj_loader, gef::Vector4& scale);

	enum HudElement
	{
		Ammo,
		EndText,
		GravLock
	};
	void Init();
	b2World* b2_world_;
	PrimitiveBuilder* primitive_builder_;
	
	Camera camera_;

	//Objects
	std::vector<GameObject*> static_game_objects_;
	std::vector<GameObject*> dynamic_game_objects_;
	std::vector<gef::MeshInstance*> background_objects_;
	std::unordered_map<int, Door*> door_objects_;
	Player player_;
	std::vector<Enemy*> enemies_;

	//Scene loading etc
	SpriteAnimator3D* sprite_animator3D_;
	gef::Scene scene_loader_;
	EndState end_state_ = NONE;
	std::vector<GameObject*> objects_to_destroy_;
	CollisionManager collision_manager_;
	const char* file_name_ = nullptr;
	OBJMeshLoader* obj_loader_ = nullptr;

	//HUD
	std::map<HudElement, Text*> hud_text_;
	std::vector<Image> healthbar_;
	gef::SpriteRenderer* sprite_renderer_ = nullptr;
	gef::Font* font_ = nullptr;
	Menu* pause_menu_ = nullptr;
	bool is_paused_ = false;

	//Audio
	gef::AudioManager* audio_manager_ = nullptr;
};
