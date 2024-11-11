#ifndef __event_handling_benchmark_tester_impl_epoll_h__
#define __event_handling_benchmark_tester_impl_epoll_h__

#include "tester/base/tester.h"
#include <list>
#include <mutex>
#include <sys/epoll.h>

namespace event_benchmark {
	namespace epoll {
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
			int32_t epoll;
			const uint64_t max_events;
		};
	}
}

#endif
