#pragma once
#include "graphics/mesh_instance.h"
#include "maths/vector4.h"
#include <box2d/box2d.h>
#include "primitive_builder.h"

class GameObject : public gef::MeshInstance {
public:
	virtual void Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, PrimitiveBuilder* builder, bool dynamic = false);
	virtual void Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder, bool dynamic = false);
	void UpdateBox2d();
	void Update();
	b2Body* GetBody() { return physics_body_; }

protected:
	b2Body* physics_body_;
};

