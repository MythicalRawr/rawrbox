#pragma once

#include <rawrbox/render/camera/perspective.hpp>
#include <rawrbox/render/engine.hpp>
#include <rawrbox/render/model/assimp/model_imported.hpp>
#include <rawrbox/render/renderer.hpp>
#include <rawrbox/render/window.hpp>

#include <memory>

namespace assimp {
	class Game : public rawrBox::Engine {
		std::unique_ptr<rawrBox::Window> _window = nullptr;
		std::shared_ptr<rawrBox::Renderer> _render = nullptr;
		std::shared_ptr<rawrBox::CameraPerspective> _camera = nullptr;

		std::shared_ptr<rawrBox::ModelImported> _model = nullptr;
		std::shared_ptr<rawrBox::ModelImported> _model2 = nullptr;
		std::shared_ptr<rawrBox::ModelImported> _model3 = nullptr;

		bool _rightClick = false;
		rawrBox::Vector2i _oldMousePos = {};

	public:
		using Engine::Engine;

		void init() override;
		void shutdown() override;
		void pollEvents() override;
		void update(float deltaTime, int64_t gameTime) override;
		void draw(const double alpha) override;

		void loadContent();
		void drawWorld();
	};
} // namespace assimp
