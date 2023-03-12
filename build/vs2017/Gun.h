#pragma once
#include "graphics/mesh_instance.h"
class Gun : public gef::MeshInstance {
public:
	void Update(gef::Vector4 translation);
	void Fire();
};

