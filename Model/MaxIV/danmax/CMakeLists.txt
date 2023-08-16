set (danmaxSources
    danmaxConnectLine.cxx DANMAX.cxx danmaxFrontEnd.cxx 
    danmaxOpticsLine.cxx danmaxVariables.cxx MirrorMono.cxx 
    SqrShield.cxx 
)

add_library (danmax SHARED
  ${danmaxSources}
)
target_include_directories (danmax PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${MAXIV_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../danmaxInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/danmaxConnectLine.cxx
  ${tarDIR}/DANMAX.cxx
  ${tarDIR}/danmaxFrontEnd.cxx
  ${tarDIR}/danmaxOpticsLine.cxx
  ${tarDIR}/danmaxVariables.cxx
  ${tarDIR}/MirrorMono.cxx
  ${tarDIR}/SqrShield.cxx
  ${tarINC}/danmaxConnectLine.h
  ${tarINC}/danmaxFrontEnd.h
  ${tarINC}/DANMAX.h
  ${tarINC}/danmaxOpticsLine.h
  ${tarINC}/SqrShield.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

