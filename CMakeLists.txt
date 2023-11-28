cmake_minimum_required(VERSION 3.9)
project(CombLayer VERSION 1.0.0)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
  "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(GSL REQUIRED)
find_package (fmt REQUIRED)
# find_package(Boost COMPONENTS system filesystem REQUIRED)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "-Wconversion -W -Wall -Wextra -Wno-comment -fexceptions -std=c++20 -O2")

set(SYSTEM_INCLUDE
  ${CMAKE_SOURCE_DIR}/System/geomInc
  ${CMAKE_SOURCE_DIR}/globalInc
)

set(SYSTEM_INCLUDE 
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/System/attachCompInc
  ${CMAKE_SOURCE_DIR}/System/compWeightsInc
  ${CMAKE_SOURCE_DIR}/System/constructInc
  ${CMAKE_SOURCE_DIR}/System/constructVarInc
  ${CMAKE_SOURCE_DIR}/System/crystalInc
  ${CMAKE_SOURCE_DIR}/System/endfInc
  ${CMAKE_SOURCE_DIR}/System/flukaPhysicsInc
  ${CMAKE_SOURCE_DIR}/System/flukaProcessInc
  ${CMAKE_SOURCE_DIR}/System/flukaTallyInc
  ${CMAKE_SOURCE_DIR}/System/funcBaseInc
  ${CMAKE_SOURCE_DIR}/System/generalProcessInc
  ${CMAKE_SOURCE_DIR}/System/geomInc
  ${CMAKE_SOURCE_DIR}/System/inputInc
  ${CMAKE_SOURCE_DIR}/System/insertUnitInc
  ${CMAKE_SOURCE_DIR}/System/logInc
  ${CMAKE_SOURCE_DIR}/System/magneticInc
  ${CMAKE_SOURCE_DIR}/System/mcnpProcessInc
  ${CMAKE_SOURCE_DIR}/System/md5Inc
  ${CMAKE_SOURCE_DIR}/System/modelSupportInc
  ${CMAKE_SOURCE_DIR}/System/monteInc
  ${CMAKE_SOURCE_DIR}/System/objectModInc
  ${CMAKE_SOURCE_DIR}/System/phitsPhysicsInc
  ${CMAKE_SOURCE_DIR}/System/phitsProcessInc
  ${CMAKE_SOURCE_DIR}/System/phitsSupportInc
  ${CMAKE_SOURCE_DIR}/System/phitsTallyInc
  ${CMAKE_SOURCE_DIR}/System/physicsInc
  ${CMAKE_SOURCE_DIR}/System/polyInc
  ${CMAKE_SOURCE_DIR}/System/simMCInc
  ${CMAKE_SOURCE_DIR}/System/sourceInc
  ${CMAKE_SOURCE_DIR}/System/supportInc
  ${CMAKE_SOURCE_DIR}/System/tallyInc
  ${CMAKE_SOURCE_DIR}/System/visitInc
  ${CMAKE_SOURCE_DIR}/System/weightsInc
  ${CMAKE_SOURCE_DIR}/System/workInc
  ${CMAKE_SOURCE_DIR}/System/worldInc
  ${CMAKE_SOURCE_DIR}/System/xmlInc
)

set(GENERAL_INCLUDE
  ${CMAKE_SOURCE_DIR}/beamlineInc
  ${CMAKE_SOURCE_DIR}/globalInc
  ${CMAKE_SOURCE_DIR}/instrumentInc
  ${CMAKE_SOURCE_DIR}/scatMatInc
  ${CMAKE_SOURCE_DIR}/transportInc
  )

set(MODEL_DIR ${CMAKE_SOURCE_DIR}/Model)


set(ESS_DIR ${CMAKE_SOURCE_DIR}/Model/ESSBeam)
set(MAXIV_DIR ${CMAKE_SOURCE_DIR}/Model/MaxIV)

set(MAXIV_INCLUDE
  ${MODEL_DIR}/maxivBuildInc
  ${MAXIV_DIR}/balderInc 
  ${MAXIV_DIR}/LinacInc
  ${MAXIV_DIR}/commonBeamInc
  ${MAXIV_DIR}/maxpeemInc
  ${MAXIV_DIR}/commonGeneratorInc
  ${MAXIV_DIR}/micromaxInc
  ${MAXIV_DIR}/cosaxsInc
  ${MAXIV_DIR}/R1CommonInc
  ${MAXIV_DIR}/danmaxInc
  ${MAXIV_DIR}/R3CommonInc
  ${MAXIV_DIR}/flexpesInc
  ${MAXIV_DIR}/softimaxInc
  ${MAXIV_DIR}/formaxInc
  ${MAXIV_DIR}/speciesInc
)
  
set(ESS_INCLUDE

  ${MODEL_DIR}/essBuildInc
  ${MODEL_DIR}/essConstructInc
  ${MODEL_DIR}/essLinacInc    
  ${ESS_DIR}/beerInc
  ${ESS_DIR}/bifrostInc
  ${ESS_DIR}/commonInc
  ${ESS_DIR}/commonVarInc
  ${ESS_DIR}/cspecInc
  ${ESS_DIR}/dreamInc
  ${ESS_DIR}/estiaInc
  ${ESS_DIR}/freiaInc
  ${ESS_DIR}/heimdalInc
  ${ESS_DIR}/lokiInc
  ${ESS_DIR}/magicInc
  ${ESS_DIR}/miraclesInc
  ${ESS_DIR}/nmxInc
  ${ESS_DIR}/nnbarInc 
  ${ESS_DIR}/odinInc
  ${ESS_DIR}/simpleItemInc
  ${ESS_DIR}/skadiInc
  ${ESS_DIR}/testBeamInc
  ${ESS_DIR}/trexInc
  ${ESS_DIR}/vespaInc
  ${ESS_DIR}/vorInc
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(src)
add_subdirectory(System)
add_subdirectory(Model)
add_subdirectory(beamline)
add_subdirectory(global)
add_subdirectory(instrument)
add_subdirectory(scatMat)
add_subdirectory(transport)
add_subdirectory(Main)
add_subdirectory(test)

add_executable (ess
  Main/ess.cxx
)

add_executable (fullBuild
  Main/fullBuild.cxx
)

add_executable (maxiv
  Main/maxiv.cxx
)

add_executable(singleItem
  Main/singleItem.cxx
)

add_executable(essLinacModel
  Main/essLinacModel.cxx
)

add_executable(reactor
  Main/reactor.cxx
)

add_executable(saxsSim
  Main/saxsSim.cxx
)

add_executable(t1Real
  Main/t1Real.cxx
)

add_executable(xrayHut
  Main/xrayHut.cxx
)

add_executable(testMain
  Main/testMain.cxx
)

set(SYSTEM_LIB
  src beamline global scatMat transport
  attachComp compWeights construct constructVar
  crystal endf flukaPhysics flukaProcess flukaTally
  funcBase generalProcess geometry input insertUnit
  instrument
  log magnetic mcnpProcess md5 modelSupport monte
  objectMod phitsPhysics phitsProcess phitsSupport
  phitsTally physics poly simMC source support
  tally visit weights work world xml
)

set(ESS_LIB
  essBuild essConstruct
  beer bifrost common commonVar cspec dream 
  estia freia heimdal loki magic miracles nmx nnbar 
  odin simpleItem skadi testBeam trex vespa vor  
)

set(MAXIV_LIB
  maxivBuild balder commonBeam commonGenerator cosaxs
  danmax flexpes formax Linac maxpeem micromax
  R1Common R3Common softimax species
)

target_link_libraries(ess
  ${SYSTEM_LIB}
  ${ESS_LIB}
  GSL::gsl
  fmt::fmt
)

target_link_libraries(essLinacModel
  ${SYSTEM_LIB}
  ${ESS_LIB}
  essLinac
  GSL::gsl
  fmt::fmt
)

target_link_libraries (fullBuild
  ${SYSTEM_LIB}
  ralBuild t1Build t2Build
  GSL::gsl
  fmt::fmt
)

target_link_libraries (maxiv
  ${SYSTEM_LIB}
  ${MAXIV_LIB}
  GSL::gsl
  fmt::fmt
)

target_link_libraries(reactor
  ${SYSTEM_LIB}
  delft
  GSL::gsl
  fmt::fmt
)

target_link_libraries(saxsSim
  ${SYSTEM_LIB}
  saxs
  GSL::gsl
  fmt::fmt
)


target_link_libraries (singleItem
  ${SYSTEM_LIB}
  ${MAXIV_LIB}
  singleItemBuild
  instrument essBuild essConstruct
  beer bifrost common commonVar cspec dream 
  estia freia heimdal loki magic miracles nmx nnbar 
  odin simpleItem skadi testBeam trex vespa vor  
  GSL::gsl
  fmt::fmt
)

target_link_libraries(t1Real
  ${SYSTEM_LIB}
  ralBuild t1Build t2Build
  GSL::gsl
  fmt::fmt
)


target_link_libraries(xrayHut
  ${SYSTEM_LIB}
  xrayHutch
  GSL::gsl
  fmt::fmt
)

target_link_libraries(testMain
  ${SYSTEM_LIB}
  test
  GSL::gsl
  fmt::fmt
)

target_include_directories (maxiv
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories (singleItem
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories (testMain
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  testInclude
)

target_include_directories (ess
  PUBLIC
  ${SYSTEM_INCLUDE}
  ${GENERAL_INCLUDE}
  ${ESS_INCLUDE}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories (maxiv
  PUBLIC
  Model/maxivBuildInc
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories (reactor
  PUBLIC
  Model/delftInc
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories (saxsSim
  PUBLIC
  Model/saxsInc
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories (t1Real
  PUBLIC
  Model/ralBuildInc
  Model/t1BuildInc
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories (xrayHut
  PUBLIC
  Model/xrayHutchInc
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(SRC_LIST ${SRC_LIST} CMakeLists.txt
   model.sh CMakeIndividual.pl)

set(tarDIR
   "${CMAKE_BINARY_DIR}")

 
#file(GLOB_RECURSE zlibTarSrc "./zlib/*.cpp"
#                "zlib/*.c")
#file(GLOB_RECURSE zlibTarInc 
#                "./zlib/*.h")

#set(SRC_LIST ${SRC_LIST}
#   ${zlibTarSrc}
#  ${tarDIR}/CMakeLists.txt)	


add_custom_target(tar COMMAND tar -zcvf CombLayer.tgz 
  ${SRC_LIST})

add_custom_target(tags COMMAND etags ${SRC_LIST})

add_custom_target(words COMMAND 
    grep -v -e "^[[:space:][:cntrl:]]*$$" ${SRC_LIST} | wc)

  


  
