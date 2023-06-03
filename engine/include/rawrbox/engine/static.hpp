#pragma once
#include <rawrbox/engine/threading.hpp>
#include <rawrbox/utils/ringbuffer.hpp>

#include <functional>
#include <memory>
#include <thread>

namespace rawrbox {
	// THREADING ----
	extern std::thread::id MAIN_THREAD_ID;
	extern jnk0le::Ringbuffer<std::function<void()>> MAIN_THREAD_INVOKES;

	// TIMING ---
	extern float DELTA_TIME;
	extern float FRAME_ALPHA;
	// -----

	// NOLINTBEGIN(clang-diagnostic-unused-function)
	static void runOnMainThread(std::function<void()> func) {
		if (MAIN_THREAD_ID != std::this_thread::get_id()) {
			MAIN_THREAD_INVOKES.insert(std::move(func));
			return;
		}

		func();
	}

	// ⚠️ INTERNAL - DO NOT CALL UNLESS YOU KNOW WHAT YOU ARE DOING ⚠️
	static void ___runThreadInvokes() {
		while (!rawrbox::MAIN_THREAD_INVOKES.isEmpty()) {
			std::function<void()> fnc;
			rawrbox::MAIN_THREAD_INVOKES.remove(fnc);
			fnc();
		}
	}
	// NOLINTEND(clang-diagnostic-unused-function)
	// -------
} // namespace rawrbox
