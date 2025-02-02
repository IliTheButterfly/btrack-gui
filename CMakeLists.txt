project("btrack-gui")


# CMakeList.txt : CMake project for btrack-core, include source and define
# project specific logic here.
#

# Add source to this project's executable.


#if (CMAKE_VERSION VERSION_GREATER 3.12)
#  set_property(TARGET btrack-core PROPERTY CXX_STANDARD 20)
#endif()

set(FETCH_NODE_EDITOR ON CACHE BOOL "Use ExternalProject to fetch nodeeditor?")
set(NODEEDITOR_DIR "" CACHE PATH "Path to the nodeeditor library if FETCH_NODE_EDITOR is OFF.")

# include_directories(${btrack_core_INCLUDES})
file(GLOB_RECURSE BTRACK_GUI_SOURCES "**.cpp")
list(FILTER BTRACK_GUI_SOURCES EXCLUDE REGEX "tests/.*")
list(FILTER BTRACK_GUI_SOURCES EXCLUDE REGEX "examples/.*")
message(STATUS "Sources " ${BTRACK_GUI_SOURCES})


find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui OpenGL)

# Import qt nodes
if (FETCH_NODE_EDITOR)
	FetchContent_Declare(
		QtNodes
		URL https://github.com/IliTheButterfly/nodeeditor/archive/refs/tags/3.0.11.zip
	)
	FetchContent_MakeAvailable(QtNodes)
	add_library(btrack-gui ${BTRACK_GUI_SOURCES})
	target_link_libraries(btrack-gui QtNodes)
else()
	include_directories("${NODEEDITOR_DIR}/include")
	add_library(btrack-gui ${BTRACK_GUI_SOURCES})
	set_property(TARGET btrack-gui PROPERTY IMPORTED_LOCATION "${NODEEDITOR_DIR}/build/lib/libQtNodes.so")
	# target_link_libraries(btrack-gui QtNodes)
	# target_link_libraries(btrack-gui "${NODEEDITOR_DIR}/build/lib/libQtNodes.so")
endif()


target_link_libraries(btrack-gui
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::OpenGL
)

if (USE_CUDA)
	if (FETCH_NODE_EDITOR)
		add_library (btrack-gui-cuda ${BTRACK_GUI_SOURCES} nodeeditor)
	else()
		add_library (btrack-gui-cuda ${BTRACK_GUI_SOURCES})
		# target_link_libraries(btrack-gui "${NODEEDITOR_DIR}/lib/libQtNodes.so" QtNodes)
		target_link_libraries(btrack-gui QtNodes)
	endif()

	target_compile_definitions(btrack-gui-cuda PUBLIC -DUSE_CUDA)
	target_link_libraries(btrack-gui-cuda btrack-core-cuda)
endif()

target_link_libraries(btrack-gui btrack-core)

enable_testing()
add_subdirectory("tests")
add_subdirectory("examples")