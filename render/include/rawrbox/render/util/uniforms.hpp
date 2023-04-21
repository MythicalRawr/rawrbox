#pragma once
#include <rawrbox/math/quaternion.hpp>
#include <rawrbox/math/vector2.hpp>
#include <rawrbox/math/vector3.hpp>

#include <bgfx/bgfx.h>

#include <string>

namespace rawrBox {
	class UniformUtils {
	public:
		static void setUniform(const bgfx::UniformHandle& handle, double data) {
			float d[1] = {static_cast<float>(data)};
			bgfx::setUniform(handle, d, 1);
		}

		static void setUniform(const bgfx::UniformHandle& handle, float data) {
			float d[1] = {data};
			bgfx::setUniform(handle, d, 1);
		}

		static void setUniform(const bgfx::UniformHandle& handle, rawrBox::Vector2f data) {
			float d[2] = {data.x, data.y};
			bgfx::setUniform(handle, d, 2);
		}

		static void setUniform(const bgfx::UniformHandle& handle, rawrBox::Vector3f data) {
			float d[3] = {data.x, data.y, data.z};
			bgfx::setUniform(handle, d, 3);
		}

		static void setUniform(const bgfx::UniformHandle& handle, rawrBox::Quaternion data) {
			float d[4] = {data.x, data.y, data.z, data.w};
			bgfx::setUniform(handle, d);
		}

		static void setUniform(const bgfx::UniformHandle& handle, rawrBox::Color data) {
			float d[4] = {data.r, data.g, data.b, data.a};
			bgfx::setUniform(handle, d);
		}
	};
} // namespace rawrBox
