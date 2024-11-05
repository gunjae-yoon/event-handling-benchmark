#ifndef __event_handling_benchmark_tester_impl_multithreading_h__
#define __event_handling_benchmark_tester_impl_multithreading_h__

#include "tester/base/tester.h"

namespace event_benchmark {
	namespace multithreading {
		using Tester = event_benchmark::Tester;
		using ReactionTime = Tester::ReactionTime;
		using Throughput = Tester::Throughput;
		
		class TesterImpl : public Tester {
		public:
			TesterImpl();
			virtual ~TesterImpl();
			
			bool construct_test_environment(const uint64_t count) override;
			void measure_reaction_time() override;
			void measure_throughput() override;
			
			[[nodiscard]] ReactionTime get_reaction_time() const override;
			[[nodiscard]] Throughput get_throughput() const override;
		};
	}
}

#endif
