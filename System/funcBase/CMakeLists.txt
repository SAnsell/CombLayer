set (funcBaseSources
    Code.cxx FFunc.cxx FItem.cxx 
    FList.cxx FuncDataBase.cxx funcList.cxx 
    FValue.cxx varBlock.cxx varList.cxx 
    varNameOrder.cxx 
)

add_library (funcBase SHARED
  ${funcBaseSources}
)
target_include_directories (funcBase PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../funcBaseInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/Code.cxx
  ${tarDIR}/FFunc.cxx
  ${tarDIR}/FItem.cxx
  ${tarDIR}/FList.cxx
  ${tarDIR}/FuncDataBase.cxx
  ${tarDIR}/funcList.cxx
  ${tarDIR}/FValue.cxx
  ${tarDIR}/varBlock.cxx
  ${tarDIR}/varList.cxx
  ${tarDIR}/varNameOrder.cxx
  ${tarINC}/Code.h
  ${tarINC}/FItem.h
  ${tarINC}/FuncDataBase.h
  ${tarINC}/funcList.h
  ${tarINC}/varBlock.h
  ${tarINC}/varList.h
  ${tarINC}/varNameOrder.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

