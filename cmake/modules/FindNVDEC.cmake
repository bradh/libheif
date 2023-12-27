include(LibFindMacros)

find_path(NVDEC_INCLUDE_DIR
    NAMES nvcuvid.h
)

find_library(NVDEC_LIBRARY
    NAMES libnvcuvid nvcuvid
)

find_package(CUDAToolkit REQUIRED)

set(NVDEC_PROCESS_LIBS NVDEC_LIBRARY)
set(NVDEC_PROCESS_INCLUDES NVDEC_INCLUDE_DIR)
libfind_process(NVDEC)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NVDEC
    REQUIRED_VARS
        NVDEC_INCLUDE_DIRS
        NVDEC_LIBRARIES
)
