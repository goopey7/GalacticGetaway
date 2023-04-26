#pragma once
#include <vector>

#include "CollisionManager.h"
#include "Player.h"

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

class Level
{
public:
	Level(gef::Platform& platform);
	void LoadFromFile(const char* filename);
	void Update(InputActionManager* iam_,float frame_time);
	void Render(gef::Renderer3D* renderer_3d);
	gef::Vector2 getPlayerPosition() const;
	const Player* getPlayer() const;
	const Gun* getGun() const;
private:
	void Init();
	b2World* b2_world_;
	PrimitiveBuilder* primitive_builder_;
	
	gef::Platform* platform_;

	std::vector<GameObject*> static_game_objects_;
	std::vector<GameObject*> dynamic_game_objects_;
	Player player_;
	
	CollisionManager collision_manager_;
};
