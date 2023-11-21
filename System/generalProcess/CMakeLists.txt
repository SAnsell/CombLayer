set (generalProcessSources
    doseFactors.cxx MainInputs.cxx MainProcess.cxx 
    meshConstruct.cxx ModelSupport.cxx Process.cxx 
)

add_library (generalProcess SHARED
  ${generalProcessSources}
)
target_include_directories (generalProcess PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../generalProcessInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/doseFactors.cxx
  ${tarDIR}/MainInputs.cxx
  ${tarDIR}/MainProcess.cxx
  ${tarDIR}/meshConstruct.cxx
  ${tarDIR}/ModelSupport.cxx
  ${tarDIR}/Process.cxx
  ${tarINC}/doseFactors.h
  ${tarINC}/MainInputs.h
  ${tarINC}/MainProcess.h
  ${tarINC}/meshConstruct.h
  ${tarINC}/ModelSupport.h
  ${tarINC}/Process.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

