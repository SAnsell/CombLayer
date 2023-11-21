set (flukaProcessSources
    cellValueSet.cxx flukaDefPhysics.cxx flukaImpConstructor.cxx 
    flukaProcess.cxx pairValueSet.cxx plotGeom.cxx 
)

add_library (flukaProcess SHARED
  ${flukaProcessSources}
)
target_include_directories (flukaProcess PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../flukaProcessInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/cellValueSet.cxx
  ${tarDIR}/flukaDefPhysics.cxx
  ${tarDIR}/flukaImpConstructor.cxx
  ${tarDIR}/flukaProcess.cxx
  ${tarDIR}/pairValueSet.cxx
  ${tarDIR}/plotGeom.cxx
  ${tarINC}/cellValueSet.h
  ${tarINC}/flukaDefPhysics.h
  ${tarINC}/flukaImpConstructor.h
  ${tarINC}/flukaProcess.h
  ${tarINC}/pairValueSet.h
  ${tarINC}/plotGeom.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

