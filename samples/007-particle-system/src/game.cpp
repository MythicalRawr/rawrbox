
#include <rawrbox/render_temp/camera/orbital.hpp>
#include <rawrbox/render_temp/gizmos.hpp>
#include <rawrbox/render_temp/model/utils/mesh.hpp>
#include <rawrbox/render_temp/particles/emitter.hpp>
#include <rawrbox/render_temp/resources/font.hpp>
#include <rawrbox/render_temp/resources/texture.hpp>
#include <rawrbox/render_temp/static.hpp>
#include <rawrbox/render_temp/textures/atlas.hpp>
#include <rawrbox/resources/manager.hpp>

#include <particle_test/game.hpp>

#include <fmt/printf.h>

#include <memory>

namespace particle_test {
	void Game::setupGLFW() {
		this->_window = std::make_unique<rawrbox::Window>();
		this->_window->setMonitor(-1);
		this->_window->setTitle("PARTICLE TEST");
		this->_window->setRenderer<>(
		    bgfx::RendererType::Count, []() {}, [this]() { this->drawWorld(); });
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
		cam->setAngle({0.F, rawrbox::MathUtils::toRad(-45), 0.F, 0.F});
		// --------------

		// Add loaders ----
		rawrbox::RESOURCES::addLoader<rawrbox::FontLoader>();
		rawrbox::RESOURCES::addLoader<rawrbox::TextureLoader>();
		// ---

		this->_window->initializeBGFX();
	}

	void Game::loadContent() {
		std::array initialContentFiles = {
		    std::make_pair<std::string, uint32_t>("content/textures/particles/particles.png", 32)};

		this->_loadingFiles = static_cast<int>(initialContentFiles.size());
		for (auto& f : initialContentFiles) {
			rawrbox::RESOURCES::loadFileAsync(f.first, f.second, [this]() {
				this->_loadingFiles--;
				if (this->_loadingFiles <= 0) {
					rawrbox::runOnRenderThread([this]() { this->contentLoaded(); });
				}
			});
		}
	}

	void Game::contentLoaded() {
		// Setup Engine
		auto texture = rawrbox::RESOURCES::getFile<rawrbox::ResourceTexture>("content/textures/particles/particles.png")->get<rawrbox::TextureAtlas>();
		this->_ps = std::make_unique<rawrbox::ParticleSystem>(*texture);

		{
			rawrbox::Emitter m;
			m.setPos({-2.F, 0.5F, 0});

			this->_ps->addEmitter(m);
		}

		{
			rawrbox::EmitterSettings s;
			s.texture = {0, 3};
			s.offsetStart = {0.F, 0.05F};
			s.rgba = {0xFF0000FF, 0xFF000FFF, 0xFF00F0FF, 0xFF000000, 0x00FFFFFF};
			s.scaleEnd = {0.5F, 0.5F};
			s.offsetEnd = {0.85F, 0.85F};
			s.preHeat = true;
			s.particlesPerSecond = 20;
			s.maxParticles = 100;

			rawrbox::Emitter m(s);
			this->_em = &this->_ps->addEmitter(m);
		}

		this->_ps->upload();

		// ------

		{
			this->_text->addText(*this->_font, "DEFAULT EMITTER SETTINGS", {-2.F, 0.15F, 0});
			this->_text->addText(*this->_font, "CUSTOM EMITTER SETTINGS", {2.F, 0.15F, 0});
			this->_text->upload();
		}

		// GRID -----
		{
			auto mesh = rawrbox::MeshUtils::generateGrid(12, {0.F, 0.F, 0.F});
			this->_modelGrid->addMesh(mesh);
			this->_modelGrid->upload();
		}

		this->_ready = true;
	}

	void Game::onThreadShutdown(rawrbox::ENGINE_THREADS thread) {
		if (thread == rawrbox::ENGINE_THREADS::THREAD_INPUT) return;

		this->_ps.reset();
		this->_modelGrid.reset();
		this->_text.reset();

		rawrbox::GIZMOS::shutdown();
		rawrbox::RESOURCES::shutdown();
		rawrbox::ASYNC::shutdown();

		this->_window->unblockPoll();
		this->_window.reset();
	}

	void Game::pollEvents() {
		if (this->_window == nullptr) return;
		this->_window->pollEvents();
	}

	float move = 0.F;
	void Game::update() {
		if (this->_window == nullptr) return;
		this->_window->update();

		if (this->_ps == nullptr) return;
		this->_ps->update();

		this->_em->setPos({2.F + std::cos(move) * 0.5F, 0.5F, std::sin(move) * 0.5F});
		move += 0.05F;
	}

	void Game::drawWorld() {
		if (!this->_ready) return;

		this->_modelGrid->draw();
		this->_ps->draw();
		this->_text->draw();
	}

	void Game::printFrames() {
		const bgfx::Stats* stats = bgfx::getStats();

		bgfx::dbgTextPrintf(1, 4, 0x6f, "GPU %0.6f [ms]", double(stats->gpuTimeEnd - stats->gpuTimeBegin) * 1000.0 / stats->gpuTimerFreq);
		bgfx::dbgTextPrintf(1, 5, 0x6f, "CPU %0.6f [ms]", double(stats->cpuTimeEnd - stats->cpuTimeBegin) * 1000.0 / stats->cpuTimerFreq);
		bgfx::dbgTextPrintf(1, 7, 0x5f, fmt::format("TRIANGLES: {}", stats->numPrims[bgfx::Topology::TriList]).c_str());
		bgfx::dbgTextPrintf(1, 8, 0x5f, fmt::format("DRAW CALLS: {}", stats->numDraw).c_str());
		bgfx::dbgTextPrintf(1, 9, 0x5f, fmt::format("COMPUTE CALLS: {}", stats->numCompute).c_str());
	}

	void Game::draw() {
		if (this->_window == nullptr) return;

		// DEBUG ----
		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(1, 1, 0x1f, "007-particle-system");
		bgfx::dbgTextPrintf(1, 2, 0x3f, "Description: Particle system test");
		printFrames();
		// -----------

		if (!this->_ready) {
			bgfx::dbgTextPrintf(1, 10, 0x70, "                                   ");
			bgfx::dbgTextPrintf(1, 11, 0x70, "          LOADING CONTENT          ");
			bgfx::dbgTextPrintf(1, 12, 0x70, "                                   ");
		}

		// Draw DEBUG ---
		rawrbox::GIZMOS::draw();
		// -----------

		this->_window->render(); // Commit primitives
	}
} // namespace particle_test
