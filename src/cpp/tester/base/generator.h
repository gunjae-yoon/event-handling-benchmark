#ifndef __event_handling_benchmark_tester_base_generator_h__
#define __event_handling_benchmark_tester_base_generator_h__

#include <cstdint>
#include <memory>
#include <thread>
#include "mq_manager.h"

namespace event_benchmark {
	class Generator {
	public:
		Generator() noexcept;
		~Generator();
		
		bool construct_test_environment(std::shared_ptr<MqManager> mq_manager);
		void measure_reaction_time();
		void measure_throughput();
		void stop() noexcept;
	
	private:
		std::shared_ptr<MqManager> manager;
		std::atomic<bool> is_running;
		std::thread running_thread;
	};
}

#endif
