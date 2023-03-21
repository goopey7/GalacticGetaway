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

class Level;
class InputActionManager;

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Font;
	class Platform;
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

	//Feasibility Demo player and level
	GameObject crate_;
	
	gef::Font* font_;
	gef::Renderer3D* renderer_3d_;
	gef::SpriteRenderer* sprite_renderer_;

	//Input Action Manager
	InputActionManager* iam_;

	Level* level_;

	float fps_;
	std::string gravity_lock_;
};

#endif // _SCENE_APP_H
