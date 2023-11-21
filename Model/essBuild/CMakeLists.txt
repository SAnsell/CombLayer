set (essBuildSources
    BeRef.cxx BeRefInnerStructure.cxx BilbaoWheelCassette.cxx 
    BilbaoWheel.cxx BilbaoWheelInnerStructure.cxx BlockAddition.cxx 
    Box.cxx BoxModerator.cxx BulkModule.cxx 
    Bunker.cxx BunkerFeed.cxx BunkerGaps.cxx 
    BunkerInsert.cxx BunkerQuake.cxx BunkerQUnit.cxx 
    BunkerRoof.cxx bunkerVariables.cxx BunkerWall.cxx 
    ButterflyModerator.cxx Chicane.cxx CompBInsert.cxx 
    Curtain.cxx CylFlowGuide.cxx CylPreMod.cxx 
    DefUnitsESS.cxx DiskLayerMod.cxx DiskPreMod.cxx 
    EdgeWater.cxx EmptyCyl.cxx EssInputs.cxx 
    EssModBase.cxx ESSPipes.cxx EssPipeVariables.cxx 
    essVariables.cxx F5Calc.cxx F5Collimator.cxx 
    FocusPoints.cxx FullBunker.cxx GuideBay.cxx 
    GuideItem.cxx H2FlowGuide.cxx H2Wing.cxx 
    HighBay.cxx IradCylinder.cxx iradVariables.cxx 
    makeESSBL.cxx makeESS.cxx makeSingleLine.cxx 
    MidWaterDivider.cxx moderatorVariables.cxx OnionCooling.cxx 
    PancakeModerator.cxx PBIP.cxx PBW.cxx 
    pillarInfo.cxx pinInfo.cxx PreModWing.cxx 
    ProtonTube.cxx RoofPillars.cxx SegWheel.cxx 
    ShutterBay.cxx targetVariables.cxx TelescopicPipe.cxx 
    TSMainBuilding.cxx TwisterModule.cxx WedgeFlightLine.cxx 
    WedgeItem.cxx WheelBase.cxx Wheel.cxx 
)

add_library (essBuild SHARED
  ${essBuildSources}
)
target_include_directories (essBuild PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/essBuildInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../essBuildInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/BeRef.cxx
  ${tarDIR}/BeRefInnerStructure.cxx
  ${tarDIR}/BilbaoWheelCassette.cxx
  ${tarDIR}/BilbaoWheel.cxx
  ${tarDIR}/BilbaoWheelInnerStructure.cxx
  ${tarDIR}/BlockAddition.cxx
  ${tarDIR}/Box.cxx
  ${tarDIR}/BoxModerator.cxx
  ${tarDIR}/BulkModule.cxx
  ${tarDIR}/Bunker.cxx
  ${tarDIR}/BunkerFeed.cxx
  ${tarDIR}/BunkerGaps.cxx
  ${tarDIR}/BunkerInsert.cxx
  ${tarDIR}/BunkerQuake.cxx
  ${tarDIR}/BunkerQUnit.cxx
  ${tarDIR}/BunkerRoof.cxx
  ${tarDIR}/bunkerVariables.cxx
  ${tarDIR}/BunkerWall.cxx
  ${tarDIR}/ButterflyModerator.cxx
  ${tarDIR}/Chicane.cxx
  ${tarDIR}/CompBInsert.cxx
  ${tarDIR}/Curtain.cxx
  ${tarDIR}/CylFlowGuide.cxx
  ${tarDIR}/CylPreMod.cxx
  ${tarDIR}/DefUnitsESS.cxx
  ${tarDIR}/DiskLayerMod.cxx
  ${tarDIR}/DiskPreMod.cxx
  ${tarDIR}/EdgeWater.cxx
  ${tarDIR}/EmptyCyl.cxx
  ${tarDIR}/EssInputs.cxx
  ${tarDIR}/EssModBase.cxx
  ${tarDIR}/ESSPipes.cxx
  ${tarDIR}/EssPipeVariables.cxx
  ${tarDIR}/essVariables.cxx
  ${tarDIR}/F5Calc.cxx
  ${tarDIR}/F5Collimator.cxx
  ${tarDIR}/FocusPoints.cxx
  ${tarDIR}/FullBunker.cxx
  ${tarDIR}/GuideBay.cxx
  ${tarDIR}/GuideItem.cxx
  ${tarDIR}/H2FlowGuide.cxx
  ${tarDIR}/H2Wing.cxx
  ${tarDIR}/HighBay.cxx
  ${tarDIR}/IradCylinder.cxx
  ${tarDIR}/iradVariables.cxx
  ${tarDIR}/makeESSBL.cxx
  ${tarDIR}/makeESS.cxx
  ${tarDIR}/makeSingleLine.cxx
  ${tarDIR}/MidWaterDivider.cxx
  ${tarDIR}/moderatorVariables.cxx
  ${tarDIR}/OnionCooling.cxx
  ${tarDIR}/PancakeModerator.cxx
  ${tarDIR}/PBIP.cxx
  ${tarDIR}/PBW.cxx
  ${tarDIR}/pillarInfo.cxx
  ${tarDIR}/pinInfo.cxx
  ${tarDIR}/PreModWing.cxx
  ${tarDIR}/ProtonTube.cxx
  ${tarDIR}/RoofPillars.cxx
  ${tarDIR}/SegWheel.cxx
  ${tarDIR}/ShutterBay.cxx
  ${tarDIR}/targetVariables.cxx
  ${tarDIR}/TelescopicPipe.cxx
  ${tarDIR}/TSMainBuilding.cxx
  ${tarDIR}/TwisterModule.cxx
  ${tarDIR}/WedgeFlightLine.cxx
  ${tarDIR}/WedgeItem.cxx
  ${tarDIR}/WheelBase.cxx
  ${tarDIR}/Wheel.cxx
  ${tarINC}/BeRef.h
  ${tarINC}/BeRefInnerStructure.h
  ${tarINC}/BilbaoWheelCassette.h
  ${tarINC}/BilbaoWheel.h
  ${tarINC}/BilbaoWheelInnerStructure.h
  ${tarINC}/BlockAddition.h
  ${tarINC}/Box.h
  ${tarINC}/BoxModerator.h
  ${tarINC}/BulkModule.h
  ${tarINC}/BunkerFeed.h
  ${tarINC}/BunkerGaps.h
  ${tarINC}/Bunker.h
  ${tarINC}/BunkerInsert.h
  ${tarINC}/BunkerQuake.h
  ${tarINC}/BunkerQUnit.h
  ${tarINC}/BunkerRoof.h
  ${tarINC}/BunkerWall.h
  ${tarINC}/ButterflyModerator.h
  ${tarINC}/Chicane.h
  ${tarINC}/CompBInsert.h
  ${tarINC}/Curtain.h
  ${tarINC}/CylFlowGuide.h
  ${tarINC}/CylPreMod.h
  ${tarINC}/DefUnitsESS.h
  ${tarINC}/DiskLayerMod.h
  ${tarINC}/DiskPreMod.h
  ${tarINC}/EdgeWater.h
  ${tarINC}/EmptyCyl.h
  ${tarINC}/EssModBase.h
  ${tarINC}/ESSPipes.h
  ${tarINC}/essVariables.h
  ${tarINC}/F5Calc.h
  ${tarINC}/F5Collimator.h
  ${tarINC}/FocusPoints.h
  ${tarINC}/FullBunker.h
  ${tarINC}/GuideBay.h
  ${tarINC}/GuideItem.h
  ${tarINC}/H2FlowGuide.h
  ${tarINC}/H2Wing.h
  ${tarINC}/HighBay.h
  ${tarINC}/IradCylinder.h
  ${tarINC}/makeESSBL.h
  ${tarINC}/makeESS.h
  ${tarINC}/makeSingleLine.h
  ${tarINC}/MidWaterDivider.h
  ${tarINC}/OnionCooling.h
  ${tarINC}/PancakeModerator.h
  ${tarINC}/PBIP.h
  ${tarINC}/PBW.h
  ${tarINC}/pillarInfo.h
  ${tarINC}/pinInfo.h
  ${tarINC}/PreModWing.h
  ${tarINC}/ProtonTube.h
  ${tarINC}/RoofPillars.h
  ${tarINC}/RoofPins.h
  ${tarINC}/SegWheel.h
  ${tarINC}/ShutterBay.h
  ${tarINC}/TelescopicPipe.h
  ${tarINC}/TSMainBuilding.h
  ${tarINC}/TwisterModule.h
  ${tarINC}/WedgeFlightLine.h
  ${tarINC}/WedgeItem.h
  ${tarINC}/WheelBase.h
  ${tarINC}/Wheel.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

