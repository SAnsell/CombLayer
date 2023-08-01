set(geomSources
  Quaternion.cxx
  Vec3D.cxx
)

#=====================================
# Target
#=====================================
add_library (geometry SHARED
  ${geomSources}
)

target_include_directories(geometry
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../geomInc
)


file(RELATIVE_PATH tarDIR 
   "${CMAKE_BINARY_DIR}"
   "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
   "${CMAKE_BINARY_DIR}"
   "${CMAKE_CURRENT_SOURCE_DIR}/../geomInc")
   
set(SRC_LIST ${SRC_LIST}
  ${tarDIR}/Quaternion.cxx
  ${tarDIR}/Vec3D.cxx
  ${tarINC}/Quaternion.h
  ${tarINC}/Vec3D.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)	
