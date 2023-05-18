#pragma once
#include <graphics/mesh.h>
#include <system/platform.h>
#include <list>
#include <unordered_map>
#include "primitive_builder.h"

class SpriteAnimator3D {
public:
	SpriteAnimator3D(gef::Platform* platform, PrimitiveBuilder* builder, const gef::Vector4& half_size, gef::Vector4 centre = gef::Vector4(0, 0, 0));
	void AddAnimation(const char* anim_name, const char* folder_name);
	const gef::Mesh* Update(float dt, const gef::Mesh* current_mesh, const char* anim_name);
protected:
	std::unordered_map<const char*, std::list<gef::Mesh>> animations_;
	std::list<gef::Mesh>::iterator it;
	//gef::Mesh* mesh_ = nullptr;
	gef::Platform* platform_;
	PrimitiveBuilder* builder_;
	gef::Vector4 half_size_;
	gef::Vector4 centre_;
	float time_passed_ = 0.3;
};

