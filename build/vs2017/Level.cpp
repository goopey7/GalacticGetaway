#include "Level.h"

#include <d3d10.h>
#include <fstream>

#include "Enemy.h"
#include "GameObject.h"
#include "json.h"
#include "obj_mesh_loader.h"
#include "primitive_builder.h"
#include "box2d/b2_math.h"
#include "box2d/b2_world.h"
#include "graphics/font.h"
#include "graphics/sprite_renderer.h"
#include "maths/math_utils.h"
#include "system/debug_log.h"

using nlohmann::json;

Level::Level(gef::Platform& platform)
	: platform_(&platform)
{
}

float fixY(float y)
{
	return y - 9.f;
}

void Level::LoadFromFile(const char* filename)
{
	// load level from file
	std::ifstream i(std::string("levels/") + std::string(filename));

	// handle error if file not found
	if (i.fail())
	{
		throw std::exception((std::string(filename)+ " not found").c_str());
	}
	
	// parse json into cpp object
	json levelJson = json::parse(i);

	Init();

	// initialize all layers
	for(const auto& layer : levelJson["layers"])
	{
		if (layer["type"] == "objectgroup")
		{
			if(layer["name"] == "StaticLevelCollisions")
			{
				for(const auto& obj : layer["objects"])
				{
					static_game_objects_.emplace_back(new GameObject());
					static_game_objects_.back()->Init(obj["width"]/2.f,obj["height"] / 2.f, 1.f,(float)obj["x"]+((float)obj["width"]/2.f), (-(float)obj["y"])-((float)obj["height"]/2.f),b2_world_, primitive_builder_);
				}
			}
			if(layer["name"] == "PlayerSpawn")
			{
				auto playerJson = layer["objects"][0];
				player_.Init(0.8f, 0.8f, 0.8f, playerJson["x"], 0-playerJson["y"], b2_world_, primitive_builder_, platform_);
			}
			if(layer["name"] == "DynamicSpawns")
			{
				for(const auto& object : layer["objects"])
				{
					if(object["properties"][0]["value"] == "enemy")
					{
						Enemy* enemy = new Enemy();
						enemy->Init(0.6f, 0.6f, 0.6f, object["x"], 0-object["y"], b2_world_, primitive_builder_, platform_, &player_);
						enemies_.push_back(enemy);
					}
					else
					{
						dynamic_game_objects_.emplace_back(new GameObject());
						GameObject* dynObject = dynamic_game_objects_.back();
						dynObject->Init(0.6f, 0.6f, 0.6f, object["x"], 0-object["y"], b2_world_, primitive_builder_, true);
						if(object["properties"][0]["value"] == "crate")
						{
							dynObject->SetTag(GameObject::Tag::Crate);
							OBJMeshLoader obj_loader;
							MeshMap mesh_map;
							if (obj_loader.Load("Models/crate/crate.obj", *platform_, mesh_map)) { //Poole (2019) Sci-fi Crate V2. Available at: https://skfb.ly/6TNVo (Accessed: 21 March 2023)
								gef::Mesh* crate_mesh = mesh_map["scificrate_low_lambert2_0"];
								if (crate_mesh) {
									dynObject->set_mesh(crate_mesh);
								}
							}
							else {
								gef::DebugOut(obj_loader.GetLastError().c_str());
								gef::DebugOut("\n");
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
	
	// initialise primitive builder to make create some 3D geometry easier
	primitive_builder_ = new PrimitiveBuilder(*platform_);
}

void Level::Update(InputActionManager* iam_, float frame_time)
{
	//--------------- Adapted from Stack Overflow - remudada (2014) : https://stackoverflow.com/a/23038284 (Accessed: 22 March 2023)
	float maximumStep = 1.0f / 165.0f;
	float progress = 0.0;
	while (progress < frame_time)
	{
		float step = min((frame_time - progress), maximumStep);
		b2_world_->Step(step, 6, 2);
		progress += step;
	}
	//---------------

	player_.Update(iam_, frame_time);

	for(int i = 0; i < dynamic_game_objects_.size(); i++)
	{
		auto* object = dynamic_game_objects_[i];
		object->Update();
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
	gef::Vector2 player_pos = getPlayerPosition();
	gef::Vector4 camera_eye(player_pos.x, player_pos.y, 30.0f);
	gef::Vector4 camera_lookat(player_pos.x, player_pos.y, 0.0f);
	gef::Vector4 camera_up(0.0f, 1.0f, 0.0f);
	gef::Matrix44 view_matrix;
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	renderer_3d->set_view_matrix(view_matrix);
	
	renderer_3d->Begin();
		player_.Render(renderer_3d, primitive_builder_);
		renderer_3d->set_override_material(&primitive_builder_->blue_material());
		for(const GameObject* object : static_game_objects_)
		{
			renderer_3d->DrawMesh(*object);
		}
		renderer_3d->set_override_material(NULL);
		for(const GameObject* object : dynamic_game_objects_)
		{
			renderer_3d->DrawMesh(*object);
		}
		for(const Enemy* enemy : enemies_)
		{
			enemy->Render(renderer_3d, primitive_builder_);
		}
	renderer_3d->End();
	
	// start drawing sprites, but don't clear the frame buffer
	sprite_renderer->Begin(false);
		DrawHUD(sprite_renderer, font);
	sprite_renderer->End();
}

void Level::DrawHUD(gef::SpriteRenderer* sprite_renderer, gef::Font* font)
{
	if (font)
	{
		// display frame rate
		//font->RenderText(sprite_renderer, gef::Vector4(5.f, platform_.height()/2, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);

		// display if player gravity lock is on 
		//font->getPlayer()->GetGravityLock() ? gravity_lock_ = "On" : gravity_lock_ = "Off";
		//font->RenderText(sprite_renderer, gef::Vector4(platform_->width() / 2, platform_->height() - 30.f, -0.9f), 1.0f, 0xffffffff, gef::TJ_CENTRE, "F - Gravity lock: %s", gravity_lock_.c_str());
		
		font->RenderText(sprite_renderer, gef::Vector4(platform_->width() / 2 + 400.f, platform_->height() - 30.f, -0.9f), 1.0f, 0xffffffff, gef::TJ_RIGHT, "Ammo: %i/%i", getGun()->getAmmoLoaded(), getGun()->getAmmoReserve());

		std::string reloading = getGun()->getReloading() ? "Reloading" : "";
		font->RenderText(sprite_renderer, gef::Vector4(platform_->width() / 2 + 400.f, platform_->height() - 60.f, -0.9f), 1.0f, 0xffffffff, gef::TJ_RIGHT, reloading.c_str());

		std::string position = "X: " + std::to_string(getPlayerPosition().x) + " Y: " + std::to_string(getPlayerPosition().y);
		font->RenderText(sprite_renderer, gef::Vector4(platform_->width() - 20.f, platform_->height() / 2, -0.9f), 1.0f, 0xffffffff, gef::TJ_RIGHT, position.c_str());

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

