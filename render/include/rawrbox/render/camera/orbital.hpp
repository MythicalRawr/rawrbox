#pragma once

#include <rawrbox/math/quaternion.hpp>
#include <rawrbox/math/vector2.hpp>
#include <rawrbox/math/vector3.hpp>
#include <rawrbox/render/camera/perspective.hpp>
#include <rawrbox/render/window.hpp>

namespace rawrbox {
	class CameraOrbital : public CameraPerspective {
	protected:
		rawrbox::Window* _window = nullptr;

		float _speed = 0.F;

		bool _rightClick = false;
		rawrbox::Vector2i _oldMousePos = {};

	public:
		explicit CameraOrbital(rawrbox::Window* window, float speed = 8.F);
		void update(float deltaTime);
	};
} // namespace rawrbox
