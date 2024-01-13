
#include <rawrbox/render/cameras/orbital.hpp>
#include <rawrbox/render/models/utils/mesh.hpp>
#include <rawrbox/render/resources/texture.hpp>
#include <rawrbox/render/static.hpp>
#include <rawrbox/resources/manager.hpp>
#include <rawrbox/webm/resources/webm.hpp>
#include <rawrbox/webm/textures/webm.hpp>

#include <webm_test/game.hpp>

#include <fmt/format.h>

namespace webm_test {
	void Game::setupGLFW() {
		auto window = rawrbox::Window::createWindow();
		window->setMonitor(-1);
		window->setTitle("WEBM TEST");
		window->init(1024, 768, rawrbox::WindowFlags::Window::WINDOWED);
		window->onWindowClose += [this](auto& /*w*/) { this->shutdown(); };
	}

	void Game::init() {
		auto window = rawrbox::Window::getWindow();

		// Setup renderer
		auto render = window->createRenderer();
		render->onIntroCompleted = [this]() { this->loadContent(); };
		render->setDrawCall([this](const rawrbox::DrawPass& pass) {
			if (pass != rawrbox::DrawPass::PASS_OPAQUE) return;
			this->drawWorld();
		});
		// ---------------

		// Setup camera
		auto cam = render->setupCamera<rawrbox::CameraOrbital>(*window);
		cam->setPos({0.F, 5.F, -10.F});
		cam->setAngle({0.F, 0.F, 0.F, 0.F});
		// --------------

		// Setup loaders
		rawrbox::RESOURCES::addLoader<rawrbox::TextureLoader>();
		rawrbox::RESOURCES::addLoader<rawrbox::WEBMLoader>();
		// ----------

		render->init();
	}

	void Game::loadContent() {
		std::array initialContentFiles = {
		    std::make_pair<std::string, uint32_t>("./assets/video/webm_test.webm", 0 | rawrbox::WEBMLoadFlags::PRELOAD),
		    std::make_pair<std::string, uint32_t>("./assets/video/webm_test_2.webm", 0 | rawrbox::WEBMLoadFlags::PRELOAD)};

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
		auto tex = rawrbox::RESOURCES::getFile<rawrbox::ResourceWEBM>("./assets/video/webm_test.webm")->get<rawrbox::TextureWEBM>();
		auto tex2 = rawrbox::RESOURCES::getFile<rawrbox::ResourceWEBM>("./assets/video/webm_test_2.webm")->get<rawrbox::TextureWEBM>();
		tex2->setTextureUV(rawrbox::TEXTURE_UV::UV_FLIP_V);
		tex->setTextureUV(rawrbox::TEXTURE_UV::UV_FLIP_V);
		// tex->setLoop(false);
		tex->onEnd += [tex]() {
			fmt::print("[RawrBox] WEBM reached end\n");
			tex->seek(6500);
		};

		this->_model->setOptimizable(false);

		{
			auto mesh = rawrbox::MeshUtils::generatePlane({2.F, 4.0F, 0.F}, {4.F, 3.F});
			mesh.setTexture(tex);
			this->_model->addMesh(mesh);
		}

		{
			auto mesh = rawrbox::MeshUtils::generatePlane({-2.F, 4.0F, 0.F}, {4.F, 7.F});
			mesh.setTexture(tex2);
			this->_model->addMesh(mesh);
		}

		{
			auto mesh = rawrbox::MeshUtils::generateGrid(12, {0.F, 0.F, 0.F});
			this->_model->addMesh(mesh);
		}
		// ----

		this->_model->upload();
		this->_ready = true;
	}

	void Game::onThreadShutdown(rawrbox::ENGINE_THREADS thread) {
		if (thread == rawrbox::ENGINE_THREADS::THREAD_INPUT) {
			rawrbox::Window::shutdown();
		} else {
			this->_model.reset();

			rawrbox::RESOURCES::shutdown();
		}
	}

	void Game::pollEvents() {
		rawrbox::Window::pollEvents();
	}

	void Game::update() {
		rawrbox::Window::update();
	}

	void Game::drawWorld() {
		if (!this->_ready) return;
		if (this->_model != nullptr) this->_model->draw();
	}

	void Game::draw() {
		rawrbox::Window::render(); // Commit primitives
	}
} // namespace webm_test
