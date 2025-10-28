SET(MC_SOURCES
    ${CMAKE_SOURCE_DIR}/src/gpt/gpt_protective_mbr_partition_record_write.c
    ${CMAKE_SOURCE_DIR}/models/shadow/gpt/property_gpt_protective_mbr_partition_record_valid.c
    main.c)

ADD_EXECUTABLE(
    model_gpt_protective_mbr_partition_record_write ${MC_SOURCES})
SET_TARGET_PROPERTIES(
    model_gpt_protective_mbr_partition_record_write PROPERTIES
    C_COMPILER_LAUNCHER "${COMPILER_CHOOSER};goto-cc"
    C_LINKER_LAUNCHER "${LINKER_CHOOSER};goto-ld")
TARGET_COMPILE_OPTIONS(
    model_gpt_protective_mbr_partition_record_write PRIVATE
        -DCBMC ${C_RELEASE_BUILD_OPTIONS})
SET_PROPERTY(
    TARGET model_gpt_protective_mbr_partition_record_write
    PROPERTY JOB_POOL_LINK console)

ADD_CUSTOM_COMMAND(
    TARGET model_gpt_protective_mbr_partition_record_write
    POST_BUILD
    COMMAND cbmc ${CBMC_OPTIONS}
                 model_gpt_protective_mbr_partition_record_write
    USES_TERMINAL)
