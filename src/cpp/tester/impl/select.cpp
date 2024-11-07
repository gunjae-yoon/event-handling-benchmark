#include "tester/impl/select.h"

using namespace std::chrono_literals;

namespace event_benchmark::select {
	TesterImpl::TesterImpl() : Tester() {
	}

	TesterImpl::~TesterImpl() {
	}

	bool TesterImpl::construct_test_environment(std::shared_ptr<MqManager> mq_manager) {
		return true;
	}

	void TesterImpl::measure_reaction_time() {
	}

	void TesterImpl::measure_throughput() {
	}

	ReactionTime TesterImpl::get_reaction_time() const {
		ReactionTime result = {0ns, 0ns, 0ns};
		return result;
	}

	Throughput TesterImpl::get_throughput() const {
		Throughput result = {0, 0, 0};
		return result;
	}
}
