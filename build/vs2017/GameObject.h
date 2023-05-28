#pragma once
#include "graphics/mesh_instance.h"
#include "maths/vector4.h"
#include <box2d/box2d.h>
#include "primitive_builder.h"
#include "maths/vector2.h"
#include "SpriteAnimator3D.h"

enum class GravityDirection { GRAVITY_UP, GRAVITY_DOWN, GRAVITY_LEFT, GRAVITY_RIGHT };

class GameObject : public gef::MeshInstance {
	
public:
	enum class Tag
	{
		None,
		Player,
		Enemy,
		Bullet,
		Hook,
		Crate,
		PressurePlate,
		Pickup,
	};
	
	virtual void Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, PrimitiveBuilder* builder, bool dynamic = false);
	virtual void Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder, bool dynamic = false);
	void UpdateBox2d();
	void Translate(gef::Vector4 translation) { translate_ = translation; };
	void Rotate(gef::Vector4 rotation) { rotate_ = rotation; };
	virtual void Update(float frame_time);
	virtual void Render(gef::Renderer3D* renderer_3d) const;
	virtual void BeginCollision(GameObject* other);
	virtual void EndCollision(GameObject* other);
	virtual void PreResolve(GameObject* other);
	virtual void PostResolve(GameObject* other);
	void EnableCollisionResolution(bool enable);
	b2Body* GetBody() { return physics_body_; }
	const b2Body* GetBody() const { return physics_body_; }
	Tag GetTag();
	void SetTag(Tag tag);
	const gef::Vector4 GetSize() const { return size_; }

	void Kill();
	bool TimeToDie();

	float GetWeight() const { return weight_; }
	void SetWeight(float weight) { weight_ = weight; }

protected:
	b2Body* physics_body_ = nullptr;
	Tag tag = Tag::None;
	bool dead = false;
	gef::Vector4 size_;
	gef::Vector4 translate_ = gef::Vector4(0, 0, 0);
	gef::Vector4 rotate_ = gef::Vector4(0, 0, 0);
	SpriteAnimator3D* sprite_animator3D_;
	float weight_ = 1;
	float anim_time_ = 0;
};

