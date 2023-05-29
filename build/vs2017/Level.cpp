#include "Level.h"

#include <d3d10.h>
#include <fstream>

#include "Enemy.h"
#include "GameObject.h"
#include "json.h"
#include "obj_mesh_loader.h"
#include "PressurePlate.h"
#include "primitive_builder.h"
#include "Text.h"
#include "box2d/b2_math.h"
#include "box2d/b2_world.h"
#include "graphics/font.h"
#include "graphics/sprite_renderer.h"
#include "maths/math_utils.h"
#include "system/debug_log.h"
#include "InputActionManager.h"
#include "input/sony_controller_input_manager.h"
#include "LoadingScreen.h"
#include "Menu.h"
#include "Button.h"
#include "audio/audio_manager.h"
#include "input/input_manager.h"

using nlohmann::json;

float fixY(float y)
{
	return y - 9.f;
}

Level::~Level()
{
	CleanUp();
}

void Level::LoadFromFile(const char* filename, LoadingScreen* loading_screen, OBJMeshLoader& obj_loader)
{
	obj_loader_ = &obj_loader;
	file_name_ = filename;
	loading_screen->SetStatusText("Reading level file...");
	// load level from file
	std::ifstream i(std::string("levels/") + std::string(filename));

	// handle error if file not found
	if (i.fail())
	{
		throw std::exception((std::string(filename)+ " not found").c_str());
	}
	
	// parse json into cpp object
	loading_screen->SetStatusText("Parsing level JSON...");
	json levelJson = json::parse(i);

	loading_screen->SetStatusText("Initializing level...");
	Init();
	camera_.GetBackground()->set_mesh(sprite_animator3D_->CreateMesh("space.png", gef::Vector4(960, 540, 0)));
	
	
	loading_screen->SetStatusText("Loading SFX...");
	audio_manager_->LoadSample("sounds/MaxAmmo.ogg", *platform_); // made in house
	gef::VolumeInfo volume_info;
	audio_manager_->GetSampleVoiceVolumeInfo(0, volume_info);
	volume_info.volume = 200.f;
	audio_manager_->SetSampleVoiceVolumeInfo(0, volume_info);
	audio_manager_->LoadSample("sounds/MaxAmmo.ogg", *platform_);
	audio_manager_->SetSampleVoiceVolumeInfo(1, volume_info);
	//audio_manager_->LoadSample("sounds/Health.ogg", *platform_);
	audio_manager_->LoadSample("sounds/lazer.ogg", *platform_); // found here: https://www.soundfishing.eu/sound/laser-gun
	audio_manager_->LoadSample("sounds/enemy_lazer.ogg", *platform_); // found here: https://www.soundfishing.eu/sound/laser-gun
	audio_manager_->LoadSample("sounds/player_death.ogg", *platform_); // found here: https://pixabay.com/sound-effects/search/death/?pagi=2
	audio_manager_->LoadSample("sounds/enemy_death.ogg", *platform_); // found here: https://pixabay.com/sound-effects/search/death/?pagi=2
	
	// LOAD 3D MODELS
	loading_screen->SetStatusText("Loading 3D meshes...");
	if(!obj_loader.Load(MeshResource::Level, "Models/Generic/crate2/crate2.obj", "Crate_1__Default_0", *platform_))
	{
		gef::DebugOut(obj_loader.GetLastError().c_str());
		gef::DebugOut("\n");
	}

	//Poole (2019) Sci-fi Crate V2. Available at: https://skfb.ly/6TNVo (Accessed: 21 March 2023)
	if (!obj_loader.Load(MeshResource::Crate, "Models/crate/crate.obj", "scificrate_low_lambert2_0", *platform_))
	{
		gef::DebugOut(obj_loader.GetLastError().c_str());
		gef::DebugOut("\n");
	}

	if (!obj_loader.Load(MeshResource::BackWall, "Models/Wall/Wall/new/untitled.obj", "defaultMaterial.001", *platform_))
	{
		gef::DebugOut(obj_loader.GetLastError().c_str());
		gef::DebugOut("\n");
	}

	if (!obj_loader.Load(MeshResource::DoorWall, "Models/Door/DoorWall.obj", "Crate_1__Default_0.001", *platform_))
	{
		gef::DebugOut(obj_loader.GetLastError().c_str());
		gef::DebugOut("\n");
	}

	if (!obj_loader.Load(MeshResource::DoorFrame, "Models/Door/DoorFrame.obj", "Door_Frame_Mat_Door_Main_0", *platform_))
	{
		gef::DebugOut(obj_loader.GetLastError().c_str());
		gef::DebugOut("\n");
	}

	if (!obj_loader.Load(MeshResource::Door, "Models/Door/Door.obj", "Door_Left_Mat_Door_Main_0", *platform_))
	{
		gef::DebugOut(obj_loader.GetLastError().c_str());
		gef::DebugOut("\n");
	}

	if (!obj_loader.Load(MeshResource::Reactor, "Models/Reactor/reactor.obj", "Reactor_Sci_Fi_TX_RT_Sci_Fi_0", *platform_))
	{
		gef::DebugOut(obj_loader.GetLastError().c_str());
		gef::DebugOut("\n");
	}


	/*scene_loader_.ReadSceneFromFile(*platform_, "Models/Window/window.scn");
	scene_loader_.CreateMaterials(*platform_);
	scene_loader_.CreateMeshes(*platform_);*/
	if (!obj_loader.Load(MeshResource::Window, "Models/Window2/window.obj", "defaultMaterial.001", *platform_))
	{
		gef::DebugOut(obj_loader.GetLastError().c_str());
		gef::DebugOut("\n");
	}
		
	// initialize all layers
	for(const auto& layer : levelJson["layers"])
	{
		if (layer["type"] == "objectgroup")
		{
			if(layer["name"] == "StaticLevelCollisions")
			{
				loading_screen->SetStatusText("Creating static game objects...");
				gef::Mesh* new_mesh;
				gef::Vector4 old_scale;

				for(const auto& obj : layer["objects"])
				{
					std::string type = std::find_if(obj["properties"].begin(), obj["properties"].end(), [](const json& element)
						{ return element["name"] == "type"; }).value()["value"];

					if (type == "level") {
						gef::Vector4 scale = gef::Vector4(obj["width"], obj["height"], 10.f);
						if (scale.x() != old_scale.x() || scale.y() != old_scale.y()) {
							old_scale.set_x(scale.x());
							old_scale.set_y(scale.y());
							new_mesh = obj_loader.GetMesh(MeshResource::Level, scale);
						}
						static_game_objects_.emplace_back(new GameObject());
						static_game_objects_.back()->Init(obj["width"] / 2.f, obj["height"] / 2.f, 1.f, (float)obj["x"] + ((float)obj["width"] / 2.f), (-(float)obj["y"]) - ((float)obj["height"] / 2.f), b2_world_, primitive_builder_, audio_manager_);
						static_game_objects_.back()->set_mesh(new_mesh);
					}
					else if (type == "win") {
						gef::Vector4 scale = gef::Vector4(obj["width"], obj["height"], 2.f);
						new_mesh = obj_loader.GetMesh(MeshResource::Reactor, scale);
						static_game_objects_.emplace_back(new GameObject());
						static_game_objects_.back()->Init(obj["width"] / 2.f, obj["height"] / 2.f, 1.f, (float)obj["x"] + ((float)obj["width"] / 2.f), (-(float)obj["y"]) - ((float)obj["height"] / 2.f), b2_world_, primitive_builder_, audio_manager_);
						static_game_objects_.back()->set_mesh(new_mesh);
						static_game_objects_.back()->SetTag(GameObject::Tag::WinObject);
					}
					else if (type == "door") {
						gef::Vector4 scale = gef::Vector4(obj["width"], obj["height"], 10.f);
						gef::Mesh* door_wall = obj_loader.GetMesh(MeshResource::DoorWall, scale);
						gef::Mesh* door_frame = obj_loader.GetMesh(MeshResource::DoorFrame, scale);
						gef::Mesh* door = obj_loader.GetMesh(MeshResource::Door, scale);

						int ID = std::find_if(obj["properties"].begin(), obj["properties"].end(), [](const json& element)
							{ return element["name"] == "ID"; }).value()["value"];
						door_objects_[ID] = new Door(gef::Vector4(obj["width"] / 2.f, obj["height"] / 2.f, 0.f), gef::Vector4((float)obj["x"] + ((float)obj["width"] / 2.f), (-(float)obj["y"]) - ((float)obj["height"] / 2.f), 0.f), b2_world_, primitive_builder_, audio_manager_, door_wall, door_frame, door);
					}
				}
			}
			if (layer["name"] == "Background") {
				loading_screen->SetStatusText("Creating background scenery...");
				gef::Matrix44 transform_matrix;
				transform_matrix.SetIdentity();
				gef::Mesh* new_mesh;
				gef::Vector4 old_scale;

				for (const auto& obj : layer["objects"])
				{
					std::string type = std::find_if(obj["properties"].begin(), obj["properties"].end(), [](const json& element)
						{ return element["name"] == "type"; }).value()["value"];
					gef::Vector4 scale = gef::Vector4(obj["width"], obj["height"], 1.f);
					if (scale.x() != old_scale.x() || scale.y() != old_scale.y()) {
						old_scale.set_x(scale.x());
						old_scale.set_y(scale.y());
					}
					if (type == "wall") {
						new_mesh = obj_loader.GetMesh(MeshResource::BackWall, scale);
					}
					else if (type == "window") {
						new_mesh = obj_loader.GetMesh(MeshResource::Window, scale);
					}

					transform_matrix.SetTranslation(gef::Vector4((float)obj["x"] + ((float)obj["width"] / 2.f), (-(float)obj["y"]) - ((float)obj["height"] / 2.f), -5.f));
					background_objects_.emplace_back(new gef::MeshInstance());
					background_objects_.back()->set_transform(transform_matrix);
					background_objects_.back()->set_mesh(new_mesh);
				}
			}
			if(layer["name"] == "PlayerSpawn")
			{
				loading_screen->SetStatusText("Creating player...");
				auto playerJson = layer["objects"][0];
				player_.Init(1, 1, 1, playerJson["x"], 0-playerJson["y"], b2_world_, sprite_animator3D_, audio_manager_, &camera_, this);
				camera_.SetPosition(gef::Vector4(playerJson["x"], 1-playerJson["y"], 30));
			}
			if(layer["name"] == "DynamicSpawns")
			{
				loading_screen->SetStatusText("Creating dynamic game objects...");
				gef::Mesh* crate_mesh;
				gef::Vector4 scale = gef::Vector4(1.f, 1.f, 1.f);
				crate_mesh = obj_loader.GetMesh(MeshResource::Crate, scale);
				for(const auto& object : layer["objects"])
				{
					std::string type = std::find_if(object["properties"].begin(), object["properties"].end(), [](const json& element)
						{ return element["name"] == "type";}).value()["value"];
					
					if(type == "enemy")
					{
						loading_screen->SetStatusText("Creating enemy...");
						Enemy* enemy = new Enemy();
						enemy->Init(1, 1, 1, object["x"], 0-object["y"], b2_world_, primitive_builder_, sprite_animator3D_, audio_manager_, &player_, dynamic_game_objects_);
						enemies_.push_back(enemy);
					}
					else if(type == "plate")
					{
						loading_screen->SetStatusText("Creating pressure plates...");
						PressurePlate* plate = new PressurePlate();
						
						float threshold = std::find_if(object["properties"].begin(), object["properties"].end(), [](const json& element)
						{ return element["name"] == "threshold";}).value()["value"];
						int door_ID = std::find_if(object["properties"].begin(), object["properties"].end(), [](const json& element)
							{ return element["name"] == "Door ID"; }).value()["value"];
						bool fussy = std::find_if(object["properties"].begin(), object["properties"].end(), [](const json& element)
							{ return element["name"] == "fussy"; }).value()["value"];
						
						plate->Init(object["width"]/2.f,0.f,1.f,object["x"] + object["width"]/2.f, (-(float)object["y"]), b2_world_, primitive_builder_, threshold, fussy);
						plate->SetOnActivate([this, door_ID]{ door_objects_[door_ID]->Open(); });
						plate->SetOnDeactivate([this, door_ID] { door_objects_[door_ID]->Close(); });
						static_game_objects_.push_back(plate);
					}
					else
					{
						if(type == "crate") loading_screen->SetStatusText("Creating crate...");
						else loading_screen->SetStatusText("Creating dynamic game object...");
						
						dynamic_game_objects_.emplace_back(new GameObject());
						GameObject* dynObject = dynamic_game_objects_.back();
						dynObject->Init(0.6f, 0.6f, 0.6f, object["x"], 0-object["y"], b2_world_, primitive_builder_, audio_manager_, true);
						if(type == "crate")
						{
							dynObject->SetTag(GameObject::Tag::Crate);
							if (crate_mesh) {
								dynObject->set_mesh(crate_mesh);
							}
						}
					}
				}
			}
		}
	}
}

void Level::CleanUp()
{
	audio_manager_->UnloadAllSamples();
	for(auto& object : static_game_objects_)
	{
		delete object;
		object = nullptr;
	}
	for(auto& object : dynamic_game_objects_)
	{
		delete object;
		object = nullptr;
	}
	for(auto& object : background_objects_)
	{
		delete object;
		object = nullptr;
	}
	for(auto& object : door_objects_)
	{
		delete object.second;
		object.second = nullptr;
	}
	for(auto& object : enemies_)
	{
		delete object;
		object = nullptr;
	}
	
	delete b2_world_;
	b2_world_ = nullptr;
	
	delete primitive_builder_;
	primitive_builder_ = nullptr;
	
	delete sprite_animator3D_;
	sprite_animator3D_ = nullptr;
}

void Level::Reset()
{
	end_state_ = NONE;
	CleanUp();
	Init();
}

void Level::Init()
{
	// initialize box2d world
	b2_world_ = new b2World(b2Vec2(0.0f, -10.f));

	// Set collision manager as contact listener for the world
	b2_world_->SetContactListener(&collision_manager_);
	b2_world_->SetAutoClearForces(false);
	
	// initialise primitive builder to make create some 3D geometry easier
	primitive_builder_ = new PrimitiveBuilder(*platform_);
	sprite_animator3D_ = new SpriteAnimator3D(platform_, primitive_builder_, gef::Vector4(1, 1, 1));
	sprite_animator3D_->Init();

	hud_text_[Ammo] = new Text({0.88f, 0.93f}, "", *platform_);
	hud_text_[Health] = new Text({0.88f, 0.85f}, "", *platform_);
	hud_text_[EndText] = new Text({0.5f, 0.5f}, "", *platform_);
}

void Level::Update(InputActionManager* iam_, float frame_time)
{
	if(iam_->isPressed(Action::Pause))
	{
		is_paused_ = !is_paused_;
	}
	/*if (iam_->getInputManager()->keyboard()->IsKeyPressed(gef::Keyboard::KC_X))
	{
		end_state_ = WIN;
	}
	else if (iam_->getInputManager()->keyboard()->IsKeyPressed(gef::Keyboard::KC_V))
	{
		end_state_ = LOSE;
	}*/

	if (end_state_ != NONE) {
		Menu* end = new Menu(*platform_, *state_manager_, false);
		Button* mainMenuButton = new Button({ 0.5,0.6 }, *platform_, "Main Menu", 200.f, 50.f, gef::Colour(1, 1, 1, 1));
		Button* quitButton = new Button({ 0.5,0.7 }, *platform_, "Quit", 200.f, 50.f, gef::Colour(1, 0, 0, 1));
		mainMenuButton->SetOnClick([this]
			{
				state_manager_->SwitchToMainMenu();
			});
		quitButton->SetOnClick([this]
			{
				state_manager_->SetShouldRun(false);
			});
		end->AddUIElement(mainMenuButton);
		end->AddUIElement(quitButton);
		
		if(end_state_ == WIN)
		{
			if(std::string(file_name_) == std::string("lvl_2.json"))
			{
				end->AddUIElement(new Text({ 0.5,0.25 }, "Thanks For Playing!"));
			}
			else
			{
				end->AddUIElement(new Text({ 0.5,0.25 }, "Level Complete"));
				Button* nextLevelButton = new Button({ 0.5,0.5 }, *platform_, "Next Level", 200.f, 50.f, gef::Colour(1, 1, 1, 1));

				std::string nlf = file_name_;
				nlf = nlf.substr(nlf.find(".json") - 1, 1);
				nlf = "lvl_" + std::to_string(std::stoi(nlf) + 1) + ".json";
				nextLevelButton->SetOnClick([this, nlf]
					{
						CleanUp();
						state_manager_->PushLevel(new Level(*platform_, *state_manager_, audio_manager_), nlf.c_str(), *obj_loader_);
						state_manager_->NextScene();
						delete this;
					});
				end->AddUIElement(nextLevelButton);
			}
		}
		else if(end_state_ == LOSE)
		{
			Button* restartButton = new Button({ 0.5,0.5 }, *platform_, "Restart Level", 200.f, 50.f, gef::Colour(1, 1, 1, 1));
			restartButton->SetOnClick([this]
				{
					CleanUp();
					state_manager_->PushLevel(new Level(*platform_, *state_manager_, audio_manager_), file_name_, *obj_loader_);
					state_manager_->NextScene();
					delete this;
				});
			end->AddUIElement(restartButton);
			end->AddUIElement(new Text({ 0.5,0.25 }, "Game Over"));
		}

		state_manager_->PushScene(end);
		state_manager_->NextScene();
	}

	else if(!is_paused_)
	{
		// Fixed Time Step caused weird jumpy physics issues
		/*
		//--------------- Adapted from Stack Overflow - remudada (2014) : https://stackoverflow.com/a/23038284 (Accessed: 22 March 2023)
		float maximumStep = 1.0f / 60.0f;
		float progress = 0.0;
		while (progress < frame_time)
		{
			float step = min((frame_time - progress), maximumStep);
			b2_world_->Step(step, 20, 20);
			progress += step;
		}
		//---------------
		*/
		
		b2_world_->Step(frame_time, 20, 20);
		b2_world_->ClearForces();

		player_.Update(iam_, frame_time);

		for(int i=0; i < static_game_objects_.size(); i++)
		{
			static_game_objects_[i]->Update(frame_time);
		}
		for (const std::pair<const int, Door*> object : door_objects_)
		{
			object.second->Update(frame_time);
		}

		for(int i = 0; i < dynamic_game_objects_.size(); i++)
		{
			auto* object = dynamic_game_objects_[i];
			object->Update(frame_time);
			if(object->TimeToDie())
			{
				objects_to_destroy_.push_back(object);
				dynamic_game_objects_.erase(dynamic_game_objects_.begin() + i);
			}
		}

		for(int i=0; i < enemies_.size(); i++)
		{
			auto* enemy = enemies_[i];
			enemy->Update(frame_time);
			if(enemy->TimeToDie())
			{
				objects_to_destroy_.push_back(enemy);
				enemies_.erase(enemies_.begin() + i);
			}
		}
		
		b2_world_->SetAllowSleeping(true);

		for(auto* object : objects_to_destroy_)
		{
			if(object->GetBody() != nullptr)
			{
				b2_world_->DestroyBody(object->GetBody());
			}
		}
		objects_to_destroy_.clear();

		for(auto hud : hud_text_)
		{
			hud.second->Update(iam_, frame_time);
		}
		
		std::ostringstream ammoOss;
		if(player_.GetGun()->getReloading())
		{
			ammoOss << "Reloading...";
		}
		else
		{
			ammoOss << "Ammo: " << player_.GetGun()->getAmmoLoaded() << "/" << player_.GetGun()->getAmmoReserve();
		}
		hud_text_[Ammo]->UpdateText(ammoOss.str());

		std::ostringstream healthOss;
		healthOss << "Health: " << player_.GetHealth();
		hud_text_[Health]->UpdateText(healthOss.str());

		std::ostringstream endOss;
		if(player_.GetTouchingEnd())
		{
			endOss << "Press " << (iam_->getUsingKeyboard() ? "X" : "Circle") << " to repair the hyperdrive!";
		}
		else
		{
			ammoOss << "";
		}
		hud_text_[EndText]->UpdateText(endOss.str());

		camera_.Update(frame_time, getPlayerPosition());

		if (camera_.GetEffectState() != EffectState::NORMAL) {
			gef::ControllerOutputData out_data = iam_->getInputManager()->controller_input()->GetController(0)->get_output_data();
			if (camera_.GetEffectState() == EffectState::SHAKE) out_data.left_rumble = 0.6f;
			else if (camera_.GetEffectState() != EffectState::WARP)  out_data.right_rumble = 0.6f;
		}
	}
	else if(pause_menu_ != nullptr)
	{
		pause_menu_->Update(iam_, frame_time);
	}
}

void Level::Render(gef::Renderer3D* renderer_3d)
{
}

void Level::Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font)
{
}

void Level::Render(gef::Renderer3D* renderer_3d, gef::SpriteRenderer* sprite_renderer, gef::Font* font)
{
	// projection
	float fov = gef::DegToRad(45.0f);
	float aspect_ratio = (float)platform_->width() / (float)platform_->height();
	gef::Matrix44 projection_matrix;
	projection_matrix = platform_->PerspectiveProjectionFov(fov, aspect_ratio, 0.1f, 100.0f);
	renderer_3d->set_projection_matrix(projection_matrix);
	
	// view
	renderer_3d->set_view_matrix(camera_.GetViewMatrix());
	
	renderer_3d->Begin();
		renderer_3d->DrawMesh(*camera_.GetBackground());
		for (const gef::MeshInstance* object : background_objects_)
		{
			renderer_3d->DrawMesh(*object);
		}
		for(const GameObject* object : static_game_objects_)
		{
			object->Render(renderer_3d);
		}
		for(const std::pair<const int, Door*> object : door_objects_)
		{
			object.second->Render(renderer_3d);
		}
		renderer_3d->set_override_material(NULL);
		for(const GameObject* object : dynamic_game_objects_)
		{
			object->Render(renderer_3d);
		}
		player_.Render(renderer_3d);
		for(const Enemy* enemy : enemies_)
		{
			enemy->Render(renderer_3d);
		}

	renderer_3d->End();
	
	// start drawing sprites, but don't clear the frame buffer
	sprite_renderer->Begin(false);
		for(auto hud : hud_text_)
		{
			hud.second->Render(sprite_renderer, font);
		}
	sprite_renderer->End();
	if(is_paused_ && pause_menu_ != nullptr)
	{
		pause_menu_->Render(renderer_3d, sprite_renderer, font);
	}
}

gef::Vector2 Level::getPlayerPosition() const
{
	return gef::Vector2(player_.transform().GetTranslation().x(),player_.transform().GetTranslation().y());
}

const Player* Level::getPlayer() const
{
	return &player_;
}

const Gun* Level::getGun() const
{
	return player_.GetGun();
}

