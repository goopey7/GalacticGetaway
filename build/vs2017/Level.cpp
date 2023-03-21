#include "Level.h"

#include <d3d10.h>
#include <fstream>

#include "GameObject.h"
#include "json.h"
#include "primitive_builder.h"
#include "box2d/b2_math.h"
#include "box2d/b2_world.h"

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
					game_objects_.emplace_back(new GameObject());
					game_objects_.back()->Init(obj["width"]/2.f,obj["height"] / 2.f, 1.f,(float)obj["x"]+((float)obj["width"]/2.f), (-(float)obj["y"])-((float)obj["height"]/2.f),b2_world_, primitive_builder_);
				}
			}
			if(layer["name"] == "PlayerSpawn")
			{
				auto playerJson = layer["objects"][0];
				player_.Init(0.8f, 0.8f, 0.8f, playerJson["x"], 0-playerJson["y"], b2_world_, primitive_builder_, platform_);
			}
		}
	}
}

const std::vector<GameObject*>* Level::GetGameObjects()
{
	return &game_objects_;
}

void Level::Init()
{
	// initialize box2d world
	b2_world_ = new b2World(b2Vec2(0.0f, -10.f));
	
	// initialise primitive builder to make create some 3D geometry easier
	primitive_builder_ = new PrimitiveBuilder(*platform_);
}

void Level::Update(InputActionManager* iam_, float frame_time)
{
	b2_world_->Step(1.f / 165.f, 6, 2);
	player_.Update(iam_, frame_time);
	
	
	b2_world_->SetAllowSleeping(true);
}

void Level::Render(gef::Renderer3D* renderer_3d)
{
	renderer_3d->set_override_material(&primitive_builder_->red_material());
	player_.Render(renderer_3d, primitive_builder_);
	
	renderer_3d->set_override_material(&primitive_builder_->blue_material());
	for(GameObject* object : game_objects_)
	{
		renderer_3d->DrawMesh(*object);
	}
}

const gef::Vector2 Level::getPlayerPosition() const
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

