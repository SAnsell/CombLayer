set (crystalSources
    AtomPos.cxx CifItem.cxx CifLoop.cxx 
    CifStore.cxx loopItem.cxx SymUnit.cxx 
)

add_library (crystal SHARED
  ${crystalSources}
)
target_include_directories (crystal PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../crystalInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/AtomPos.cxx
  ${tarDIR}/CifItem.cxx
  ${tarDIR}/CifLoop.cxx
  ${tarDIR}/CifStore.cxx
  ${tarDIR}/loopItem.cxx
  ${tarDIR}/SymUnit.cxx
  ${tarINC}/AtomData.h
  ${tarINC}/AtomPos.h
  ${tarINC}/CifItem.h
  ${tarINC}/CifLoop.h
  ${tarINC}/CifStore.h
  ${tarINC}/loopItem.h
  ${tarINC}/SymUnit.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

