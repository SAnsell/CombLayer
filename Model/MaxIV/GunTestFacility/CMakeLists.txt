set (GunTestFacilitySources
    BuildingB.cxx ConcreteDoor.cxx GTFLine.cxx
    GunTestFacility.cxx IonPumpGammaVacuum.cxx RFGun.cxx
    Solenoid.cxx GTFGateValve.cxx CurrentMonitor.cxx variables.cxx
)

add_library (GunTestFacility SHARED
  ${GunTestFacilitySources}
)
target_include_directories (GunTestFacility PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${MAXIV_INCLUDE}
)

file(RELATIVE_PATH tarDIR
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../GunTestFacilityInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/BuildingB.cxx
  ${tarDIR}/ConcreteDoor.cxx
  ${tarDIR}/GTFLine.cxx
  ${tarDIR}/GunTestFacility.cxx
  ${tarDIR}/IonPumpGammaVacuum.cxx
  ${tarDIR}/RFGun.cxx
  ${tarDIR}/Solenoid.cxx
  ${tarDIR}/GTFGateValve.cxx
  ${tarDIR}/variables.cxx
  ${tarINC}/BuildingB.h
  ${tarINC}/ConcreteDoor.h
  ${tarINC}/GTFLine.h
  ${tarINC}/GunTestFacility.h
  ${tarINC}/IonPumpGammaVacuum.h
  ${tarINC}/RFGun.h
  ${tarINC}/Solenoid.h
  ${tarINC}/GTFGateValve.h
  ${tarINC}/CurrentMonitor.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)
