

#include <rawrbox/render/resources/texture.hpp>
#include <rawrbox/render/static.hpp>
#include <rawrbox/resources/manager.hpp>
#include <rawrbox/utils/keys.hpp>
#include <rawrbox/utils/timer.hpp>

#include <instance_test/game.hpp>

#include <fmt/format.h>

#include <vector>

namespace instance_test {
	void Game::setupGLFW() {
		this->_window = std::make_unique<rawrbox::Window>();
		this->_window->setMonitor(-1);
		this->_window->setTitle("INSTANCE TEST");
		this->_window->setRenderer(bgfx::RendererType::Count);
		this->_window->create(1024, 768, rawrbox::WindowFlags::Debug::TEXT | rawrbox::WindowFlags::Debug::PROFILER | rawrbox::WindowFlags::Window::WINDOWED | rawrbox::WindowFlags::Features::MULTI_THREADED);
		this->_window->onWindowClose += [this](auto& w) { this->shutdown(); };
	}

	void Game::init() {
		if (this->_window == nullptr) return;
		this->_window->initializeBGFX();

		// Setup camera
		this->_camera = std::make_unique<rawrbox::CameraOrbital>(*this->_window);
		this->_camera->setPos({0.F, 5.F, -5.F});
		this->_camera->setAngle({0.F, bx::toRad(-45), 0.F, 0.F});
		// --------------

		// Setup loaders
		rawrbox::RESOURCES::addLoader(std::make_unique<rawrbox::TextureLoader>());

		// Load content ---
		this->loadContent();
		// -----
	}

	void Game::loadContent() {
		std::array<std::string, 1> initialContentFiles = {
		    "content/textures/crate_hl1.png",
		};

		for (auto& f : initialContentFiles) {
			this->_loadingFiles++;

			rawrbox::RESOURCES::loadFileAsync(f, 0, [this]() {
				this->_loadingFiles--;
				if (this->_loadingFiles <= 0) {
					rawrbox::runOnRenderThread([this]() { this->contentLoaded(); });
				}
			});
		}

		this->_window->upload();
	}

	void Game::contentLoaded() {
		int total = 4;
		auto t = rawrbox::RESOURCES::getFile<rawrbox::ResourceTexture>("./content/textures/crate_hl1.png")->get();

		for (int y = 0.F; y < total; y++) {
			for (int x = 0.F; x < total; x++) {
				auto mesh = this->_model->generateCube({static_cast<float>(x) * 1.5F, 0, static_cast<float>(y) * 1.5F}, {0.5F, 0.5F, 0.5F});
				mesh.setTexture(t);
				mesh.setOptimizable(false);

				this->_model->addMesh(mesh);
			}
		}

		this->_model->upload();
		this->_ready = true;
	}

	void Game::onThreadShutdown(rawrbox::ENGINE_THREADS thread) {
		if (thread == rawrbox::ENGINE_THREADS::THREAD_INPUT) return;
		this->_camera.reset();
		this->_model.reset();

		rawrbox::RESOURCES::shutdown();
		rawrbox::ASYNC::shutdown();

		this->_window->unblockPoll();
		this->_window.reset();
	}

	void Game::pollEvents() {
		if (this->_window == nullptr) return;
		this->_window->pollEvents();
	}

	void Game::update() {
		this->_camera->update();
	}

	void Game::printFrames() {
		const bgfx::Stats* stats = bgfx::getStats();

		bgfx::dbgTextPrintf(1, 4, 0x6f, "GPU %0.6f [ms]", double(stats->gpuTimeEnd - stats->gpuTimeBegin) * 1000.0 / stats->gpuTimerFreq);
		bgfx::dbgTextPrintf(1, 5, 0x6f, "CPU %0.6f [ms]", double(stats->cpuTimeEnd - stats->cpuTimeBegin) * 1000.0 / stats->cpuTimerFreq);
		bgfx::dbgTextPrintf(1, 6, 0x6f, fmt::format("TRIANGLES: {} ----->    DRAW CALLS: {}", stats->numPrims[bgfx::Topology::TriList], stats->numDraw).c_str());
	}

	void Game::drawWorld() {
		if (this->_model == nullptr) return;
		this->_model->draw({});
	}

	void Game::draw() {
		if (this->_window == nullptr) return;
		this->_window->clear(); // Clean up and set renderer

		// DEBUG ----
		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(1, 1, 0x1f, "010-instancing-test");
		bgfx::dbgTextPrintf(1, 2, 0x3f, "Description: INSTANCING test");
		this->printFrames();
		// -----------

		if (!this->_ready) {
			bgfx::dbgTextPrintf(1, 10, 0x70, "                                   ");
			bgfx::dbgTextPrintf(1, 11, 0x70, "          LOADING CONTENT          ");
			bgfx::dbgTextPrintf(1, 12, 0x70, "                                   ");
		} else {
			this->drawWorld();
		}

		this->_window->frame(); // Commit primitives
		bgfx::setViewTransform(rawrbox::CURRENT_VIEW_ID, this->_camera->getViewMtx().data(), this->_camera->getProjMtx().data());
	}
} // namespace instance_test
