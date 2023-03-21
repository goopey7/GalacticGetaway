#include "scene_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <maths/math_utils.h>
#include <input/input_manager.h>
#include <input/keyboard.h>
#include <input/touch_input_manager.h>
#include "InputActionManager.h"
#include "obj_mesh_loader.h"
#include <string>

#include "Level.h"

SceneApp::SceneApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	renderer_3d_(NULL),
	font_(NULL)
{
}

void SceneApp::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);

	// create the renderer for draw 3D geometry
	renderer_3d_ = gef::Renderer3D::Create(platform_);

	// initialise input action manager
	iam_ = new InputActionManager(platform_);
	if (iam_->getInputManager() && iam_->getInputManager()->touch_manager() && (iam_->getInputManager()->touch_manager()->max_num_panels() > 0)) {
		iam_->getInputManager()->touch_manager()->EnablePanel(0);
	}

	level_ = new Level(platform_);
	level_->LoadFromFile("level.json");

	//level_->GetB2World()->SetAllowSleeping(false);

	// setup the player and crate

	/*
	crate_.Init(0.6f, 0.6f, 0.6f, 10, 4, level_->GetB2World(), level_->GetPrimitiveBuilder(), true);
	*/

	InitFont();
	SetupLights();
}

void SceneApp::CleanUp()
{
	CleanUpFont();

	/*
	delete level_->GetPrimitiveBuilder();
	level_->GetPrimitiveBuilder() = NULL;
	*/

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;
}

bool SceneApp::Update(float frame_time)
{
	iam_->Update();
	if (iam_->isPressed(Quit)) return false;

	fps_ = 1.0f / frame_time;

	level_->Update(iam_, frame_time);
	//crate_.Update();

	return true;
}

void SceneApp::Render()
{
	// setup camera

	// projection
	float fov = gef::DegToRad(45.0f);
	float aspect_ratio = (float)platform_.width() / (float)platform_.height();
	gef::Matrix44 projection_matrix;
	projection_matrix = platform_.PerspectiveProjectionFov(fov, aspect_ratio, 0.1f, 100.0f);
	renderer_3d_->set_projection_matrix(projection_matrix);

	// view
	gef::Vector2 player_pos = level_->getPlayerPosition();
	gef::Vector4 camera_eye(player_pos.x, player_pos.y, 30.0f);
	gef::Vector4 camera_lookat(player_pos.x, player_pos.y, 0.0f);
	gef::Vector4 camera_up(0.0f, 1.0f, 0.0f);
	gef::Matrix44 view_matrix;
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	renderer_3d_->set_view_matrix(view_matrix);

	// draw 3d geometry
	renderer_3d_->Begin();
	level_->Render(renderer_3d_);
	renderer_3d_->End();

	// start drawing sprites, but don't clear the frame buffer
	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();
}
void SceneApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void SceneApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void SceneApp::DrawHUD()
{
	if (font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(0.f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);

		// display if player gravity lock is on 
		level_->getPlayer()->GetGravityLock() ? gravity_lock_ = "On" : gravity_lock_ = "Off";
		font_->RenderText(sprite_renderer_, gef::Vector4(platform_.width() / 2, 510.f, -0.9f), 1.0f, 0xffffffff, gef::TJ_CENTRE, "F - Gravity lock: %s", gravity_lock_.c_str());
		
		font_->RenderText(sprite_renderer_, gef::Vector4(platform_.width() / 2 + 400.f, 510.f, -0.9f), 1.0f, 0xffffffff, gef::TJ_RIGHT, "Ammo: %i/%i", level_->getGun()->getAmmoLoaded(), level_->getGun()->getAmmoReserve());

		std::string reloading = level_->getGun()->getReloading() ? "Reloading" : "";
		font_->RenderText(sprite_renderer_, gef::Vector4(platform_.width() / 2 + 400.f, 480.f, -0.9f), 1.0f, 0xffffffff, gef::TJ_RIGHT, reloading.c_str());

		std::string position = "X: " + std::to_string(level_->getPlayerPosition().x) + " Y: " + std::to_string(level_->getPlayerPosition().y);
		font_->RenderText(sprite_renderer_, gef::Vector4(platform_.width() / 2 + 400.f, 200.f, -0.9f), 1.0f, 0xffffffff, gef::TJ_RIGHT, position.c_str());

	}
}

void SceneApp::SetupLights()
{
	// grab the data for the default shader used for rendering 3D geometry
	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();

	// set the ambient light
	default_shader_data.set_ambient_light_colour(gef::Colour(0.25f, 0.25f, 0.25f, 1.0f));

	// add a point light that is almost white, but with a blue tinge
	// the position of the light is set far away so it acts light a directional light
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-500.0f, 400.0f, 700.0f));
	default_shader_data.AddPointLight(default_point_light);
}
