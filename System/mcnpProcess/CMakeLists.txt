set (mcnpProcessSources
    mcnpDefPhysics.cxx pointDetOpt.cxx SimProcess.cxx 
    VolSum.cxx 
)

add_library (mcnpProcess SHARED
  ${mcnpProcessSources}
)
target_include_directories (mcnpProcess PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcnpProcessInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/mcnpDefPhysics.cxx
  ${tarDIR}/pointDetOpt.cxx
  ${tarDIR}/SimProcess.cxx
  ${tarDIR}/VolSum.cxx
  ${tarINC}/mcnpDefPhysics.h
  ${tarINC}/mcnpProcess.h
  ${tarINC}/pointDetOpt.h
  ${tarINC}/SimImportance.h
  ${tarINC}/SimProcess.h
  ${tarINC}/VolSum.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

