#ifndef __event_handling_benchmark_tester_base_mq_manager_h__
#define __event_handling_benchmark_tester_base_mq_manager_h__

#include <mqueue.h>
#include <atomic>
#include <vector>
#include <cstdint>
#include <cstring>
#include <string>

namespace event_benchmark {
	using mq_desc = std::pair<std::string, mqd_t>;
	using mq_iter = std::vector<std::pair<std::string, mqd_t>>::const_iterator;

	class MqManager {
	public:
		MqManager();
		~MqManager();
		
		bool reset(const uint64_t count = 0, const uint64_t msg_size = 0);
	
	public:
		std::vector<mq_desc> queues;
		
	private:
		struct mq_attr attr;
		static std::atomic<uint64_t> id;
	};
}

#endif
