set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/test)

add_executable(future_test future_test.cpp)
target_link_libraries(future_test arcane)

