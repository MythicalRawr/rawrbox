
#pragma once

#include <rawrbox/scripting/utils/lua.hpp>
#include <rawrbox/utils/logger.hpp>

#include <filesystem>
#include <memory>

namespace rawrbox {
	class Mod {
		// LUA ----
		lua_State* _L = nullptr;
		// --------

		// TABLE ---
		luabridge::LuaRef _modTable;
		// ---------

		// LOGGER ------
		std::unique_ptr<rawrbox::Logger> _logger = std::make_unique<rawrbox::Logger>("RawrBox-Mod");
		// -------------

		// Settings ---
		std::filesystem::path _folder;
		std::string _id = "UNKNOWN";
		// -----------

	public:
		Mod(const std::string id, std::filesystem::path folderName);
		Mod(const Mod&) = delete;
		Mod(Mod&&) = delete;
		Mod& operator=(const Mod&) = delete;
		Mod& operator=(Mod&&) = delete;
		virtual ~Mod();

		virtual void init();
		virtual void gc();

		// LOADING -------
		virtual void load();
		// ---------------

		virtual void script(const std::string& script);

		// UTILS ----
		[[nodiscard]] virtual const std::string& getID() const;
		[[nodiscard]] virtual const std::string getEntryFilePath() const;
		[[nodiscard]] virtual const std::filesystem::path& getFolder() const;

		virtual lua_State* getEnvironment();
		// -----

		template <typename... CallbackArgs>
		luabridge::LuaResult call(const std::string& name, CallbackArgs&&... args) {
			auto fnc = this->_modTable[name];

			try {
				return luabridge::call(fnc, std::forward<CallbackArgs>(args)...);
			} catch (luabridge::LuaException& err) {
				throw _logger->error("{}", err.what());
			}
		}
	};
} // namespace rawrbox
