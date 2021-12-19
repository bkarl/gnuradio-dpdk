INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_DPDK dpdk)

FIND_PATH(
    DPDK_INCLUDE_DIRS
    NAMES dpdk/api.h
    HINTS $ENV{DPDK_DIR}/include
        ${PC_DPDK_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    DPDK_LIBRARIES
    NAMES gnuradio-dpdk
    HINTS $ENV{DPDK_DIR}/lib
        ${PC_DPDK_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/dpdkTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DPDK DEFAULT_MSG DPDK_LIBRARIES DPDK_INCLUDE_DIRS)
MARK_AS_ADVANCED(DPDK_LIBRARIES DPDK_INCLUDE_DIRS)
