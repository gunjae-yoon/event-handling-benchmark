#ifndef __event_handling_benchmark_tester_base_tester_h__
#define __event_handling_benchmark_tester_base_tester_h__

#include <cstdint>
#include <chrono>

#if __cplusplus >= 202300L
#include <stdfloat>
#else
using float64_t = double;
using float43_t = float;
#endif

namespace event_benchmark {
	class Tester {
	public:
		struct ReactionTime {
			std::chrono::nanoseconds minimum;
			std::chrono::nanoseconds maximum;
			std::chrono::nanoseconds average;
		};
		struct Throughput {
			uint64_t minimum;
			uint64_t maximum;
			uint64_t average;
		};

	public:
		Tester() { }
		virtual ~Tester() { }
		
		virtual bool construct_test_environment(uint64_t count) = 0;
		virtual void measure_reaction_time() = 0;
		virtual void measure_throughput() = 0;
		
		virtual ReactionTime get_reaction_time() = 0;
		virtual Throughput get_throughput() = 0;
	};
}

#endif
