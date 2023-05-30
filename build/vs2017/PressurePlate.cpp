#include "PressurePlate.h"

#include <sstream>

#include "audio/audio_manager.h"
#include "graphics/font.h"
#include "graphics/renderer_3d.h"
#include "graphics/sprite_renderer.h"

void PressurePlate::Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, PrimitiveBuilder* builder, gef::
						SpriteRenderer* sr, gef::Font* font, float
						threshold, gef::Platform* platform, gef::AudioManager* am, bool is_fussy)
{
	audio_manager_ = am;
	platform_ = platform;
	sprite_renderer_ = sr;
	font_ = font;
 	tag = Tag::PressurePlate;
	threshold_ = threshold;
	is_fussy_ = is_fussy;
 
	set_mesh(builder->CreateBoxMesh(gef::Vector4(size_x, size_y, size_z)));
 	physics_world_ = world;
 
 	b2BodyDef body_def;
 	body_def.type = b2_staticBody;
 	body_def.position = b2Vec2(pos_x, pos_y);
 	body_def.userData.pointer = reinterpret_cast<uintptr_t>(this);
 
 	b2PolygonShape shape;
 	shape.SetAsBox(size_x, size_y);
 
 	b2FixtureDef fixture;
 	fixture.shape = &shape;
 	fixture.density = 1.f;
 	fixture.friction = 0.7f;
 	fixture.userData.pointer = reinterpret_cast<uintptr_t>(this);
 
 	physics_body_ = world->CreateBody(&body_def);
 	physics_body_->CreateFixture(&fixture);
 	physics_body_->SetSleepingAllowed(false);
 	physics_body_->SetFixedRotation(true);
 
 	UpdateBox2d();

}

void PressurePlate::Update(float frame_time)
{
	const bool wasActivated = (current_load_ >= threshold_ && !is_fussy_) || (current_load_ == threshold_ && is_fussy_);

	std::ostringstream oss;
	oss << current_load_ << " / " << threshold_;
	hud_ = oss.str();
	
	if(GetBody()->GetContactList() == nullptr)
	{
		current_load_ = 0.f;
		on_deactivate_();
		if(wasActivated)
		{
			audio_manager_->PlaySample(6);
		}
		return;
	}
	
	std::set<GameObject*> visited_objects;

	current_load_ = 0.f;
	TraverseContactChain(reinterpret_cast<GameObject*>(GetBody()->GetContactList()->other->GetUserData().pointer), visited_objects, current_load_);

	if(!wasActivated && ((current_load_ >= threshold_ && !is_fussy_) || (current_load_ == threshold_ && is_fussy_)))
	{
		audio_manager_->PlaySample(6);
		on_activate_();
	}
	else if(wasActivated && (current_load_ < threshold_ || (current_load_ != threshold_ && is_fussy_)))
	{
		audio_manager_->PlaySample(6);
		on_deactivate_();
	}
}

void PressurePlate::Render(gef::Renderer3D* renderer_3d) const
{
	renderer_3d->DrawMesh(*this);
	sprite_renderer_->Begin(false);
		font_->RenderText(sprite_renderer_, {platform_->width() - 200.f, 96.f, -0.9f}, 1.f, 0xffffffff, gef::TJ_CENTRE, hud_.c_str());
		font_->RenderText(sprite_renderer_, {platform_->width() - 200.f, 64.f, -0.9f},1.f, 0xffffffff, gef::TJ_CENTRE, "Pressure Plate");
	sprite_renderer_->End();
}

void PressurePlate::Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder, float threshold, gef::SpriteRenderer*
						sr, gef::Font* font, gef::Platform* platform, gef::AudioManager* am, bool is_fussy)
{
	Init(size.x(), size.y(), size.z(), pos.x(), pos.y(), world, builder, sr, font, threshold, platform, am, is_fussy);
}

void PressurePlate::TraverseContactChain(GameObject* game_object, std::set<GameObject*>& visited_objects, float& total_weight)
{
	// Check if the body has already been visited to avoid infinite recursion
	if (game_object == nullptr || visited_objects.contains(game_object))
	{
		return;
	}

	if(game_object->GetTag() == Tag::None || game_object->GetTag() == Tag::PressurePlate)
	{
		return;
	}
	
	visited_objects.insert(game_object);

	total_weight += game_object->GetWeight();

	// Iterate through all contacts of the body
	for (const b2ContactEdge* edge = game_object->GetBody()->GetContactList(); edge; edge = edge->next)
	{
		// Traverse the contact chain recursively
		TraverseContactChain(reinterpret_cast<GameObject*>(edge->other->GetUserData().pointer), visited_objects, total_weight);
	}
}
