
set (MainSources
  essBeamline.cxx ess.cxx essLinac.cxx
  fullBuild.cxx maxiv.cxx pipe.cxx
  reactor.cxx saxs.cxx singleItem.cxx
  t1Real.cxx testMain.cxx xrayHut.cxx
)


file(RELATIVE_PATH tarDIR
   "${CMAKE_BINARY_DIR}"
   "${CMAKE_CURRENT_SOURCE_DIR}")

set(SRC_LIST
  ${SRC_LIST}
  ${tarDIR}/essBeamline.cxx ${tarDIR}/ess.cxx ${tarDIR}/essLinacModel.cxx
  ${tarDIR}/fullBuild.cxx ${tarDIR}/maxiv.cxx ${tarDIR}/pipe.cxx
  ${tarDIR}/reactor.cxx ${tarDIR}/saxsSim.cxx ${tarDIR}/singleItem.cxx
  ${tarDIR}/t1Real.cxx ${tarDIR}/testMain.cxx ${tarDIR}/xrayHut.cxx
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

