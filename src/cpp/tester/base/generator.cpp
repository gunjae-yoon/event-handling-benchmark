#include "tester/base/generator.h"

namespace event_benchmark {
	Generator::Generator() {
	}

	Generator::~Generator() {
	}

	bool Generator::construct_test_environment(const uint64_t count) {
		return true;
	}

	void Generator::measure_reaction_time() {
	}

	void Generator::measure_throughput() {
	}
}
