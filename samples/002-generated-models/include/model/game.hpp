#pragma once

#include <rawrbox/engine/engine.hpp>
#include <rawrbox/render/camera/perspective.hpp>
#include <rawrbox/render/model/model.hpp>
#include <rawrbox/render/model/sprite.hpp>
#include <rawrbox/render/model/text3D.hpp>
#include <rawrbox/render/renderer.hpp>
#include <rawrbox/render/text/engine.hpp>
#include <rawrbox/render/texture/gif.hpp>
#include <rawrbox/render/texture/image.hpp>
#include <rawrbox/render/window.hpp>

#include <memory>

namespace model {
	class Game : public rawrBox::Engine {
		std::unique_ptr<rawrBox::Window> _window = nullptr;
		std::shared_ptr<rawrBox::Renderer> _render = nullptr;
		std::shared_ptr<rawrBox::CameraPerspective> _camera = nullptr;
		std::unique_ptr<rawrBox::TextEngine> _textEngine = nullptr;

		std::shared_ptr<rawrBox::TextureImage> _texture = nullptr;
		std::shared_ptr<rawrBox::TextureGIF> _texture2 = nullptr;

		std::shared_ptr<rawrBox::Model<>> _model = std::make_shared<rawrBox::Model<>>();
		std::shared_ptr<rawrBox::Sprite<>> _sprite = std::make_shared<rawrBox::Sprite<>>();
		std::shared_ptr<rawrBox::Text3D> _text = std::make_shared<rawrBox::Text3D>();

		rawrBox::Font* _font = nullptr;

		bool _rightClick = false;
		rawrBox::Vector2i _oldMousePos = {};

	public:
		using Engine::Engine;

		void init() override;
		void shutdown() override;
		void pollEvents() override;
		void update(float deltaTime, int64_t gameTime) override;
		void draw(const double alpha) override;

		void loadContent();
		void drawWorld();
	};
} // namespace model
