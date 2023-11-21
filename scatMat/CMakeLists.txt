set (scatMatSources
    CryMat.cxx DBNeutMaterial.cxx GlassMaterial.cxx 
    neutMaterial.cxx photonMaterial.cxx SQWmaterial.cxx 
)

add_library (scatMat SHARED
  ${scatMatSources}
)
target_include_directories (scatMat PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../scatMatInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/CryMat.cxx
  ${tarDIR}/DBNeutMaterial.cxx
  ${tarDIR}/GlassMaterial.cxx
  ${tarDIR}/neutMaterial.cxx
  ${tarDIR}/photonMaterial.cxx
  ${tarDIR}/SQWmaterial.cxx
  ${tarINC}/CryMat.h
  ${tarINC}/DBNeutMaterial.h
  ${tarINC}/GlassMaterial.h
  ${tarINC}/neutMaterial.h
  ${tarINC}/photonMaterial.h
  ${tarINC}/SQWmaterial.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

