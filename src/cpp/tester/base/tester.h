#ifndef __event_handling_benchmark_tester_base_tester_h__
#define __event_handling_benchmark_tester_base_tester_h__

#include "mq_manager.h"
#include <cstdint>
#include <chrono>
#include <memory>
#include <thread>

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
		Tester();
		virtual ~Tester();
		
		virtual bool construct_test_environment(std::shared_ptr<MqManager> mq_manager) = 0;
		virtual void measure_reaction_time() = 0;
		virtual void measure_throughput() = 0;
		void stop() noexcept;
		
		[[nodiscard]] virtual ReactionTime get_reaction_time() const = 0;
		[[nodiscard]] virtual Throughput get_throughput() const = 0;
	
	protected:
		std::shared_ptr<MqManager> manager;
		std::atomic<bool> is_running;
		std::thread running_thread;
	};
}

#endif
