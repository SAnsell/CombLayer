set (md5Sources
    MD5hash.cxx 
)

add_library (md5 SHARED
  ${md5Sources}
)
target_include_directories (md5 PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../md5Inc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/MD5hash.cxx
  ${tarINC}/MD5hash.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

