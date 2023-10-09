#pragma once

#include <rawrbox/engine/engine.hpp>
#include <rawrbox/render_temp/materials/lit.hpp>
#include <rawrbox/render_temp/model/instanced.hpp>
#include <rawrbox/render_temp/window.hpp>

#ifdef RAWRBOX_UI
	#include <rawrbox/ui/root.hpp>
#endif

#include <memory>

namespace scripting_test {
	class Game : public rawrbox::Engine {
		std::unique_ptr<rawrbox::Window> _window = nullptr;

#ifdef RAWRBOX_UI
		std::unique_ptr<rawrbox::UIRoot> _ROOT_UI = nullptr;
#endif

		std::shared_ptr<rawrbox::Model> _model = std::make_shared<rawrbox::Model>();
		std::shared_ptr<rawrbox::InstancedModel> _instance = std::make_shared<rawrbox::InstancedModel>();

		std::atomic<int> _loadingFiles = 0;
		bool _ready = false;

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

		void drawWorld();
		void drawOverlay();
		void printFrames();

		void loadContent();
		void contentLoaded();
	};
} // namespace scripting_test
