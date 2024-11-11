#include "tester/impl/multithreading.h"
#include <iostream>

using namespace std::chrono_literals;

namespace event_benchmark::multithreading {
	TesterImpl::TesterImpl() : Tester() {
		reaction_time.statistics.minimum = std::chrono::nanoseconds::max();
		reaction_time.statistics.maximum = 0ns;
		reaction_time.statistics.average = 0ns;
		throughput.statistics.minimum = std::numeric_limits<uint64_t>::max();
		throughput.statistics.maximum = 0;
		throughput.statistics.average = 0;
	}

	TesterImpl::~TesterImpl() {
	}

	bool TesterImpl::construct_test_environment(std::shared_ptr<MqManager> mq_manager) {
		if (Tester::construct_test_environment(mq_manager) == false) {
			return false;
		}
		
		return true;
	}

	void TesterImpl::measure_reaction_time() {
		// step 1. check condition
		if (is_running.load()) {
			return;
		}
		is_running.store(true);

		// step 2. clear results from the last test
		{
			std::unique_lock<std::mutex> lock(reaction_time.mutex);
			reaction_time.results.clear();
		}
		
		// step 3. create main thread for reaction time test of multithreading
		running_thread = std::thread([this]() {
			// step 3.1. create threads for multithreading
			for (mq_desc& desc : manager->queues) {
				threads.push_back(
					std::thread([this, desc]() {
						std::list<uint64_t> results;
						int64_t received = 0;
						struct timespec ts;
						uint64_t msg_size = manager->msg_size();
						char* buffer = new char[msg_size];
						do {
							clock_gettime(CLOCK_REALTIME, &ts);
							ts.tv_sec += 1;
							received = mq_timedreceive(desc.second, buffer, msg_size, nullptr, &ts);
							if (received > 0) {
								std::chrono::nanoseconds now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch());
								uint64_t received_ns = now.count();
								uint64_t sent_ns = reinterpret_cast<uint64_t*>(buffer)[0];
								uint64_t result = received_ns - sent_ns;
								results.push_back(result);
							}
						} while (is_running.load() && (received > 0));
						delete[] (buffer);
						
						std::unique_lock<std::mutex> lock(reaction_time.mutex);
						reaction_time.results.splice(reaction_time.results.end(), results);
					})
				);
			}
			
			// step 3.2. wait for threads
			for (std::thread& thread : threads) {
				if (thread.joinable()) {
					thread.join();
				}
			}
		});
	}

	void TesterImpl::measure_throughput() {
		// step 1. check condition
		if (is_running.load()) {
			return;
		}
		is_running.store(true);

		// step 2. clear results from the last test
		{
			std::unique_lock<std::mutex> lock(throughput.mutex);
			throughput.results.clear();
		}
		
		// step 3. create main thread for reaction time test of multithreading
		uint64_t total = manager->queues.size();
		threads.reserve(total + 1);
		std::atomic<uint64_t>* counts = new std::atomic<uint64_t>[total];
		running_thread = std::thread([this, counts, total]() {
			// step 3.1. create threads for multithreading
			for (size_t idx = 0; idx < manager->queues.size(); idx++) {
				mq_desc& desc = manager->queues[idx];
				threads.push_back(
					std::thread([this, counts, idx, desc]() {
						std::list<uint64_t> results;
						int64_t received = 0;
						struct timespec ts;
						uint64_t msg_size = manager->msg_size();
						char* buffer = new char[msg_size];
						do {
							clock_gettime(CLOCK_REALTIME, &ts);
							ts.tv_sec += 1;
							received = mq_timedreceive(desc.second, buffer, msg_size, nullptr, &ts);
							if (received > 0) {
								counts[idx]++;
							}
						} while (is_running.load() && (received > 0));
						is_running.store(false);
						delete[](buffer);
					})
				);
			}
			
			threads.push_back(
				std::thread([this, counts, total]() {
					// remove first step
					std::this_thread::sleep_for(1s);
					for (size_t idx = 0; idx < total; idx++) {
						counts[idx].store(0);
					}
					
					while (is_running.load()) {
						std::this_thread::sleep_for(1s);
						for (size_t idx = 0; idx < total; idx++) {
							uint64_t the_last_value = counts[idx].exchange(0);
							if (the_last_value > 0) {
								throughput.results.push_back(the_last_value);
							}
						}
					}
					
					if (throughput.results.size() > total) {
						for (size_t cnt = 0; cnt < total; cnt++) {
							throughput.results.pop_back();
						}
					}
				})
			);
			
			// step 3.2. wait for threads
			for (std::thread& thread : threads) {
				if (thread.joinable()) {
					thread.join();
				}
			}
			delete[] (counts);
		});
	}

	ReactionTime TesterImpl::get_reaction_time() {
		if (running_thread.joinable()) {
			running_thread.join();
		}

		// step 4. generate statistics
		std::unique_lock<std::mutex> lock(reaction_time.mutex);
		
		// step 4.1. reset the lastest statistics
		reaction_time.statistics.minimum = std::chrono::nanoseconds::max();
		reaction_time.statistics.maximum = 0ns;
		reaction_time.statistics.average = 0ns;
		
		// step 4.2. generate statistics
		for (uint64_t result : reaction_time.results) {
			if (result < reaction_time.statistics.minimum.count()) {
				reaction_time.statistics.minimum = std::chrono::nanoseconds(result);
			}
			if (result > reaction_time.statistics.maximum.count()) {
				reaction_time.statistics.maximum = std::chrono::nanoseconds(result);
			}
			reaction_time.statistics.average += std::chrono::nanoseconds(result);
		}
		if (reaction_time.results.size() > 0) {
			reaction_time.statistics.average = reaction_time.statistics.average / reaction_time.results.size();
		}
		return reaction_time.statistics;
	}

	Throughput TesterImpl::get_throughput() {
		if (running_thread.joinable()) {
			running_thread.join();
		}

		// step 4. generate statistics
		std::unique_lock<std::mutex> lock(throughput.mutex);
		
		// step 4.1. reset the lastest statistics
		throughput.statistics.minimum = std::numeric_limits<uint64_t>::max();
		throughput.statistics.maximum = 0;
		throughput.statistics.average = 0;
		
		// step 4.2. generate statistics
		for (uint64_t result : throughput.results) {
			if (result < throughput.statistics.minimum) {
				throughput.statistics.minimum = result;
			}
			if (result > throughput.statistics.maximum) {
				throughput.statistics.maximum = result;
			}
			throughput.statistics.average += result;
		}
		if (throughput.results.size() > 0) {
			throughput.statistics.average = throughput.statistics.average / throughput.results.size();
		}

		return throughput.statistics;
	}
}
