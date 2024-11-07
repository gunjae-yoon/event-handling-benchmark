#include "tester.h"
#include <stdexcept>

namespace event_benchmark {
	Tester::Tester() :
		manager(nullptr),
		is_running(false) {
	}

	Tester::~Tester() {
		stop();
		manager.reset();
	}

	bool Tester::construct_test_environment(std::shared_ptr<MqManager> mq_manager) {
		if (is_running.load() && running_thread.joinable()) {
			throw std::runtime_error("Tester is busy");
		}
		
		manager = mq_manager;
		return true;
	}

	void Tester::stop() noexcept {
		is_running.store(false);
		if (running_thread.joinable()) {
			running_thread.join();
		}
	}
}
