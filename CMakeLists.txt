cmake_minimum_required(VERSION 3.16)

project(lazy_sockets)

option(LSC_BUILD_TESTS "Build tests for lazy_sockets" OFF)

if (LSC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
unset(LSC_BUILD_TESTS CACHE)

# Get platform.
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(LSC_PLATFORM_NAME "unix")

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(LSC_PLATFORM_NAME "unix") # cuz we lazy up in here
  message(WARNING "OSX version might not work")

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(LSC_PLATFORM_NAME "win")

endif()

# include_directories("src")

file(GLOB lazy_sockets_src
	"src/*.h"
	"src/soc_${LSC_PLATFORM_NAME}.cpp"
)

# Add lib
add_library(lazy_sockets STATIC
	${lazy_sockets_src}
	${lazy_sockets_src_platform}
)

# set(CMAKE_CXX_STANDARD 20)

message(STATUS "Selecting compile options from platform: ${LSC_PLATFORM_NAME}")
if (${LSC_PLATFORM_NAME} MATCHES "win")
  if (MSVC)
    target_compile_options(lazy_sockets PRIVATE -W4 -WX)
  else()
    target_compile_options(lazy_sockets PRIVATE -Wall -Wextra -pedantic -Werror)
  endif()
  target_compile_definitions(lazy_sockets PUBLIC WIN)
elseif (${LSC_PLATFORM_NAME} MATCHES "unix")
    target_compile_options(lazy_sockets PRIVATE -Wall -Wextra -pedantic -Werror)
  target_compile_definitions(lazy_sockets PUBLIC LINUX)
endif()

target_link_libraries(lazy_sockets
	-lpthread
)
set_target_properties(lazy_sockets PROPERTIES LINKER_LANGUAGE CXX)

install(TARGETS lazy_sockets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

INSTALL (
    DIRECTORY ${CMAKE_SOURCE_DIR}/src/
    DESTINATION include/lazy_sockets
    FILES_MATCHING PATTERN "*.h*")
