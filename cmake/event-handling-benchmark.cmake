set(CMAKE_TARGET_NAME "event-handling-benchmark")

add_executable(${CMAKE_TARGET_NAME}
	src/main.cpp
)

target_include_directories(${CMAKE_TARGET_NAME} PRIVATE
	src
)

target_link_libraries(${CMAKE_TARGET_NAME} PRIVATE
	pthread
)
