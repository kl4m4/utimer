enable_testing()

add_executable(
  utimer_tests
  test.cpp
)
target_link_libraries(
  utimer_tests
  PUBLIC
  utimer
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(utimer_tests)