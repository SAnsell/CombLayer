set (testSources
    simpleObj.cxx testAlgebra.cxx testAttachSupport.cxx 
    testBinData.cxx testBnId.cxx testBoundary.cxx 
    testBoxLine.cxx testCone.cxx testContained.cxx 
    testConvex2D.cxx testConvex.cxx testCylinder.cxx 
    testDBMaterial.cxx testDoubleErr.cxx testElement.cxx 
    testEllipsoid.cxx testEllipticCyl.cxx testExtControl.cxx 
    testFace.cxx testFortranWrite.cxx testFunc.cxx 
    testFunction.cxx testGenerateSurf.cxx testGeomSupport.cxx 
    testGroupRange.cxx testHeadRule.cxx testIndexCounter.cxx
    testInputParam.cxx testInsertComp.cxx testLine.cxx
    testLineTrack.cxx testLog.cxx testMapRange.cxx testMapSupport.cxx 
    testMasterRotate.cxx testMaterial.cxx testMathSupport.cxx 
    testMatrix.cxx testMD5.cxx testMesh3D.cxx 
    testModelSupport.cxx testMultiData.cxx testNeutron.cxx testNList.cxx 
    testNRange.cxx testObject.cxx testObjectRegister.cxx 
    testObjectTrackAct.cxx testObjSurfMap.cxx testObjTrackItem.cxx 
    testPipeLine.cxx testPipeUnit.cxx testPlane.cxx 
    testPoly.cxx testQuadratic.cxx testQuaternion.cxx 
    testRecTriangle.cxx testRules.cxx testSimpleObj.cxx
    testSimpson.cxx testSimulation.cxx testSolveValues.cxx
    testSource.cxx testSupport.cxx 
    testSurfDIter.cxx testSurfDivide.cxx testSurfEqual.cxx 
    testSurfExpand.cxx testSurfImplicate.cxx testSurfRegister.cxx 
    testSurIntersect.cxx testSVD.cxx testTally.cxx 
    testUnitSupport.cxx testVarBlock.cxx testVarNameOrder.cxx 
    testVec3D.cxx testVolumes.cxx testWorkData.cxx 
    testWrapper.cxx testWriteSupport.cxx testXML.cxx 
)

add_library (test SHARED
  ${testSources}
)
target_include_directories (test PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/testInclude
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../testInclude")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/simpleObj.cxx
  ${tarDIR}/testAlgebra.cxx
  ${tarDIR}/testAttachSupport.cxx
  ${tarDIR}/testBinData.cxx
  ${tarDIR}/testBnId.cxx
  ${tarDIR}/testBoundary.cxx
  ${tarDIR}/testBoxLine.cxx
  ${tarDIR}/testCone.cxx
  ${tarDIR}/testContained.cxx
  ${tarDIR}/testConvex2D.cxx
  ${tarDIR}/testConvex.cxx
  ${tarDIR}/testCylinder.cxx
  ${tarDIR}/testDBMaterial.cxx
  ${tarDIR}/testDoubleErr.cxx
  ${tarDIR}/testElement.cxx
  ${tarDIR}/testEllipsoid.cxx
  ${tarDIR}/testEllipticCyl.cxx
  ${tarDIR}/testExtControl.cxx
  ${tarDIR}/testFace.cxx
  ${tarDIR}/testFortranWrite.cxx
  ${tarDIR}/testFunc.cxx
  ${tarDIR}/testFunction.cxx
  ${tarDIR}/testGenerateSurf.cxx
  ${tarDIR}/testGeomSupport.cxx
  ${tarDIR}/testGroupRange.cxx
  ${tarDIR}/testHeadRule.cxx
  ${tarDIR}/testIndexCounter.cxx
  ${tarDIR}/testInputParam.cxx
  ${tarDIR}/testInsertComp.cxx
  ${tarDIR}/testLine.cxx
  ${tarDIR}/testLineTrack.cxx
  ${tarDIR}/testLog.cxx
  ${tarDIR}/testMapRange.cxx
  ${tarDIR}/testMapSupport.cxx
  ${tarDIR}/testMasterRotate.cxx
  ${tarDIR}/testMaterial.cxx
  ${tarDIR}/testMathSupport.cxx
  ${tarDIR}/testMatrix.cxx
  ${tarDIR}/testMD5.cxx
  ${tarDIR}/testMesh3D.cxx
  ${tarDIR}/testModelSupport.cxx
  ${tarDIR}/testMultiData.cxx
  ${tarDIR}/testNeutron.cxx
  ${tarDIR}/testNList.cxx
  ${tarDIR}/testNRange.cxx
  ${tarDIR}/testObject.cxx
  ${tarDIR}/testObjectRegister.cxx
  ${tarDIR}/testObjectTrackAct.cxx
  ${tarDIR}/testObjSurfMap.cxx
  ${tarDIR}/testObjTrackItem.cxx
  ${tarDIR}/testPipeLine.cxx
  ${tarDIR}/testPipeUnit.cxx
  ${tarDIR}/testPlane.cxx
  ${tarDIR}/testPoly.cxx
  ${tarDIR}/testQuadratic.cxx
  ${tarDIR}/testQuaternion.cxx
  ${tarDIR}/testRecTriangle.cxx
  ${tarDIR}/testRotCounter.cxx
  ${tarDIR}/testRules.cxx
  ${tarDIR}/testSimpleObj.cxx
  ${tarDIR}/testSimpson.cxx
  ${tarDIR}/testSimulation.cxx
  ${tarDIR}/testSolveValues.cxx
  ${tarDIR}/testSource.cxx
  ${tarDIR}/testSupport.cxx
  ${tarDIR}/testSurfDIter.cxx
  ${tarDIR}/testSurfDivide.cxx
  ${tarDIR}/testSurfEqual.cxx
  ${tarDIR}/testSurfExpand.cxx
  ${tarDIR}/testSurfImplicate.cxx
  ${tarDIR}/testSurfRegister.cxx
  ${tarDIR}/testSurIntersect.cxx
  ${tarDIR}/testSVD.cxx
  ${tarDIR}/testTally.cxx
  ${tarDIR}/testUnitSupport.cxx
  ${tarDIR}/testVarBlock.cxx
  ${tarDIR}/testVarNameOrder.cxx
  ${tarDIR}/testVec3D.cxx
  ${tarDIR}/testVolumes.cxx
  ${tarDIR}/testWorkData.cxx
  ${tarDIR}/testWrapper.cxx
  ${tarDIR}/testWriteSupport.cxx
  ${tarDIR}/testXML.cxx
  ${tarINC}/simpleObj.h
  ${tarINC}/testAlgebra.h
  ${tarINC}/testAttachSupport.h
  ${tarINC}/testBinData.h
  ${tarINC}/testBnId.h
  ${tarINC}/testBoundary.h
  ${tarINC}/testBoxLine.h
  ${tarINC}/testCone.h
  ${tarINC}/testContained.h
  ${tarINC}/testConvex2D.h
  ${tarINC}/testConvex.h
  ${tarINC}/testCylinder.h
  ${tarINC}/testDBMaterial.h
  ${tarINC}/testDoubleErr.h
  ${tarINC}/testElement.h
  ${tarINC}/testEllipsoid.h
  ${tarINC}/testEllipticCyl.h
  ${tarINC}/testExtControl.h
  ${tarINC}/testFace.h
  ${tarINC}/testFortranWrite.h
  ${tarINC}/testFunc.h
  ${tarINC}/testFunction.h
  ${tarINC}/testGenerateSurf.h
  ${tarINC}/testGeomSupport.h
  ${tarINC}/testGroupRange.h
  ${tarINC}/testHeadRule.h
  ${tarINC}/testIndexCounter.h
  ${tarINC}/testInputParam.h
  ${tarINC}/testInsertComp.h
  ${tarINC}/testLine.h
  ${tarINC}/testLineTrack.h
  ${tarINC}/testLog.h
  ${tarINC}/testMapRange.h
  ${tarINC}/testMapSupport.h
  ${tarINC}/testMasterRotate.h
  ${tarINC}/testMaterial.h
  ${tarINC}/testMathSupport.h
  ${tarINC}/testMatrix.h
  ${tarINC}/testMD5.h
  ${tarINC}/testMesh3D.h
  ${tarINC}/testModelSupport.h
  ${tarINC}/testMultiData.h
  ${tarINC}/testNeutron.h
  ${tarINC}/testNList.h
  ${tarINC}/testNRange.h
  ${tarINC}/testObject.h
  ${tarINC}/testObjectRegister.h
  ${tarINC}/testObjectTrackAct.h
  ${tarINC}/testObjSurfMap.h
  ${tarINC}/testObjTrackItem.h
  ${tarINC}/testPairItem.h
  ${tarINC}/testPipeLine.h
  ${tarINC}/testPipeUnit.h
  ${tarINC}/testPlane.h
  ${tarINC}/testPoly.h
  ${tarINC}/testQuadratic.h
  ${tarINC}/testQuaternion.h
  ${tarINC}/testRecTriangle.h
  ${tarINC}/testRefPlate.h
  ${tarINC}/testRotCounter.h
  ${tarINC}/testRules.h
  ${tarINC}/testSimpleObj.h
  ${tarINC}/testSimpson.h
  ${tarINC}/testSimulation.h
  ${tarINC}/testSolveValues.h
  ${tarINC}/testSource.h
  ${tarINC}/testSupport.h
  ${tarINC}/testSurfDIter.h
  ${tarINC}/testSurfDivide.h
  ${tarINC}/testSurfEqual.h
  ${tarINC}/testSurfExpand.h
  ${tarINC}/testSurfImplicate.h
  ${tarINC}/testSurfRegister.h
  ${tarINC}/testSurIntersect.h
  ${tarINC}/testSVD.h
  ${tarINC}/testTally.h
  ${tarINC}/testUnitSupport.h
  ${tarINC}/testVarBlock.h
  ${tarINC}/testVarNameOrder.h
  ${tarINC}/testVec3D.h
  ${tarINC}/testVolumes.h
  ${tarINC}/testWorkData.h
  ${tarINC}/testWrapper.h
  ${tarINC}/testWriteSupport.h
  ${tarINC}/testXML.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

