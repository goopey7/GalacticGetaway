#pragma once
#include "GameObject.h"

class PressurePlate : public GameObject
{
public:
	void Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder, float threshold);
	void Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, PrimitiveBuilder* builder, float threshold);
	void Update(float frame_time) override;
	void BeginCollision(GameObject* other) override;
	void Render(gef::Renderer3D* renderer_3d, PrimitiveBuilder* builder) const override;
private:
	b2World* physics_world_;
	float threshold_;
};
