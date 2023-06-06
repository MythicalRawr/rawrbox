#pragma once

#include <rawrbox/math/vector3.hpp>
#include <rawrbox/render/model/sprite.hpp>
#include <rawrbox/render/texture/image.hpp>

#include <array>
#include <cmath>
#include <memory>
#include <string>
#include <unordered_map>

namespace rawrbox {

#if RAWRBOX_BASS
	class SoundInstance;
#endif

	class Emitter;
	class LightBase;
	class GIZMOS {
	protected:
		static std::unique_ptr<rawrbox::Sprite<>> _gizmo_lights;
		static std::unique_ptr<rawrbox::Sprite<>> _gizmo_sounds;
		static std::unique_ptr<rawrbox::Sprite<>> _gizmo_emitters;

		static std::unordered_map<std::string, std::unique_ptr<rawrbox::TextureImage>> _textures;

	public:
		static void shutdown();
		static void upload();

		// UTILS ----
		static void addLight(rawrbox::LightBase* l);
		static void removeLight(rawrbox::LightBase* l);

		static void addEmitter(rawrbox::Emitter* l);
		static void removeEmitter(rawrbox::Emitter* l);

#if RAWRBOX_BASS
		static void addSound(rawrbox::SoundInstance* l);
		static void removeSound(rawrbox::SoundInstance* l);
#endif
		// ------
		static void updateGizmo(const std::string& id, const rawrbox::Vector3f& pos);
		static void draw();
	};
} // namespace rawrbox
