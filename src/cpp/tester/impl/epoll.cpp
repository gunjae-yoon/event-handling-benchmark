#include "tester/impl/epoll.h"

using namespace std::chrono_literals;

namespace event_benchmark::epoll {
	TesterImpl::TesterImpl() : Tester() {
	}

	TesterImpl::~TesterImpl() {
	}

	bool TesterImpl::construct_test_environment(std::shared_ptr<MqManager> mq_manager) {
		if (Tester::construct_test_environment(mq_manager) == false) {
			return false;
		}
		
		return true;
	}

	void TesterImpl::measure_reaction_time() {
		if (is_running.load()) {
			return;
		}
		
		is_running.store(true);
	}

	void TesterImpl::measure_throughput() {
		if (is_running.load()) {
			return;
		}
		
		is_running.store(true);
	}

	ReactionTime TesterImpl::get_reaction_time() {
		ReactionTime result = {0ns, 0ns, 0ns};
		return result;
	}

	Throughput TesterImpl::get_throughput() {
		Throughput result = {0, 0, 0};
		return result;
	}
}
