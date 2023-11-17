set (supportSources
    Binary.cxx ClebschGordan.cxx cycIterator.cxx 
    doubleErr.cxx Exception.cxx fileSupport.cxx 
    fortranWrite.cxx groupRange.cxx IndexCounter.cxx
    InputControl.cxx MapRange.cxx MapSupport.cxx
    mathSupport.cxx MatrixBase.cxx Matrix.cxx
    mcnpStringSupport.cxx polySupport.cxx regexBuild.cxx
    regexSupport.cxx splineSupport.cxx stringCombine.cxx
    support.cxx SVD.cxx writeSupport.cxx 
)

add_library (support SHARED
  ${supportSources}
)
target_include_directories (support PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../supportInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/Binary.cxx
  ${tarDIR}/ClebschGordan.cxx
  ${tarDIR}/cycIterator.cxx
  ${tarDIR}/doubleErr.cxx
  ${tarDIR}/Exception.cxx
  ${tarDIR}/fileSupport.cxx
  ${tarDIR}/fortranWrite.cxx
  ${tarDIR}/groupRange.cxx
  ${tarDIR}/InputControl.cxx
  ${tarDIR}/IndexCounter.cxx
  ${tarDIR}/MapRange.cxx
  ${tarDIR}/MapSupport.cxx
  ${tarDIR}/mathSupport.cxx
  ${tarDIR}/MatrixBase.cxx
  ${tarDIR}/Matrix.cxx
  ${tarDIR}/mcnpStringSupport.cxx
  ${tarDIR}/polySupport.cxx
  ${tarDIR}/regexBuild.cxx
  ${tarDIR}/regexSupport.cxx
  ${tarDIR}/splineSupport.cxx
  ${tarDIR}/stringCombine.cxx
  ${tarDIR}/support.cxx
  ${tarDIR}/SVD.cxx
  ${tarDIR}/Tensor.cxx
  ${tarDIR}/vectorSupport.cxx
  ${tarDIR}/writeSupport.cxx
  ${tarINC}/Binary.h
  ${tarINC}/ClebschGordan.h
  ${tarINC}/cycIterator.h
  ${tarINC}/doubleErr.h
  ${tarINC}/Exception.h
  ${tarINC}/fileSupport.h
  ${tarINC}/fortranWrite.h
  ${tarINC}/groupRange.h
  ${tarINC}/InputControl.h
  ${tarINC}/IndexCounter.h
  ${tarINC}/ManagedPtr.h
  ${tarINC}/MapRange.h
  ${tarINC}/MapSupport.h
  ${tarINC}/mathSupport.h
  ${tarINC}/MatrixBase.h
  ${tarINC}/Matrix.h
  ${tarINC}/mcnpStringSupport.h
  ${tarINC}/permSort.h
  ${tarINC}/polySupport.h
  ${tarINC}/Random.h
  ${tarINC}/regexBuild.h
  ${tarINC}/regexSupport.h
  ${tarINC}/Simpson.h
  ${tarINC}/splineSupport.h
  ${tarINC}/stringCombine.h
  ${tarINC}/stringSearch.h
  ${tarINC}/stringWrite.h
  ${tarINC}/support.h
  ${tarINC}/SVD.h
  ${tarINC}/Tensor.h
  ${tarINC}/TypeString.h
  ${tarINC}/vectorSupport.h
  ${tarINC}/writeSupport.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

