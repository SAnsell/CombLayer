set (phitsTallySources
    phitsTallyBuilder.cxx phitsTally.cxx phitsTallyModification.cxx 
    phitsTallySelector.cxx T3DShow.cxx tcrossConstruct.cxx 
    TCross.cxx tgshowConstruct.cxx TGShow.cxx 
    TMesh.cxx ttrackMeshConstruct.cxx 
)

add_library (phitsTally SHARED
  ${phitsTallySources}
)
target_include_directories (phitsTally PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../phitsTallyInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/phitsTallyBuilder.cxx
  ${tarDIR}/phitsTally.cxx
  ${tarDIR}/phitsTallyModification.cxx
  ${tarDIR}/phitsTallySelector.cxx
  ${tarDIR}/T3DShow.cxx
  ${tarDIR}/tcrossConstruct.cxx
  ${tarDIR}/TCross.cxx
  ${tarDIR}/tgshowConstruct.cxx
  ${tarDIR}/TGShow.cxx
  ${tarDIR}/TMesh.cxx
  ${tarDIR}/ttrackMeshConstruct.cxx
  ${tarINC}/phitsTallyBuilder.h
  ${tarINC}/phitsTally.h
  ${tarINC}/phitsTallyModification.h
  ${tarINC}/phitsTallySelector.h
  ${tarINC}/T3DShow.h
  ${tarINC}/tcrossConstruct.h
  ${tarINC}/TCross.h
  ${tarINC}/tgshowConstruct.h
  ${tarINC}/TGShow.h
  ${tarINC}/TMesh.h
  ${tarINC}/ttrackMeshConstruct.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

