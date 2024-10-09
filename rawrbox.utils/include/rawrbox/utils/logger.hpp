#pragma once

#include <fmt/color.h>
#include <fmt/format.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <string>

#define CRITICAL_RAWRBOX(...) \
	{ \
		spdlog::critical("] {}\n  └── {}:{}\n\t└── {}", fmt::format(__VA_ARGS__), __FILE__, __LINE__, fmt::styled(__FUNCTION__, fmt::fg(fmt::color::red))); \
		throw std::runtime_error(fmt::format(__VA_ARGS__)); \
	}

#define ERROR_RAWRBOX(...) \
	spdlog::error("] {}\n  └── {}:{}\n\t└── {}", fmt::format(__VA_ARGS__), __FILE__, __LINE__, fmt::styled(__FUNCTION__, fmt::fg(fmt::color::red)));

namespace rawrbox {
	class Logger {
	protected:
		std::string _title;

	public:
		static std::shared_ptr<spdlog::logger> LOGGER;
		static spdlog::level::level_enum LEVEL;
		static bool ENABLE_FILE_LOGGING;

		Logger(const std::string& title);

		// LOGGING ----
		template <typename... T>
		void info(fmt::format_string<T...> fmt, T&&... args) {
			if (LOGGER == nullptr) return;

			auto str = fmt::format(fmt, std::forward<T>(args)...);
			LOGGER->info(" ▓ {}] {}", this->_title, str);
		}

		template <typename... T>
		void warn(fmt::format_string<T...> fmt, T&&... args) {
			if (LOGGER == nullptr) return;

			auto str = fmt::format(fmt, std::forward<T>(args)...);
			LOGGER->warn(" ▓ {}] {}", this->_title, fmt::styled(str, fmt::fg(fmt::color::gold)));
		}

		template <typename... T>
		void trace(fmt::format_string<T...> fmt, T&&... args) {
			if (LOGGER == nullptr) return;

			auto str = fmt::format(fmt, std::forward<T>(args)...);
			LOGGER->trace(" ▓ {}] {}", this->_title, str);
		}

		template <typename... T>
		void debug(fmt::format_string<T...> fmt, T&&... args) {
			if (LOGGER == nullptr) return;

			auto str = fmt::format(fmt, std::forward<T>(args)...);
			LOGGER->debug(" ▓ {}] {}", this->_title, str);
		}
		// ----------
	};
} // namespace rawrbox
