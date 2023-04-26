#pragma once
#include "GameObject.h"
#include "maths/vector2.h"

class Hook : public GameObject
{
public:
	Hook(b2World* world_, PrimitiveBuilder* builder_);
	void Update() override;
	void Fire(gef::Vector2 target_vector, gef::Vector2 start_pos);
	void BeginCollision(GameObject* other) override;
	void EndCollision(GameObject* other) override;
	const gef::Material* GetColor();
	void SetColor(gef::Material* color);
	
private:
	b2World* world_;
	PrimitiveBuilder* builder_;
	const gef::Material* color_;
};
