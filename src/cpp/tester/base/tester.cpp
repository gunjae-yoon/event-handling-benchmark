#include "tester.h"

namespace event_benchmark {
	Tester::Tester() :
		manager(nullptr),
		is_running(false) {
	}

	Tester::~Tester() {
		stop();
		manager.reset();
	}

	void Tester::stop() noexcept {
		is_running.store(false);
		if (running_thread.joinable()) {
			running_thread.join();
		}
	}
}
