#pragma once

#include <rawrbox/engine/engine.hpp>
#include <rawrbox/render/models/model.hpp>
#include <rawrbox/render/models/text3D.hpp>
#include <rawrbox/render/text/engine.hpp>

#include <memory>

namespace light {
	class Game : public rawrbox::Engine {
		std::unique_ptr<rawrbox::Model<>> _model = std::make_unique<rawrbox::Model<>>();
		std::unique_ptr<rawrbox::Text3D<>> _text = std::make_unique<rawrbox::Text3D<>>();

		rawrbox::Font* _font = nullptr;

		std::atomic<int> _loadingFiles = 0;
		bool _ready = false;

		rawrbox::Vector3f _sunDir = {};

		void setupGLFW() override;
		void init() override;
		void onThreadShutdown(rawrbox::ENGINE_THREADS thread) override;
		void pollEvents() override;
		void update() override;
		void draw() override;

	public:
		Game() = default;
		Game(const Game&) = delete;
		Game(Game&&) = delete;
		Game& operator=(const Game&) = delete;
		Game& operator=(Game&&) = delete;
		~Game() override = default;

		void loadContent();
		void contentLoaded();
		void drawWorld();
	};
} // namespace light
