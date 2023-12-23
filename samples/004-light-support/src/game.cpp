
#include <rawrbox/render/cameras/orbital.hpp>
#include <rawrbox/render/light/point.hpp>
#include <rawrbox/render/light/spot.hpp>
#include <rawrbox/render/models/utils/mesh.hpp>
#include <rawrbox/render/plugins/clustered_light.hpp>
#include <rawrbox/render/resources/font.hpp>
#include <rawrbox/render/resources/texture.hpp>
#include <rawrbox/resources/manager.hpp>
#include <rawrbox/utils/keys.hpp>

#include <light/game.hpp>

namespace light {

	void Game::setupGLFW() {
		auto window = rawrbox::Window::createWindow();
		window->setMonitor(-1);
		window->setTitle("LIGHT TEST");
		window->init(1024, 768, rawrbox::WindowFlags::Window::WINDOWED);
		window->onWindowClose += [this](auto& /*w*/) { this->shutdown(); };
	}

	void Game::init() {
		auto window = rawrbox::Window::getWindow();

		// Setup renderer
		auto render = window->createRenderer();
		render->skipIntros(true);
		render->addPlugin<rawrbox::ClusteredLightPlugin>();
		render->onIntroCompleted = [this]() { this->loadContent(); };
		render->setDrawCall([this](const rawrbox::DrawPass& pass) {
			if (pass != rawrbox::DrawPass::PASS_OPAQUE) return;
			this->drawWorld();
		});
		// ---------------

		// Setup camera
		auto cam = render->setupCamera<rawrbox::CameraOrbital>(*window);
		cam->setPos({0.F, 5.F, -5.F});
		cam->setAngle({0.F, rawrbox::MathUtils::toRad(-45), 0.F, 0.F});
		// --------------

		// Add loaders ----
		rawrbox::RESOURCES::addLoader<rawrbox::FontLoader>();
		rawrbox::RESOURCES::addLoader<rawrbox::TextureLoader>();
		// -----

		render->init();
	}

	void Game::loadContent() {
		std::vector<std::pair<std::string, uint32_t>> initialContentFiles = {
		    std::make_pair<std::string, uint32_t>("./assets/fonts/LiberationMono-Regular.ttf", 0),
		    std::make_pair<std::string, uint32_t>("./assets/textures/light_test/planks.png", 0),
		    std::make_pair<std::string, uint32_t>("./assets/textures/light_test/planksSpec.png", 0)};

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
		this->_font = rawrbox::RESOURCES::getFile<rawrbox::ResourceFont>("./assets/fonts/LiberationMono-Regular.ttf")->getSize(24);

		auto tex = rawrbox::RESOURCES::getFile<rawrbox::ResourceTexture>("./assets/textures/light_test/planks.png")->get();
		auto texSpec = rawrbox::RESOURCES::getFile<rawrbox::ResourceTexture>("./assets/textures/light_test/planksSpec.png")->get();

		// Setup
		{
			auto mesh = rawrbox::MeshUtils::generateGrid<>(12, {0.F, 0.F, 0.F});
			this->_model->addMesh(mesh);
		}

		{
			auto mesh = rawrbox::MeshUtils::generatePlane<rawrbox::MaterialLit>({3.5F, 0.F, -0.01F}, {3.F, 3.F}, rawrbox::Colors::White());
			mesh.setTexture(tex);
			mesh.setSpecularTexture(texSpec, 25.F);
			mesh.setEulerAngle({rawrbox::MathUtils::toRad(90), 0, 0});
			this->_model2->addMesh(mesh);
		}

		{
			auto mesh = rawrbox::MeshUtils::generatePlane<rawrbox::MaterialLit>({-3.5F, 0.F, -0.01F}, {3.F, 3.F}, rawrbox::Colors::White());
			mesh.setTexture(tex);
			mesh.setSpecularTexture(texSpec, 25.F);
			mesh.setEulerAngle({rawrbox::MathUtils::toRad(90), 0, 0});
			this->_model2->addMesh(mesh);
		}
		// ----

		// Text test ----
		this->_text->addText(*this->_font, "POINT LIGHT", {-3.5F, 0.5F, 0});
		this->_text->addText(*this->_font, "SPOT LIGHT", {3.5F, 0.5F, 0});
		// ------

		rawrbox::LIGHTS::setFog(rawrbox::FOG_TYPE::FOG_EXP, 40.F, 0.8F);

		rawrbox::LIGHTS::addLight<rawrbox::PointLight>(rawrbox::Vector3f{-3.5F, 0.2F, 0}, rawrbox::Colors::Blue(), 1.2F);
		rawrbox::LIGHTS::addLight<rawrbox::SpotLight>(rawrbox::Vector3f{3.5F, 1.F, 0}, rawrbox::Vector3f{0.F, -1.F, 0.F}, rawrbox::Colors::Red(), 20.F, 40.F, 100.F);

		this->_model->upload();
		this->_model2->upload();
		this->_text->upload();

#ifdef _DEBUG
		// Setup binds ---
		rawrbox::Window::getWindow()->onKey += [](rawrbox::Window& /*w*/, uint32_t key, uint32_t /*scancode*/, uint32_t action, uint32_t /*mods*/) {
			if (action != KEY_ACTION_UP) return;

			if (key == KEY_F1) rawrbox::MaterialLit::DEBUG_LEVEL = 0;
			if (key == KEY_F2) rawrbox::MaterialLit::DEBUG_LEVEL = 1;
		};
		// ----------
#endif

		this->_ready = true;
	}

	void Game::onThreadShutdown(rawrbox::ENGINE_THREADS thread) {
		if (thread == rawrbox::ENGINE_THREADS::THREAD_INPUT) {
			rawrbox::Window::shutdown();
		} else {
			rawrbox::RESOURCES::shutdown();
			rawrbox::ASYNC::shutdown();
		}

		this->_model.reset();
		this->_model2.reset();
		this->_text.reset();
	}

	void Game::pollEvents() {
		rawrbox::Window::pollEvents();
	}

	void Game::update() {
		rawrbox::Window::update();

		/*if (this->_ready) {
			this->_sunDir = {std::cos(rawrbox::FRAME * 0.01F) * 1.F, 1.F, std::sin(rawrbox::FRAME * 0.01F) * 1.F};
			rawrbox::LIGHTS::setSun(this->_sunDir, {0.2F, 0.2F, 0.2F, 1.F});

			auto light = rawrbox::LIGHTS::getLight(0);
			if (light != nullptr) {
				light->setOffsetPos({0, std::cos(rawrbox::FRAME * 0.01F) * 1.F, 0});
			}

			light = rawrbox::LIGHTS::getLight(1);
			if (light != nullptr) {
				light->setOffsetPos({0, std::cos(rawrbox::FRAME * 0.01F) * 1.F, 0});
			}
		}*/
	}

	void Game::drawWorld() {
		if (!this->_ready || this->_model == nullptr || this->_model2 == nullptr || this->_text == nullptr) return;

		this->_model->draw();
		this->_model2->draw();

		this->_text->draw();
	}

	void Game::draw() {
		rawrbox::Window::render(); // Commit primitives
	}
} // namespace light
