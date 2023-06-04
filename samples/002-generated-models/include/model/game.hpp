#pragma once

#include <rawrbox/engine/engine.hpp>
#include <rawrbox/render/camera/orbital.hpp>
#include <rawrbox/render/model/model.hpp>
#include <rawrbox/render/model/sprite.hpp>
#include <rawrbox/render/model/text3D.hpp>
#include <rawrbox/render/texture/gif.hpp>
#include <rawrbox/render/texture/image.hpp>
#include <rawrbox/render/window.hpp>

#include <memory>

namespace model {
	class Game : public rawrbox::Engine {
		std::shared_ptr<rawrbox::Window> _window = nullptr;
		std::shared_ptr<rawrbox::CameraOrbital> _camera = nullptr;

		std::shared_ptr<rawrbox::TextureImage> _texture = nullptr;
		std::shared_ptr<rawrbox::TextureGIF> _texture2 = nullptr;

		std::shared_ptr<rawrbox::Model<>> _model = std::make_shared<rawrbox::Model<>>();
		std::shared_ptr<rawrbox::Sprite<>> _sprite = std::make_shared<rawrbox::Sprite<>>();
		std::shared_ptr<rawrbox::Text3D> _text = std::make_shared<rawrbox::Text3D>();

		std::weak_ptr<rawrbox::Font> _font;

		std::atomic<int> _loadingFiles = 0;
		bool _ready = false;

		void setupGLFW() override;
		void init() override;
		void onThreadShutdown(rawrbox::ENGINE_THREADS thread) override;
		void pollEvents() override;
		void update() override;
		void draw() override;

	public:
		void printFrames();

		void loadContent();
		void contentLoaded();

		void drawWorld();
	};
} // namespace model
