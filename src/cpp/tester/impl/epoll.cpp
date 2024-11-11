#include "tester/impl/epoll.h"
#include <unistd.h>
#include <stdexcept>

#include <iostream>

using namespace std::chrono_literals;

namespace event_benchmark::epoll {
	TesterImpl::TesterImpl() : Tester(),
		epoll(-1),
		max_events(100) {
	}

	TesterImpl::~TesterImpl() {
	}

	bool TesterImpl::construct_test_environment(std::shared_ptr<MqManager> mq_manager) {
		if (Tester::construct_test_environment(mq_manager) == false) {
			return false;
		}
		
		// step 1. create epoll
		if (epoll != -1) {
			// close the last epoll
			close(epoll);
			epoll = -1;
		}
		
		epoll = epoll_create1(0);
		if (epoll == -1) {
			// failed to create epoll
			return false;
		}
		
		// step 2. register message queues to epoll
		/*
		// Note. indexing style: mqd_t
		for (mq_desc& attr : manager->queues) {
			epoll_event event;
			event.events = EPOLLIN;
			event.data.fd = attr.second;
			if (epoll_ctl(epoll, EPOLL_CTL_ADD, attr.second, &event) == -1) {
				throw std::runtime_error("Failed to add a mq descriptor to epoll");
			}
		}
		*/
		// Note. indexing style: index of descriptor
		for (uint64_t idx = 0; idx < manager->queues.size(); idx++) {
			epoll_event event;
			//event.events = EPOLLIN | EPOLLET;
			event.events = EPOLLIN;
			event.data.u64 = idx;
			if (epoll_ctl(epoll, EPOLL_CTL_ADD, manager->queues[idx].second, &event) == -1) {
				throw std::runtime_error("Failed to add a mq descriptor to epoll");
			}
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
		
		// step 3. create main thread for reaction time test of epoll
		running_thread = std::thread([this]() {
			uint64_t timeout_ms = 1000;
			int64_t received = 0;
			uint64_t msg_size = manager->msg_size();
			epoll_event events[max_events];
			char* buffer = new char[msg_size];
			
			do {
				int32_t announced = epoll_wait(epoll, events, max_events, timeout_ms);
				if (announced == -1) {
					received = 0;
					// failed to wait epoll
				} else if (announced == 0) {
					received = 0;
					// timeout
				}
				
				for (size_t idx = 0; idx < announced; idx++) {
					// Note. indexing style: mqd_t
					//mqd_t mq = events[idx].data.fd;
					// Note. indexing style: index of descriptor
					mqd_t mq = manager->queues[events[idx].data.u64].second;
					received = mq_receive(mq, buffer, msg_size, nullptr);
					if (received > 0) {
						std::chrono::nanoseconds now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch());
						uint64_t received_ns = now.count();
						uint64_t sent_ns = reinterpret_cast<uint64_t*>(buffer)[0];
						uint64_t result = received_ns - sent_ns;
						reaction_time.results.push_back(result);
					}
				}
			} while (is_running.load() && (received > 0));
			
			delete[] (buffer);
			close(epoll);
			epoll = -1;
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
			std::unique_lock<std::mutex> lock(reaction_time.mutex);
			reaction_time.results.clear();
		}
		
		// step 3. create main thread for reaction time test of epoll
		running_thread = std::thread([this]() {
			const uint64_t timeout_ms = 1000;
			const uint64_t msg_size = manager->msg_size();
			const uint64_t total = manager->queues.size();
			epoll_event events[max_events];
			char* buffer = new char[msg_size];
			std::atomic<uint64_t>* counts = new std::atomic<uint64_t>[total];
			int64_t received = 0;
			
			std::thread counting_thread = std::thread([this, counts, total]() {
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
			});
			
			do {
				int32_t announced = epoll_wait(epoll, events, max_events, timeout_ms);
				if (announced == -1) {
					received = 0;
					// failed to wait epoll
				} else if (announced == 0) {
					received = 0;
					// timeout
				}
				
				for (size_t idx = 0; idx < announced; idx++) {
					// Note. indexing style: mqd_t
					//mqd_t mq = events[idx].data.fd;
					// Note. indexing style: index of descriptor
					uint64_t mq_idx = events[idx].data.u64;
					mqd_t mq = manager->queues[mq_idx].second;
					
					received = mq_receive(mq, buffer, msg_size, nullptr);
					if (received > 0) {
						counts[mq_idx]++;
					} else {
						break;
					}
				}
			} while (is_running.load() && (received > 0));
			
			is_running.store(false);
			
			if (counting_thread.joinable()) {
				counting_thread.join();
			}
			
			delete[] (buffer);
			delete[] (counts);
			close(epoll);
			epoll = -1;
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
