SET(MC_SOURCES
    ${CMAKE_SOURCE_DIR}/src/guid/guid_init_from_string.c
    ${CMAKE_SOURCE_DIR}/models/shadow/stdc/isxdigit.c
    main.c)

ADD_EXECUTABLE(model_guid_init_from_string ${MC_SOURCES})
SET_TARGET_PROPERTIES(
    model_guid_init_from_string PROPERTIES
    C_COMPILER_LAUNCHER "${COMPILER_CHOOSER};goto-cc"
    C_LINKER_LAUNCHER "${LINKER_CHOOSER};goto-ld")
TARGET_COMPILE_OPTIONS(
    model_guid_init_from_string PRIVATE -DCBMC ${C_RELEASE_BUILD_OPTIONS})
SET_PROPERTY(
    TARGET model_guid_init_from_string
    PROPERTY JOB_POOL_LINK console)

ADD_CUSTOM_COMMAND(
    TARGET model_guid_init_from_string
    POST_BUILD
    COMMAND cbmc ${CBMC_OPTIONS} --unwindset fat32_708394ec_192d_4989_9512_fbfb7845b0c6_V0_0_guid_init_from_string.0:41 model_guid_init_from_string
    USES_TERMINAL)
