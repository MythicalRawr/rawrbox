
#include <rawrbox/render/model/mesh.hpp>
#include <rawrbox/render/resources/font.hpp>
#include <rawrbox/render/resources/gif.hpp>
#include <rawrbox/render/resources/texture.hpp>
#include <rawrbox/resources/manager.hpp>
#include <rawrbox/utils/keys.hpp>

#include <model/game.hpp>

#include <bx/bx.h>
#include <bx/math.h>

#include <vector>

namespace model {

	void Game::setupGLFW() {
		this->_window = std::make_unique<rawrbox::Window>();
		this->_window->setMonitor(-1);
		this->_window->setTitle("SIMPLE MODEL TEST");
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

		rawrbox::RESOURCES::addLoader(std::make_unique<rawrbox::TextureLoader>());
		rawrbox::RESOURCES::addLoader(std::make_unique<rawrbox::GIFLoader>());
		rawrbox::RESOURCES::addLoader(std::make_unique<rawrbox::FontLoader>());

		// Load content ---
		this->loadContent();
		// -----
	}

	void Game::loadContent() {
		std::array<std::string, 4> initialContentFiles = {
		    "cour.ttf",
		    "content/textures/screem.png",
		    "content/textures/meow3.gif",
		    "content/textures/displacement_test.png",
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
		this->_ready = true;

		auto texture = rawrbox::RESOURCES::getFile<rawrbox::ResourceTexture>("./content/textures/screem.png")->get();
		this->_texture2 = rawrbox::RESOURCES::getFile<rawrbox::ResourceGIF>("./content/textures/meow3.gif")->get();
		auto texture3 = rawrbox::RESOURCES::getFile<rawrbox::ResourceTexture>("./content/textures/displacement_test.png")->get();

		this->_font = rawrbox::RESOURCES::getFile<rawrbox::ResourceFont>("cour.ttf")->getSize(16);

		// Model test ----
		{
			auto mesh = this->_model->generatePlane({2, 0, 0}, {0.5F, 0.5F});
			mesh.setTexture(texture);
			this->_model->addMesh(mesh);
		}

		{
			auto mesh = this->_model->generateCube({-2, 0, 0}, {0.5F, 0.5F, 0.5F}, rawrbox::Colors::White);
			mesh.setTexture(this->_texture2);
			this->_model->addMesh(mesh);
		}

		{
			auto mesh = this->_model->generateCube({-3, 0, 0}, {0.5F, 0.5F, 0.5F}, rawrbox::Colors::White);
			mesh.setTexture(this->_texture2);
			mesh.setVertexSnap(24.F);
			mesh.setOptimizable(false);
			this->_model->addMesh(mesh);
		}

		{
			auto mesh = this->_model->generateAxis(1, {0.F, 0.F, 0.F});
			this->_model->addMesh(mesh);
		}

		{
			auto mesh = this->_model->generateGrid(12, {0.F, 0.F, 0.F});
			this->_model->addMesh(mesh);
		}

		// -----

		// Displacement test ----
		this->_displacement = std::make_unique<rawrbox::Displacement<>>(64);
		this->_displacement->setPos({0, 0.1F, -4});
		this->_displacement->setScale({0.05F, 0.05F, 0.05F});
		this->_displacement->setTexture(this->_texture2);
		this->_displacement->setHeightMap(texture3, 16.F);
		this->_displacement->upload();
		// -----

		// Sprite test ----
		{
			auto mesh = this->_sprite->generatePlane({0, 1, 0}, {0.2F, 0.2F});
			mesh.setTexture(texture);
			this->_sprite->addMesh(mesh);
		}
		// -----

		// Text test ----
		{
			this->_text->addText(*this->_font, "PLANE", {2.F, 0.5F, 0});
			this->_text->addText(*this->_font, "CUBE", {-2.F, 0.55F, 0});
			this->_text->addText(*this->_font, "CUBE\nVertex snap", {-3.F, 0.55F, 0});
			this->_text->addText(*this->_font, "AXIS", {0.F, 0.5F, 0});
			this->_text->addText(*this->_font, "SPRITE", {0.F, 1.2F, 0});
		}
		// ------

		this->_model->upload();
		this->_sprite->upload();
		this->_text->upload();
	}

	void Game::onThreadShutdown(rawrbox::ENGINE_THREADS thread) {
		if (thread == rawrbox::ENGINE_THREADS::THREAD_INPUT) return;

		this->_texture2 = nullptr;
		this->_camera.reset();

		this->_displacement.reset();
		this->_model.reset();
		this->_sprite.reset();
		this->_text.reset();

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
		if (this->_texture2 == nullptr || this->_camera == nullptr) return;
		this->_camera->update();
		this->_texture2->step();
	}

	void Game::drawWorld() {
		if (this->_model == nullptr || this->_sprite == nullptr || this->_text == nullptr || this->_displacement == nullptr) return;

		auto& pos = this->_camera->getPos();

		this->_model->draw(pos);
		this->_displacement->draw(pos);
		this->_sprite->draw(pos);
		this->_text->draw(pos);
	}

	void Game::printFrames() {
		const bgfx::Stats* stats = bgfx::getStats();

		bgfx::dbgTextPrintf(1, 4, 0x6f, "GPU %0.6f [ms]", double(stats->gpuTimeEnd - stats->gpuTimeBegin) * 1000.0 / stats->gpuTimerFreq);
		bgfx::dbgTextPrintf(1, 5, 0x6f, "CPU %0.6f [ms]", double(stats->cpuTimeEnd - stats->cpuTimeBegin) * 1000.0 / stats->cpuTimerFreq);
		bgfx::dbgTextPrintf(1, 6, 0x6f, fmt::format("TRIANGLES: {} ----->    DRAW CALLS: {}", stats->numPrims[bgfx::Topology::TriList], stats->numDraw).c_str());
	}

	void Game::draw() {
		if (this->_window == nullptr) return;
		this->_window->clear(); // Clean up and set renderer

		// DEBUG ----
		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(1, 1, 0x1f, "002-generated-models");
		bgfx::dbgTextPrintf(1, 2, 0x3f, "Description: Generated models test");
		printFrames();
		// -----------

		if (this->_ready) {
			this->drawWorld();
		} else {
			bgfx::dbgTextPrintf(1, 10, 0x70, "                                   ");
			bgfx::dbgTextPrintf(1, 11, 0x70, "          LOADING CONTENT          ");
			bgfx::dbgTextPrintf(1, 12, 0x70, "                                   ");
		}

		this->_window->frame(); // Commit primitives
		bgfx::setViewTransform(rawrbox::CURRENT_VIEW_ID, this->_camera->getViewMtx().data(), this->_camera->getProjMtx().data());
	}

} // namespace model
