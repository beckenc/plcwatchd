add_executable(plcwatchd main.cpp)
include_directories(../pushover)
target_link_libraries(plcwatchd
PRIVATE
  libpushover
  libsnap7
  curl
  snap7
)

install(TARGETS plcwatchd
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )
