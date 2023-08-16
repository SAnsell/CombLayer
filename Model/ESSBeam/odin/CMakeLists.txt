set (odinSources
    Hut.cxx ODIN.cxx ODINvariables.cxx 
    RentrantBS.cxx 
)

add_library (odin SHARED
  ${odinSources}
)
target_include_directories (odin PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../odinInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/Hut.cxx
  ${tarDIR}/ODIN.cxx
  ${tarDIR}/ODINvariables.cxx
  ${tarDIR}/RentrantBS.cxx
  ${tarINC}/Hut.h
  ${tarINC}/ODIN.h
  ${tarINC}/RentrantBS.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

