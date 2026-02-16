# Set your client name
set(DIGITALCURLING_CLIENT_NAME "MyClientName")
# Set your client version (format: major.minor.patch)
set(DIGITALCURLING_CLIENT_VERSION "1.0")

# set additional source files for the client (optional)
set(DIGITALCURLING_CLIENT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/example/rulebased.cpp"
)
# set your client to use the plugin loader (optional, default: OFF)
set(DIGITALCURLING_CLIENT_USE_LOADER ON)
