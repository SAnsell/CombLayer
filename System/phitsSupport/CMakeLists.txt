set (phitsSupportSources
    aType.cxx eType.cxx MeshXYZ.cxx 
    phitsWriteSupport.cxx 
)

add_library (phitsSupport SHARED
  ${phitsSupportSources}
)
target_include_directories (phitsSupport PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../phitsSupportInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/aType.cxx
  ${tarDIR}/eType.cxx
  ${tarDIR}/MeshXYZ.cxx
  ${tarDIR}/phitsWriteSupport.cxx
  ${tarINC}/aType.h
  ${tarINC}/eType.h
  ${tarINC}/MeshXYZ.h
  ${tarINC}/phitsWriteSupport.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

