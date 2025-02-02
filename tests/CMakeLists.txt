


FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
if (MSVC)
	set_property(GLOBAL PROPERTY
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

FetchContent_MakeAvailable(googletest)

include(GoogleTest)
include_directories(${btrack_core_INCLUDES})

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


