#ifndef __event_handling_benchmark_tester_base_generator_h__
#define __event_handling_benchmark_tester_base_generator_h__

#include <cstdint>

namespace event_benchmark {
	class Generator {
	public:
		Generator();
		~Generator();
		
		bool construct_test_environment(const uint64_t count);
		void measure_reaction_time();
		void measure_throughput();
	};
}

#endif
