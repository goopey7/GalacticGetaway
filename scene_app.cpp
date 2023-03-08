#include "scene_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <maths/math_utils.h>
#include <input/input_manager.h>
#include <input/keyboard.h>

SceneApp::SceneApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	renderer_3d_(NULL),
	primitive_builder_(NULL),
	font_(NULL)
{
}

void SceneApp::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);

	// create the renderer for draw 3D geometry
	renderer_3d_ = gef::Renderer3D::Create(platform_);

	// create input manager
	input_ = gef::InputManager::Create(platform_);

	// initialise primitive builder to make create some 3D geometry easier
	primitive_builder_ = new PrimitiveBuilder(platform_);

	// initialise box2d world
	b2_world_ = new b2World(b2Vec2(0.f, -9.8));
	//b2_world_->SetAllowSleeping(false);

	// setup the player
	player_.Init(0.5f, 0.5f, 0.5f, 0, 4, b2_world_, primitive_builder_);

	// setup feasibility demo level
	ground_.Init(10.f, 0.5f, 0.5f, 0, -5.f, b2_world_, primitive_builder_);
	ceiling_.Init(10.f, 0.5f, 0.5f, 0, 5.f, b2_world_, primitive_builder_);
	wall_left_.Init(0.5f, 5.5f, 0.5f, -10.5f, 0, b2_world_, primitive_builder_);
	wall_right_.Init(0.5f, 5.5f, 0.5f, 10.5f, 0, b2_world_, primitive_builder_);
	crate_.Init(0.3f, 0.3f, 0.3f, 5, 4, b2_world_, primitive_builder_, true);

	InitFont();
	SetupLights();
}

void SceneApp::CleanUp()
{
	CleanUpFont();

	delete primitive_builder_;
	primitive_builder_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;
}

bool SceneApp::Update(float frame_time)
{
	input_->Update();
	if (input_->keyboard()->IsKeyDown(gef::Keyboard::KC_ESCAPE)) return false;

	fps_ = 1.0f / frame_time;

	b2_world_->Step(1.f / 165.f, 6, 2);

	player_.Update(input_, frame_time);
	crate_.Update();

	b2_world_->SetAllowSleeping(true);
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
	gef::Vector4 camera_eye(0, 0, 20.0f);
	gef::Vector4 camera_lookat(0.0f, 0.0f, 0.0f);
	gef::Vector4 camera_up(0.0f, 1.0f, 0.0f);
	gef::Matrix44 view_matrix;
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	renderer_3d_->set_view_matrix(view_matrix);


	// draw 3d geometry
	renderer_3d_->Begin();

	renderer_3d_->set_override_material(&primitive_builder_->red_material());
	renderer_3d_->DrawMesh(player_);

	renderer_3d_->set_override_material(&primitive_builder_->blue_material());
	renderer_3d_->DrawMesh(ground_);
	renderer_3d_->DrawMesh(ceiling_);
	renderer_3d_->DrawMesh(wall_left_);
	renderer_3d_->DrawMesh(wall_right_);

	renderer_3d_->set_override_material(&primitive_builder_->green_material());
	renderer_3d_->DrawMesh(crate_);

	renderer_3d_->set_override_material(NULL);

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
	if(font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);

		// display if player gravity lock is on 
		player_.GetGravityLock() ? gravity_lock_ = "On" : gravity_lock_ = "Off";
		font_->RenderText(sprite_renderer_, gef::Vector4(platform_.width()/2, 510.f, -0.9f), 1.0f, 0xffffffff, gef::TJ_CENTRE, "F - Gravity lock: %s", gravity_lock_.c_str());
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
