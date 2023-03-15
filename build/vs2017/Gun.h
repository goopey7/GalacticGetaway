#pragma once
#include "graphics/sprite.h"
#include "graphics/mesh_instance.h"
#include <system/platform.h>

class InputActionManager;

class Gun : public gef::MeshInstance {
public:
	void Update(gef::Vector4 translation, InputActionManager* input, gef::Platform* platform);
	void Fire();

protected:
	gef::Vector4 WorldToScreen(const gef::Vector4 pos, gef::Platform* platform);
	gef::Vector2 target_vector_;

};

