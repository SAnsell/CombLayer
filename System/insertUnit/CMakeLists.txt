set (insertUnitSources
    addInsertObj.cxx insertCurve.cxx insertCylinder.cxx 
    insertGrid.cxx insertObject.cxx insertPlate.cxx 
    insertShell.cxx insertSphere.cxx 
)

add_library (insertUnit SHARED
  ${insertUnitSources}
)
target_include_directories (insertUnit PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../insertUnitInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/addInsertObj.cxx
  ${tarDIR}/insertCurve.cxx
  ${tarDIR}/insertCylinder.cxx
  ${tarDIR}/insertGrid.cxx
  ${tarDIR}/insertObject.cxx
  ${tarDIR}/insertPlate.cxx
  ${tarDIR}/insertShell.cxx
  ${tarDIR}/insertSphere.cxx
  ${tarINC}/addInsertObj.h
  ${tarINC}/insertCurve.h
  ${tarINC}/insertCylinder.h
  ${tarINC}/insertGrid.h
  ${tarINC}/insertObject.h
  ${tarINC}/insertPlate.h
  ${tarINC}/insertShell.h
  ${tarINC}/insertSphere.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

