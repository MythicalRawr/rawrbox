#pragma once

#include <rawrbox/bass/sound/base.hpp>
#include <rawrbox/utils/event_named.hpp>

#include <filesystem>
#include <unordered_map>

namespace rawrBox {
	class SoundManager {
	protected:
		bool _initialized = false;

		float _masterVolume = 1.0F;
		bool _muteOnUnfocus = true;

		rawrBox::Vector3f _oldLocation = {};

	public:
		std::unordered_map<std::string, std::shared_ptr<rawrBox::SoundBase>> httpSounds = {}; // Keep alive the http sounds

		rawrBox::EventNamed<std::pair<uint32_t, double>> onBEAT;
		rawrBox::EventNamed<std::pair<uint32_t, float>> onBPM;
		rawrBox::EventNamed<uint32_t> onSoundEnd;

		void initialize();
		void shutdown();

		std::shared_ptr<rawrBox::SoundBase> loadSound(const std::filesystem::path& path, uint32_t flags = SoundFlags::NONE);
		std::shared_ptr<rawrBox::SoundBase> loadHTTPSound(const std::string& url, uint32_t flags = SoundFlags::NONE);

		// UTILS -----
		[[nodiscard]] float getMasterVolume() const;

		void setMasterVolume(float volume, bool set = true);
		void setMuteOnUnfocus(bool set);
		void setHasFocus(bool hasFocus);
		void setListenerLocation(const rawrBox::Vector3f& location, const rawrBox::Vector3f& front = {0, 0, -1}, const rawrBox::Vector3f& top = {0, -1, 0});
		// -----

		static SoundManager& get() {
			static SoundManager cl;
			return cl;
		}
	};
} // namespace rawrBox
