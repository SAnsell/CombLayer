set (attachCompSources
    AttachSupport.cxx AttachSupportLine.cxx BaseMap.cxx 
    BlockZone.cxx CellMap.cxx ContainedComp.cxx 
    ContainedGroup.cxx CopiedComp.cxx ExternalCut.cxx 
    FixedComp.cxx FixedCompSplitCell.cxx FixedGroup.cxx 
    FixedOffset.cxx FixedOffsetGroup.cxx FixedRotate.cxx 
    FixedRotateGroup.cxx FrontBackCut.cxx InsertComp.cxx 
    LayerComp.cxx LinearComp.cxx LinkSupport.cxx 
    LinkUnit.cxx PointMap.cxx PositionSupport.cxx 
    SpaceCut.cxx SurfMap.cxx WrapperCell.cxx 
)

add_library (attachComp SHARED
  ${attachCompSources}
)
target_include_directories (attachComp PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../attachCompInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/AttachSupport.cxx
  ${tarDIR}/AttachSupportLine.cxx
  ${tarDIR}/BaseMap.cxx
  ${tarDIR}/BlockZone.cxx
  ${tarDIR}/CellMap.cxx
  ${tarDIR}/ContainedComp.cxx
  ${tarDIR}/ContainedGroup.cxx
  ${tarDIR}/CopiedComp.cxx
  ${tarDIR}/ExternalCut.cxx
  ${tarDIR}/FixedComp.cxx
  ${tarDIR}/FixedCompSplitCell.cxx
  ${tarDIR}/FixedGroup.cxx
  ${tarDIR}/FixedOffset.cxx
  ${tarDIR}/FixedOffsetGroup.cxx
  ${tarDIR}/FixedRotate.cxx
  ${tarDIR}/FixedRotateGroup.cxx
  ${tarDIR}/FrontBackCut.cxx
  ${tarDIR}/InsertComp.cxx
  ${tarDIR}/LayerComp.cxx
  ${tarDIR}/LinearComp.cxx
  ${tarDIR}/LinkSupport.cxx
  ${tarDIR}/LinkUnit.cxx
  ${tarDIR}/PointMap.cxx
  ${tarDIR}/PositionSupport.cxx
  ${tarDIR}/SpaceCut.cxx
  ${tarDIR}/SurfMap.cxx
  ${tarDIR}/WrapperCell.cxx
  ${tarINC}/AttachSupport.h
  ${tarINC}/BaseMap.h
  ${tarINC}/BlockZone.h
  ${tarINC}/CellMap.h
  ${tarINC}/ContainedComp.h
  ${tarINC}/ContainedGroup.h
  ${tarINC}/CopiedComp.h
  ${tarINC}/ExternalCut.h
  ${tarINC}/FixedComp.h
  ${tarINC}/FixedGroup.h
  ${tarINC}/FixedOffsetGroup.h
  ${tarINC}/FixedOffset.h
  ${tarINC}/FixedOffsetUnit.h
  ${tarINC}/FixedRotateGroup.h
  ${tarINC}/FixedRotate.h
  ${tarINC}/FixedRotateUnit.h
  ${tarINC}/FixedUnit.h
  ${tarINC}/FrontBackCut.h
  ${tarINC}/InsertComp.h
  ${tarINC}/LayerComp.h
  ${tarINC}/LinearComp.h
  ${tarINC}/LinkSupport.h
  ${tarINC}/LinkUnit.h
  ${tarINC}/PointMap.h
  ${tarINC}/PositionSupport.h
  ${tarINC}/SpaceCut.h
  ${tarINC}/SurfMap.h
  ${tarINC}/WrapperCell.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

