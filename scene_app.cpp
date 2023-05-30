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
#include <chrono>
#include "Button.h"
#include "Image.h"
#include "InputActionManager.h"
#include "StateManager.h"
#include "Level.h"
#include "LoadingScreen.h"
#include "Menu.h"
#include "SplashScreen.h"
#include "Text.h"
#include "audio/audio_manager.h"
#include "graphics/image_data.h"
#include "graphics/texture.h"

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

	audio_manager_ = gef::AudioManager::Create();
	audio_manager_->LoadSample("sounds/MaxAmmo.ogg", platform_); // made in house
	gef::VolumeInfo volume_info;
	audio_manager_->GetSampleVoiceVolumeInfo(0, volume_info);
	volume_info.volume = 200.f;
	audio_manager_->SetSampleVoiceVolumeInfo(0, volume_info);
	audio_manager_->LoadSample("sounds/MaxAmmo.ogg", platform_);
	audio_manager_->SetSampleVoiceVolumeInfo(1, volume_info);
	//audio_manager_->LoadSample("sounds/Health.ogg", *platform_);
	audio_manager_->LoadSample("sounds/lazer.ogg", platform_); // found here: https://www.soundfishing.eu/sound/laser-gun
	audio_manager_->LoadSample("sounds/enemy_lazer.ogg", platform_); // found here: https://www.soundfishing.eu/sound/laser-gun
	audio_manager_->LoadSample("sounds/player_death.ogg", platform_); // found here: https://pixabay.com/sound-effects/search/death/?pagi=2
	audio_manager_->LoadSample("sounds/enemy_death.ogg", platform_); // found here: https://pixabay.com/sound-effects/search/death/?pagi=2
	audio_manager_->LoadSample("sounds/door.ogg", platform_); // found here: https://pixabay.com/sound-effects/search/death/?pagi=2

	// initialise input action manager
	iam_ = new InputActionManager(platform_);
	if (iam_->getInputManager() && iam_->getInputManager()->touch_manager() && (iam_->getInputManager()->touch_manager()->max_num_panels() > 0)) {
		iam_->getInputManager()->touch_manager()->EnablePanel(0);
	}

	// LOADING SCREEN
	LoadingScreen* loading_screen = new LoadingScreen(platform_, *state_manager_);
	loading_screen->SetStatusText("Loading...");
	state_manager_ = new StateManager(loading_screen, &should_run_, audio_manager_, &platform_);

	gef::Sprite* menuBkg = new gef::Sprite();
	gef::ImageData menu_img_data("space.png");
	menuBkg->set_texture(gef::Texture::Create(platform_, menu_img_data));
	menuBkg->set_width(platform_.width());
	menuBkg->set_height(platform_.height());
	Image* menuBkg_img = new Image({0.5f,0.5f}, menuBkg, platform_);
	menuBkg_img->SetIgnoreAlpha(true);
	
	// SPLASH SCREEN
	SplashScreen* splash_screen = new SplashScreen(platform_, *state_manager_);

	splash_screen->SetBkg(menuBkg_img);
	
	splash_screen->AddUIElement(new Text({0.5,0.5}, "DarkSpace Studios Presents", platform_));
	
	gef::Sprite* splash1 = new gef::Sprite();
	gef::ImageData image_data("gg.png");
	splash1->set_texture(gef::Texture::Create(platform_, image_data));
	splash1->set_width(image_data.width()/2.f);
	splash1->set_height(image_data.height()/2.f);
	Image* splash_img = new Image({0.5,0.5}, splash1, platform_);
	splash_screen->AddUIElement(splash_img);
	state_manager_->SetSplashScreen(splash_screen);
	
	/*gef::Sprite* logo2_sprite = new gef::Sprite();
	gef::ImageData menu_image_data("musician.png");
	logo2_sprite->set_texture(gef::Texture::Create(platform_, menu_image_data));
	logo2_sprite->set_width(menu_image_data.width());
	logo2_sprite->set_height(menu_image_data.height());
	Image* logo2 = new Image({0.5,0.5}, logo2_sprite, platform_);
	splash_screen->AddUIElement(logo2);*/
	
	// SETTINGS MENU
	Menu* settings_menu = new Menu(platform_, *state_manager_, false);
	
	settings_menu->AddUIElement(menuBkg_img);
	state_manager_->SetSettingsMenu(settings_menu);
	settings_menu->AddUIElement(new Text({0.5,0.25}, "Settings"));
	Button* settingsBackButton = new Button({0.5,0.4}, platform_, "Back", 200.f, 50.f, gef::Colour(1,1,1,1));
	settingsBackButton->SetOnClick([this]
	{
		state_manager_->SwitchToMainMenu();
	});
	settings_menu->AddUIElement(settingsBackButton);

	settings_menu->AddUIElement(new Text({0.25,0.5}, "Music Volume"));
	Button* music_volume_down = new Button({0.4,0.5}, platform_, "-", 50.f, 50.f, gef::Colour(1,1,1,1));
	Text* music_volume_text = new Text({0.5,0.5}, "20", platform_);
	Button* music_volume_up = new Button({0.6,0.5}, platform_, "+", 50.f, 50.f, gef::Colour(1,1,1,1));
	music_volume_down->SetOnClick([this, music_volume_text]
	{
		std::string text = music_volume_text->GetText();
		int volume = std::stoi(text) - 10;
		if(volume - 10 < 0)
		{
			volume = 0;
		}
		gef::VolumeInfo vi;
		audio_manager_->GetMusicVolumeInfo(vi);
		vi.volume = volume;
		audio_manager_->SetMusicVolumeInfo(vi);
		text = std::to_string(volume);
		music_volume_text->UpdateText(text.c_str());
	});
	music_volume_up->SetOnClick([this, music_volume_text]
	{
		std::string text = music_volume_text->GetText();
		int volume = std::stoi(text) + 10;
		if(volume + 10 > 100)
		{
			volume = 100;
		}
		gef::VolumeInfo vi;
		audio_manager_->GetMusicVolumeInfo(vi);
		vi.volume = volume;
		audio_manager_->SetMusicVolumeInfo(vi);
		text = std::to_string(volume);
		music_volume_text->UpdateText(text.c_str());
	});
	settings_menu->AddUIElement(music_volume_down);
	settings_menu->AddUIElement(music_volume_text);
	settings_menu->AddUIElement(music_volume_up);

	settings_menu->AddUIElement(new Text({0.25,0.6}, "SFX Volume"));
	Button* sfx_volume_down = new Button({0.4,0.6}, platform_, "-", 50.f, 50.f, gef::Colour(1,1,1,1));
	Text* sfx_volume_text = new Text({0.5,0.6}, "20", platform_);
	Button* sfx_volume_up = new Button({0.6,0.6}, platform_, "+", 50.f, 50.f, gef::Colour(1,1,1,1));
	sfx_volume_down->SetOnClick([this, sfx_volume_text]
	{
		std::string text = sfx_volume_text->GetText();
		int volume = std::stoi(text) - 10;
		if(volume - 10 < 0)
		{
			volume = 0;
		}
		for(int i=0; i < 7; i++)
		{
			gef::VolumeInfo vi;
			audio_manager_->GetSampleVoiceVolumeInfo(i, vi);
			vi.volume = volume;
			audio_manager_->SetSampleVoiceVolumeInfo(i, vi);
		}
		text = std::to_string(volume);
		sfx_volume_text->UpdateText(text.c_str());
	});

	sfx_volume_up->SetOnClick([this, sfx_volume_text]
	{
		std::string text = sfx_volume_text->GetText();
		int volume = std::stoi(text) + 10;
		if(volume + 10 > 100)
		{
			volume = 100;
		}
		for(int i=0; i < 7; i++)
		{
			gef::VolumeInfo vi;
			audio_manager_->GetSampleVoiceVolumeInfo(i, vi);
			vi.volume = volume;
			audio_manager_->SetSampleVoiceVolumeInfo(i, vi);
		}
		text = std::to_string(volume);
		sfx_volume_text->UpdateText(text.c_str());
	});
	settings_menu->AddUIElement(sfx_volume_down);
	settings_menu->AddUIElement(sfx_volume_text);
	settings_menu->AddUIElement(sfx_volume_up);
	
	// MAIN MENU
	Menu* menu = new Menu(platform_, *state_manager_, false);
	menu->AddUIElement(menuBkg_img);
	state_manager_->SetMainMenu(menu);
	//menu->AddUIElement(new Text({0.5,0.25}, "Galactic Getaway"));

	gef::Sprite* logo = new gef::Sprite();
	gef::ImageData logo_image_data("logo.png");
	logo->set_texture(gef::Texture::Create(platform_, logo_image_data));
	logo->set_width(logo_image_data.width());
	logo->set_height(logo_image_data.height());
	Image* logo_img = new Image({ 0.5,0.35 }, logo, platform_);
	menu->AddUIElement(logo_img);

	Button* menuStartButton = new Button({0.5,0.5}, platform_, "Start", 200.f, 50.f, gef::Colour(1,1,1,1));
	Button* menuSettingsButton = new Button({0.5,0.6}, platform_, "Settings", 200.f, 50.f, gef::Colour(1,1,1,1));
	Button* quitButton = new Button({0.5,0.7}, platform_, "Quit", 200.f, 50.f, gef::Colour(1,0,0,1));
	menuStartButton->SetOnClick([this]
	{
		state_manager_->PushLevel(new Level(platform_, sprite_renderer_, font_, *state_manager_, audio_manager_ ), "lvl_1.json", mesh_loader_);
		state_manager_->NextScene();
	});
	menuSettingsButton->SetOnClick([this]
	{
		state_manager_->SwitchToSettingsMenu();
	});
	quitButton->SetOnClick([this]
	{
		should_run_ = false;
	});
	menu->AddUIElement(menuStartButton);
	menu->AddUIElement(menuSettingsButton);
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
	font_->Load("ranger");
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
