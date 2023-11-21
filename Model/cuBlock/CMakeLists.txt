set (cuBlockSources
    CuCollet.cxx cuVariables.cxx makeCu.cxx 
    test.cxx 
)

add_library (cuBlock SHARED
  ${cuBlockSources}
)
target_include_directories (cuBlock PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/cuBlockInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../cuBlockInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/CuCollet.cxx
  ${tarDIR}/cuVariables.cxx
  ${tarDIR}/makeCu.cxx
  ${tarDIR}/test.cxx
  ${tarINC}/CuCollet.h
  ${tarINC}/makeCu.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

