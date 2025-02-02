



include(GoogleTest)
# include_directories(${btrack_core_INCLUDES})

add_executable(
	gui_tests
	gui_tests.cpp
)
target_link_libraries(
    gui_tests
    GTest::gtest_main
)

set_tests_properties(${Tests} PROPERTIES TIMEOUT 10)

gtest_discover_tests(gui_tests)


