#pragma once
#include "Scene.h"
#include "graphics/sprite.h"

class LoadingScreen : public Scene
{
	public:
    	LoadingScreen(gef::Platform& platform, StateManager& state_manager) : Scene(platform, state_manager)
    	{
			sprite_.set_width(20);
			sprite_.set_height(20);
    		offsets_.push_back(-spacing_);
    		offsets_.push_back(0);
    		offsets_.push_back(spacing_);
    	}
		void SetStatusText(const char* text);
    	void Update(InputActionManager* iam, float frame_time) override;
    	void Render(gef::Renderer3D* renderer_3d) override {}
    	void Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font) override;
    	void Render(gef::Renderer3D* renderer_3d, gef::SpriteRenderer* sprite_renderer, gef::Font* font) override {Render(sprite_renderer, font);}
    private:
		const char* status_text_{};
		gef::Sprite sprite_;
		float speed_ = 0.2f;
		float timer = 0.f;
		int spacing_ = 50;
		int current_offset_ = 0;
		std::vector<int> offsets_;
		bool animating_right_ = true;
};
