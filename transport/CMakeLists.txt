set (transportSources
    AreaBeam.cxx BandDetector.cxx countUnit.cxx 
    Detector.cxx DetGroup.cxx ParticleInObj.cxx 
    photonInObj.cxx PointDetector.cxx VolumeBeam.cxx 
)

add_library (transport SHARED
  ${transportSources}
)
target_include_directories (transport PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../transportInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/AreaBeam.cxx
  ${tarDIR}/BandDetector.cxx
  ${tarDIR}/countUnit.cxx
  ${tarDIR}/Detector.cxx
  ${tarDIR}/DetGroup.cxx
  ${tarDIR}/ParticleInObj.cxx
  ${tarDIR}/photonInObj.cxx
  ${tarDIR}/PointDetector.cxx
  ${tarDIR}/VolumeBeam.cxx
  ${tarINC}/AreaBeam.h
  ${tarINC}/BandDetector.h
  ${tarINC}/Beam.h
  ${tarINC}/ChannelDet.h
  ${tarINC}/countUnit.h
  ${tarINC}/Detector.h
  ${tarINC}/DetGroup.h
  ${tarINC}/ParticleInObj.h
  ${tarINC}/photonInObj.h
  ${tarINC}/PointDetector.h
  ${tarINC}/VolumeBeam.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

