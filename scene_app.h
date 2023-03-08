#ifndef _SCENE_APP_H
#define _SCENE_APP_H

#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <box2d/box2d.h>
#include "GameObject.h"
#include "Player.h"
#include <string>

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}

class SceneApp : public gef::Application
{
public:
	SceneApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
private:
	void InitFont();
	void CleanUpFont();
	void DrawHUD();
	void SetupLights();

	gef::InputManager* input_;

	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::Renderer3D* renderer_3d_;

	PrimitiveBuilder* primitive_builder_;

	b2World* b2_world_;

	//Feasibility Demo player and level
	Player player_;
	GameObject ground_;
	GameObject ceiling_;
	GameObject wall_left_;
	GameObject wall_right_;
	GameObject crate_;

	float fps_;
	std::string gravity_lock_;
};

#endif // _SCENE_APP_H
