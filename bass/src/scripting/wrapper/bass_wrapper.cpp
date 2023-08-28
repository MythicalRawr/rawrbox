
#include <rawrbox/bass/scripting/wrapper/bass_wrapper.hpp>
#include <rawrbox/scripting/utils/lua.hpp>

#ifdef RAWRBOX_RESOURCES
	#include <rawrbox/resources/manager.hpp>
#endif

#include <fmt/format.h>

namespace rawrbox {
	BASSWrapper::BASSWrapper(rawrbox::Mod* mod) : _mod(mod) {}

	// UTILS -----
	float BASSWrapper::getMasterVolume() {
		return BASS::getMasterVolume();
	}

	void BASSWrapper::setMasterVolume(float volume, sol::optional<bool> set) {
		return BASS::setMasterVolume(volume, set.value_or(true));
	}

	void BASSWrapper::setListenerLocation(const rawrbox::Vector3f& location, sol::optional<rawrbox::Vector3f> front, sol::optional<rawrbox::Vector3f> top) {
		return BASS::setListenerLocation(location, front.value_or(rawrbox::Vector3f(0, 0, -1)), top.value_or(rawrbox::Vector3f(0, -1, 0)));
	}
	// -----

	// LOAD -----
	rawrbox::SoundInstanceWrapper BASSWrapper::loadSound(const std::string& path, sol::optional<uint32_t> flags) {
		if (this->_mod == nullptr) throw std::runtime_error("[RawrBox-FontLoader] MOD not set!");
		auto fixedPath = rawrbox::LuaUtils::getContent(path, this->_mod->getFolder());

#ifdef RAWRBOX_RESOURCES
		if (!rawrbox::RESOURCES::isLoaded(fixedPath)) {
			fmt::print("[Resources] Loading '{}' RUNTIME! You should load content on the mod's load stage!\n", fixedPath);
		}
#endif
		auto sound = BASS::loadSound(fixedPath, flags.value_or(0));
		if (sound == nullptr) throw std::runtime_error(fmt::format("[RawrBox-BASSWrapper] Failed to load sound '{}'", path));

		return {sound->createInstance()};
	}

	rawrbox::SoundInstanceWrapper BASSWrapper::loadHTTPSound(const std::string& url, sol::optional<uint32_t> flags) {
		auto sound = BASS::loadHTTPSound(url, flags.value_or(0));
		if (sound == nullptr) throw std::runtime_error(fmt::format("[RawrBox-BASSWrapper] Failed to load http sound '{}'", url));

		return {sound->createInstance()};
	}
	// -----

	void BASSWrapper::registerLua(sol::state& lua) {
		lua.new_usertype<BASSWrapper>("BASS",
		    sol::no_constructor,

		    // UTILS -----
		    "getMasterVolume", &BASSWrapper::getMasterVolume,
		    "setMasterVolume", &BASSWrapper::setMasterVolume,
		    "setListenerLocation", &BASSWrapper::setListenerLocation,
		    // -----

		    // LOAD ---
		    "loadSound", &BASSWrapper::loadSound,
		    "loadHTTPSound", &BASSWrapper::loadHTTPSound
		    // ---------
		);
	}
} // namespace rawrbox
