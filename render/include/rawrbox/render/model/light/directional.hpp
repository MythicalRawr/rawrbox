#pragma once
#ifdef RAWRBOX_DEBUG
	#include <rawrbox/debug/gizmos.hpp>
#endif
#include <rawrbox/math/color.hpp>
#include <rawrbox/math/vector3.hpp>
#include <rawrbox/render/model/light/base.hpp>

namespace rawrBox {
	class LightDirectional : public rawrBox::LightBase {
	protected:
		rawrBox::Vector3 _direction;

	public:
		LightDirectional(rawrBox::Vector3f posMatrix, rawrBox::Vector3 dir, rawrBox::Colorf diffuse, rawrBox::Colorf specular) : rawrBox::LightBase(posMatrix, diffuse, specular), _direction(dir) {
#ifdef RAWRBOX_DEBUG
			GIZMOS::get().addLight(this);
#endif
		};

		LightDirectional(LightDirectional&&) = delete;
		LightDirectional& operator=(LightDirectional&&) = delete;
		LightDirectional(const LightDirectional&) = delete;
		LightDirectional& operator=(const LightDirectional&) = delete;
		~LightDirectional() override {
#ifdef RAWRBOX_DEBUG
			GIZMOS::get().removeLight(this);
#endif
		};

		LightType getType() override { return LightType::LIGHT_DIR; };
		std::array<float, 16> const getDataMatrix() override {
			return {
			    this->_diffuse.r, this->_specular.r, this->_direction.x, static_cast<float>(this->getType()),
			    this->_diffuse.g, this->_specular.g, this->_direction.y, 0,
			    this->_diffuse.b, this->_specular.b, this->_direction.z, 0,
			    0, 0, 0, this->_isOn ? 1.F : 0};
		}
	};
} // namespace rawrBox
