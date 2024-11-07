set(CMAKE_TARGET_NAME "event-handling-benchmark")

add_executable(${CMAKE_TARGET_NAME}
	src/cpp/app/main.cpp
	src/cpp/tester/base/generator.cpp
	src/cpp/tester/base/mq_manager.cpp
	src/cpp/tester/base/tester.cpp
	src/cpp/tester/impl/epoll.cpp
	src/cpp/tester/impl/multithreading.cpp
	src/cpp/tester/impl/select.cpp
)

target_include_directories(${CMAKE_TARGET_NAME} PRIVATE
	src/cpp
)

target_link_libraries(${CMAKE_TARGET_NAME} PRIVATE
	pthread
)
