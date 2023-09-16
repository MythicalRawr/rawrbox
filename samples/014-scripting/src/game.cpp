

#ifdef RAWRBOX_BASS
	#include <rawrbox/bass/resources/sound.hpp>
	#include <rawrbox/bass/scripting/plugin.hpp>
#endif

#ifdef RAWRBOX_NETWORK
	#include <rawrbox/network/scripting/plugin.hpp>
#endif

#include <rawrbox/render/camera/orbital.hpp>
#include <rawrbox/render/model/utils/mesh.hpp>
#include <rawrbox/render/resources/font.hpp>
#include <rawrbox/render/resources/texture.hpp>
#include <rawrbox/render/scripting/plugin.hpp>
#include <rawrbox/render/static.hpp>
#include <rawrbox/resources/manager.hpp>
#include <rawrbox/resources/scripting/plugin.hpp>
#include <rawrbox/scripting/scripting.hpp>

#ifdef RAWRBOX_UI
	#include <rawrbox/ui/scripting/plugin.hpp>
	#include <rawrbox/ui/static.hpp>
#endif

#include <rawrbox/utils/timer.hpp>

#include <scripting_test/game.hpp>
#include <scripting_test/wrapper_test.hpp>

#include <fmt/format.h>

namespace scripting_test {
	void Game::setupGLFW() {
		this->_window = std::make_unique<rawrbox::Window>();
		this->_window->setMonitor(-1);
		this->_window->setTitle("SCRIPTING TEST");
		this->_window->setRenderer<>(
		    bgfx::RendererType::Count, [this]() { this->drawOverlay(); }, [this]() { this->drawWorld(); });
		this->_window->create(1024, 768, rawrbox::WindowFlags::Debug::TEXT | rawrbox::WindowFlags::Debug::PROFILER | rawrbox::WindowFlags::Window::WINDOWED | rawrbox::WindowFlags::Features::MULTI_THREADED);
		this->_window->onWindowClose += [this](auto& /*w*/) { this->shutdown(); };
		this->_window->onIntroCompleted += [this]() {
			this->loadContent();
		};
	}

	void Game::init() {
		if (this->_window == nullptr) return;

		// Setup camera
		auto cam = this->_window->setupCamera<rawrbox::CameraOrbital>(*this->_window);
		cam->setPos({0.F, 5.F, -5.F});
		cam->setAngle({0.F, bx::toRad(-45), 0.F, 0.F});
		// --------------

#ifdef RAWRBOX_UI
		// SETUP UI
		this->_ROOT_UI = std::make_unique<rawrbox::UIRoot>(*this->_window);
		// ----
#endif

		// Setup loaders
		rawrbox::RESOURCES::addLoader<rawrbox::TextureLoader>();
#ifdef RAWRBOX_BASS
		rawrbox::RESOURCES::addLoader<rawrbox::BASSLoader>();
#endif
		rawrbox::RESOURCES::addLoader<rawrbox::FontLoader>();
		// ----------

		// Setup scripting
		rawrbox::SCRIPTING::registerPlugin<rawrbox::RenderPlugin>(this->_window.get());
		rawrbox::SCRIPTING::registerPlugin<rawrbox::ResourcesPlugin>();

#ifdef RAWRBOX_BASS
		rawrbox::SCRIPTING::registerPlugin<rawrbox::BASSPlugin>();
#endif

#ifdef RAWRBOX_NETWORK
		rawrbox::SCRIPTING::registerPlugin<rawrbox::NetworkPlugin>();
#endif

#ifdef RAWRBOX_UI
		rawrbox::SCRIPTING::registerPlugin<rawrbox::UIPlugin>(this->_ROOT_UI.get());
#endif

		// Custom non-plugin ---
		rawrbox::SCRIPTING::registerType<rawrbox::TestWrapper>();
		rawrbox::SCRIPTING::onRegisterGlobals += [this](rawrbox::Mod* mod) {
			mod->getEnvironment()["test"] = rawrbox::TestWrapper();
			mod->getEnvironment()["test_model"] = [this]() -> sol::object {
				if (!this->_ready || this->_model == nullptr) return sol::nil;
				return this->_model->getScriptingWrapper();
			};

			mod->getEnvironment()["test_model2"] = [this]() -> sol::object {
				if (!this->_ready || this->_instance == nullptr) return sol::nil;
				return this->_instance->getScriptingWrapper();
			};
		};
		// ----

		rawrbox::SCRIPTING::init(2000); // Check files every 2 seconds

		// Load lua mods
		rawrbox::SCRIPTING::load();
		rawrbox::SCRIPTING::call("init");
		// ----

		this->_window->initializeBGFX();
	}

	void Game::loadContent() {
		std::vector initialContentFiles = {
		    std::make_pair<std::string, uint32_t>("./content/textures/crate_hl1.png", 0)};

#ifdef RAWRBOX_UI
		initialContentFiles.insert(initialContentFiles.begin(), rawrbox::UI_RESOURCES.begin(), rawrbox::UI_RESOURCES.end()); // Insert the UI resources
#endif

		for (auto& f : initialContentFiles) {
			rawrbox::RESOURCES::preLoadFile(f.first, f.second);
		}

		// Load pre-content mod stuff ---
		rawrbox::SCRIPTING::call("onLoad");
		// ---

		// Start loading ----
		this->_loadingFiles = static_cast<int>(rawrbox::RESOURCES::getTotalPreload());
		rawrbox::RESOURCES::startPreLoadQueueAsync(nullptr, [this](std::string f, uint32_t) {
			fmt::print("Loaded {}!\n", f);
			this->_loadingFiles--;
			if (this->_loadingFiles <= 0) {
				rawrbox::runOnRenderThread([this]() { this->contentLoaded(); });
			}
		});
		// -----
	}

	void Game::contentLoaded() {
		auto tex = rawrbox::RESOURCES::getFile<rawrbox::ResourceTexture>("./content/textures/crate_hl1.png")->get();
		this->_model->setOptimizable(false);

		{
			auto mesh = rawrbox::MeshUtils::generateCube({0.F, 1.0F, 0.F}, {1.F, 1.F, 1.F});
			mesh.setTexture(tex);
			this->_model->addMesh(mesh);
		}

		{
			auto mesh = rawrbox::MeshUtils::generateGrid(12, {0.F, 0.F, 0.F});
			this->_model->addMesh(mesh);
		}
		// ----

		this->_instance->setTemplate(rawrbox::MeshUtils::generateCube({0.F, 0.0F, 0.F}, {0.1F, 0.1F, 0.1F}));
		this->_instance->upload();

		this->_model->upload();
		this->_ready = true;

		rawrbox::SCRIPTING::call("onReady");
	}

	void Game::onThreadShutdown(rawrbox::ENGINE_THREADS thread) {
		if (thread == rawrbox::ENGINE_THREADS::THREAD_INPUT) return;

#ifdef RAWRBOX_UI
		this->_ROOT_UI.reset();
#endif

		this->_model.reset();
		this->_instance.reset();

		rawrbox::RESOURCES::shutdown();
		rawrbox::ASYNC::shutdown();
		rawrbox::SCRIPTING::shutdown();

		this->_window->unblockPoll();
		this->_window.reset();
	}

	void Game::pollEvents() {
		if (this->_window == nullptr) return;
		this->_window->pollEvents();
	}

	void Game::update() {
		if (this->_window == nullptr) return;
		this->_window->update();

		if (!this->_ready) return;

#ifdef RAWRBOX_UI
		this->_ROOT_UI->update();
#endif

		rawrbox::SCRIPTING::call("update");
	}

	void Game::printFrames() {
		const bgfx::Stats* stats = bgfx::getStats();

		bgfx::dbgTextPrintf(1, 4, 0x6f, "GPU %0.6f [ms]", double(stats->gpuTimeEnd - stats->gpuTimeBegin) * 1000.0 / stats->gpuTimerFreq);
		bgfx::dbgTextPrintf(1, 5, 0x6f, "CPU %0.6f [ms]", double(stats->cpuTimeEnd - stats->cpuTimeBegin) * 1000.0 / stats->cpuTimerFreq);
		bgfx::dbgTextPrintf(1, 7, 0x5f, fmt::format("TRIANGLES: {}", stats->numPrims[bgfx::Topology::TriList]).c_str());
		bgfx::dbgTextPrintf(1, 8, 0x5f, fmt::format("DRAW CALLS: {}", stats->numDraw).c_str());
		bgfx::dbgTextPrintf(1, 9, 0x5f, fmt::format("COMPUTE CALLS: {}", stats->numCompute).c_str());
	}

	void Game::drawWorld() {
		if (!this->_ready) return;
		if (this->_model != nullptr) this->_model->draw();
		if (this->_instance != nullptr) this->_instance->draw();
	}

	void Game::drawOverlay() {
		if (!this->_ready) return;

		rawrbox::SCRIPTING::call("drawOverlay");

#ifdef RAWRBOX_UI
		this->_ROOT_UI->render();
#endif
	}

	void Game::draw() {
		if (this->_window == nullptr) return;

		// DEBUG ----
		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(1, 1, 0x1f, "014-scripting");
		bgfx::dbgTextPrintf(1, 2, 0x3f, "Description: SCRIPTING test");
		this->printFrames();
		// -----------

		if (!this->_ready) {
			bgfx::dbgTextPrintf(1, 10, 0x70, "                                   ");
			bgfx::dbgTextPrintf(1, 11, 0x70, "          LOADING CONTENT          ");
			bgfx::dbgTextPrintf(1, 12, 0x70, "                                   ");
		}

		this->_window->render(); // Commit primitives
	}
} // namespace scripting_test
