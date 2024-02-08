#include <rawrbox/scripting/utils/lua.hpp>
#include <rawrbox/utils/path.hpp>
#include <rawrbox/utils/string.hpp>

#include <Luau/Compiler.h>
#include <fmt/format.h>

namespace rawrbox {

	void LuaUtils::compileAndLoadFile(lua_State* L, const std::string& chunkID, const std::filesystem::path& path) {
		if (L == nullptr) throw std::runtime_error("Invalid lua state");
		if (!std::filesystem::exists(path)) throw std::runtime_error("File not found");

		// Load script ---
		auto bytes = rawrbox::PathUtils::getRawData(path);
		if (bytes.empty()) throw std::runtime_error("File empty / failed to load");
		// --------------

		compileAndLoadScript(L, chunkID, std::string(bytes.begin(), bytes.end()));
	}

	void LuaUtils::compileAndLoadScript(lua_State* L, const std::string& chunkID, const std::string& script) {
		if (L == nullptr) throw std::runtime_error("Invalid lua state");

		// Create a new thread ----
		auto loadThread = lua_newthread(L);
		// --------------

		// Compile ----
		Luau::CompileOptions options = {};
#ifndef _DEBUG
		options.optimizationLevel = 2; // Remove debug info & inline lua
		options.debugLevel = 0;
#endif

		Luau::ParseOptions parser = {};
		parser.allowDeclarationSyntax = false;
		parser.captureComments = false;

		std::string bytecode = Luau::compile(script, options, parser);
		if (bytecode.empty() || bytecode[0] == '\0') {
			size_t pos = bytecode.find(':'); // extract the error message
			std::string errorMessage = pos != std::string::npos ? bytecode.substr(pos + 1) : "Unknown lua error";
			throw std::runtime_error(errorMessage);
		}
		// ----------

		// Load -------
		std::string chunk = fmt::format("={}", chunkID);
		if (luau_load(loadThread, chunk.c_str(), bytecode.data(), bytecode.size(), 0) != 0) {
			throw std::runtime_error(rawrbox::LuaUtils::getError(L));
		}
		// -----------

		// Run the thread ---
		rawrbox::LuaUtils::resume(loadThread, L);
		// -----------
	}

	void LuaUtils::resume(lua_State* L, lua_State* from) {
		if (L == nullptr) throw std::runtime_error("Invalid lua state");
		if (lua_resume(L, from, 0) != 0) {
			throw std::runtime_error(rawrbox::LuaUtils::getError(L));
		}
	}

	void LuaUtils::run(lua_State* L) {
		if (L == nullptr) throw std::runtime_error("Invalid lua state");
		if (lua_pcall(L, 0, 0, 0) != 0) {
			throw std::runtime_error(rawrbox::LuaUtils::getError(L));
		}
	}
	void LuaUtils::collect_garbage(lua_State* L) {
		if (L == nullptr) throw std::runtime_error("Invalid lua state");
		lua_gc(L, LUA_GCCOLLECT, 0);
	}

	std::string LuaUtils::getError(lua_State* L) {
		if (L == nullptr) throw std::runtime_error("Invalid lua state");
		return lua_tostring(L, -1);
	}

	std::vector<std::string> LuaUtils::getStringVariadicArgs(lua_State* L) {
		std::vector<std::string> args = {};

		int nargs = lua_gettop(L);
		if (nargs == 0) return args;

		for (int i = 1; i <= nargs; i++) {
			if (!lua_isstring(L, i)) continue;
			args.emplace_back(lua_tostring(L, i));
		}

		return args;
	}

	void LuaUtils::getVariadicArgs(const luabridge::LuaRef& in, luabridge::LuaRef& out) {
		auto L = in.state();

		int nargs = lua_gettop(L);
		if (nargs == 0) return;

		int validArgs = 0;
		for (int i = 1; i <= nargs; i++) {
			int type = lua_type(L, i);

			switch (type) {
				case LUA_TNUMBER:
					out[validArgs++] = lua_tonumber(L, i);
					break;
				case LUA_TSTRING:
					out[validArgs++] = lua_tostring(L, i);
					break;
				case LUA_TBOOLEAN:
					out[validArgs++] = lua_toboolean(L, i) != 0;
					break;
				default:
					break;
			}
		}
	}

	luabridge::LuaRef LuaUtils::jsonToLua(lua_State* L, const nlohmann::json& json) {
		if (L == nullptr) throw std::runtime_error("Invalid lua state");

		if (json.is_null()) {
			return {L, luabridge::LuaNil()};
		} else if (json.is_boolean()) {
			return {L, json.get<bool>()};
		} else if (json.is_number()) { // Lua only works with doubles
			return {L, json.get<double>()};
		} else if (json.is_string()) {
			return {L, json.get<std::string>()};
		} else if (json.is_object()) {
			auto obj = luabridge::newTable(L);
			for (nlohmann::json::const_iterator it = json.begin(); it != json.end(); ++it) {
				obj[it.key().c_str()] = jsonToLua(L, *it);
			}

			return obj;
		} else if (json.is_array()) {
			auto obj = luabridge::newTable(L);
			unsigned long i = 1;
			for (const auto& it : json) {
				obj[i++] = jsonToLua(L, it);
			}

			return obj;
		}

		throw std::runtime_error("Unknown json type");
	}

	nlohmann::json LuaUtils::luaToJsonObject(const luabridge::LuaRef& ref) {
		nlohmann::json result = {};
		if (!ref.isTable()) return result;

		for (auto pair : luabridge::pairs(ref)) {
			auto key = pair.first.unsafe_cast<std::string>();
			luabridge::LuaRef value = pair.second;

			switch (value.type()) {
				case LUA_TBOOLEAN:
					result[key] = value.unsafe_cast<bool>();
					break;
				case LUA_TNUMBER:
					result[key] = value.unsafe_cast<double>(); // Lua uses double for all numeric types
					break;
				case LUA_TSTRING:
					result[key] = value.unsafe_cast<std::string>();
					break;
				case LUA_TVECTOR: // eeehhh, might die?
				case LUA_TTABLE:
					result[key] = luaToJsonObject(value); // Recursive conversion
					break;
				case LUA_TFUNCTION:
					result[key] = fmt::format("function({})", lua_topointer(value, -1));
					break;
				case LUA_TNONE:
				case LUA_TNIL:
				default:
					// Handle other types as needed, or ignore them
					break;
			}
		}

		return result;
	}

	std::string LuaUtils::getLuaENVVar(lua_State* L, const std::string& varId) {
		if (L == nullptr) throw std::runtime_error("Invalid lua state");

		lua_getfield(L, LUA_ENVIRONINDEX, varId.c_str());
		if (!lua_isstring(L, -1)) throw std::runtime_error(fmt::format("Invalid lua env variable '{}'", varId));

		return lua_tostring(L, -1);
	}

	// #/ == System content
	// @/ == Root content
	// @cats/ == `cats` mod
	// normal_path == current mod
	std::string LuaUtils::getContent(const std::filesystem::path& path, const std::filesystem::path& modPath) {
		if (path.empty()) return modPath.generic_string(); // Invalid path

		auto pth = path.generic_string();
		if (pth.starts_with("#")) {
			auto slashPos = pth.find("/"); // Find the first /
			return pth.substr(slashPos + 1);
		} // System path

		if (pth.starts_with("mods/")) return modPath.generic_string(); // Already has the mod
		pth = rawrbox::StrUtils::replace(pth, "\\", "/");
		pth = rawrbox::StrUtils::replace(pth, "./", "");
		pth = rawrbox::StrUtils::replace(pth, "../", "");

		// content/blabalba.png = my current mod
		if (!modPath.empty() && pth.front() != '@') {
			return std::filesystem::path(fmt::format("{}/{}", modPath.generic_string(), pth)).string(); // Becomes mods/mymod/content/blabalba.png
		} else if (pth.front() == '@') {
			auto slashPos = pth.find("/"); // Find the first /
			std::string cleanPath = pth.substr(slashPos + 1);

			// @/textures/blabalba.png = c++ content
			if (pth.rfind("@/", 0) == 0) { // C++
				return std::filesystem::path(fmt::format("content/{}", cleanPath)).string();
			} else { // @otherMod/textures/blabalba.png = @othermod content
				return std::filesystem::path(fmt::format("{}/{}", pth.substr(1, slashPos - 1), cleanPath)).string();
			}
		}

		return pth;
	}
} // namespace rawrbox
