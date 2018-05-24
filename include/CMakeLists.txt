
target_include_directories(tsym-internal-config
    INTERFACE
    tsym)

install(DIRECTORY tsym
    DESTINATION ${tsym_installIncludeDir} COMPONENT development)
