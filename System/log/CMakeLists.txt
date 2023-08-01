set (logSources
    Debug.cxx debugMethod.cxx EReport.cxx 
    FileReport.cxx MemStack.cxx NameStack.cxx 
    OutputLog.cxx RegMethod.cxx 
)

add_library (log SHARED
  ${logSources}
)
target_include_directories (log PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../logInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/Debug.cxx
  ${tarDIR}/debugMethod.cxx
  ${tarDIR}/EReport.cxx
  ${tarDIR}/FileReport.cxx
  ${tarDIR}/MemStack.cxx
  ${tarDIR}/NameStack.cxx
  ${tarDIR}/OutputLog.cxx
  ${tarDIR}/RegMethod.cxx
  ${tarINC}/Debug.h
  ${tarINC}/debugMethod.h
  ${tarINC}/FileReport.h
  ${tarINC}/GTKreport.h
  ${tarINC}/MemStack.h
  ${tarINC}/NameStack.h
  ${tarINC}/OutputLog.h
  ${tarINC}/RegMethod.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

