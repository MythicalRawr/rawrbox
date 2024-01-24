#include <rawrbox/render/lights/manager.hpp>

#ifdef RAWRBOX_SCRIPTING
	#include <rawrbox/scripting/scripting.hpp>
// #include <rawrbox/render_temp/scripting/wrappers/light/base_wrapper.hpp>
#endif

namespace rawrbox {
	LightBase::LightBase(const rawrbox::Vector3f& pos, const rawrbox::Colorf& color, float radius) : _pos(pos), _color(color), _radius(radius){};

#ifdef RAWRBOX_SCRIPTING
	LightBase::~LightBase() {
		if (this->_luaWrapper.valid()) this->_luaWrapper.abandon();
	}

	void LightBase::initializeLua() {
		if (!SCRIPTING::initialized) return;
		// this->_luaWrapper = sol::make_object(rawrbox::SCRIPTING::getLUA(), rawrbox::LightBaseWrapper(shared_from_this()));
	}

	sol::object& LightBase::getScriptingWrapper() {
		if (!this->_luaWrapper.valid()) this->initializeLua();
		return this->_luaWrapper;
	}
#endif

	const rawrbox::Colorf LightBase::getColor() const { return this->_color; }
	void LightBase::setColor(const rawrbox::Colorf& col) {
		if (this->_color == col) return;

		this->_color = col;
		rawrbox::__LIGHT_DIRTY__ = true;
	}

	float LightBase::getRadius() const { return this->_radius; }
	void LightBase::setRadius(float radius) {
		if (this->_radius == radius) return;

		this->_radius = radius;
		rawrbox::__LIGHT_DIRTY__ = true;
	}

	[[nodiscard]] float LightBase::getIntensity() const { return this->_intensity; }
	void LightBase::setIntensity(float intensity) {
		if (this->_intensity == intensity) return;

		this->_intensity = intensity;
		rawrbox::__LIGHT_DIRTY__ = true;
	}

	const rawrbox::Vector4f LightBase::getData() const { return {}; }

	void LightBase::setId(size_t id) { this->_id = id; };
	size_t LightBase::id() const { return this->_id; };

	bool LightBase::isOn() const { return this->_isOn; }
	void LightBase::setStatus(bool on) {
		if (this->_isOn == on) return;

		this->_isOn = on;
		rawrbox::__LIGHT_DIRTY__ = true;
	};

	const rawrbox::Vector3f& LightBase::getPos() const { return this->_pos; }
	void LightBase::setPos(const rawrbox::Vector3f& pos) {
		if (this->_pos == pos) return;

		this->_pos = pos;
		rawrbox::__LIGHT_DIRTY__ = true;
	}

	const rawrbox::Vector3f& LightBase::getOffsetPos() const { return this->_offset; }
	void LightBase::setOffsetPos(const rawrbox::Vector3f& pos) {
		if (this->_offset == pos) return;

		this->_offset = pos;
		rawrbox::__LIGHT_DIRTY__ = true;
	}

	const rawrbox::Vector3f LightBase::getWorldPos() const { return this->getPos() + this->getOffsetPos(); }
	rawrbox::LightType LightBase::getType() const { return rawrbox::LightType::UNKNOWN; }

	const rawrbox::Vector3f& LightBase::getDirection() const { return this->_direction; };
	void LightBase::setDirection(const rawrbox::Vector3f& dir) {
		if (this->_direction == dir) return;

		this->_direction = dir;
		rawrbox::__LIGHT_DIRTY__ = true;
	}

} // namespace rawrbox
