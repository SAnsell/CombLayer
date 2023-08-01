set (srcSources
    flukaGenParticle.cxx LowMat.cxx mainJobs.cxx 
    masterRotate.cxx multiData.cxx NGroup.cxx 
    NList.cxx NodePoint.cxx NRange.cxx 
    NTree.cxx objectGroups.cxx pairRange.cxx 
    particleConv.cxx ReadFunctions.cxx SimFLUKA.cxx 
    SimMCNP.cxx SimPHITS.cxx SimPhoton.cxx 
    SimPOVRay.cxx SimTrack.cxx Simulation.cxx 
    SimValid.cxx transComp.cxx Triple.cxx 
    version.cxx xrayFormFactor.cxx 
)

add_library (src SHARED
  ${srcSources}
)
target_include_directories (src PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../srcInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/flukaGenParticle.cxx
  ${tarDIR}/LowMat.cxx
  ${tarDIR}/mainJobs.cxx
  ${tarDIR}/masterRotate.cxx
  ${tarDIR}/multiData.cxx
  ${tarDIR}/NGroup.cxx
  ${tarDIR}/NList.cxx
  ${tarDIR}/NodePoint.cxx
  ${tarDIR}/NRange.cxx
  ${tarDIR}/NTree.cxx
  ${tarDIR}/objectGroups.cxx
  ${tarDIR}/pairRange.cxx
  ${tarDIR}/particleConv.cxx
  ${tarDIR}/ReadFunctions.cxx
  ${tarDIR}/SimFLUKA.cxx
  ${tarDIR}/SimMCNP.cxx
  ${tarDIR}/SimPHITS.cxx
  ${tarDIR}/SimPhoton.cxx
  ${tarDIR}/SimPOVRay.cxx
  ${tarDIR}/SimTrack.cxx
  ${tarDIR}/Simulation.cxx
  ${tarDIR}/SimValid.cxx
  ${tarDIR}/transComp.cxx
  ${tarDIR}/Triple.cxx
  ${tarDIR}/version.cxx
  ${tarDIR}/xrayFormFactor.cxx
  ${tarINC}/dataSlice.h
  ${tarINC}/flukaGenParticle.h
  ${tarINC}/LowMat.h
  ${tarINC}/mainJobs.h
  ${tarINC}/masterRotate.h
  ${tarINC}/multiData.h
  ${tarINC}/NGroup.h
  ${tarINC}/NList.h
  ${tarINC}/NodePoint.h
  ${tarINC}/NRange.h
  ${tarINC}/NTree.h
  ${tarINC}/objectGroups.h
  ${tarINC}/pairRange.h
  ${tarINC}/particleConv.h
  ${tarINC}/ReadFunctions.h
  ${tarINC}/SimFLUKA.h
  ${tarINC}/SimMCNP.h
  ${tarINC}/SimPHITS.h
  ${tarINC}/SimPhoton.h
  ${tarINC}/SimPOVRay.h
  ${tarINC}/SimTrack.h
  ${tarINC}/Simulation.h
  ${tarINC}/SimValid.h
  ${tarINC}/transComp.h
  ${tarINC}/Triple.h
  ${tarINC}/version.h
  ${tarINC}/xrayFormFactor.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

