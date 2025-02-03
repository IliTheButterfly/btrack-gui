

set(EXAMPLES
	"base_gui"
)

include_directories(${btrack_core_INCLUDES})
foreach(EXAMPLE ${EXAMPLES})
	add_executable(${EXAMPLE} "${EXAMPLE}.cpp")
	target_link_libraries(${EXAMPLE} btrack-gui)
endforeach(EXAMPLE ${EXAMPLES})

