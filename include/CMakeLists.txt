
target_include_directories(tsym-internal-config
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tsym>)

install(DIRECTORY tsym
    DESTINATION ${tsym_installIncludeDir} COMPONENT development)
