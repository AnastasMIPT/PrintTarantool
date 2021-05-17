
# - Find c-connector library
# The module defines the following variables:
#
#  CCONN_FOUND        - true if c-connector was found
#  CCONN_INCLUDE_DIRS - the directory of the c-connector headers
#  CCONN_LIBRARIES    - the c-connector static library needed for linking
#

find_path(CCONN_INCLUDE_DIR tnt_stream.h PATH_SUFFIXES tarantool)
find_library(CCONN_LIBRARY NAMES libtarantool.a)
find_library(CCONN_LIBRARY_SO NAMES libtarantool.so)

message(STATUS "c-connector include dir is ${CCONN_INCLUDE_DIR}")
message(STATUS "c-connector library dir is ${CCONN_LIBRARY}")
message(STATUS "c-connector .so library dir is ${CCONN_LIBRARY_SO}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CConn
    REQUIRED_VARS CCONN_INCLUDE_DIR CCONN_LIBRARY CCONN_LIBRARY_SO)
set(CCONN_INCLUDE_DIRS ${CCONN_INCLUDE_DIR})
set(CCONN_LIBRARYS ${CCONN_LIBRARY} ${CCONN_LIBRARY_SO})
mark_as_advanced(MSGPUCK_INCLUDE_DIR MSGPUCK_INCLUDE_DIRS
                 MSGPUCK_LIBRARY MSGPUCK_LIBRARIES CCONN_LIBRARY_SO)