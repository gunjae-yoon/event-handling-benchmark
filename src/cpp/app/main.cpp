#include "tester/testers.h"

#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <cstring>

using namespace event_benchmark;
using namespace std::chrono_literals;

void test_reaction_time_of_multithreading(const uint64_t count, const uint64_t size);
void test_throughput_of_multithreading(const uint64_t count, const uint64_t size);

int main(int argc, char** argv) {
	test_reaction_time_of_multithreading(10, 1024);
	test_throughput_of_multithreading(10, 1024);
}

void test_reaction_time_of_multithreading(const uint64_t count, const uint64_t size) {
	std::shared_ptr<MqManager> manager = std::make_shared<MqManager>();
	if (manager->reset(count, size) == false) {
		manager.reset();
		return;
	}

	pid_t pid = fork();
	if (pid < 0) { // error
		return;
	} else if (pid == 0) { // child process
		Generator generator;
		if (generator.construct_test_environment(manager) == false) {
			return;
		}
		
		std::this_thread::sleep_for(100ms);
		generator.measure_reaction_time();
		std::this_thread::sleep_for(10s);
		generator.stop();
	} else { // parent process
		multithreading::TesterImpl tester;
		if (tester.construct_test_environment(manager) == false) {
			manager->dispose();
			return;
		}
		
		tester.measure_reaction_time();
		Tester::ReactionTime result = tester.get_reaction_time();
		
		manager->dispose();
		
		waitpid(pid, nullptr, 0);
		std::cout << "minimum reaction time: " << result.minimum.count() << " ns" << std::endl;
		std::cout << "maximum reaction time: " << result.maximum.count() << " ns" << std::endl;
		std::cout << "average reaction time: " << result.average.count() << " ns" << std::endl;
	}

	manager.reset();
}

void test_throughput_of_multithreading(const uint64_t count, const uint64_t size) {
	std::shared_ptr<MqManager> manager = std::make_shared<MqManager>();
	if (manager->reset(count, size) == false) {
		manager.reset();
		return;
	}

	pid_t pid = fork();
	if (pid < 0) { // error
		return;
	} else if (pid == 0) { // child process
		Generator generator;
		if (generator.construct_test_environment(manager) == false) {
			return;
		}
		
		std::this_thread::sleep_for(100ms);
		generator.measure_throughput();
		std::this_thread::sleep_for(10s);
		generator.stop();
	} else { // parent process
		multithreading::TesterImpl tester;
		if (tester.construct_test_environment(manager) == false) {
			manager->dispose();
			return;
		}
		
		tester.measure_throughput();
		Tester::Throughput result = tester.get_throughput();
		
		manager->dispose();
		
		waitpid(pid, nullptr, 0);
		std::cout << "minimum throhgput: " << result.minimum << " dps" << std::endl;
		std::cout << "maximum throhgput: " << result.maximum << " dps" << std::endl;
		std::cout << "average throhgput: " << result.average << " dps" << std::endl;
	}

	manager.reset();
}
