#include <rawrbox/engine/static.hpp>
#include <rawrbox/scripting/manager.hpp>
#include <rawrbox/scripting/mod.hpp>
#include <rawrbox/scripting/utils/lua.hpp>
#include <rawrbox/scripting/wrappers/console.hpp>
#include <rawrbox/scripting/wrappers/console_command.hpp>
#include <rawrbox/scripting/wrappers/hooks.hpp>
#include <rawrbox/scripting/wrappers/i18n.hpp>
#include <rawrbox/scripting/wrappers/io.hpp>
#include <rawrbox/scripting/wrappers/math/aabb.hpp>
#include <rawrbox/scripting/wrappers/math/bbox.hpp>
#include <rawrbox/scripting/wrappers/math/color.hpp>
#include <rawrbox/scripting/wrappers/math/matrix.hpp>
#include <rawrbox/scripting/wrappers/math/vector2.hpp>
#include <rawrbox/scripting/wrappers/math/vector3.hpp>
#include <rawrbox/scripting/wrappers/math/vector4.hpp>
#include <rawrbox/scripting/wrappers/mod.hpp>
#include <rawrbox/scripting/wrappers/timer.hpp>
#include <rawrbox/utils/i18n.hpp>
#include <rawrbox/utils/logger.hpp>
#include <rawrbox/utils/path.hpp>
#include <rawrbox/utils/time.hpp>

#include <fmt/args.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

/*⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣠⣤⣤⣤⣄⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣤⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣤⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣤⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢺⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠋⠉⠉⠁⠀⠈⠛⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⢀⠀⠙⣿⠿⣿⣿⣿⣿⣿⣿⣿⣷⣶⣾⣿⣿⣿⣶⣤⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠻⠿⢿⣿⣿⣿⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠟⠛⠛⣛⡛⡿⣿⡟⢻⣿⣿⣿⣿⣿⣿⣣⣼⡟⠛⠳⣆⠘⢻⣿⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠋⠀⣶⠉⠉⠙⠶⠉⠁⣸⣿⣿⣿⣿⣿⣿⣯⣝⣀⣀⣀⣩⣄⣽⣿⣿⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣶⣶⣶⣾⠟⠀⣰⣿⣿⣿⡿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⠿⣿⣿⣿⣾⠀⣼⣿⣿⣿⣿⣧⣈⡙⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⣿⣿⣏⠀⠹⢿⣿⣿⡿⠿⠿⢿⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⠟⠀⠀⠈⠙⠛⢛⣃⣀⢀⠈⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀ ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⡿⠁⢠⡶⠿⠿⠿⠟⠻⠿⠛⠿⣷⣦⣹⣿⣿⣿⣿⣿⣿⣿⣿⡿⠀⠀⠀ ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣈⠿⣿⠀⣀⡈⠋⠀⢀⣀⣀⣀⣀⡀⠈⠹⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠀⠉⣰⡏⠀⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠛⠀⠾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⡿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡏⠁⠛⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢽⠛⠛⣿⣿⣿⡟⣿⣿⣿⣿⡇⠀⠀⠒⠀⢠⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠖⠃⡀⢿⡿⠃⢸⣿⣿⡅⠀⢀⣀⠀⠀⢫⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠛⠛⠃⠀⠘⢿⡿⠇⢸⡇⠀⠀⢄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣴⣦⣤⣄⣀⣤⣤⣾⣿⣿⣦⢻⠀⢀⣿⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠀⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣿⣿⣿⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣤⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣿⠿⠟⠋⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀

				   If you are stuck with lua, just remember...
							What would garry do?
*/

namespace rawrbox {
	// PROTECTED ----
	std::unordered_map<std::string, std::unique_ptr<rawrbox::Mod>> SCRIPTING::_mods = {};
	std::unordered_map<std::string, std::vector<std::filesystem::path>> SCRIPTING::_loadedLuaFiles = {};

	std::unique_ptr<rawrbox::FileWatcher> SCRIPTING::_watcher = nullptr;
	std::vector<std::unique_ptr<rawrbox::ScriptingPlugin>> SCRIPTING::_plugins = {};

	// LOGGER ------
	std::unique_ptr<rawrbox::Logger> SCRIPTING::_logger = std::make_unique<rawrbox::Logger>("RawrBox-SCRIPTING");
	// -------------

	rawrbox::Console* SCRIPTING::_console = nullptr;
	bool SCRIPTING::_hotReloadEnabled = false;
	// --------------

	// PUBLIC ----
	rawrbox::Event<rawrbox::Mod&> SCRIPTING::onRegisterTypes;
	rawrbox::Event<rawrbox::Mod&> SCRIPTING::onRegisterGlobals;
	rawrbox::Event<rawrbox::Mod&> SCRIPTING::onLoadLibraries;
	rawrbox::Event<rawrbox::Mod&> SCRIPTING::onModHotReload;

	bool SCRIPTING::initialized = false;
	// ------

	// LOAD -----
	void SCRIPTING::loadLibraries(rawrbox::Mod& mod) {
		auto* L = mod.getEnvironment();
		if (L == nullptr) throw _logger->error("LUA is not set! Reference got destroyed?");

		// COMMON -----
		luaL_openlibs(L); // Should be safe, since LUAU takes care of non-secure libs (https://luau-lang.org/sandbox#library)
				  //  -----

		// OTHER LIBS ---
		rawrbox::LuaUtils::compileAndLoadFile(L, "SHA", "./lua/sha2.lua");
		rawrbox::LuaUtils::compileAndLoadFile(L, "JSON", "./lua/json.lua");
		// --------------

		// Rawrbox LIBS ----
		rawrbox::LuaUtils::compileAndLoadFile(L, "RawrBox::Math", "./lua/math.lua");
		rawrbox::LuaUtils::compileAndLoadFile(L, "RawrBox::String", "./lua/string.lua");
		rawrbox::LuaUtils::compileAndLoadFile(L, "RawrBox::Table", "./lua/table.lua");
		// -----------------

		// Rawrbox enums ---
		if (_console != nullptr) rawrbox::LuaUtils::compileAndLoadFile(L, "RawrBox::Enums::Console", "./lua/enums/console.lua");
		rawrbox::LuaUtils::compileAndLoadFile(L, "RawrBox::Enums::Input", "./lua/enums/input.lua");
		// -----------------

		// Register plugins libraries ---
		for (auto& p : _plugins)
			p->loadLibraries(L);
		//  -----

		// Custom ----
		onLoadLibraries(mod);
		// ----
	}

	void SCRIPTING::loadTypes(rawrbox::Mod& mod) {
		auto* L = mod.getEnvironment();
		if (L == nullptr) throw _logger->error("LUA is not set! Reference got destroyed?");
		// Register types, these will be read-only & sandboxed!

		// Rawrbox ---
		rawrbox::Hooks::registerLua(L);
		rawrbox::TimerWrapper::registerLua(L);
		rawrbox::I18NWrapper::registerLua(L);
		rawrbox::MODWrapper::registerLua(L);

#ifdef RAWRBOX_SCRIPTING_UNSAFE
		rawrbox::IOWrapper::registerLua(L); // TODO: Might have security vulnerabilities
#endif

		if (_console != nullptr) {
			rawrbox::ConsoleCommandWrapper::registerLua(L);
			rawrbox::ConsoleWrapper::registerLua(L);
		}
		// -----------

		// Rawrbox Math
		rawrbox::Vector2Wrapper::registerLua(L);
		rawrbox::Vector3Wrapper::registerLua(L);
		rawrbox::Vector4Wrapper::registerLua(L);

		rawrbox::ColorWrapper::registerLua(L);
		rawrbox::AABBWrapper::registerLua(L);
		rawrbox::BBOXWrapper::registerLua(L);

		rawrbox::MatrixWrapper::registerLua(L);
		// -------------

		// Register plugins types ---
		for (auto& p : _plugins)
			p->registerTypes(L);
		//  -----

		// Custom ----
		onRegisterTypes(mod);
		// ----
	}

	void SCRIPTING::loadGlobals(rawrbox::Mod& mod) {
		auto* L = mod.getEnvironment();
		if (L == nullptr) throw _logger->error("LUA is not set! Reference got destroyed?");
		// Register globals, these will be read-only & sandboxed!

		// OVERRIDES ----
		luabridge::getGlobalNamespace(L)
		    .addFunction("printTable", [](const luabridge::LuaRef& ref) {
			    std::string output;

			    glz::error_ctx ec = glz::write<glz::opts{.prettify = 1U}>(rawrbox::LuaUtils::luaToJsonObject(ref), output);
			    if (ec) throw std::runtime_error(fmt::format("Failed to printTable"));

			    _logger->info("{}", output);
		    })
		    .addFunction("print", [](lua_State* state) {
			    auto args = rawrbox::LuaUtils::argsToString(state);
			    if (args.empty()) return;

			    _logger->info("{}", fmt::join(args, " "));
		    })
		    .addFunction("include", [](lua_State* state) {
			    if (lua_type(state, 1) != LUA_TSTRING) throw std::runtime_error("Invalid param, string expected");
			    const auto* path = lua_tostring(state, 1);

			    auto modID = rawrbox::LuaUtils::getLuaENVVar(state, "__mod_id");
			    auto modFolder = rawrbox::LuaUtils::getLuaENVVar(state, "__mod_folder");

			    auto fixedPath = LuaUtils::getContent(path, modFolder);
			    rawrbox::LuaUtils::compileAndLoadFile(state, modID, fixedPath);

			    // Register file for hot-reloading
			    registerLoadedFile(modID, fixedPath);
			    // ----
		    });
		// ---------

		// TIME UTILS ---
		luabridge::getGlobalNamespace(L)
		    .beginNamespace("time", {})
		    .addFunction("curtime", []() { return rawrbox::TimeUtils::curtime(); })
		    .addFunction("time", []() { return rawrbox::TimeUtils::time(); })
		    .endNamespace();
		// -------------

		// MODDING ---
		luabridge::getGlobalNamespace(L)
		    .beginNamespace("mod", {})
		    .addFunction("exists", [](const std::string& id) {
			    return _mods.find(id) != _mods.end();
		    })
		    .addFunction("get", [](const std::string& id) {
			    auto fnd = _mods.find(id);
			    if (fnd == _mods.end()) throw std::runtime_error(fmt::format("Mod {} not found", id));

			    return rawrbox::MODWrapper(fnd->second.get());
		    })
		    .endNamespace();
		// ----------

		// STRING ----
		luabridge::getGlobalNamespace(L)
		    .beginNamespace("string", {})
		    .addFunction("vformat", [](lua_State* state) {
			    auto vars = rawrbox::LuaUtils::argsToString(state);
			    if (vars.empty()) throw std::runtime_error("Missing params");

			    fmt::dynamic_format_arg_store<fmt::format_context> fmtArgs = {};
			    for (size_t i = 1; i < vars.size(); i++) {
				    fmtArgs.push_back(vars[i]);
			    }

			    return fmt::vformat(vars[0], fmtArgs);
		    })
		    .endNamespace();
		// -----------

		// ENGINE ------
		luabridge::getGlobalNamespace(L)
		    .beginNamespace("engine", {})
		    .addProperty("deltaTime", &rawrbox::DELTA_TIME, false)
		    .addProperty("fixedDeltaTime", &rawrbox::FIXED_DELTA_TIME, false)
		    .addProperty("frameAlpha", &rawrbox::FRAME_ALPHA, false)
		    .endNamespace();
		// -------------

		// Register plugins globals ---
		for (auto& p : _plugins)
			p->registerGlobal(L);
		//  -----

		// Custom ----
		onRegisterGlobals(mod);
		// -------------
	}
	// -------------

	// MOD LOAD ---
	void SCRIPTING::loadI18N(const rawrbox::Mod& mod) {
		auto i18nPath = mod.getFolder() / "i18n";
		if (!std::filesystem::exists(i18nPath)) return;

		rawrbox::I18N::loadLanguagePack(mod.getID(), i18nPath);
	}
	// ------------

	// HOT RELOAD -----
	void SCRIPTING::registerLoadedFile(const std::string& modId, const std::filesystem::path& filePath) {
		auto mdFnd = _loadedLuaFiles.find(modId);

		if (mdFnd != _loadedLuaFiles.end()) {
			auto fileFnd = std::find(mdFnd->second.begin(), mdFnd->second.end(), filePath);
			if (fileFnd != mdFnd->second.end()) return; // Already registered

			mdFnd->second.push_back(filePath);
		} else {
			_loadedLuaFiles[modId] = {filePath};
		}

		if (hotReloadEnabled()) {
			_logger->info("Registered {} -> {} for hot reload", fmt::styled(modId, fmt::fg(fmt::color::coral)), filePath.generic_string());
			_watcher->watchFile(filePath);
		}
	}

	void SCRIPTING::hotReload(const std::filesystem::path& filePath) {
		// Find the owner
		for (auto& pt : _loadedLuaFiles) {
			auto fnd = std::find(pt.second.begin(), pt.second.end(), filePath) != pt.second.end();
			if (!fnd) continue;

			auto md = _mods.find(pt.first);
			if (md == _mods.end()) return;
			_logger->warn("Hot-reloading lua file '{}'", filePath.generic_string());

			// Cleanup and load -----
			auto* env = md->second->getEnvironment();
			md->second->gc(); // Cleanup

			try {
				rawrbox::LuaUtils::compileAndLoadFile(env, md->first, filePath);
			} catch (const std::exception& err) {
				_logger->printError("{}", err.what());
			}
			// ---------------

			onModHotReload(*md->second);
			break;
		};
	}
	// -------------
	// ----------

	void SCRIPTING::init(int hotReloadMs) {
		_hotReloadEnabled = hotReloadMs > 0;
		if (_hotReloadEnabled) {
			_logger->info("Enabled lua hot-reloading\n  └── Delay: {}ms", fmt::styled(hotReloadMs, fmt::fg(fmt::color::gold)));

			_watcher = std::make_unique<rawrbox::FileWatcher>(
			    [](const std::string& pth, rawrbox::FileStatus status) {
				    if (status != rawrbox::FileStatus::modified) return;
				    hotReload(pth);
			    },
			    std::chrono::milliseconds(hotReloadMs));
			_watcher->start();
		}

		// Setup  --
		if (_console != nullptr) rawrbox::ConsoleWrapper::init(_console);
		// ----------------
	}

	// LOADING ----
	void SCRIPTING::loadMods(const std::filesystem::path& rootFolder, bool requireMetadata) { // Load mods
		if (!std::filesystem::exists(rootFolder)) throw _logger->error("Failed to locate root folder '{}'", rootFolder.generic_string());

		for (const auto& p : std::filesystem::directory_iterator(rootFolder)) {
			if (!p.is_directory()) continue;
			loadMod(p, requireMetadata);
		}
	}

	void SCRIPTING::loadMod(const std::filesystem::path& modFolder, bool requireMetadata) {
		if (!std::filesystem::exists(modFolder)) throw _logger->error("Failed to locate mod folder '{}'", modFolder.generic_string());

		std::string id = modFolder.filename().generic_string();
		glz::json_t metadataJSON = {};

		if (requireMetadata) {
			std::filesystem::path configPath = modFolder / "mod.json";

			if (std::filesystem::exists(configPath)) {
				auto ec = glz::read_file_json(metadataJSON, configPath.generic_string(), std::string{});
				if (ec) throw _logger->error("Failed to load mod.json");

				if (metadataJSON.contains("id")) id = metadataJSON["id"].get<std::string>();
			} else {
				_logger->warn("Failed to locate mod metadata '{}'", modFolder.generic_string());
			}
		}

		if (id.empty()) throw _logger->error("Mod ID is empty");

		auto mod = std::make_unique<rawrbox::Mod>(id, modFolder);
		mod->setMetadata(metadataJSON);

		// Prepare env ----------
		loadLibraries(*mod);
		loadTypes(*mod);
		loadGlobals(*mod);
		loadI18N(*mod);
		// ----------

		try {
			mod->init(); // Sandbox env
			mod->load();

			mod->call("init");
			registerLoadedFile(mod->getID(), mod->getEntryFilePath()); // Register file for hot-reloading
		} catch (const std::runtime_error& err) {
			_logger->printError("{}", err.what());
		}

		_mods.emplace(id, std::move(mod));
	}
	// -----------

	void SCRIPTING::shutdown() {
		_console = nullptr;
		_watcher.reset();

		_loadedLuaFiles.clear();
		_mods.clear();
		_plugins.clear();
	}

	void SCRIPTING::setConsole(rawrbox::Console* console) { _console = console; }

	// UTILS ----
	const std::unordered_map<std::string, std::unique_ptr<Mod>>& SCRIPTING::getMods() { return _mods; }
	std::vector<std::string> SCRIPTING::getModsIds() {
		std::vector<std::string> modNames = {};
		modNames.reserve(_mods.size());
		for (auto& mod : _mods) {
			modNames.push_back(mod.second->getID());
		}

		return modNames;
	}

	bool SCRIPTING::hotReloadEnabled() { return _hotReloadEnabled; }
	bool SCRIPTING::isLuaFileMounted(const std::string& path) {
		for (auto& pt : _loadedLuaFiles) {
			auto fnd = std::find(pt.second.begin(), pt.second.end(), path) != pt.second.end();
			if (fnd) return true;
		};

		return false;
	}
	// -----
} // namespace rawrbox
