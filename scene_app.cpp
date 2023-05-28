#include "scene_app.h"

#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <graphics/renderer_3d.h>
#include <maths/math_utils.h>
#include <input/input_manager.h>
#include <input/touch_input_manager.h>
#include <platform/d3d11/system/platform_d3d11.h>
#include <string>

#include "Button.h"
#include "InputActionManager.h"
#include "StateManager.h"
#include "Level.h"
#include "LoadingScreen.h"
#include "Menu.h"
#include "Text.h"

SceneApp::SceneApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	renderer_3d_(NULL),
	font_(NULL)
{
	platform_d3d_ = reinterpret_cast<gef::PlatformD3D11*>(&platform);
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

	LoadingScreen* loading_screen = new LoadingScreen(platform_, *state_manager_);
	loading_screen->SetStatusText("Loading...");
	state_manager_ = new StateManager(loading_screen);
	
	// MAIN MENU
	Menu* menu = new Menu(platform_, *state_manager_, false);
	state_manager_->SetMainMenu(menu);
	menu->AddUIElement(new Text({0.5,0.25}, "Main Menu"));
	Button* menuStartButton = new Button({0.5,0.4}, platform_, "Start", 200.f, 50.f, gef::Colour(1,1,1,1));
	Button* menuEnemyButton = new Button({0.5,0.5}, platform_, "Enemy Testing", 200.f, 50.f, gef::Colour(1,1,0,1));
	Button* menuPuzzleButton = new Button({0.5,0.6}, platform_, "Puzzle Testing", 200.f, 50.f, gef::Colour(1,1,0,1));
	Button* quitButton = new Button({0.5,0.7}, platform_, "Quit", 200.f, 50.f, gef::Colour(1,0,0,1));
	menuStartButton->SetOnClick([this]
	{
		state_manager_->PushLevel(new Level(platform_, *state_manager_), "level.json", mesh_loader_);
		state_manager_->NextScene();
	});
	menuEnemyButton->SetOnClick([this]
	{
		state_manager_->PushLevel(new Level(platform_, *state_manager_), "enemy.json", mesh_loader_);
		state_manager_->NextScene();
	});
	menuPuzzleButton->SetOnClick([this]
	{
		state_manager_->PushLevel(new Level(platform_, *state_manager_), "puzzle.json", mesh_loader_);
		state_manager_->NextScene();
	});
	quitButton->SetOnClick([this]
	{
		should_run_ = false;
	});
	menu->AddUIElement(menuStartButton);
	menu->AddUIElement(menuEnemyButton);
	menu->AddUIElement(menuPuzzleButton);
	menu->AddUIElement(quitButton);

	// PAUSE MENU
	Menu* pause = new Menu(platform_, *state_manager_, true);
	state_manager_->SetPauseMenu(pause);
	pause->AddUIElement(new Text({0.5,0.25}, "Paused"));
	Button* resumeButton = new Button({0.5,0.5}, platform_, "Resume", 200.f, 50.f, gef::Colour(1,1,1,1));
	resumeButton->SetOnClick([this]
	{
		state_manager_->Unpause();
	});
	pause->AddUIElement(resumeButton);
	Button* mainMenuButton = new Button({0.5,0.6}, platform_, "Main Menu", 200.f, 50.f, gef::Colour(1,1,1,1));
	mainMenuButton->SetOnClick([this]
	{
		state_manager_->SwitchToMainMenu();
	});
	pause->AddUIElement(mainMenuButton);
	pause->AddUIElement(quitButton);
	//Level* next_level = new Level(platform_, *state_manager_);

	
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
	// CAUSES MEMORY LEAK
	/*
	if(platform_d3d_)
	{
		RECT rect;
		GetClientRect(platform_d3d_->hwnd(), &rect);
		float width = rect.right - rect.left;
		float height = rect.bottom - rect.top;
		if(currentWidth != width || currentHeight != height)
		{
			platform_d3d_->Resize(width, height);
			sprite_renderer_->set_projection_matrix(platform_.OrthographicFrustum(0.0f, (float)platform_.width(), 0.0f, (float)platform_.height(), -1.0f, 1.0f));
		}
	}
	*/

	iam_->Update();
	//if (iam_->isPressed(Quit)) return false;

	fps_ = 1.0f / frame_time;

	state_manager_->Update(iam_, frame_time);
	//crate_.Update();

	return should_run_;
}

void SceneApp::Render()
{
	// draw current scene
	state_manager_->Render(renderer_3d_, sprite_renderer_, font_);

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
