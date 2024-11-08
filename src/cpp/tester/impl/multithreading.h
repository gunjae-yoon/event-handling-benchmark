#ifndef __event_handling_benchmark_tester_impl_multithreading_h__
#define __event_handling_benchmark_tester_impl_multithreading_h__

#include "tester/base/tester.h"
#include <list>
#include <mutex>

namespace event_benchmark {
	namespace multithreading {
		using Tester = event_benchmark::Tester;
		using ReactionTime = Tester::ReactionTime;
		using Throughput = Tester::Throughput;
		
		class TesterImpl : public Tester {
		public:
			TesterImpl();
			virtual ~TesterImpl();
			
			bool construct_test_environment(std::shared_ptr<MqManager> mq_manager) override;
			void measure_reaction_time() override;
			void measure_throughput() override;
			
			[[nodiscard]] ReactionTime get_reaction_time() override;
			[[nodiscard]] Throughput get_throughput() override;
		
		private:
			std::vector<std::thread> threads;
			struct {
				std::list<uint64_t> results;
				std::mutex mutex;
				ReactionTime statistics;
			} reaction_time;
			struct {
				std::list<uint64_t> results;
				std::mutex mutex;
				Throughput statistics;
			} throughput;
		};
	}
}

#endif
