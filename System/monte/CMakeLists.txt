set (monteSources
    Acomp.cxx AcompExtra.cxx AcompTools.cxx 
    Algebra.cxx BnId.cxx DBMaterial.cxx 
    DBMaterialRegister.cxx DBModify.cxx Element.cxx 
    eTrack.cxx FEMdatabase.cxx HeadRule.cxx 
    Importance.cxx Intersection.cxx IsoTable.cxx 
    LayerDivide1D.cxx LayerDivide3D.cxx LineIntersectVisit.cxx 
    mapIterator.cxx Material.cxx MXcards.cxx 
    neutron.cxx Object.cxx objectSupport.cxx 
    particle.cxx photon.cxx RotCounter.cxx 
    RuleBinary.cxx RuleCheck.cxx RuleItems.cxx 
    Rules.cxx RuleSupport.cxx SurfPoint.cxx 
    SurfVertex.cxx Token.cxx Track.cxx 
    Union.cxx Zaid.cxx 
)

add_library (monte SHARED
  ${monteSources}
)
target_include_directories (monte PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../monteInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/Acomp.cxx
  ${tarDIR}/AcompExtra.cxx
  ${tarDIR}/AcompTools.cxx
  ${tarDIR}/Algebra.cxx
  ${tarDIR}/BnId.cxx
  ${tarDIR}/DBMaterial.cxx
  ${tarDIR}/DBMaterialRegister.cxx
  ${tarDIR}/DBModify.cxx
  ${tarDIR}/Element.cxx
  ${tarDIR}/eTrack.cxx
  ${tarDIR}/FEMdatabase.cxx
  ${tarDIR}/HeadRule.cxx
  ${tarDIR}/Importance.cxx
  ${tarDIR}/Intersection.cxx
  ${tarDIR}/IsoTable.cxx
  ${tarDIR}/LayerDivide1D.cxx
  ${tarDIR}/LayerDivide3D.cxx
  ${tarDIR}/LineIntersectVisit.cxx
  ${tarDIR}/mapIterator.cxx
  ${tarDIR}/Material.cxx
  ${tarDIR}/MXcards.cxx
  ${tarDIR}/neutron.cxx
  ${tarDIR}/Object.cxx
  ${tarDIR}/objectSupport.cxx
  ${tarDIR}/particle.cxx
  ${tarDIR}/photon.cxx
  ${tarDIR}/RotCounter.cxx
  ${tarDIR}/RuleBinary.cxx
  ${tarDIR}/RuleCheck.cxx
  ${tarDIR}/RuleItems.cxx
  ${tarDIR}/Rules.cxx
  ${tarDIR}/RuleSupport.cxx
  ${tarDIR}/SurfPoint.cxx
  ${tarDIR}/SurfVertex.cxx
  ${tarDIR}/Token.cxx
  ${tarDIR}/Track.cxx
  ${tarDIR}/Union.cxx
  ${tarDIR}/Zaid.cxx
  ${tarINC}/Acomp.h
  ${tarINC}/AcompTools.h
  ${tarINC}/Algebra.h
  ${tarINC}/BnId.h
  ${tarINC}/DBMaterial.h
  ${tarINC}/DBModify.h
  ${tarINC}/Element.h
  ${tarINC}/eTrack.h
  ${tarINC}/FEMdatabase.h
  ${tarINC}/HeadRule.h
  ${tarINC}/Importance.h
  ${tarINC}/IsoTable.h
  ${tarINC}/LayerDivide1D.h
  ${tarINC}/LayerDivide3D.h
  ${tarINC}/LineIntersectVisit.h
  ${tarINC}/mapIterator.h
  ${tarINC}/Material.h
  ${tarINC}/MXcards.h
  ${tarINC}/neutron.h
  ${tarINC}/Object.h
  ${tarINC}/objectSupport.h
  ${tarINC}/particle.h
  ${tarINC}/photon.h
  ${tarINC}/RefCon.h
  ${tarINC}/RotCounter.h
  ${tarINC}/RuleBinary.h
  ${tarINC}/RuleCheck.h
  ${tarINC}/Rules.h
  ${tarINC}/RuleSupport.h
  ${tarINC}/SurfVertex.h
  ${tarINC}/Token.h
  ${tarINC}/Track.h
  ${tarINC}/Zaid.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

