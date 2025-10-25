SET(MC_SOURCES
    ${CMAKE_SOURCE_DIR}/src/guid/guid_write_to_string.c
    ${CMAKE_SOURCE_DIR}/models/shadow/guid/guid_init_from_string.c
    main.c)

ADD_EXECUTABLE(model_guid_write_to_string ${MC_SOURCES})
SET_TARGET_PROPERTIES(
    model_guid_write_to_string PROPERTIES
    C_COMPILER_LAUNCHER "${COMPILER_CHOOSER};goto-cc"
    C_LINKER_LAUNCHER "${LINKER_CHOOSER};goto-ld")
TARGET_COMPILE_OPTIONS(
    model_guid_write_to_string PRIVATE -DCBMC ${C_RELEASE_BUILD_OPTIONS})
SET_PROPERTY(
    TARGET model_guid_write_to_string
    PROPERTY JOB_POOL_LINK console)

ADD_CUSTOM_COMMAND(
    TARGET model_guid_write_to_string
    POST_BUILD
    COMMAND cbmc ${CBMC_OPTIONS} model_guid_write_to_string
    USES_TERMINAL)
