#include "generator.h"
#include <stdexcept>
#include <iostream>

namespace event_benchmark {
	Generator::Generator() noexcept :
		manager(nullptr),
		is_running(false) {
	}

	Generator::~Generator() {
		stop();
		manager.reset();
	}

	bool Generator::construct_test_environment(std::shared_ptr<MqManager> mq_manager) {
		if (is_running.load() && running_thread.joinable()) {
			throw std::runtime_error("The data generator is busy");
		}
		
		manager = mq_manager;
		return true;
	}

	void Generator::measure_reaction_time() {
		if (is_running.load()) {
			return;
		}
		
		is_running.store(true);
		running_thread = std::thread([this]() {
			while (is_running) {
				for (mq_desc& desc : manager->queues) {
					std::chrono::nanoseconds now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch());
					uint64_t count = now.count();
					if (mq_send(desc.second, reinterpret_cast<const char*>(&count), sizeof(uint64_t), 0) == -1) {
						std::cerr << "Failed to send message to " << desc.first << std::endl;
					}
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			};
		});
	}

	void Generator::measure_throughput() {
		if (is_running.load()) {
			return;
		}
		
		is_running.store(true);
		running_thread = std::thread([this]() {
			std::list<std::thread> threads;
			for (mq_desc& desc : manager->queues) {
				threads.push_back(
					std::thread([this, desc]() {
						uint64_t count = 0;
						while (is_running) {
							count++;
							if (mq_send(desc.second, reinterpret_cast<const char*>(&count), sizeof(uint64_t), 0) == -1) {
								std::cerr << "Failed to send message to " << desc.first << std::endl;
							}
						}
					})
				);
			}
			
			for (std::thread& thread : threads) {
				if (thread.joinable()) {
					thread.join();
				}
			}
		});
	}

	void Generator::stop() noexcept {
		is_running.store(false);
		if (running_thread.joinable()) {
			running_thread.join();
		}
	}
}
