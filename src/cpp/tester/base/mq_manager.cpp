#include "mq_manager.h"
#include <stdexcept>
#include <iostream>

namespace event_benchmark {
	std::atomic<uint64_t> MqManager::id = 0;

	MqManager::MqManager() noexcept {
		attr.mq_curmsgs = 0;
		attr.mq_flags = 0;
		attr.mq_maxmsg = 10;
		attr.mq_msgsize = sizeof(uint64_t);
	}

	MqManager::~MqManager() {
		reset();
	}

	bool MqManager::reset(const uint64_t count, const uint64_t msg_size) {
		// step 1. close and remove(unlink) previous message queues
		for (mq_desc& desc : queues) {
			mq_close(desc.second);
		}
		queues.clear();
		if (queues.capacity() < count) {
			queues.reserve(count);
		}
		
		// step 2. set message queue attribute
		if (count == 0) {
			return true;
		} else if (msg_size < sizeof(uint64_t)) {
			throw std::invalid_argument("Argument size is too small. It must be at least sizeof(uint64_t).");
		}
		attr.mq_msgsize = msg_size;
		
		// step 3. open message queues
		for (uint64_t counter = 0; counter < count; counter++) {
			std::string mq_name = "/testcase_" + std::to_string(id.fetch_add(1));
			mqd_t mq_handle = mq_open(mq_name.c_str(), O_CREAT | O_RDWR, 0666, &attr);
			if (mq_handle == -1) {
				std::cerr << "Failed to open message queue" << std::endl;
				return false;
			}
			
			queues.push_back({mq_name, mq_handle});
		}
		
		return true;
	}

	void MqManager::dispose() {
		for (mq_desc& desc : queues) {
			mq_close(desc.second);
			mq_unlink(desc.first.c_str());
		}
		queues.clear();
	}

	uint64_t MqManager::msg_size() {
		return attr.mq_msgsize;
	}
}
