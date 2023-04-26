#pragma once
#include "graphics/mesh_instance.h"
#include "maths/vector4.h"
#include <box2d/box2d.h>
#include "primitive_builder.h"


class GameObject : public gef::MeshInstance {
	
public:
	enum Tag
	{
		None,
		Player,
		Enemy,
		Bullet,
		Hook,
	};
	
	virtual void Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, PrimitiveBuilder* builder, bool dynamic = false);
	virtual void Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder, bool dynamic = false);
	void UpdateBox2d();
	virtual void Update();
	virtual void BeginCollision(GameObject* other);
	virtual void EndCollision(GameObject* other);
	virtual void PreResolve(GameObject* other);
	virtual void PostResolve(GameObject* other);
	b2Body* GetBody() { return physics_body_; }
	Tag GetTag();

protected:
	b2Body* physics_body_ = nullptr;
	Tag tag = None;
};

