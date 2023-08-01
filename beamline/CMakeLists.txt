set (beamlineSources
    BenderUnit.cxx DBenderUnit.cxx GuideUnit.cxx 
    PlateUnit.cxx 
)

add_library (beamline SHARED
  ${beamlineSources}
)
target_include_directories (beamline PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../beamlineInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/BenderUnit.cxx
  ${tarDIR}/DBenderUnit.cxx
  ${tarDIR}/GuideUnit.cxx
  ${tarDIR}/PlateUnit.cxx
  ${tarINC}/beamlineConstructor.h
  ${tarINC}/BenderUnit.h
  ${tarINC}/convexShape.h
  ${tarINC}/DBenderUnit.h
  ${tarINC}/GuideUnit.h
  ${tarINC}/PlateUnit.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

