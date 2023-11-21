set (physicsSources
    DXTConstructor.cxx DXTControl.cxx ELPTConstructor.cxx 
    EUnit.cxx ExtConstructor.cxx ExtControl.cxx 
    FCLConstructor.cxx FCLControl.cxx LSwitchCard.cxx 
    ModeCard.cxx nameCard.cxx PhysicsCardFunctions.cxx 
    PhysicsCards.cxx PhysImp.cxx PSimple.cxx 
    PStandard.cxx PWTConstructor.cxx PWTControl.cxx 
    ZoneUnit.cxx 
)

add_library (physics SHARED
  ${physicsSources}
)
target_include_directories (physics PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../physicsInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/DXTConstructor.cxx
  ${tarDIR}/DXTControl.cxx
  ${tarDIR}/ELPTConstructor.cxx
  ${tarDIR}/EUnit.cxx
  ${tarDIR}/ExtConstructor.cxx
  ${tarDIR}/ExtControl.cxx
  ${tarDIR}/FCLConstructor.cxx
  ${tarDIR}/FCLControl.cxx
  ${tarDIR}/LSwitchCard.cxx
  ${tarDIR}/ModeCard.cxx
  ${tarDIR}/nameCard.cxx
  ${tarDIR}/PhysicsCardFunctions.cxx
  ${tarDIR}/PhysicsCards.cxx
  ${tarDIR}/PhysImp.cxx
  ${tarDIR}/PSimple.cxx
  ${tarDIR}/PStandard.cxx
  ${tarDIR}/PWTConstructor.cxx
  ${tarDIR}/PWTControl.cxx
  ${tarDIR}/ZoneUnit.cxx
  ${tarINC}/DXTConstructor.h
  ${tarINC}/DXTControl.h
  ${tarINC}/ELPTConstructor.h
  ${tarINC}/EUnit.h
  ${tarINC}/ExtConstructor.h
  ${tarINC}/ExtControl.h
  ${tarINC}/FCLConstructor.h
  ${tarINC}/FCLControl.h
  ${tarINC}/IMPConstructor.h
  ${tarINC}/LSwitchCard.h
  ${tarINC}/ModeCard.h
  ${tarINC}/nameCard.h
  ${tarINC}/PhysCard.h
  ${tarINC}/PhysicsCardFunctions.h
  ${tarINC}/PhysicsCards.h
  ${tarINC}/PhysImp.h
  ${tarINC}/PSimple.h
  ${tarINC}/PStandard.h
  ${tarINC}/PWTConstructor.h
  ${tarINC}/PWTControl.h
  ${tarINC}/WWGcard.h
  ${tarINC}/ZoneUnit.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

