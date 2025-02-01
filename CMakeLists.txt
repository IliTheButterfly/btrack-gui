project("btrack-gui")



# CMakeList.txt : CMake project for btrack-core, include source and define
# project specific logic here.
#

# Add source to this project's executable.


#if (CMAKE_VERSION VERSION_GREATER 3.12)
#  set_property(TARGET btrack-core PROPERTY CXX_STANDARD 20)
#endif()


include_directories(${btrack_core_INCLUDES})
file(GLOB_RECURSE BTRACK_GUI_SOURCES "**.cpp" "btrack-gui.cpp")
add_library(btrack-gui ${BTRACK_GUI_SOURCES})

if (USE_CUDA)
	add_library (btrack-gui-cuda ${SourceFiles})
	target_compile_definitions(btrack-gui-cuda PUBLIC -DUSE_CUDA)
	target_link_libraries(btrack-gui-cuda btrack-core-cuda)
endif()

target_link_libraries(btrack-gui btrack-core)

enable_testing()
add_subdirectory("tests")