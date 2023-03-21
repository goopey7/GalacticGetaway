#include "Level.h"

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
		}
	}
}

const std::vector<GameObject*>* Level::GetGameObjects()
{
	return &game_objects_;
}

b2World* Level::GetB2World()
{
	return b2_world_;
}

PrimitiveBuilder* Level::GetPrimitiveBuilder()
{
	return primitive_builder_;
}

void Level::Init()
{
	// initialize box2d world
	b2_world_ = new b2World(b2Vec2(0.0f, -9.8f));
	
	// initialise primitive builder to make create some 3D geometry easier
	primitive_builder_ = new PrimitiveBuilder(*platform_);
}

