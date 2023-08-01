set (endfSources
    ENDF.cxx ENDFmaterial.cxx ENDFreaction.cxx 
    SEtable.cxx SQWtable.cxx 
)

add_library (endf SHARED
  ${endfSources}
)
target_include_directories (endf PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../endfInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/ENDF.cxx
  ${tarDIR}/ENDFmaterial.cxx
  ${tarDIR}/ENDFreaction.cxx
  ${tarDIR}/SEtable.cxx
  ${tarDIR}/SQWtable.cxx
  ${tarINC}/ENDF.h
  ${tarINC}/ENDFmaterial.h
  ${tarINC}/ENDFreaction.h
  ${tarINC}/ePrimeFunc.h
  ${tarINC}/SEtable.h
  ${tarINC}/SQWtable.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

