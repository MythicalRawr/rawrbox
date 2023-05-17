#pragma once

#include <rawrbox/engine/engine.hpp>
#include <rawrbox/render/camera/orbital.hpp>
#include <rawrbox/render/model/assimp/assimp_model.hpp>
#include <rawrbox/render/model/material/lit.hpp>
#include <rawrbox/render/model/text3D.hpp>
#include <rawrbox/render/text/engine.hpp>
#include <rawrbox/render/window.hpp>

#include <memory>

namespace light {
	class Game : public rawrbox::Engine {
		std::shared_ptr<rawrbox::Window> _window = nullptr;
		std::shared_ptr<rawrbox::CameraOrbital> _camera = nullptr;
		std::unique_ptr<rawrbox::TextEngine> _textEngine = nullptr;

		std::shared_ptr<rawrbox::AssimpModel<rawrbox::MaterialLit>> _model = std::make_shared<rawrbox::AssimpModel<rawrbox::MaterialLit>>();
		std::shared_ptr<rawrbox::Text3D> _text = std::make_shared<rawrbox::Text3D>();

		std::weak_ptr<rawrbox::Font> _font;

		bool _ready = false;

	public:
		using Engine::Engine;

		void init() override;
		void shutdown() override;
		void pollEvents() override;
		void update() override;
		void draw() override;

		void loadContent();
		void contentLoaded();
		void drawWorld();
	};
} // namespace light
