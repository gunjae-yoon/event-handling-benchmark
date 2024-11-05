#ifndef __event_handling_benchmark_tester_impl_epoll_h__
#define __event_handling_benchmark_tester_impl_epoll_h__

#include "tester/base/tester.h"

namespace event_benchmark {
	namespace epoll {
		using Tester = event_benchmark::Tester;
		using ReactionTime = Tester::ReactionTime;
		using Throughput = Tester::Throughput;
		
		class TesterImpl : public Tester {
		public:
			TesterImpl();
			virtual ~TesterImpl();
			
			bool construct_test_environment(uint64_t count);
			void measure_reaction_time();
			void measure_throughput();
			
			ReactionTime get_reaction_time();
			Throughput get_throughput();
		};
	}
}

#endif
