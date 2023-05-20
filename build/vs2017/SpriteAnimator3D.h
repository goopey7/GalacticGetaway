#pragma once
#include <graphics/mesh.h>
#include <system/platform.h>
#include <list>
#include <unordered_map>
#include "primitive_builder.h"

struct AnimationInfo {
	std::list<gef::Mesh> frames_;
	float frame_speed_;
	bool looping_;
	bool reached_end_ = false;
};

class SpriteAnimator3D {
public:
	SpriteAnimator3D(gef::Platform* platform, PrimitiveBuilder* builder, const gef::Vector4& half_size, gef::Vector4 centre = gef::Vector4(0, 0, 0));
	void AddAnimation(const char* anim_name, const char* folder_name, float speed, bool looping = true);
	const gef::Mesh* Update(float dt, const gef::Mesh* current_mesh, const char* anim_name);
	const gef::Mesh* GetFirstFrame(const char* anim_name);
	bool ReachedEnd(const char* anim_name) { return animations_[anim_name].reached_end_; }
	gef::Mesh* CreateMesh(const char* filepath, const gef::Vector4& half_size, gef::Vector4 centre = gef::Vector4(0, 0, 0));
protected:
	std::unordered_map<const char*, AnimationInfo> animations_;
	std::list<gef::Mesh>::iterator it;
	//gef::Mesh* mesh_ = nullptr;
	gef::Platform* platform_;
	PrimitiveBuilder* builder_;
	gef::Vector4 half_size_;
	gef::Vector4 centre_;
	float time_passed_ = 0;
};

