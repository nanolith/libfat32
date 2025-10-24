SET(MC_SOURCES
    ${CMAKE_SOURCE_DIR}/src/guid/guid_init_from_data.c
    main.c)

ADD_EXECUTABLE(model_guid_init_from_data ${MC_SOURCES})
SET_TARGET_PROPERTIES(
    model_guid_init_from_data PROPERTIES
    C_COMPILER_LAUNCHER "${COMPILER_CHOOSER};goto-cc"
    C_LINKER_LAUNCHER "${LINKER_CHOOSER};goto-ld")
TARGET_COMPILE_OPTIONS(
    model_guid_init_from_data PRIVATE -DCBMC ${C_RELEASE_BUILD_OPTIONS})
SET_PROPERTY(
    TARGET model_guid_init_from_data
    PROPERTY JOB_POOL_LINK console)

ADD_CUSTOM_COMMAND(
    TARGET model_guid_init_from_data
    POST_BUILD
    COMMAND cbmc ${CBMC_OPTIONS} model_guid_init_from_data
    USES_TERMINAL)
