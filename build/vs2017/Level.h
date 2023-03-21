#pragma once
#include <vector>

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

class Level
{
public:
	Level(gef::Platform& platform);
	void LoadFromFile(const char* filename);
private:
	void Init();
	b2World* b2_world_;
	PrimitiveBuilder* primitive_builder_;
	
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::Renderer3D* renderer_3d_;
	gef::Platform* platform_;

	std::vector<GameObject*> game_objects_;
};
