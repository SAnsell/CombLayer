set (phitsProcessSources
    phitsDefPhyscs.cxx phitsProcess.cxx 
)

add_library (phitsProcess SHARED
  ${phitsProcessSources}
)
target_include_directories (phitsProcess PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../phitsProcessInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/phitsDefPhyscs.cxx
  ${tarDIR}/phitsProcess.cxx
  ${tarINC}/phitsDefPhysics.h
  ${tarINC}/phitsImpConstructor.h
  ${tarINC}/phitsProcess.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

