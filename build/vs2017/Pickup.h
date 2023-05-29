#pragma once
#include "GameObject.h"

class Pickup : public GameObject
{
public:
	enum Type
	{
		None,
		MaxAmmo,
		Health,
	};
	Pickup();
	void Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, PrimitiveBuilder* builder, gef::AudioManager* am, bool dynamic) override;
	void Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder, gef::AudioManager* am, bool dynamic) override;
	void SetTargetBody(b2Body* target_body);
	void Update(float frame_time) override;
	void Render(gef::Renderer3D* renderer_3d) const override;
	void BeginCollision(GameObject* other) override;
	void SetType(Type type);
	void Activate();
	void Activate(Type type);
private:
	bool is_active_ = false;
	b2Body* target_body_ = nullptr;
	Type type_ = None;
};
