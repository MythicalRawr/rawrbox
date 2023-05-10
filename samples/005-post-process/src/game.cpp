
#include <rawrbox/render/model/light/manager.hpp>
#include <rawrbox/render/model/mesh.hpp>
#include <rawrbox/render/postprocess/bloom.hpp>
#include <rawrbox/render/postprocess/dither_psx.hpp>
#include <rawrbox/render/postprocess/static_noise.hpp>
#include <rawrbox/utils/keys.hpp>

#include <post_process/game.hpp>

#include <bx/bx.h>
#include <bx/math.h>
#include <bx/timer.h>

#include <vector>

namespace post_process {
	void Game::init() {
		int width = 1024;
		int height = 768;

		this->_window = std::make_unique<rawrbox::Window>();
		this->_window->setMonitor(-1);
		this->_window->setTitle("POST-PROCESS TEST");
		this->_window->setRenderer(bgfx::RendererType::Count);
		this->_window->onResize += [this](auto& w, auto& size) {
			if (this->_render == nullptr) return;
			this->_render->resizeView(size);
		};

		this->_window->onWindowClose += [this](auto& w) {
			this->shutdown();
		};

		this->_window->initialize(width, height, rawrbox::WindowFlags::Window::WINDOWED | rawrbox::WindowFlags::Debug::TEXT);

		this->_render = std::make_shared<rawrbox::Renderer>(0, this->_window->getSize());
		this->_render->setClearColor(0x00000000);

		this->_postProcess = std::make_shared<rawrbox::PostProcessManager>(this->_window->getSize());
		this->_postProcess->add(std::make_shared<rawrbox::PostProcessBloom>(0.015F));
		this->_postProcess->add(std::make_shared<rawrbox::PostProcessPSXDither>(rawrbox::DITHER_SIZE::SLOW_MODE));
		this->_postProcess->add(std::make_shared<rawrbox::PostProcessStaticNoise>(0.1F));

		// Setup camera
		this->_camera = std::make_shared<rawrbox::CameraOrbital>(this->_window.get());
		this->_camera->setPos({0.F, 5.F, -5.F});
		this->_camera->setAngle({0.F, bx::toRad(-45), 0.F, 0.F});
		// --------------

		// Load content ---
		this->loadContent();
		// -----
	}

	void Game::loadContent() {
		this->_render->upload();
		this->_postProcess->upload();

		// Assimp test ---
		this->_model->load("./content/models/ps1_road/scene.gltf", rawrbox::ModelLoadFlags::IMPORT_TEXTURES);
		this->_model->setScale({0.01F, 0.01F, 0.01F});
		this->_model->upload();
		// -----
	}

	void Game::shutdown() {
		this->_render = nullptr;
		this->_camera = nullptr;
		this->_model = nullptr;

		rawrbox::LightManager::get().destroy();
		rawrbox::Engine::shutdown();
	}

	void Game::pollEvents() {
		if (this->_window == nullptr) return;
		this->_window->pollEvents();
	}

	void Game::update() {
		if (this->_camera == nullptr) return;
		this->_camera->update();
	}

	void Game::drawWorld() {
		if (this->_model == nullptr) return;
		this->_model->draw(this->_camera->getPos());

		auto& stencil = this->_render->getStencil();
		rawrbox::Vector3f out2 = this->_camera->worldToScreen({0, 0, 0});

		stencil.begin();
		stencil.drawBox({out2.x, out2.y}, {50, 50});
		stencil.end();

		bgfx::setViewTransform(rawrbox::CURRENT_VIEW_ID, this->_camera->getViewMtx().data(), this->_camera->getProjMtx().data());
	}

	void printFrames() {
		int64_t now = bx::getHPCounter();
		static int64_t last = now;
		const int64_t frameTime = now - last;
		last = now;

		const auto freq = static_cast<double>(bx::getHPFrequency());
		const double toMs = 1000.0 / freq;

		bgfx::dbgTextPrintf(1, 4, 0x0f, "Frame: %7.3f[ms]", double(frameTime) * toMs);
	}

	void Game::draw() {
		if (this->_render == nullptr) return;
		this->_render->clear(); // Clean up and set renderer

		// DEBUG ----
		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(1, 1, 0x1f, "005-post-process");
		bgfx::dbgTextPrintf(1, 2, 0x3f, "Description: Post-processing test");
		printFrames();
		// -----------

		this->_postProcess->begin();
		this->drawWorld();
		this->_postProcess->end();

		this->_render->frame(); // Commit primitives

		bgfx::setViewTransform(rawrbox::CURRENT_VIEW_ID, this->_camera->getViewMtx().data(), this->_camera->getProjMtx().data());
	}
} // namespace post_process
