# Disable wabt features not needed by CDT
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_LIBWASM OFF CACHE BOOL "" FORCE)
set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(WITH_WASI OFF CACHE BOOL "" FORCE)
add_subdirectory(wabt)
