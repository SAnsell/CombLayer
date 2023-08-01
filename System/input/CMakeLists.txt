set (inputSources
    IItemBase.cxx inputParam.cxx inputParamSupport.cxx 
    inputSupport.cxx 
)

add_library (input SHARED
  ${inputSources}
)
target_include_directories (input PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../inputInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/IItemBase.cxx
  ${tarDIR}/inputParam.cxx
  ${tarDIR}/inputParamSupport.cxx
  ${tarDIR}/inputSupport.cxx
  ${tarINC}/IItemBase.h
  ${tarINC}/inputParam.h
  ${tarINC}/inputSupport.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

