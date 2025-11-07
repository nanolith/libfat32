SET(CBMC_OPTIONS
	"--bounds-check" "--pointer-check" "--memory-leak-check"
	"--malloc-may-fail" "--malloc-fail-null" "--div-by-zero-check"
    "--pointer-overflow-check" "--trace" "--stop-on-fail"
	"--drop-unused-functions" "--unwind" "10" "--unwinding-assertions")

ADD_SUBDIRECTORY(crc)
ADD_SUBDIRECTORY(gpt)
ADD_SUBDIRECTORY(guid)
