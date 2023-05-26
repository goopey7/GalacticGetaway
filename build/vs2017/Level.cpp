﻿#include "Level.h"

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
#include "LoadingScreen.h"
#include "input/input_manager.h"

using nlohmann::json;

float fixY(float y)
{
	return y - 9.f;
}

void Level::LoadFromFile(const char* filename, LoadingScreen* loading_screen)
{
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

	// initialize all layers
	for(const auto& layer : levelJson["layers"])
	{
		if (layer["type"] == "objectgroup")
		{
			if(layer["name"] == "StaticLevelCollisions")
			{
				OBJMeshLoader obj_loader;
				MeshMap mesh_map;
				gef::Mesh* new_mesh;
				gef::Vector4 old_scale;

				for(const auto& obj : layer["objects"])
				{
					gef::Vector4 scale = gef::Vector4(obj["width"], obj["height"], 10.f);
					if (scale.x() != old_scale.x() || scale.y() != old_scale.y() || scale.z() != old_scale.z()) {
						old_scale.set_x(scale.x());
						old_scale.set_y(scale.y());
						old_scale.set_z(scale.z());
						
						// TODO load mesh once not for every object
						loading_screen->SetStatusText("Loading crate mesh...");
						if (obj_loader.Load("Models/Generic/crate2/crate2.obj", *platform_, mesh_map, scale)) {
							new_mesh = mesh_map["Crate_1__Default_0"];
						}
						else {
							gef::DebugOut(obj_loader.GetLastError().c_str());
							gef::DebugOut("\n");
						}
					}

					loading_screen->SetStatusText("Creating static game object...");
					static_game_objects_.emplace_back(new GameObject());
					static_game_objects_.back()->Init(obj["width"] / 2.f, obj["height"] / 2.f, 1.f, (float)obj["x"] + ((float)obj["width"] / 2.f), (-(float)obj["y"]) - ((float)obj["height"] / 2.f), b2_world_, primitive_builder_);
					static_game_objects_.back()->set_mesh(new_mesh);
				}
			}
			if(layer["name"] == "PlayerSpawn")
			{
				loading_screen->SetStatusText("Creating player...");
				auto playerJson = layer["objects"][0];
				player_.Init(1, 1, 1, playerJson["x"], 0-playerJson["y"], b2_world_, sprite_animator3D_, &camera_);
				camera_.SetPosition(gef::Vector4(playerJson["x"], 1-playerJson["y"], 30));
			}
			if(layer["name"] == "DynamicSpawns")
			{
				loading_screen->SetStatusText("Creating dynamic game objects...");
				OBJMeshLoader obj_loader;
				MeshMap mesh_map;
				gef::Mesh* crate_mesh;
				gef::Vector4 scale = gef::Vector4(1.f, 1.f, 1.f);
				if (obj_loader.Load("Models/crate/crate.obj", *platform_, mesh_map, scale)) { //Poole (2019) Sci-fi Crate V2. Available at: https://skfb.ly/6TNVo (Accessed: 21 March 2023)
					crate_mesh = mesh_map["scificrate_low_lambert2_0"];
				}
				else {
					gef::DebugOut(obj_loader.GetLastError().c_str());
					gef::DebugOut("\n");
				}
				for(const auto& object : layer["objects"])
				{
					std::string type = std::find_if(object["properties"].begin(), object["properties"].end(), [](const json& element)
						{ return element["name"] == "type";}).value()["value"];
					
					if(type == "enemy")
					{
						loading_screen->SetStatusText("Creating enemy...");
						Enemy* enemy = new Enemy();
						enemy->Init(1, 1, 1, object["x"], 0-object["y"], b2_world_, sprite_animator3D_, &player_);
						enemies_.push_back(enemy);
					}
					else if(type == "plate")
					{
						loading_screen->SetStatusText("Creating pressure plate...");
						PressurePlate* plate = new PressurePlate();
						
						float threshold = std::find_if(object["properties"].begin(), object["properties"].end(), [](const json& element)
						{ return element["name"] == "threshold";}).value()["value"];
						
						plate->Init(object["width"]/2.f,object["height"]/2.f,1,object["x"] + object["width"]/2.f, (-(float)object["y"]) - ((float)object["height"]/2.f), b2_world_, primitive_builder_, threshold);
						plate->SetOnActivate([]{ gef::DebugOut("Plate Activated\n");});
						plate->SetOnDeactivate([]{ gef::DebugOut("Plate Deactivated\n");});
						static_game_objects_.push_back(plate);
					}
					else
					{
						if(type == "crate") loading_screen->SetStatusText("Creating crate...");
						else loading_screen->SetStatusText("Creating dynamic game object...");
						
						dynamic_game_objects_.emplace_back(new GameObject());
						GameObject* dynObject = dynamic_game_objects_.back();
						dynObject->Init(0.6f, 0.6f, 0.6f, object["x"], 0-object["y"], b2_world_, primitive_builder_, true);
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
}

void Level::Update(InputActionManager* iam_, float frame_time)
{
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
	b2_world_->ClearForces();

	player_.Update(iam_, frame_time);

	for(int i=0; i < static_game_objects_.size(); i++)
	{
		static_game_objects_[i]->Update(frame_time);
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

	camera_.Update(frame_time, getPlayerPosition());
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
		for(const GameObject* object : static_game_objects_)
		{
			object->Render(renderer_3d, primitive_builder_);
		}
		renderer_3d->set_override_material(NULL);
		for(const GameObject* object : dynamic_game_objects_)
		{
			object->Render(renderer_3d, primitive_builder_);
		}
		player_.Render(renderer_3d, primitive_builder_);
		for(const Enemy* enemy : enemies_)
		{
			enemy->Render(renderer_3d, primitive_builder_);
		}

	renderer_3d->End();
	
	// start drawing sprites, but don't clear the frame buffer
	sprite_renderer->Begin(false);
		for(auto hud : hud_text_)
		{
			hud.second->Render(sprite_renderer, font);
		}
	sprite_renderer->End();
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

