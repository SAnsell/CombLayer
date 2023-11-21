set (photonSources
    B4CCollimator.cxx CylContainer.cxx CylLayer.cxx 
    EQDetector.cxx He3Tubes.cxx HeShield.cxx 
    makePhoton2.cxx makePhoton3.cxx makePhoton.cxx 
    ModContainer.cxx PhotonVariables.cxx plateInfo.cxx 
    PlateMod.cxx TableSupport.cxx TubeCollimator.cxx 
    TubeMod.cxx VacuumVessel.cxx 
)

add_library (photon SHARED
  ${photonSources}
)
target_include_directories (photon PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/photonInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../photonInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/B4CCollimator.cxx
  ${tarDIR}/CylContainer.cxx
  ${tarDIR}/CylLayer.cxx
  ${tarDIR}/EQDetector.cxx
  ${tarDIR}/He3Tubes.cxx
  ${tarDIR}/HeShield.cxx
  ${tarDIR}/makePhoton2.cxx
  ${tarDIR}/makePhoton3.cxx
  ${tarDIR}/makePhoton.cxx
  ${tarDIR}/ModContainer.cxx
  ${tarDIR}/PhotonVariables.cxx
  ${tarDIR}/plateInfo.cxx
  ${tarDIR}/PlateMod.cxx
  ${tarDIR}/TableSupport.cxx
  ${tarDIR}/TubeCollimator.cxx
  ${tarDIR}/TubeMod.cxx
  ${tarDIR}/VacuumVessel.cxx
  ${tarINC}/B4CCollimator.h
  ${tarINC}/CylContainer.h
  ${tarINC}/CylLayer.h
  ${tarINC}/EQDetector.h
  ${tarINC}/He3Tubes.h
  ${tarINC}/HeShield.h
  ${tarINC}/makePhoton2.h
  ${tarINC}/makePhoton3.h
  ${tarINC}/makePhoton.h
  ${tarINC}/ModContainer.h
  ${tarINC}/plateInfo.h
  ${tarINC}/PlateMod.h
  ${tarINC}/TableSupport.h
  ${tarINC}/TubeCollimator.h
  ${tarINC}/TubeMod.h
  ${tarINC}/VacuumVessel.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

