#include <rawrbox/engine/static.hpp>
#include <rawrbox/scripting/manager.hpp>
#include <rawrbox/scripting/mod.hpp>
#include <rawrbox/scripting/utils/lua.hpp>
#include <rawrbox/utils/logger.hpp>
#include <rawrbox/utils/path.hpp>
#include <rawrbox/utils/time.hpp>

#include <fmt/format.h>

namespace rawrbox {
	// PROTECTED ----
	std::unordered_map<std::string, std::unique_ptr<rawrbox::Mod>> SCRIPTING::_mods = {};
	std::unordered_map<std::string, std::vector<std::string>> SCRIPTING::_loadedLuaFiles = {};

	std::unique_ptr<rawrbox::FileWatcher> SCRIPTING::_watcher = nullptr;
	std::unique_ptr<rawrbox::Hooks> SCRIPTING::_hooks = nullptr;
	std::vector<std::unique_ptr<rawrbox::ScriptingPlugin>> SCRIPTING::_plugins = {};

	// LOGGER ------
	std::unique_ptr<rawrbox::Logger> SCRIPTING::_logger = std::make_unique<rawrbox::Logger>("RawrBox-SCRIPTING");
	// -------------

	bool SCRIPTING::_hotReloadEnabled = false;

	// LUA ------
	lua_State* SCRIPTING::_L = nullptr;
	// -------------

	// --------------

	// PUBLIC ----
	bool SCRIPTING::initialized = false;
	// ------

	// LOAD -----
	void SCRIPTING::loadLibraries() {
		if (_L == nullptr) throw _logger->error("LUA is not set! Reference got destroyed?");

		// COMMON -----
		luaL_openlibs(_L); // Should be safe, since LUAU takes care of non-secure libs (https://luau-lang.org/sandbox#library)
				   //  -----
	}

	void SCRIPTING::loadTypes() {
		if (_L == nullptr) throw _logger->error("LUA is not set! Reference got destroyed?");
		// Register types, these will be read-only & sandboxed!
	}

	void SCRIPTING::loadGlobals() {
		if (_L == nullptr) throw _logger->error("LUA is not set! Reference got destroyed?");
		// Register globals, these will be read-only & sandboxed!

		auto globalTable = luabridge::getGlobalNamespace(_L);

		// TIME UTILS ---
		globalTable.addFunction("curtime", []() { return rawrbox::TimeUtils::curtime(); });
		globalTable.addFunction("time", []() { return rawrbox::TimeUtils::time(); });
		// -------------

		// UTILS ----
		globalTable.addFunction("printTable", [](lua_State* state) {
			nlohmann::json json = rawrbox::LuaUtils::luaToJsonObject(state);
			_logger->info("{}\n", json.dump(1, ' ', false));
		});

		// Override print to support fmt?
		globalTable.addFunction("print", [](lua_State* state) {
			int nargs = lua_gettop(state);

			std::vector<std::string> prtData = {};
			for (int i = 1; i <= nargs; i++) {
				if (!lua_isstring(state, i)) {
					prtData.emplace_back("nil"); // Cannot be converted
				} else {
					prtData.emplace_back(lua_tostring(state, i));
				}
			}

			if (prtData.empty()) return;
			_logger->info("{}", fmt::join(prtData, " "));
		});

		globalTable.addFunction("include", [](lua_State* state) {
			if (lua_type(state, 1) != LUA_TSTRING) throw std::runtime_error("Invalid param, string expected");
			auto path = lua_tostring(state, 1);

			lua_getfield(state, LUA_ENVIRONINDEX, "__mod_id");
			if (lua_type(state, -1) != LUA_TSTRING) throw std::runtime_error("Invalid mod! Missing '__mod_id' on lua env");
			auto modID = lua_tostring(state, -1);

			lua_getfield(state, LUA_ENVIRONINDEX, "__mod_folder");
			if (lua_type(state, -1) != LUA_TSTRING) throw std::runtime_error("Invalid mod! Missing '__mod_folder' on lua env");
			auto modFolder = lua_tostring(state, -1);

			auto fixedPath = LuaUtils::getContent(path, modFolder);
			rawrbox::LuaUtils::compileAndLoad(state, modID, fixedPath);
			rawrbox::LuaUtils::run(state);

			// Register file for hot-reloading
			registerLoadedFile(modID, fixedPath);
			// ----
		});
		// ----------

		// ENGINE ------
		globalTable.beginNamespace("engine")
		    .addProperty("deltaTime", &rawrbox::DELTA_TIME, false)
		    .addProperty("fixedDeltaTime", &rawrbox::FIXED_DELTA_TIME, false)
		    .addProperty("frameAlpha", &rawrbox::FRAME_ALPHA, false)
		    .endNamespace();
		// -------------
	}
	// -------------

	// HOT RELOAD -----
	void SCRIPTING::registerLoadedFile(const std::string& modId, const std::string& filePath) {
		auto mdFnd = _loadedLuaFiles.find(modId);
		if (mdFnd != _loadedLuaFiles.end()) {
			auto fileFnd = std::find(mdFnd->second.begin(), mdFnd->second.end(), filePath);
			if (fileFnd != mdFnd->second.end()) return; // Already registered
			mdFnd->second.push_back(filePath);
		} else {
			_loadedLuaFiles[modId] = {filePath};
		}

		if (hotReloadEnabled()) _watcher->watchFile(filePath);
	}

	void SCRIPTING::hotReload(const std::string& filePath) {
		// Find the owner
		for (auto& pt : _loadedLuaFiles) {
			auto fnd = std::find(pt.second.begin(), pt.second.end(), filePath) != pt.second.end();
			if (!fnd) continue;

			auto md = _mods.find(pt.first);
			if (md == _mods.end()) return;
			_logger->warn("Hot-reloading lua file '{}'", filePath);

			// Cleanup and load -----
			auto env = md->second->getEnvironment();

			md->second->gc(); // Cleanup
			rawrbox::LuaUtils::compileAndLoad(env, md->first, filePath);
			rawrbox::LuaUtils::run(env);
			// ---------------

			// onModHotReload(md->second.get());
			break;
		};
	}
	// -------------
	// ----------

	void SCRIPTING::init(int hotReloadMs) {
		_L = luaL_newstate();

		_hotReloadEnabled = hotReloadMs > 0;
		if (_hotReloadEnabled) {
			_logger->info("Enabled lua hot-reloading\n  └── Delay: {}ms\n", hotReloadMs);

			_watcher = std::make_unique<rawrbox::FileWatcher>(
			    [](std::string pth, rawrbox::FileStatus status) {
				    if (status != rawrbox::FileStatus::modified) return;
				    hotReload(pth);
			    },
			    std::chrono::milliseconds(hotReloadMs));
			_watcher->start();
		}

		// Loading initial libs ---
		loadLibraries();
		loadTypes();
		loadGlobals();
		//  ----

		// Prepare mods (but dont load) ---
		prepareMods();
		// ----------------

		// Freeze lua env ---
		// No more modifications to the global table are allowed after this point
		luaL_sandbox(_L);
		// --------------

		// TEST ---
		/*rawrbox::Mod test = {_L, "test", "./assets/mods/test-luau"};
		rawrbox::Mod test2 = {_L, "test2", "./assets/mods/test-luau-2"};

		test.init();
		test.load();

		test2.init();
		test2.load();

		test.call("test");
		test2.call("test");

		test.call("init");
		test2.call("init");*/
		// ----------
	}

	void SCRIPTING::prepareMods() {
		if (!std::filesystem::exists("./mods")) throw _logger->error("Failed to locate folder './mods'"); // TODO: SUPPORT FOLDER CONFIGURATION

		// TODO: mod load ordering & mod settings to inject on sandboxed env
		for (auto& p : std::filesystem::directory_iterator("./mods")) {
			if (!p.is_directory()) continue;

			auto id = p.path().filename().string();
			auto folderPath = fmt::format("mods/{}", id);

			_mods.emplace(id, std::make_unique<rawrbox::Mod>(_L, id, folderPath));
		}
	}

	void SCRIPTING::load() {
		if (!std::filesystem::exists("./mods")) throw _logger->error("Failed to locate folder './mods'");

		// Load & initialize
		for (auto& mod : _mods) {
			mod.second->init();

			// loadLuaExtensions(mod.second.get());
			// loadGlobals(mod.second.get());
			// loadI18N(mod.second.get());

			try {
				mod.second->load();

				// Register file for hot-reloading
				registerLoadedFile(mod.first, mod.second->getEntryFilePath());
				// ----
			} catch (const cpptrace::exception_with_message err) {
				_logger->warn("{}", err.message());
			}
		}
		// -----
	}

	void SCRIPTING::shutdown() {
		_watcher.reset();
		_hooks.reset();

		_loadedLuaFiles.clear();
		_mods.clear();
		_plugins.clear();

		// Shutdown lua --
		rawrbox::LuaUtils::collect_garbage(_L);
		_L = nullptr;
		// ----------------
	}

	// UTILS ----
	lua_State* SCRIPTING::getLUA() { return _L; }

	const std::unordered_map<std::string, std::unique_ptr<Mod>>& SCRIPTING::getMods() { return _mods; }
	const std::vector<std::string> SCRIPTING::getModsIds() {
		std::vector<std::string> modNames = {};
		for (auto& mod : _mods) {
			modNames.push_back(mod.second->getID());
		}

		return modNames;
	}

	bool SCRIPTING::hotReloadEnabled() { return _hotReloadEnabled; }
	// -----
} // namespace rawrbox
