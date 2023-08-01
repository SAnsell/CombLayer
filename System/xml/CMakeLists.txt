set (xmlSources
    XMLattribute.cxx XMLcollect.cxx XMLcomment.cxx 
    XMLcomp.cxx XMLdatablock.cxx XMLgrid.cxx 
    XMLgridSupport.cxx XMLgroup.cxx XMLiterator.cxx 
    XMLload.cxx XMLmap.cxx XMLnamespace.cxx 
    XMLobject.cxx XMLread.cxx XMLvector.cxx 
)

add_library (xml SHARED
  ${xmlSources}
)
target_include_directories (xml PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../xmlInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/XMLattribute.cxx
  ${tarDIR}/XMLcollect.cxx
  ${tarDIR}/XMLcomment.cxx
  ${tarDIR}/XMLcomp.cxx
  ${tarDIR}/XMLdatablock.cxx
  ${tarDIR}/XMLgrid.cxx
  ${tarDIR}/XMLgridSupport.cxx
  ${tarDIR}/XMLgroup.cxx
  ${tarDIR}/XMLiterator.cxx
  ${tarDIR}/XMLload.cxx
  ${tarDIR}/XMLmap.cxx
  ${tarDIR}/XMLnamespace.cxx
  ${tarDIR}/XMLobject.cxx
  ${tarDIR}/XMLread.cxx
  ${tarDIR}/XMLvector.cxx
  ${tarINC}/XMLattribute.h
  ${tarINC}/XMLcollect.h
  ${tarINC}/XMLcomment.h
  ${tarINC}/XMLcomp.h
  ${tarINC}/XMLdatablock.h
  ${tarINC}/XMLgrid.h
  ${tarINC}/XMLgridSupport.h
  ${tarINC}/XMLgroup.h
  ${tarINC}/XMLiterator.h
  ${tarINC}/XMLload.h
  ${tarINC}/XMLmap.h
  ${tarINC}/XMLnamespace.h
  ${tarINC}/XMLobject.h
  ${tarINC}/XMLread.h
  ${tarINC}/XMLvector.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

