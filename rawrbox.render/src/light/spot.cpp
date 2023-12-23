#include <rawrbox/render/light/spot.hpp>

namespace rawrbox {
	SpotLight::SpotLight(const rawrbox::Vector3f& posMatrix, const rawrbox::Vector3f& direction, const rawrbox::Colorf& color, float penumbra, float umbra, float power) : rawrbox::LightBase(posMatrix, color, power), _penumbra(penumbra), _umbra(umbra) {
		this->_direction = direction.normalized();
	};

	rawrbox::LightType SpotLight::getType() const { return LightType::SPOT; };
	const rawrbox::Vector4f SpotLight::getData() const { return {_penumbra, _umbra, 0, 0}; }

	float SpotLight::getUmbra() const { return _umbra; };
	float SpotLight::getPenumbra() const { return _penumbra; };

} // namespace rawrbox
