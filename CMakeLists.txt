project(CombLayer)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-fPIC -Wconversion -W -Wall -Wextra -Wno-comment -fexceptions -std=c++17 -O2 ")
set(CMAKE_CXX_RELEASE_FLAGS "-fPIC -Wconversion -W -Wall -Wextra -Wno-comment -fexceptions -std=c++17 -O2 ")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ./lib)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -undefined dynamic_lookup")
endif()
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/beamlineInc")
include_directories("${PROJECT_SOURCE_DIR}/globalInc")
include_directories("${PROJECT_SOURCE_DIR}/instrumentInc")
include_directories("${PROJECT_SOURCE_DIR}/scatMatInc")
include_directories("${PROJECT_SOURCE_DIR}/specialInc")
include_directories("${PROJECT_SOURCE_DIR}/transportInc")
include_directories("${PROJECT_SOURCE_DIR}/testInclude")
include_directories("${PROJECT_SOURCE_DIR}/System/attachCompInc")
include_directories("${PROJECT_SOURCE_DIR}/System/compWeightsInc")
include_directories("${PROJECT_SOURCE_DIR}/System/constructInc")
include_directories("${PROJECT_SOURCE_DIR}/System/constructVarInc")
include_directories("${PROJECT_SOURCE_DIR}/System/crystalInc")
include_directories("${PROJECT_SOURCE_DIR}/System/endfInc")
include_directories("${PROJECT_SOURCE_DIR}/System/flukaPhysicsInc")
include_directories("${PROJECT_SOURCE_DIR}/System/flukaProcessInc")
include_directories("${PROJECT_SOURCE_DIR}/System/flukaTallyInc")
include_directories("${PROJECT_SOURCE_DIR}/System/funcBaseInc")
include_directories("${PROJECT_SOURCE_DIR}/System/generalProcessInc")
include_directories("${PROJECT_SOURCE_DIR}/System/geomInc")
include_directories("${PROJECT_SOURCE_DIR}/System/inputInc")
include_directories("${PROJECT_SOURCE_DIR}/System/insertUnitInc")
include_directories("${PROJECT_SOURCE_DIR}/System/logInc")
include_directories("${PROJECT_SOURCE_DIR}/System/magneticInc")
include_directories("${PROJECT_SOURCE_DIR}/System/mcnpProcessInc")
include_directories("${PROJECT_SOURCE_DIR}/System/md5Inc")
include_directories("${PROJECT_SOURCE_DIR}/System/modelSupportInc")
include_directories("${PROJECT_SOURCE_DIR}/System/monteInc")
include_directories("${PROJECT_SOURCE_DIR}/System/objectModInc")
include_directories("${PROJECT_SOURCE_DIR}/System/phitsPhysicsInc")
include_directories("${PROJECT_SOURCE_DIR}/System/phitsProcessInc")
include_directories("${PROJECT_SOURCE_DIR}/System/phitsSupportInc")
include_directories("${PROJECT_SOURCE_DIR}/System/phitsTallyInc")
include_directories("${PROJECT_SOURCE_DIR}/System/physicsInc")
include_directories("${PROJECT_SOURCE_DIR}/System/polyInc")
include_directories("${PROJECT_SOURCE_DIR}/System/simMCInc")
include_directories("${PROJECT_SOURCE_DIR}/System/sourceInc")
include_directories("${PROJECT_SOURCE_DIR}/System/supportInc")
include_directories("${PROJECT_SOURCE_DIR}/System/tallyInc")
include_directories("${PROJECT_SOURCE_DIR}/System/visitInc")
include_directories("${PROJECT_SOURCE_DIR}/System/weightsInc")
include_directories("${PROJECT_SOURCE_DIR}/System/workInc")
include_directories("${PROJECT_SOURCE_DIR}/System/worldInc")
include_directories("${PROJECT_SOURCE_DIR}/System/xmlInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/bibBuildInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/cuBlockInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/d4cModelInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/delftInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/epbBuildInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/essBuildInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/essConstructInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/essLinacInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/exampleBuildInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/filterInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/gammaBuildInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/lensModelInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/maxivBuildInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/moderatorInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/muonInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/photonInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/pipeBuildInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ralBuildInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/saxsInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/sinbadBuildInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/singleItemBuildInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/snsBuildInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/t1BuildInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/t2BuildInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/xrayHutchInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/beerInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/bifrostInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/commonInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/commonVarInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/cspecInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/dreamInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/estiaInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/freiaInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/heimdalInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/lokiInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/magicInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/miraclesInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/nmxInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/nnbarInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/odinInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/simpleItemInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/skadiInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/testBeamInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/trexInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/vespaInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/ESSBeam/vorInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/MaxIV/balderInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/MaxIV/commonBeamInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/MaxIV/commonGeneratorInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/MaxIV/cosaxsInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/MaxIV/danmaxInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/MaxIV/flexpesInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/MaxIV/formaxInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/MaxIV/LinacInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/MaxIV/maxpeemInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/MaxIV/micromaxInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/MaxIV/R1CommonInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/MaxIV/R3CommonInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/MaxIV/softimaxInc")
include_directories("${PROJECT_SOURCE_DIR}/Model/MaxIV/speciesInc")
## END INCLUDES 

## GLOBS 
file(GLOB flukaPhysics "${PROJECT_SOURCE_DIR}/System/flukaPhysics/*.cxx")
add_library(libflukaPhysics SHARED ${flukaPhysics})
file(GLOB md5 "${PROJECT_SOURCE_DIR}/System/md5/*.cxx")
add_library(libmd5 SHARED ${md5})
file(GLOB source "${PROJECT_SOURCE_DIR}/System/source/*.cxx")
add_library(libsource SHARED ${source})
file(GLOB vespa "${PROJECT_SOURCE_DIR}/Model/ESSBeam/vespa/*.cxx")
add_library(libvespa SHARED ${vespa})
file(GLOB micromax "${PROJECT_SOURCE_DIR}/Model/MaxIV/micromax/*.cxx")
add_library(libmicromax SHARED ${micromax})
file(GLOB physics "${PROJECT_SOURCE_DIR}/System/physics/*.cxx")
add_library(libphysics SHARED ${physics})
file(GLOB testBeam "${PROJECT_SOURCE_DIR}/Model/ESSBeam/testBeam/*.cxx")
add_library(libtestBeam SHARED ${testBeam})
file(GLOB input "${PROJECT_SOURCE_DIR}/System/input/*.cxx")
add_library(libinput SHARED ${input})
file(GLOB geometry "${PROJECT_SOURCE_DIR}/System/geometry/*.cxx")
add_library(libgeometry SHARED ${geometry})
file(GLOB phitsTally "${PROJECT_SOURCE_DIR}/System/phitsTally/*.cxx")
add_library(libphitsTally SHARED ${phitsTally})
file(GLOB attachComp "${PROJECT_SOURCE_DIR}/System/attachComp/*.cxx")
add_library(libattachComp SHARED ${attachComp})
file(GLOB balder "${PROJECT_SOURCE_DIR}/Model/MaxIV/balder/*.cxx")
add_library(libbalder SHARED ${balder})
file(GLOB cosaxs "${PROJECT_SOURCE_DIR}/Model/MaxIV/cosaxs/*.cxx")
add_library(libcosaxs SHARED ${cosaxs})
file(GLOB dream "${PROJECT_SOURCE_DIR}/Model/ESSBeam/dream/*.cxx")
add_library(libdream SHARED ${dream})
file(GLOB essBuild "${PROJECT_SOURCE_DIR}/Model/essBuild/*.cxx")
add_library(libessBuild SHARED ${essBuild})
file(GLOB crystal "${PROJECT_SOURCE_DIR}/System/crystal/*.cxx")
add_library(libcrystal SHARED ${crystal})
file(GLOB gammaBuild "${PROJECT_SOURCE_DIR}/Model/gammaBuild/*.cxx")
add_library(libgammaBuild SHARED ${gammaBuild})
file(GLOB magic "${PROJECT_SOURCE_DIR}/Model/ESSBeam/magic/*.cxx")
add_library(libmagic SHARED ${magic})
file(GLOB compWeights "${PROJECT_SOURCE_DIR}/System/compWeights/*.cxx")
add_library(libcompWeights SHARED ${compWeights})
file(GLOB delft "${PROJECT_SOURCE_DIR}/Model/delft/*.cxx")
add_library(libdelft SHARED ${delft})
file(GLOB endf "${PROJECT_SOURCE_DIR}/System/endf/*.cxx")
add_library(libendf SHARED ${endf})
file(GLOB insertUnit "${PROJECT_SOURCE_DIR}/System/insertUnit/*.cxx")
add_library(libinsertUnit SHARED ${insertUnit})
file(GLOB transport "${PROJECT_SOURCE_DIR}/transport/*.cxx")
add_library(libtransport SHARED ${transport})
file(GLOB flexpes "${PROJECT_SOURCE_DIR}/Model/MaxIV/flexpes/*.cxx")
add_library(libflexpes SHARED ${flexpes})
file(GLOB exampleBuild "${PROJECT_SOURCE_DIR}/Model/exampleBuild/*.cxx")
add_library(libexampleBuild SHARED ${exampleBuild})
file(GLOB monte "${PROJECT_SOURCE_DIR}/System/monte/*.cxx")
add_library(libmonte SHARED ${monte})
file(GLOB phitsProcess "${PROJECT_SOURCE_DIR}/System/phitsProcess/*.cxx")
add_library(libphitsProcess SHARED ${phitsProcess})
file(GLOB R3Common "${PROJECT_SOURCE_DIR}/Model/MaxIV/R3Common/*.cxx")
add_library(libR3Common SHARED ${R3Common})
file(GLOB filter "${PROJECT_SOURCE_DIR}/Model/filter/*.cxx")
add_library(libfilter SHARED ${filter})
file(GLOB Linac "${PROJECT_SOURCE_DIR}/Model/MaxIV/Linac/*.cxx")
add_library(libLinac SHARED ${Linac})
file(GLOB epbBuild "${PROJECT_SOURCE_DIR}/Model/epbBuild/*.cxx")
add_library(libepbBuild SHARED ${epbBuild})
file(GLOB vor "${PROJECT_SOURCE_DIR}/Model/ESSBeam/vor/*.cxx")
add_library(libvor SHARED ${vor})
file(GLOB snsBuild "${PROJECT_SOURCE_DIR}/Model/snsBuild/*.cxx")
add_library(libsnsBuild SHARED ${snsBuild})
file(GLOB beer "${PROJECT_SOURCE_DIR}/Model/ESSBeam/beer/*.cxx")
add_library(libbeer SHARED ${beer})
file(GLOB scatMat "${PROJECT_SOURCE_DIR}/scatMat/*.cxx")
add_library(libscatMat SHARED ${scatMat})
file(GLOB miracles "${PROJECT_SOURCE_DIR}/Model/ESSBeam/miracles/*.cxx")
add_library(libmiracles SHARED ${miracles})
file(GLOB log "${PROJECT_SOURCE_DIR}/System/log/*.cxx")
add_library(liblog SHARED ${log})
file(GLOB danmax "${PROJECT_SOURCE_DIR}/Model/MaxIV/danmax/*.cxx")
add_library(libdanmax SHARED ${danmax})
file(GLOB saxs "${PROJECT_SOURCE_DIR}/Model/saxs/*.cxx")
add_library(libsaxs SHARED ${saxs})
file(GLOB flukaProcess "${PROJECT_SOURCE_DIR}/System/flukaProcess/*.cxx")
add_library(libflukaProcess SHARED ${flukaProcess})
file(GLOB phitsPhysics "${PROJECT_SOURCE_DIR}/System/phitsPhysics/*.cxx")
add_library(libphitsPhysics SHARED ${phitsPhysics})
file(GLOB generalProcess "${PROJECT_SOURCE_DIR}/System/generalProcess/*.cxx")
add_library(libgeneralProcess SHARED ${generalProcess})
file(GLOB modelSupport "${PROJECT_SOURCE_DIR}/System/modelSupport/*.cxx")
add_library(libmodelSupport SHARED ${modelSupport})
file(GLOB estia "${PROJECT_SOURCE_DIR}/Model/ESSBeam/estia/*.cxx")
add_library(libestia SHARED ${estia})
file(GLOB cspec "${PROJECT_SOURCE_DIR}/Model/ESSBeam/cspec/*.cxx")
add_library(libcspec SHARED ${cspec})
file(GLOB constructVar "${PROJECT_SOURCE_DIR}/System/constructVar/*.cxx")
add_library(libconstructVar SHARED ${constructVar})
file(GLOB R1Common "${PROJECT_SOURCE_DIR}/Model/MaxIV/R1Common/*.cxx")
add_library(libR1Common SHARED ${R1Common})
file(GLOB xrayHutch "${PROJECT_SOURCE_DIR}/Model/xrayHutch/*.cxx")
add_library(libxrayHutch SHARED ${xrayHutch})
file(GLOB weights "${PROJECT_SOURCE_DIR}/System/weights/*.cxx")
add_library(libweights SHARED ${weights})
file(GLOB flukaTally "${PROJECT_SOURCE_DIR}/System/flukaTally/*.cxx")
add_library(libflukaTally SHARED ${flukaTally})
file(GLOB commonVar "${PROJECT_SOURCE_DIR}/Model/ESSBeam/commonVar/*.cxx")
add_library(libcommonVar SHARED ${commonVar})
file(GLOB species "${PROJECT_SOURCE_DIR}/Model/MaxIV/species/*.cxx")
add_library(libspecies SHARED ${species})
file(GLOB essConstruct "${PROJECT_SOURCE_DIR}/Model/essConstruct/*.cxx")
add_library(libessConstruct SHARED ${essConstruct})
file(GLOB odin "${PROJECT_SOURCE_DIR}/Model/ESSBeam/odin/*.cxx")
add_library(libodin SHARED ${odin})
file(GLOB loki "${PROJECT_SOURCE_DIR}/Model/ESSBeam/loki/*.cxx")
add_library(libloki SHARED ${loki})
file(GLOB trex "${PROJECT_SOURCE_DIR}/Model/ESSBeam/trex/*.cxx")
add_library(libtrex SHARED ${trex})
file(GLOB tally "${PROJECT_SOURCE_DIR}/System/tally/*.cxx")
add_library(libtally SHARED ${tally})
file(GLOB t1Build "${PROJECT_SOURCE_DIR}/Model/t1Build/*.cxx")
add_library(libt1Build SHARED ${t1Build})
file(GLOB xml "${PROJECT_SOURCE_DIR}/System/xml/*.cxx")
add_library(libxml SHARED ${xml})
file(GLOB funcBase "${PROJECT_SOURCE_DIR}/System/funcBase/*.cxx")
add_library(libfuncBase SHARED ${funcBase})
file(GLOB instrument "${PROJECT_SOURCE_DIR}/instrument/*.cxx")
add_library(libinstrument SHARED ${instrument})
file(GLOB ralBuild "${PROJECT_SOURCE_DIR}/Model/ralBuild/*.cxx")
add_library(libralBuild SHARED ${ralBuild})
file(GLOB cuBlock "${PROJECT_SOURCE_DIR}/Model/cuBlock/*.cxx")
add_library(libcuBlock SHARED ${cuBlock})
file(GLOB global "${PROJECT_SOURCE_DIR}/global/*.cxx")
add_library(libglobal SHARED ${global})
file(GLOB commonBeam "${PROJECT_SOURCE_DIR}/Model/MaxIV/commonBeam/*.cxx")
add_library(libcommonBeam SHARED ${commonBeam})
file(GLOB commonGenerator "${PROJECT_SOURCE_DIR}/Model/MaxIV/commonGenerator/*.cxx")
add_library(libcommonGenerator SHARED ${commonGenerator})
file(GLOB simpleItem "${PROJECT_SOURCE_DIR}/Model/ESSBeam/simpleItem/*.cxx")
add_library(libsimpleItem SHARED ${simpleItem})
file(GLOB bibBuild "${PROJECT_SOURCE_DIR}/Model/bibBuild/*.cxx")
add_library(libbibBuild SHARED ${bibBuild})
file(GLOB bifrost "${PROJECT_SOURCE_DIR}/Model/ESSBeam/bifrost/*.cxx")
add_library(libbifrost SHARED ${bifrost})
file(GLOB pipeBuild "${PROJECT_SOURCE_DIR}/Model/pipeBuild/*.cxx")
add_library(libpipeBuild SHARED ${pipeBuild})
file(GLOB sinbadBuild "${PROJECT_SOURCE_DIR}/Model/sinbadBuild/*.cxx")
add_library(libsinbadBuild SHARED ${sinbadBuild})
file(GLOB lensModel "${PROJECT_SOURCE_DIR}/Model/lensModel/*.cxx")
add_library(liblensModel SHARED ${lensModel})
file(GLOB world "${PROJECT_SOURCE_DIR}/System/world/*.cxx")
add_library(libworld SHARED ${world})
file(GLOB maxpeem "${PROJECT_SOURCE_DIR}/Model/MaxIV/maxpeem/*.cxx")
add_library(libmaxpeem SHARED ${maxpeem})
file(GLOB mcnpProcess "${PROJECT_SOURCE_DIR}/System/mcnpProcess/*.cxx")
add_library(libmcnpProcess SHARED ${mcnpProcess})
file(GLOB poly "${PROJECT_SOURCE_DIR}/System/poly/*.cxx")
add_library(libpoly SHARED ${poly})
file(GLOB nnbar "${PROJECT_SOURCE_DIR}/Model/ESSBeam/nnbar/*.cxx")
add_library(libnnbar SHARED ${nnbar})
file(GLOB magnetic "${PROJECT_SOURCE_DIR}/System/magnetic/*.cxx")
add_library(libmagnetic SHARED ${magnetic})
file(GLOB test "${PROJECT_SOURCE_DIR}/test/*.cxx")
add_library(libtest SHARED ${test})
file(GLOB construct "${PROJECT_SOURCE_DIR}/System/construct/*.cxx")
add_library(libconstruct SHARED ${construct})
file(GLOB freia "${PROJECT_SOURCE_DIR}/Model/ESSBeam/freia/*.cxx")
add_library(libfreia SHARED ${freia})
file(GLOB simMC "${PROJECT_SOURCE_DIR}/System/simMC/*.cxx")
add_library(libsimMC SHARED ${simMC})
file(GLOB maxivBuild "${PROJECT_SOURCE_DIR}/Model/maxivBuild/*.cxx")
add_library(libmaxivBuild SHARED ${maxivBuild})
file(GLOB moderator "${PROJECT_SOURCE_DIR}/Model/moderator/*.cxx")
add_library(libmoderator SHARED ${moderator})
file(GLOB beamline "${PROJECT_SOURCE_DIR}/beamline/*.cxx")
add_library(libbeamline SHARED ${beamline})
file(GLOB essLinac "${PROJECT_SOURCE_DIR}/Model/essLinac/*.cxx")
add_library(libessLinac SHARED ${essLinac})
file(GLOB formax "${PROJECT_SOURCE_DIR}/Model/MaxIV/formax/*.cxx")
add_library(libformax SHARED ${formax})
file(GLOB skadi "${PROJECT_SOURCE_DIR}/Model/ESSBeam/skadi/*.cxx")
add_library(libskadi SHARED ${skadi})
file(GLOB t2Build "${PROJECT_SOURCE_DIR}/Model/t2Build/*.cxx")
add_library(libt2Build SHARED ${t2Build})
file(GLOB work "${PROJECT_SOURCE_DIR}/System/work/*.cxx")
add_library(libwork SHARED ${work})
file(GLOB photon "${PROJECT_SOURCE_DIR}/Model/photon/*.cxx")
add_library(libphoton SHARED ${photon})
file(GLOB objectMod "${PROJECT_SOURCE_DIR}/System/objectMod/*.cxx")
add_library(libobjectMod SHARED ${objectMod})
file(GLOB softimax "${PROJECT_SOURCE_DIR}/Model/MaxIV/softimax/*.cxx")
add_library(libsoftimax SHARED ${softimax})
file(GLOB phitsSupport "${PROJECT_SOURCE_DIR}/System/phitsSupport/*.cxx")
add_library(libphitsSupport SHARED ${phitsSupport})
file(GLOB nmx "${PROJECT_SOURCE_DIR}/Model/ESSBeam/nmx/*.cxx")
add_library(libnmx SHARED ${nmx})
file(GLOB support "${PROJECT_SOURCE_DIR}/System/support/*.cxx")
add_library(libsupport SHARED ${support})
file(GLOB singleItemBuild "${PROJECT_SOURCE_DIR}/Model/singleItemBuild/*.cxx")
add_library(libsingleItemBuild SHARED ${singleItemBuild})
file(GLOB visit "${PROJECT_SOURCE_DIR}/System/visit/*.cxx")
add_library(libvisit SHARED ${visit})
file(GLOB common "${PROJECT_SOURCE_DIR}/Model/ESSBeam/common/*.cxx")
add_library(libcommon SHARED ${common})
file(GLOB heimdal "${PROJECT_SOURCE_DIR}/Model/ESSBeam/heimdal/*.cxx")
add_library(libheimdal SHARED ${heimdal})
file(GLOB d4cModel "${PROJECT_SOURCE_DIR}/Model/d4cModel/*.cxx")
add_library(libd4cModel SHARED ${d4cModel})
file(GLOB src "${PROJECT_SOURCE_DIR}/src/*.cxx")
add_library(libsrc SHARED ${src})
## END GLOBS 

## EXECUTABLES 
add_executable(essBeamline ${PROJECT_SOURCE_DIR}/Main/essBeamline.cxx)
target_link_libraries(essBeamline -Wl,--start-group libinstrument)
target_link_libraries(essBeamline  libessBuild)
target_link_libraries(essBeamline  libvisit)
target_link_libraries(essBeamline  libsrc)
target_link_libraries(essBeamline  libsimMC)
target_link_libraries(essBeamline  libconstruct)
target_link_libraries(essBeamline  libphysics)
target_link_libraries(essBeamline  libinput)
target_link_libraries(essBeamline  libgeneralProcess)
target_link_libraries(essBeamline  libobjectMod)
target_link_libraries(essBeamline  libtransport)
target_link_libraries(essBeamline  libscatMat)
target_link_libraries(essBeamline  libendf)
target_link_libraries(essBeamline  libcrystal)
target_link_libraries(essBeamline  libsource)
target_link_libraries(essBeamline  libmonte)
target_link_libraries(essBeamline  libfuncBase)
target_link_libraries(essBeamline  liblog)
target_link_libraries(essBeamline  libmonte)
target_link_libraries(essBeamline  libflukaProcess)
target_link_libraries(essBeamline  libflukaPhysics)
target_link_libraries(essBeamline  libflukaTally)
target_link_libraries(essBeamline  libphitsProcess)
target_link_libraries(essBeamline  libphitsTally)
target_link_libraries(essBeamline  libphitsSupport)
target_link_libraries(essBeamline  libtally)
target_link_libraries(essBeamline  libgeometry)
target_link_libraries(essBeamline  libsrc)
target_link_libraries(essBeamline  libworld)
target_link_libraries(essBeamline  libwork)
target_link_libraries(essBeamline  libxml)
target_link_libraries(essBeamline  libpoly)
target_link_libraries(essBeamline  libsupport)
target_link_libraries(essBeamline  libweights)
target_link_libraries(essBeamline  libinsertUnit)
target_link_libraries(essBeamline  libmd5)
target_link_libraries(essBeamline  libconstruct)
target_link_libraries(essBeamline  libmodelSupport)
target_link_libraries(essBeamline  libglobal)
target_link_libraries(essBeamline  libconstructVar)
target_link_libraries(essBeamline  libphysics)
target_link_libraries(essBeamline  libsimMC)
target_link_libraries(essBeamline  libtransport)
target_link_libraries(essBeamline  libattachComp)
target_link_libraries(essBeamline  libvisit)
target_link_libraries(essBeamline  libpoly)
target_link_libraries(essBeamline  libmagnetic)
target_link_libraries(essBeamline  libphitsPhysics)
target_link_libraries(essBeamline  libmcnpProcess)
target_link_libraries(essBeamline  libscatMat)
target_link_libraries(essBeamline  libendf)
target_link_libraries(essBeamline  libcrystal)
target_link_libraries(essBeamline  libessConstruct)
target_link_libraries(essBeamline  libcommonVar)
target_link_libraries(essBeamline  libcommon)
target_link_libraries(essBeamline  libbeer)
target_link_libraries(essBeamline  libbifrost)
target_link_libraries(essBeamline  libcspec)
target_link_libraries(essBeamline  libdream)
target_link_libraries(essBeamline  libestia)
target_link_libraries(essBeamline  libfreia)
target_link_libraries(essBeamline  libheimdal)
target_link_libraries(essBeamline  libloki)
target_link_libraries(essBeamline  libmagic)
target_link_libraries(essBeamline  libmiracles)
target_link_libraries(essBeamline  libnmx)
target_link_libraries(essBeamline  libnnbar)
target_link_libraries(essBeamline  libodin)
target_link_libraries(essBeamline  libskadi)
target_link_libraries(essBeamline  libtestBeam)
target_link_libraries(essBeamline  libtrex)
target_link_libraries(essBeamline  libvor)
target_link_libraries(essBeamline  libvespa)
target_link_libraries(essBeamline  libsimpleItem)
target_link_libraries(essBeamline  libbeamline)
target_link_libraries(essBeamline libinstrument -Wl,--end-group)
target_link_libraries(essBeamline boost_filesystem)
target_link_libraries(essBeamline stdc++)
target_link_libraries(essBeamline gsl)
target_link_libraries(essBeamline gslcblas)
target_link_libraries(essBeamline m)
add_executable(ess ${PROJECT_SOURCE_DIR}/Main/ess.cxx)
target_link_libraries(ess -Wl,--start-group libinstrument)
target_link_libraries(ess  libessBuild)
target_link_libraries(ess  libvisit)
target_link_libraries(ess  libsrc)
target_link_libraries(ess  libsimMC)
target_link_libraries(ess  libconstruct)
target_link_libraries(ess  libphysics)
target_link_libraries(ess  libinput)
target_link_libraries(ess  libgeneralProcess)
target_link_libraries(ess  libobjectMod)
target_link_libraries(ess  libtransport)
target_link_libraries(ess  libscatMat)
target_link_libraries(ess  libendf)
target_link_libraries(ess  libcrystal)
target_link_libraries(ess  libsource)
target_link_libraries(ess  libmonte)
target_link_libraries(ess  libfuncBase)
target_link_libraries(ess  liblog)
target_link_libraries(ess  libmonte)
target_link_libraries(ess  libflukaProcess)
target_link_libraries(ess  libflukaPhysics)
target_link_libraries(ess  libflukaTally)
target_link_libraries(ess  libphitsProcess)
target_link_libraries(ess  libphitsTally)
target_link_libraries(ess  libphitsSupport)
target_link_libraries(ess  libtally)
target_link_libraries(ess  libgeometry)
target_link_libraries(ess  libsrc)
target_link_libraries(ess  libworld)
target_link_libraries(ess  libwork)
target_link_libraries(ess  libxml)
target_link_libraries(ess  libpoly)
target_link_libraries(ess  libsupport)
target_link_libraries(ess  libweights)
target_link_libraries(ess  libinsertUnit)
target_link_libraries(ess  libmd5)
target_link_libraries(ess  libconstruct)
target_link_libraries(ess  libmodelSupport)
target_link_libraries(ess  libglobal)
target_link_libraries(ess  libconstructVar)
target_link_libraries(ess  libphysics)
target_link_libraries(ess  libsimMC)
target_link_libraries(ess  libtransport)
target_link_libraries(ess  libattachComp)
target_link_libraries(ess  libvisit)
target_link_libraries(ess  libpoly)
target_link_libraries(ess  libmagnetic)
target_link_libraries(ess  libphitsPhysics)
target_link_libraries(ess  libmcnpProcess)
target_link_libraries(ess  libscatMat)
target_link_libraries(ess  libendf)
target_link_libraries(ess  libcrystal)
target_link_libraries(ess  libessConstruct)
target_link_libraries(ess  libcommonVar)
target_link_libraries(ess  libcommon)
target_link_libraries(ess  libbeer)
target_link_libraries(ess  libbifrost)
target_link_libraries(ess  libcspec)
target_link_libraries(ess  libdream)
target_link_libraries(ess  libestia)
target_link_libraries(ess  libfreia)
target_link_libraries(ess  libheimdal)
target_link_libraries(ess  libloki)
target_link_libraries(ess  libmagic)
target_link_libraries(ess  libmiracles)
target_link_libraries(ess  libnmx)
target_link_libraries(ess  libnnbar)
target_link_libraries(ess  libodin)
target_link_libraries(ess  libskadi)
target_link_libraries(ess  libtestBeam)
target_link_libraries(ess  libtrex)
target_link_libraries(ess  libvor)
target_link_libraries(ess  libvespa)
target_link_libraries(ess  libsimpleItem)
target_link_libraries(ess  libbeamline)
target_link_libraries(ess libinstrument -Wl,--end-group)
target_link_libraries(ess boost_filesystem)
target_link_libraries(ess stdc++)
target_link_libraries(ess gsl)
target_link_libraries(ess gslcblas)
target_link_libraries(ess m)
add_executable(fullBuild ${PROJECT_SOURCE_DIR}/Main/fullBuild.cxx)
target_link_libraries(fullBuild -Wl,--start-group libcrystal)
target_link_libraries(fullBuild  libmoderator)
target_link_libraries(fullBuild  libt2Build)
target_link_libraries(fullBuild  libralBuild)
target_link_libraries(fullBuild  libvisit)
target_link_libraries(fullBuild  libsrc)
target_link_libraries(fullBuild  libsimMC)
target_link_libraries(fullBuild  libconstruct)
target_link_libraries(fullBuild  libphysics)
target_link_libraries(fullBuild  libinput)
target_link_libraries(fullBuild  libgeneralProcess)
target_link_libraries(fullBuild  libobjectMod)
target_link_libraries(fullBuild  libtransport)
target_link_libraries(fullBuild  libscatMat)
target_link_libraries(fullBuild  libendf)
target_link_libraries(fullBuild  libcrystal)
target_link_libraries(fullBuild  libsource)
target_link_libraries(fullBuild  libmonte)
target_link_libraries(fullBuild  libfuncBase)
target_link_libraries(fullBuild  liblog)
target_link_libraries(fullBuild  libmonte)
target_link_libraries(fullBuild  libflukaProcess)
target_link_libraries(fullBuild  libflukaPhysics)
target_link_libraries(fullBuild  libflukaTally)
target_link_libraries(fullBuild  libphitsProcess)
target_link_libraries(fullBuild  libphitsTally)
target_link_libraries(fullBuild  libphitsSupport)
target_link_libraries(fullBuild  libtally)
target_link_libraries(fullBuild  libgeometry)
target_link_libraries(fullBuild  libsrc)
target_link_libraries(fullBuild  libworld)
target_link_libraries(fullBuild  libwork)
target_link_libraries(fullBuild  libxml)
target_link_libraries(fullBuild  libpoly)
target_link_libraries(fullBuild  libsupport)
target_link_libraries(fullBuild  libweights)
target_link_libraries(fullBuild  libinsertUnit)
target_link_libraries(fullBuild  libmd5)
target_link_libraries(fullBuild  libconstruct)
target_link_libraries(fullBuild  libmodelSupport)
target_link_libraries(fullBuild  libglobal)
target_link_libraries(fullBuild  libconstructVar)
target_link_libraries(fullBuild  libphysics)
target_link_libraries(fullBuild  libsimMC)
target_link_libraries(fullBuild  libtransport)
target_link_libraries(fullBuild  libattachComp)
target_link_libraries(fullBuild  libvisit)
target_link_libraries(fullBuild  libpoly)
target_link_libraries(fullBuild  libmagnetic)
target_link_libraries(fullBuild  libphitsPhysics)
target_link_libraries(fullBuild  libmcnpProcess)
target_link_libraries(fullBuild  libscatMat)
target_link_libraries(fullBuild  libendf)
target_link_libraries(fullBuild libcrystal -Wl,--end-group)
target_link_libraries(fullBuild boost_filesystem)
target_link_libraries(fullBuild stdc++)
target_link_libraries(fullBuild gsl)
target_link_libraries(fullBuild gslcblas)
target_link_libraries(fullBuild m)
add_executable(maxiv ${PROJECT_SOURCE_DIR}/Main/maxiv.cxx)
target_link_libraries(maxiv -Wl,--start-group libspecies)
target_link_libraries(maxiv  libmaxivBuild)
target_link_libraries(maxiv  libvisit)
target_link_libraries(maxiv  libsrc)
target_link_libraries(maxiv  libsimMC)
target_link_libraries(maxiv  libconstruct)
target_link_libraries(maxiv  libphysics)
target_link_libraries(maxiv  libinput)
target_link_libraries(maxiv  libgeneralProcess)
target_link_libraries(maxiv  libobjectMod)
target_link_libraries(maxiv  libtransport)
target_link_libraries(maxiv  libscatMat)
target_link_libraries(maxiv  libendf)
target_link_libraries(maxiv  libcrystal)
target_link_libraries(maxiv  libsource)
target_link_libraries(maxiv  libmonte)
target_link_libraries(maxiv  libfuncBase)
target_link_libraries(maxiv  liblog)
target_link_libraries(maxiv  libmonte)
target_link_libraries(maxiv  libflukaProcess)
target_link_libraries(maxiv  libflukaPhysics)
target_link_libraries(maxiv  libflukaTally)
target_link_libraries(maxiv  libphitsProcess)
target_link_libraries(maxiv  libphitsTally)
target_link_libraries(maxiv  libphitsSupport)
target_link_libraries(maxiv  libtally)
target_link_libraries(maxiv  libgeometry)
target_link_libraries(maxiv  libsrc)
target_link_libraries(maxiv  libworld)
target_link_libraries(maxiv  libwork)
target_link_libraries(maxiv  libxml)
target_link_libraries(maxiv  libpoly)
target_link_libraries(maxiv  libsupport)
target_link_libraries(maxiv  libweights)
target_link_libraries(maxiv  libinsertUnit)
target_link_libraries(maxiv  libmd5)
target_link_libraries(maxiv  libconstruct)
target_link_libraries(maxiv  libmodelSupport)
target_link_libraries(maxiv  libglobal)
target_link_libraries(maxiv  libconstructVar)
target_link_libraries(maxiv  libphysics)
target_link_libraries(maxiv  libsimMC)
target_link_libraries(maxiv  libtransport)
target_link_libraries(maxiv  libattachComp)
target_link_libraries(maxiv  libvisit)
target_link_libraries(maxiv  libpoly)
target_link_libraries(maxiv  libmagnetic)
target_link_libraries(maxiv  libphitsPhysics)
target_link_libraries(maxiv  libmcnpProcess)
target_link_libraries(maxiv  libscatMat)
target_link_libraries(maxiv  libendf)
target_link_libraries(maxiv  libcrystal)
target_link_libraries(maxiv  libbalder)
target_link_libraries(maxiv  libcosaxs)
target_link_libraries(maxiv  libdanmax)
target_link_libraries(maxiv  libR3Common)
target_link_libraries(maxiv  libflexpes)
target_link_libraries(maxiv  libformax)
target_link_libraries(maxiv  libmaxpeem)
target_link_libraries(maxiv  libmicromax)
target_link_libraries(maxiv  libsoftimax)
target_link_libraries(maxiv  libcommonGenerator)
target_link_libraries(maxiv  libcommonBeam)
target_link_libraries(maxiv  libLinac)
target_link_libraries(maxiv  libR3Common)
target_link_libraries(maxiv  libR1Common)
target_link_libraries(maxiv libspecies -Wl,--end-group)
target_link_libraries(maxiv boost_filesystem)
target_link_libraries(maxiv stdc++)
target_link_libraries(maxiv gsl)
target_link_libraries(maxiv gslcblas)
target_link_libraries(maxiv m)
add_executable(pipe ${PROJECT_SOURCE_DIR}/Main/pipe.cxx)
target_link_libraries(pipe -Wl,--start-group libcrystal)
target_link_libraries(pipe  libpipeBuild)
target_link_libraries(pipe  libvisit)
target_link_libraries(pipe  libsrc)
target_link_libraries(pipe  libsimMC)
target_link_libraries(pipe  libconstruct)
target_link_libraries(pipe  libphysics)
target_link_libraries(pipe  libinput)
target_link_libraries(pipe  libgeneralProcess)
target_link_libraries(pipe  libobjectMod)
target_link_libraries(pipe  libtransport)
target_link_libraries(pipe  libscatMat)
target_link_libraries(pipe  libendf)
target_link_libraries(pipe  libcrystal)
target_link_libraries(pipe  libsource)
target_link_libraries(pipe  libmonte)
target_link_libraries(pipe  libfuncBase)
target_link_libraries(pipe  liblog)
target_link_libraries(pipe  libmonte)
target_link_libraries(pipe  libflukaProcess)
target_link_libraries(pipe  libflukaPhysics)
target_link_libraries(pipe  libflukaTally)
target_link_libraries(pipe  libphitsProcess)
target_link_libraries(pipe  libphitsTally)
target_link_libraries(pipe  libphitsSupport)
target_link_libraries(pipe  libtally)
target_link_libraries(pipe  libgeometry)
target_link_libraries(pipe  libsrc)
target_link_libraries(pipe  libworld)
target_link_libraries(pipe  libwork)
target_link_libraries(pipe  libxml)
target_link_libraries(pipe  libpoly)
target_link_libraries(pipe  libsupport)
target_link_libraries(pipe  libweights)
target_link_libraries(pipe  libinsertUnit)
target_link_libraries(pipe  libmd5)
target_link_libraries(pipe  libconstruct)
target_link_libraries(pipe  libmodelSupport)
target_link_libraries(pipe  libglobal)
target_link_libraries(pipe  libconstructVar)
target_link_libraries(pipe  libphysics)
target_link_libraries(pipe  libsimMC)
target_link_libraries(pipe  libtransport)
target_link_libraries(pipe  libattachComp)
target_link_libraries(pipe  libvisit)
target_link_libraries(pipe  libpoly)
target_link_libraries(pipe  libmagnetic)
target_link_libraries(pipe  libphitsPhysics)
target_link_libraries(pipe  libmcnpProcess)
target_link_libraries(pipe  libscatMat)
target_link_libraries(pipe  libendf)
target_link_libraries(pipe libcrystal -Wl,--end-group)
target_link_libraries(pipe boost_filesystem)
target_link_libraries(pipe stdc++)
target_link_libraries(pipe gsl)
target_link_libraries(pipe gslcblas)
target_link_libraries(pipe m)
add_executable(reactor ${PROJECT_SOURCE_DIR}/Main/reactor.cxx)
target_link_libraries(reactor -Wl,--start-group libcrystal)
target_link_libraries(reactor  libdelft)
target_link_libraries(reactor  libvisit)
target_link_libraries(reactor  libsrc)
target_link_libraries(reactor  libsimMC)
target_link_libraries(reactor  libconstruct)
target_link_libraries(reactor  libphysics)
target_link_libraries(reactor  libinput)
target_link_libraries(reactor  libgeneralProcess)
target_link_libraries(reactor  libobjectMod)
target_link_libraries(reactor  libtransport)
target_link_libraries(reactor  libscatMat)
target_link_libraries(reactor  libendf)
target_link_libraries(reactor  libcrystal)
target_link_libraries(reactor  libsource)
target_link_libraries(reactor  libmonte)
target_link_libraries(reactor  libfuncBase)
target_link_libraries(reactor  liblog)
target_link_libraries(reactor  libmonte)
target_link_libraries(reactor  libflukaProcess)
target_link_libraries(reactor  libflukaPhysics)
target_link_libraries(reactor  libflukaTally)
target_link_libraries(reactor  libphitsProcess)
target_link_libraries(reactor  libphitsTally)
target_link_libraries(reactor  libphitsSupport)
target_link_libraries(reactor  libtally)
target_link_libraries(reactor  libgeometry)
target_link_libraries(reactor  libsrc)
target_link_libraries(reactor  libworld)
target_link_libraries(reactor  libwork)
target_link_libraries(reactor  libxml)
target_link_libraries(reactor  libpoly)
target_link_libraries(reactor  libsupport)
target_link_libraries(reactor  libweights)
target_link_libraries(reactor  libinsertUnit)
target_link_libraries(reactor  libmd5)
target_link_libraries(reactor  libconstruct)
target_link_libraries(reactor  libmodelSupport)
target_link_libraries(reactor  libglobal)
target_link_libraries(reactor  libconstructVar)
target_link_libraries(reactor  libphysics)
target_link_libraries(reactor  libsimMC)
target_link_libraries(reactor  libtransport)
target_link_libraries(reactor  libattachComp)
target_link_libraries(reactor  libvisit)
target_link_libraries(reactor  libpoly)
target_link_libraries(reactor  libmagnetic)
target_link_libraries(reactor  libphitsPhysics)
target_link_libraries(reactor  libmcnpProcess)
target_link_libraries(reactor  libscatMat)
target_link_libraries(reactor  libendf)
target_link_libraries(reactor libcrystal -Wl,--end-group)
target_link_libraries(reactor boost_filesystem)
target_link_libraries(reactor stdc++)
target_link_libraries(reactor gsl)
target_link_libraries(reactor gslcblas)
target_link_libraries(reactor m)
add_executable(singleItem ${PROJECT_SOURCE_DIR}/Main/singleItem.cxx)
target_link_libraries(singleItem -Wl,--start-group libLinac)
target_link_libraries(singleItem  libsingleItemBuild)
target_link_libraries(singleItem  libvisit)
target_link_libraries(singleItem  libsrc)
target_link_libraries(singleItem  libsimMC)
target_link_libraries(singleItem  libconstruct)
target_link_libraries(singleItem  libphysics)
target_link_libraries(singleItem  libinput)
target_link_libraries(singleItem  libgeneralProcess)
target_link_libraries(singleItem  libobjectMod)
target_link_libraries(singleItem  libtransport)
target_link_libraries(singleItem  libscatMat)
target_link_libraries(singleItem  libendf)
target_link_libraries(singleItem  libcrystal)
target_link_libraries(singleItem  libsource)
target_link_libraries(singleItem  libmonte)
target_link_libraries(singleItem  libfuncBase)
target_link_libraries(singleItem  liblog)
target_link_libraries(singleItem  libmonte)
target_link_libraries(singleItem  libflukaProcess)
target_link_libraries(singleItem  libflukaPhysics)
target_link_libraries(singleItem  libflukaTally)
target_link_libraries(singleItem  libphitsProcess)
target_link_libraries(singleItem  libphitsTally)
target_link_libraries(singleItem  libphitsSupport)
target_link_libraries(singleItem  libtally)
target_link_libraries(singleItem  libgeometry)
target_link_libraries(singleItem  libsrc)
target_link_libraries(singleItem  libworld)
target_link_libraries(singleItem  libwork)
target_link_libraries(singleItem  libxml)
target_link_libraries(singleItem  libpoly)
target_link_libraries(singleItem  libsupport)
target_link_libraries(singleItem  libweights)
target_link_libraries(singleItem  libinsertUnit)
target_link_libraries(singleItem  libmd5)
target_link_libraries(singleItem  libconstruct)
target_link_libraries(singleItem  libmodelSupport)
target_link_libraries(singleItem  libglobal)
target_link_libraries(singleItem  libconstructVar)
target_link_libraries(singleItem  libphysics)
target_link_libraries(singleItem  libsimMC)
target_link_libraries(singleItem  libtransport)
target_link_libraries(singleItem  libattachComp)
target_link_libraries(singleItem  libvisit)
target_link_libraries(singleItem  libpoly)
target_link_libraries(singleItem  libmagnetic)
target_link_libraries(singleItem  libphitsPhysics)
target_link_libraries(singleItem  libmcnpProcess)
target_link_libraries(singleItem  libscatMat)
target_link_libraries(singleItem  libendf)
target_link_libraries(singleItem  libcrystal)
target_link_libraries(singleItem  libessConstruct)
target_link_libraries(singleItem  libcommonVar)
target_link_libraries(singleItem  libcommonGenerator)
target_link_libraries(singleItem  libR1Common)
target_link_libraries(singleItem  libR3Common)
target_link_libraries(singleItem  libcommonBeam)
target_link_libraries(singleItem libLinac -Wl,--end-group)
target_link_libraries(singleItem boost_filesystem)
target_link_libraries(singleItem stdc++)
target_link_libraries(singleItem gsl)
target_link_libraries(singleItem gslcblas)
target_link_libraries(singleItem m)
add_executable(t1Real ${PROJECT_SOURCE_DIR}/Main/t1Real.cxx)
target_link_libraries(t1Real -Wl,--start-group libcrystal)
target_link_libraries(t1Real  libt1Build)
target_link_libraries(t1Real  libralBuild)
target_link_libraries(t1Real  libvisit)
target_link_libraries(t1Real  libsrc)
target_link_libraries(t1Real  libsimMC)
target_link_libraries(t1Real  libconstruct)
target_link_libraries(t1Real  libphysics)
target_link_libraries(t1Real  libinput)
target_link_libraries(t1Real  libgeneralProcess)
target_link_libraries(t1Real  libobjectMod)
target_link_libraries(t1Real  libtransport)
target_link_libraries(t1Real  libscatMat)
target_link_libraries(t1Real  libendf)
target_link_libraries(t1Real  libcrystal)
target_link_libraries(t1Real  libsource)
target_link_libraries(t1Real  libmonte)
target_link_libraries(t1Real  libfuncBase)
target_link_libraries(t1Real  liblog)
target_link_libraries(t1Real  libmonte)
target_link_libraries(t1Real  libflukaProcess)
target_link_libraries(t1Real  libflukaPhysics)
target_link_libraries(t1Real  libflukaTally)
target_link_libraries(t1Real  libphitsProcess)
target_link_libraries(t1Real  libphitsTally)
target_link_libraries(t1Real  libphitsSupport)
target_link_libraries(t1Real  libtally)
target_link_libraries(t1Real  libgeometry)
target_link_libraries(t1Real  libsrc)
target_link_libraries(t1Real  libworld)
target_link_libraries(t1Real  libwork)
target_link_libraries(t1Real  libxml)
target_link_libraries(t1Real  libpoly)
target_link_libraries(t1Real  libsupport)
target_link_libraries(t1Real  libweights)
target_link_libraries(t1Real  libinsertUnit)
target_link_libraries(t1Real  libmd5)
target_link_libraries(t1Real  libconstruct)
target_link_libraries(t1Real  libmodelSupport)
target_link_libraries(t1Real  libglobal)
target_link_libraries(t1Real  libconstructVar)
target_link_libraries(t1Real  libphysics)
target_link_libraries(t1Real  libsimMC)
target_link_libraries(t1Real  libtransport)
target_link_libraries(t1Real  libattachComp)
target_link_libraries(t1Real  libvisit)
target_link_libraries(t1Real  libpoly)
target_link_libraries(t1Real  libmagnetic)
target_link_libraries(t1Real  libphitsPhysics)
target_link_libraries(t1Real  libmcnpProcess)
target_link_libraries(t1Real  libscatMat)
target_link_libraries(t1Real  libendf)
target_link_libraries(t1Real libcrystal -Wl,--end-group)
target_link_libraries(t1Real boost_filesystem)
target_link_libraries(t1Real stdc++)
target_link_libraries(t1Real gsl)
target_link_libraries(t1Real gslcblas)
target_link_libraries(t1Real m)
add_executable(testMain ${PROJECT_SOURCE_DIR}/Main/testMain.cxx)
target_link_libraries(testMain -Wl,--start-group libcrystal)
target_link_libraries(testMain  libtest)
target_link_libraries(testMain  libvisit)
target_link_libraries(testMain  libsrc)
target_link_libraries(testMain  libsimMC)
target_link_libraries(testMain  libconstruct)
target_link_libraries(testMain  libphysics)
target_link_libraries(testMain  libinput)
target_link_libraries(testMain  libgeneralProcess)
target_link_libraries(testMain  libobjectMod)
target_link_libraries(testMain  libtransport)
target_link_libraries(testMain  libscatMat)
target_link_libraries(testMain  libendf)
target_link_libraries(testMain  libcrystal)
target_link_libraries(testMain  libsource)
target_link_libraries(testMain  libmonte)
target_link_libraries(testMain  libfuncBase)
target_link_libraries(testMain  liblog)
target_link_libraries(testMain  libmonte)
target_link_libraries(testMain  libflukaProcess)
target_link_libraries(testMain  libflukaPhysics)
target_link_libraries(testMain  libflukaTally)
target_link_libraries(testMain  libphitsProcess)
target_link_libraries(testMain  libphitsTally)
target_link_libraries(testMain  libphitsSupport)
target_link_libraries(testMain  libtally)
target_link_libraries(testMain  libgeometry)
target_link_libraries(testMain  libsrc)
target_link_libraries(testMain  libworld)
target_link_libraries(testMain  libwork)
target_link_libraries(testMain  libxml)
target_link_libraries(testMain  libpoly)
target_link_libraries(testMain  libsupport)
target_link_libraries(testMain  libweights)
target_link_libraries(testMain  libinsertUnit)
target_link_libraries(testMain  libmd5)
target_link_libraries(testMain  libconstruct)
target_link_libraries(testMain  libmodelSupport)
target_link_libraries(testMain  libglobal)
target_link_libraries(testMain  libconstructVar)
target_link_libraries(testMain  libphysics)
target_link_libraries(testMain  libsimMC)
target_link_libraries(testMain  libtransport)
target_link_libraries(testMain  libattachComp)
target_link_libraries(testMain  libvisit)
target_link_libraries(testMain  libpoly)
target_link_libraries(testMain  libmagnetic)
target_link_libraries(testMain  libphitsPhysics)
target_link_libraries(testMain  libmcnpProcess)
target_link_libraries(testMain  libscatMat)
target_link_libraries(testMain  libendf)
target_link_libraries(testMain libcrystal -Wl,--end-group)
target_link_libraries(testMain boost_filesystem)
target_link_libraries(testMain stdc++)
target_link_libraries(testMain gsl)
target_link_libraries(testMain gslcblas)
target_link_libraries(testMain m)
add_executable(xrayHut ${PROJECT_SOURCE_DIR}/Main/xrayHut.cxx)
target_link_libraries(xrayHut -Wl,--start-group libcrystal)
target_link_libraries(xrayHut  libxrayHutch)
target_link_libraries(xrayHut  libvisit)
target_link_libraries(xrayHut  libsrc)
target_link_libraries(xrayHut  libsimMC)
target_link_libraries(xrayHut  libconstruct)
target_link_libraries(xrayHut  libphysics)
target_link_libraries(xrayHut  libinput)
target_link_libraries(xrayHut  libgeneralProcess)
target_link_libraries(xrayHut  libobjectMod)
target_link_libraries(xrayHut  libtransport)
target_link_libraries(xrayHut  libscatMat)
target_link_libraries(xrayHut  libendf)
target_link_libraries(xrayHut  libcrystal)
target_link_libraries(xrayHut  libsource)
target_link_libraries(xrayHut  libmonte)
target_link_libraries(xrayHut  libfuncBase)
target_link_libraries(xrayHut  liblog)
target_link_libraries(xrayHut  libmonte)
target_link_libraries(xrayHut  libflukaProcess)
target_link_libraries(xrayHut  libflukaPhysics)
target_link_libraries(xrayHut  libflukaTally)
target_link_libraries(xrayHut  libphitsProcess)
target_link_libraries(xrayHut  libphitsTally)
target_link_libraries(xrayHut  libphitsSupport)
target_link_libraries(xrayHut  libtally)
target_link_libraries(xrayHut  libgeometry)
target_link_libraries(xrayHut  libsrc)
target_link_libraries(xrayHut  libworld)
target_link_libraries(xrayHut  libwork)
target_link_libraries(xrayHut  libxml)
target_link_libraries(xrayHut  libpoly)
target_link_libraries(xrayHut  libsupport)
target_link_libraries(xrayHut  libweights)
target_link_libraries(xrayHut  libinsertUnit)
target_link_libraries(xrayHut  libmd5)
target_link_libraries(xrayHut  libconstruct)
target_link_libraries(xrayHut  libmodelSupport)
target_link_libraries(xrayHut  libglobal)
target_link_libraries(xrayHut  libconstructVar)
target_link_libraries(xrayHut  libphysics)
target_link_libraries(xrayHut  libsimMC)
target_link_libraries(xrayHut  libtransport)
target_link_libraries(xrayHut  libattachComp)
target_link_libraries(xrayHut  libvisit)
target_link_libraries(xrayHut  libpoly)
target_link_libraries(xrayHut  libmagnetic)
target_link_libraries(xrayHut  libphitsPhysics)
target_link_libraries(xrayHut  libmcnpProcess)
target_link_libraries(xrayHut  libscatMat)
target_link_libraries(xrayHut  libendf)
target_link_libraries(xrayHut libcrystal -Wl,--end-group)
target_link_libraries(xrayHut boost_filesystem)
target_link_libraries(xrayHut stdc++)
target_link_libraries(xrayHut gsl)
target_link_libraries(xrayHut gslcblas)
target_link_libraries(xrayHut m)
## END EXECUTABLE 

set(ALLCXX 
     ./System/flukaPhysics/*.cxx 
     ./System/md5/*.cxx 
     ./System/source/*.cxx 
     ./Model/ESSBeam/vespa/*.cxx 
     ./Model/MaxIV/micromax/*.cxx 
     ./System/physics/*.cxx 
     ./Model/ESSBeam/testBeam/*.cxx 
     ./System/input/*.cxx 
     ./System/geometry/*.cxx 
     ./System/phitsTally/*.cxx 
     ./System/attachComp/*.cxx 
     ./Model/MaxIV/balder/*.cxx 
     ./Model/MaxIV/cosaxs/*.cxx 
     ./Model/ESSBeam/dream/*.cxx 
     ./Model/essBuild/*.cxx 
     ./System/crystal/*.cxx 
     ./Model/gammaBuild/*.cxx 
     ./Model/ESSBeam/magic/*.cxx 
     ./System/compWeights/*.cxx 
     ./Model/delft/*.cxx 
     ./System/endf/*.cxx 
     ./System/insertUnit/*.cxx 
     ./transport/*.cxx 
     ./Model/MaxIV/flexpes/*.cxx 
     ./Model/exampleBuild/*.cxx 
     ./System/monte/*.cxx 
     ./System/phitsProcess/*.cxx 
     ./Model/MaxIV/R3Common/*.cxx 
     ./Model/filter/*.cxx 
     ./Model/MaxIV/Linac/*.cxx 
     ./Model/epbBuild/*.cxx 
     ./Model/ESSBeam/vor/*.cxx 
     ./Model/snsBuild/*.cxx 
     ./Model/ESSBeam/beer/*.cxx 
     ./scatMat/*.cxx 
     ./Model/ESSBeam/miracles/*.cxx 
     ./System/log/*.cxx 
     ./Model/MaxIV/danmax/*.cxx 
     ./Model/saxs/*.cxx 
     ./System/flukaProcess/*.cxx 
     ./System/phitsPhysics/*.cxx 
     ./System/generalProcess/*.cxx 
     ./System/modelSupport/*.cxx 
     ./Model/ESSBeam/estia/*.cxx 
     ./Model/ESSBeam/cspec/*.cxx 
     ./System/constructVar/*.cxx 
     ./Model/MaxIV/R1Common/*.cxx 
     ./Model/xrayHutch/*.cxx 
     ./System/weights/*.cxx 
     ./System/flukaTally/*.cxx 
     ./Model/ESSBeam/commonVar/*.cxx 
     ./Model/MaxIV/species/*.cxx 
     ./Model/essConstruct/*.cxx 
     ./Model/ESSBeam/odin/*.cxx 
     ./Model/ESSBeam/loki/*.cxx 
     ./Model/ESSBeam/trex/*.cxx 
     ./System/tally/*.cxx 
     ./Model/t1Build/*.cxx 
     ./System/xml/*.cxx 
     ./System/funcBase/*.cxx 
     ./instrument/*.cxx 
     ./Model/ralBuild/*.cxx 
     ./Model/cuBlock/*.cxx 
     ./global/*.cxx 
     ./Model/MaxIV/commonBeam/*.cxx 
     ./Model/MaxIV/commonGenerator/*.cxx 
     ./Model/ESSBeam/simpleItem/*.cxx 
     ./Model/bibBuild/*.cxx 
     ./Model/ESSBeam/bifrost/*.cxx 
     ./Model/pipeBuild/*.cxx 
     ./Model/sinbadBuild/*.cxx 
     ./Model/lensModel/*.cxx 
     ./System/world/*.cxx 
     ./Model/MaxIV/maxpeem/*.cxx 
     ./System/mcnpProcess/*.cxx 
     ./System/poly/*.cxx 
     ./Model/ESSBeam/nnbar/*.cxx 
     ./System/magnetic/*.cxx 
     ./test/*.cxx 
     ./System/construct/*.cxx 
     ./Model/ESSBeam/freia/*.cxx 
     ./System/simMC/*.cxx 
     ./Model/maxivBuild/*.cxx 
     ./Model/moderator/*.cxx 
     ./beamline/*.cxx 
     ./Model/essLinac/*.cxx 
     ./Model/MaxIV/formax/*.cxx 
     ./Model/ESSBeam/skadi/*.cxx 
     ./Model/t2Build/*.cxx 
     ./System/work/*.cxx 
     ./Model/photon/*.cxx 
     ./System/objectMod/*.cxx 
     ./Model/MaxIV/softimax/*.cxx 
     ./System/phitsSupport/*.cxx 
     ./Model/ESSBeam/nmx/*.cxx 
     ./System/support/*.cxx 
     ./Model/singleItemBuild/*.cxx 
     ./System/visit/*.cxx 
     ./Model/ESSBeam/common/*.cxx 
     ./Model/ESSBeam/heimdal/*.cxx 
     ./Model/d4cModel/*.cxx 
     ./src/*.cxx 
     ./Main/*.cxx )
set(ALLHXX 
     ./include/*.h 
     ./beamlineInc/*.h 
     ./globalInc/*.h 
     ./instrumentInc/*.h 
     ./scatMatInc/*.h 
     ./specialInc/*.h 
     ./transportInc/*.h 
     ./testInclude/*.h 
     ./System/attachCompInc/*.h 
     ./System/compWeightsInc/*.h 
     ./System/constructInc/*.h 
     ./System/constructVarInc/*.h 
     ./System/crystalInc/*.h 
     ./System/endfInc/*.h 
     ./System/flukaPhysicsInc/*.h 
     ./System/flukaProcessInc/*.h 
     ./System/flukaTallyInc/*.h 
     ./System/funcBaseInc/*.h 
     ./System/generalProcessInc/*.h 
     ./System/geomInc/*.h 
     ./System/inputInc/*.h 
     ./System/insertUnitInc/*.h 
     ./System/logInc/*.h 
     ./System/magneticInc/*.h 
     ./System/mcnpProcessInc/*.h 
     ./System/md5Inc/*.h 
     ./System/modelSupportInc/*.h 
     ./System/monteInc/*.h 
     ./System/objectModInc/*.h 
     ./System/phitsPhysicsInc/*.h 
     ./System/phitsProcessInc/*.h 
     ./System/phitsSupportInc/*.h 
     ./System/phitsTallyInc/*.h 
     ./System/physicsInc/*.h 
     ./System/polyInc/*.h 
     ./System/simMCInc/*.h 
     ./System/sourceInc/*.h 
     ./System/supportInc/*.h 
     ./System/tallyInc/*.h 
     ./System/visitInc/*.h 
     ./System/weightsInc/*.h 
     ./System/workInc/*.h 
     ./System/worldInc/*.h 
     ./System/xmlInc/*.h 
     ./Model/bibBuildInc/*.h 
     ./Model/cuBlockInc/*.h 
     ./Model/d4cModelInc/*.h 
     ./Model/delftInc/*.h 
     ./Model/epbBuildInc/*.h 
     ./Model/essBuildInc/*.h 
     ./Model/essConstructInc/*.h 
     ./Model/essLinacInc/*.h 
     ./Model/exampleBuildInc/*.h 
     ./Model/filterInc/*.h 
     ./Model/gammaBuildInc/*.h 
     ./Model/lensModelInc/*.h 
     ./Model/maxivBuildInc/*.h 
     ./Model/moderatorInc/*.h 
     ./Model/muonInc/*.h 
     ./Model/photonInc/*.h 
     ./Model/pipeBuildInc/*.h 
     ./Model/ralBuildInc/*.h 
     ./Model/saxsInc/*.h 
     ./Model/sinbadBuildInc/*.h 
     ./Model/singleItemBuildInc/*.h 
     ./Model/snsBuildInc/*.h 
     ./Model/t1BuildInc/*.h 
     ./Model/t2BuildInc/*.h 
     ./Model/xrayHutchInc/*.h 
     ./Model/ESSBeam/beerInc/*.h 
     ./Model/ESSBeam/bifrostInc/*.h 
     ./Model/ESSBeam/commonInc/*.h 
     ./Model/ESSBeam/commonVarInc/*.h 
     ./Model/ESSBeam/cspecInc/*.h 
     ./Model/ESSBeam/dreamInc/*.h 
     ./Model/ESSBeam/estiaInc/*.h 
     ./Model/ESSBeam/freiaInc/*.h 
     ./Model/ESSBeam/heimdalInc/*.h 
     ./Model/ESSBeam/lokiInc/*.h 
     ./Model/ESSBeam/magicInc/*.h 
     ./Model/ESSBeam/miraclesInc/*.h 
     ./Model/ESSBeam/nmxInc/*.h 
     ./Model/ESSBeam/nnbarInc/*.h 
     ./Model/ESSBeam/odinInc/*.h 
     ./Model/ESSBeam/simpleItemInc/*.h 
     ./Model/ESSBeam/skadiInc/*.h 
     ./Model/ESSBeam/testBeamInc/*.h 
     ./Model/ESSBeam/trexInc/*.h 
     ./Model/ESSBeam/vespaInc/*.h 
     ./Model/ESSBeam/vorInc/*.h 
     ./Model/MaxIV/balderInc/*.h 
     ./Model/MaxIV/commonBeamInc/*.h 
     ./Model/MaxIV/commonGeneratorInc/*.h 
     ./Model/MaxIV/cosaxsInc/*.h 
     ./Model/MaxIV/danmaxInc/*.h 
     ./Model/MaxIV/flexpesInc/*.h 
     ./Model/MaxIV/formaxInc/*.h 
     ./Model/MaxIV/LinacInc/*.h 
     ./Model/MaxIV/maxpeemInc/*.h 
     ./Model/MaxIV/micromaxInc/*.h 
     ./Model/MaxIV/R1CommonInc/*.h 
     ./Model/MaxIV/R3CommonInc/*.h 
     ./Model/MaxIV/softimaxInc/*.h 
     ./Model/MaxIV/speciesInc/*.h 
      )
set(ASRC ${ALLHXX} ${ALLCXX} )
add_custom_target(doxygen  COMMAND  { cat Doxyfile \; echo "INPUT=" ${ASRC} "" \;} | doxygen - )
add_custom_target(sloc  COMMAND sloccount ${ASRC} )
add_custom_target(words  COMMAND grep -v -e '^[[:space:][:cntrl:]]*$$' 
     ./System/flukaPhysics/*.cxx 
     ./System/md5/*.cxx 
     ./System/source/*.cxx 
     ./Model/ESSBeam/vespa/*.cxx 
     ./Model/MaxIV/micromax/*.cxx 
     ./System/physics/*.cxx 
     ./Model/ESSBeam/testBeam/*.cxx 
     ./System/input/*.cxx 
     ./System/geometry/*.cxx 
     ./System/phitsTally/*.cxx 
     ./System/attachComp/*.cxx 
     ./Model/MaxIV/balder/*.cxx 
     ./Model/MaxIV/cosaxs/*.cxx 
     ./Model/ESSBeam/dream/*.cxx 
     ./Model/essBuild/*.cxx 
     ./System/crystal/*.cxx 
     ./Model/gammaBuild/*.cxx 
     ./Model/ESSBeam/magic/*.cxx 
     ./System/compWeights/*.cxx 
     ./Model/delft/*.cxx 
     ./System/endf/*.cxx 
     ./System/insertUnit/*.cxx 
     ./transport/*.cxx 
     ./Model/MaxIV/flexpes/*.cxx 
     ./Model/exampleBuild/*.cxx 
     ./System/monte/*.cxx 
     ./System/phitsProcess/*.cxx 
     ./Model/MaxIV/R3Common/*.cxx 
     ./Model/filter/*.cxx 
     ./Model/MaxIV/Linac/*.cxx 
     ./Model/epbBuild/*.cxx 
     ./Model/ESSBeam/vor/*.cxx 
     ./Model/snsBuild/*.cxx 
     ./Model/ESSBeam/beer/*.cxx 
     ./scatMat/*.cxx 
     ./Model/ESSBeam/miracles/*.cxx 
     ./System/log/*.cxx 
     ./Model/MaxIV/danmax/*.cxx 
     ./Model/saxs/*.cxx 
     ./System/flukaProcess/*.cxx 
     ./System/phitsPhysics/*.cxx 
     ./System/generalProcess/*.cxx 
     ./System/modelSupport/*.cxx 
     ./Model/ESSBeam/estia/*.cxx 
     ./Model/ESSBeam/cspec/*.cxx 
     ./System/constructVar/*.cxx 
     ./Model/MaxIV/R1Common/*.cxx 
     ./Model/xrayHutch/*.cxx 
     ./System/weights/*.cxx 
     ./System/flukaTally/*.cxx 
     ./Model/ESSBeam/commonVar/*.cxx 
     ./Model/MaxIV/species/*.cxx 
     ./Model/essConstruct/*.cxx 
     ./Model/ESSBeam/odin/*.cxx 
     ./Model/ESSBeam/loki/*.cxx 
     ./Model/ESSBeam/trex/*.cxx 
     ./System/tally/*.cxx 
     ./Model/t1Build/*.cxx 
     ./System/xml/*.cxx 
     ./System/funcBase/*.cxx 
     ./instrument/*.cxx 
     ./Model/ralBuild/*.cxx 
     ./Model/cuBlock/*.cxx 
     ./global/*.cxx 
     ./Model/MaxIV/commonBeam/*.cxx 
     ./Model/MaxIV/commonGenerator/*.cxx 
     ./Model/ESSBeam/simpleItem/*.cxx 
     ./Model/bibBuild/*.cxx 
     ./Model/ESSBeam/bifrost/*.cxx 
     ./Model/pipeBuild/*.cxx 
     ./Model/sinbadBuild/*.cxx 
     ./Model/lensModel/*.cxx 
     ./System/world/*.cxx 
     ./Model/MaxIV/maxpeem/*.cxx 
     ./System/mcnpProcess/*.cxx 
     ./System/poly/*.cxx 
     ./Model/ESSBeam/nnbar/*.cxx 
     ./System/magnetic/*.cxx 
     ./test/*.cxx 
     ./System/construct/*.cxx 
     ./Model/ESSBeam/freia/*.cxx 
     ./System/simMC/*.cxx 
     ./Model/maxivBuild/*.cxx 
     ./Model/moderator/*.cxx 
     ./beamline/*.cxx 
     ./Model/essLinac/*.cxx 
     ./Model/MaxIV/formax/*.cxx 
     ./Model/ESSBeam/skadi/*.cxx 
     ./Model/t2Build/*.cxx 
     ./System/work/*.cxx 
     ./Model/photon/*.cxx 
     ./System/objectMod/*.cxx 
     ./Model/MaxIV/softimax/*.cxx 
     ./System/phitsSupport/*.cxx 
     ./Model/ESSBeam/nmx/*.cxx 
     ./System/support/*.cxx 
     ./Model/singleItemBuild/*.cxx 
     ./System/visit/*.cxx 
     ./Model/ESSBeam/common/*.cxx 
     ./Model/ESSBeam/heimdal/*.cxx 
     ./Model/d4cModel/*.cxx 
     ./src/*.cxx 
     ./include/*.h 
     ./beamlineInc/*.h 
     ./globalInc/*.h 
     ./instrumentInc/*.h 
     ./scatMatInc/*.h 
     ./specialInc/*.h 
     ./transportInc/*.h 
     ./testInclude/*.h 
     ./System/attachCompInc/*.h 
     ./System/compWeightsInc/*.h 
     ./System/constructInc/*.h 
     ./System/constructVarInc/*.h 
     ./System/crystalInc/*.h 
     ./System/endfInc/*.h 
     ./System/flukaPhysicsInc/*.h 
     ./System/flukaProcessInc/*.h 
     ./System/flukaTallyInc/*.h 
     ./System/funcBaseInc/*.h 
     ./System/generalProcessInc/*.h 
     ./System/geomInc/*.h 
     ./System/inputInc/*.h 
     ./System/insertUnitInc/*.h 
     ./System/logInc/*.h 
     ./System/magneticInc/*.h 
     ./System/mcnpProcessInc/*.h 
     ./System/md5Inc/*.h 
     ./System/modelSupportInc/*.h 
     ./System/monteInc/*.h 
     ./System/objectModInc/*.h 
     ./System/phitsPhysicsInc/*.h 
     ./System/phitsProcessInc/*.h 
     ./System/phitsSupportInc/*.h 
     ./System/phitsTallyInc/*.h 
     ./System/physicsInc/*.h 
     ./System/polyInc/*.h 
     ./System/simMCInc/*.h 
     ./System/sourceInc/*.h 
     ./System/supportInc/*.h 
     ./System/tallyInc/*.h 
     ./System/visitInc/*.h 
     ./System/weightsInc/*.h 
     ./System/workInc/*.h 
     ./System/worldInc/*.h 
     ./System/xmlInc/*.h 
     ./Model/bibBuildInc/*.h 
     ./Model/cuBlockInc/*.h 
     ./Model/d4cModelInc/*.h 
     ./Model/delftInc/*.h 
     ./Model/epbBuildInc/*.h 
     ./Model/essBuildInc/*.h 
     ./Model/essConstructInc/*.h 
     ./Model/essLinacInc/*.h 
     ./Model/exampleBuildInc/*.h 
     ./Model/filterInc/*.h 
     ./Model/gammaBuildInc/*.h 
     ./Model/lensModelInc/*.h 
     ./Model/maxivBuildInc/*.h 
     ./Model/moderatorInc/*.h 
     ./Model/muonInc/*.h 
     ./Model/photonInc/*.h 
     ./Model/pipeBuildInc/*.h 
     ./Model/ralBuildInc/*.h 
     ./Model/saxsInc/*.h 
     ./Model/sinbadBuildInc/*.h 
     ./Model/singleItemBuildInc/*.h 
     ./Model/snsBuildInc/*.h 
     ./Model/t1BuildInc/*.h 
     ./Model/t2BuildInc/*.h 
     ./Model/xrayHutchInc/*.h 
     ./Model/ESSBeam/beerInc/*.h 
     ./Model/ESSBeam/bifrostInc/*.h 
     ./Model/ESSBeam/commonInc/*.h 
     ./Model/ESSBeam/commonVarInc/*.h 
     ./Model/ESSBeam/cspecInc/*.h 
     ./Model/ESSBeam/dreamInc/*.h 
     ./Model/ESSBeam/estiaInc/*.h 
     ./Model/ESSBeam/freiaInc/*.h 
     ./Model/ESSBeam/heimdalInc/*.h 
     ./Model/ESSBeam/lokiInc/*.h 
     ./Model/ESSBeam/magicInc/*.h 
     ./Model/ESSBeam/miraclesInc/*.h 
     ./Model/ESSBeam/nmxInc/*.h 
     ./Model/ESSBeam/nnbarInc/*.h 
     ./Model/ESSBeam/odinInc/*.h 
     ./Model/ESSBeam/simpleItemInc/*.h 
     ./Model/ESSBeam/skadiInc/*.h 
     ./Model/ESSBeam/testBeamInc/*.h 
     ./Model/ESSBeam/trexInc/*.h 
     ./Model/ESSBeam/vespaInc/*.h 
     ./Model/ESSBeam/vorInc/*.h 
     ./Model/MaxIV/balderInc/*.h 
     ./Model/MaxIV/commonBeamInc/*.h 
     ./Model/MaxIV/commonGeneratorInc/*.h 
     ./Model/MaxIV/cosaxsInc/*.h 
     ./Model/MaxIV/danmaxInc/*.h 
     ./Model/MaxIV/flexpesInc/*.h 
     ./Model/MaxIV/formaxInc/*.h 
     ./Model/MaxIV/LinacInc/*.h 
     ./Model/MaxIV/maxpeemInc/*.h 
     ./Model/MaxIV/micromaxInc/*.h 
     ./Model/MaxIV/R1CommonInc/*.h 
     ./Model/MaxIV/R3CommonInc/*.h 
     ./Model/MaxIV/softimaxInc/*.h 
     ./Model/MaxIV/speciesInc/*.h 
     ./Main/*.cxx 
     ./CMake.pl  
     ./CMakeList.pm 
     ./CMakeSupport.pm 
 | wc )

add_custom_target(tar  COMMAND tar zcvf ${PROJECT_SOURCE_DIR}/Master.tgz 
     ./System/flukaPhysics/*.cxx 
     ./System/md5/*.cxx 
     ./System/source/*.cxx 
     ./Model/ESSBeam/vespa/*.cxx 
     ./Model/MaxIV/micromax/*.cxx 
     ./System/physics/*.cxx 
     ./Model/ESSBeam/testBeam/*.cxx 
     ./System/input/*.cxx 
     ./System/geometry/*.cxx 
     ./System/phitsTally/*.cxx 
     ./System/attachComp/*.cxx 
     ./Model/MaxIV/balder/*.cxx 
     ./Model/MaxIV/cosaxs/*.cxx 
     ./Model/ESSBeam/dream/*.cxx 
     ./Model/essBuild/*.cxx 
     ./System/crystal/*.cxx 
     ./Model/gammaBuild/*.cxx 
     ./Model/ESSBeam/magic/*.cxx 
     ./System/compWeights/*.cxx 
     ./Model/delft/*.cxx 
     ./System/endf/*.cxx 
     ./System/insertUnit/*.cxx 
     ./transport/*.cxx 
     ./Model/MaxIV/flexpes/*.cxx 
     ./Model/exampleBuild/*.cxx 
     ./System/monte/*.cxx 
     ./System/phitsProcess/*.cxx 
     ./Model/MaxIV/R3Common/*.cxx 
     ./Model/filter/*.cxx 
     ./Model/MaxIV/Linac/*.cxx 
     ./Model/epbBuild/*.cxx 
     ./Model/ESSBeam/vor/*.cxx 
     ./Model/snsBuild/*.cxx 
     ./Model/ESSBeam/beer/*.cxx 
     ./scatMat/*.cxx 
     ./Model/ESSBeam/miracles/*.cxx 
     ./System/log/*.cxx 
     ./Model/MaxIV/danmax/*.cxx 
     ./Model/saxs/*.cxx 
     ./System/flukaProcess/*.cxx 
     ./System/phitsPhysics/*.cxx 
     ./System/generalProcess/*.cxx 
     ./System/modelSupport/*.cxx 
     ./Model/ESSBeam/estia/*.cxx 
     ./Model/ESSBeam/cspec/*.cxx 
     ./System/constructVar/*.cxx 
     ./Model/MaxIV/R1Common/*.cxx 
     ./Model/xrayHutch/*.cxx 
     ./System/weights/*.cxx 
     ./System/flukaTally/*.cxx 
     ./Model/ESSBeam/commonVar/*.cxx 
     ./Model/MaxIV/species/*.cxx 
     ./Model/essConstruct/*.cxx 
     ./Model/ESSBeam/odin/*.cxx 
     ./Model/ESSBeam/loki/*.cxx 
     ./Model/ESSBeam/trex/*.cxx 
     ./System/tally/*.cxx 
     ./Model/t1Build/*.cxx 
     ./System/xml/*.cxx 
     ./System/funcBase/*.cxx 
     ./instrument/*.cxx 
     ./Model/ralBuild/*.cxx 
     ./Model/cuBlock/*.cxx 
     ./global/*.cxx 
     ./Model/MaxIV/commonBeam/*.cxx 
     ./Model/MaxIV/commonGenerator/*.cxx 
     ./Model/ESSBeam/simpleItem/*.cxx 
     ./Model/bibBuild/*.cxx 
     ./Model/ESSBeam/bifrost/*.cxx 
     ./Model/pipeBuild/*.cxx 
     ./Model/sinbadBuild/*.cxx 
     ./Model/lensModel/*.cxx 
     ./System/world/*.cxx 
     ./Model/MaxIV/maxpeem/*.cxx 
     ./System/mcnpProcess/*.cxx 
     ./System/poly/*.cxx 
     ./Model/ESSBeam/nnbar/*.cxx 
     ./System/magnetic/*.cxx 
     ./test/*.cxx 
     ./System/construct/*.cxx 
     ./Model/ESSBeam/freia/*.cxx 
     ./System/simMC/*.cxx 
     ./Model/maxivBuild/*.cxx 
     ./Model/moderator/*.cxx 
     ./beamline/*.cxx 
     ./Model/essLinac/*.cxx 
     ./Model/MaxIV/formax/*.cxx 
     ./Model/ESSBeam/skadi/*.cxx 
     ./Model/t2Build/*.cxx 
     ./System/work/*.cxx 
     ./Model/photon/*.cxx 
     ./System/objectMod/*.cxx 
     ./Model/MaxIV/softimax/*.cxx 
     ./System/phitsSupport/*.cxx 
     ./Model/ESSBeam/nmx/*.cxx 
     ./System/support/*.cxx 
     ./Model/singleItemBuild/*.cxx 
     ./System/visit/*.cxx 
     ./Model/ESSBeam/common/*.cxx 
     ./Model/ESSBeam/heimdal/*.cxx 
     ./Model/d4cModel/*.cxx 
     ./src/*.cxx 
     ./include/*.h 
     ./beamlineInc/*.h 
     ./globalInc/*.h 
     ./instrumentInc/*.h 
     ./scatMatInc/*.h 
     ./specialInc/*.h 
     ./transportInc/*.h 
     ./testInclude/*.h 
     ./System/attachCompInc/*.h 
     ./System/compWeightsInc/*.h 
     ./System/constructInc/*.h 
     ./System/constructVarInc/*.h 
     ./System/crystalInc/*.h 
     ./System/endfInc/*.h 
     ./System/flukaPhysicsInc/*.h 
     ./System/flukaProcessInc/*.h 
     ./System/flukaTallyInc/*.h 
     ./System/funcBaseInc/*.h 
     ./System/generalProcessInc/*.h 
     ./System/geomInc/*.h 
     ./System/inputInc/*.h 
     ./System/insertUnitInc/*.h 
     ./System/logInc/*.h 
     ./System/magneticInc/*.h 
     ./System/mcnpProcessInc/*.h 
     ./System/md5Inc/*.h 
     ./System/modelSupportInc/*.h 
     ./System/monteInc/*.h 
     ./System/objectModInc/*.h 
     ./System/phitsPhysicsInc/*.h 
     ./System/phitsProcessInc/*.h 
     ./System/phitsSupportInc/*.h 
     ./System/phitsTallyInc/*.h 
     ./System/physicsInc/*.h 
     ./System/polyInc/*.h 
     ./System/simMCInc/*.h 
     ./System/sourceInc/*.h 
     ./System/supportInc/*.h 
     ./System/tallyInc/*.h 
     ./System/visitInc/*.h 
     ./System/weightsInc/*.h 
     ./System/workInc/*.h 
     ./System/worldInc/*.h 
     ./System/xmlInc/*.h 
     ./Model/bibBuildInc/*.h 
     ./Model/cuBlockInc/*.h 
     ./Model/d4cModelInc/*.h 
     ./Model/delftInc/*.h 
     ./Model/epbBuildInc/*.h 
     ./Model/essBuildInc/*.h 
     ./Model/essConstructInc/*.h 
     ./Model/essLinacInc/*.h 
     ./Model/exampleBuildInc/*.h 
     ./Model/filterInc/*.h 
     ./Model/gammaBuildInc/*.h 
     ./Model/lensModelInc/*.h 
     ./Model/maxivBuildInc/*.h 
     ./Model/moderatorInc/*.h 
     ./Model/muonInc/*.h 
     ./Model/photonInc/*.h 
     ./Model/pipeBuildInc/*.h 
     ./Model/ralBuildInc/*.h 
     ./Model/saxsInc/*.h 
     ./Model/sinbadBuildInc/*.h 
     ./Model/singleItemBuildInc/*.h 
     ./Model/snsBuildInc/*.h 
     ./Model/t1BuildInc/*.h 
     ./Model/t2BuildInc/*.h 
     ./Model/xrayHutchInc/*.h 
     ./Model/ESSBeam/beerInc/*.h 
     ./Model/ESSBeam/bifrostInc/*.h 
     ./Model/ESSBeam/commonInc/*.h 
     ./Model/ESSBeam/commonVarInc/*.h 
     ./Model/ESSBeam/cspecInc/*.h 
     ./Model/ESSBeam/dreamInc/*.h 
     ./Model/ESSBeam/estiaInc/*.h 
     ./Model/ESSBeam/freiaInc/*.h 
     ./Model/ESSBeam/heimdalInc/*.h 
     ./Model/ESSBeam/lokiInc/*.h 
     ./Model/ESSBeam/magicInc/*.h 
     ./Model/ESSBeam/miraclesInc/*.h 
     ./Model/ESSBeam/nmxInc/*.h 
     ./Model/ESSBeam/nnbarInc/*.h 
     ./Model/ESSBeam/odinInc/*.h 
     ./Model/ESSBeam/simpleItemInc/*.h 
     ./Model/ESSBeam/skadiInc/*.h 
     ./Model/ESSBeam/testBeamInc/*.h 
     ./Model/ESSBeam/trexInc/*.h 
     ./Model/ESSBeam/vespaInc/*.h 
     ./Model/ESSBeam/vorInc/*.h 
     ./Model/MaxIV/balderInc/*.h 
     ./Model/MaxIV/commonBeamInc/*.h 
     ./Model/MaxIV/commonGeneratorInc/*.h 
     ./Model/MaxIV/cosaxsInc/*.h 
     ./Model/MaxIV/danmaxInc/*.h 
     ./Model/MaxIV/flexpesInc/*.h 
     ./Model/MaxIV/formaxInc/*.h 
     ./Model/MaxIV/LinacInc/*.h 
     ./Model/MaxIV/maxpeemInc/*.h 
     ./Model/MaxIV/micromaxInc/*.h 
     ./Model/MaxIV/R1CommonInc/*.h 
     ./Model/MaxIV/R3CommonInc/*.h 
     ./Model/MaxIV/softimaxInc/*.h 
     ./Model/MaxIV/speciesInc/*.h 
     ./Main/*.cxx 
     ./CMake.pl  
     ./CMakeList.pm 
     ./CMakeSupport.pm 
 )

add_custom_target(tags  COMMAND etags  
     ${PROJECT_SOURCE_DIR}/System/flukaPhysics/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/md5/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/source/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/vespa/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/micromax/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/physics/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/testBeam/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/input/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/geometry/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/phitsTally/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/attachComp/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/balder/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/cosaxs/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/dream/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/essBuild/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/crystal/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/gammaBuild/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/magic/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/compWeights/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/delft/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/endf/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/insertUnit/*.cxx 
     ${PROJECT_SOURCE_DIR}/transport/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/flexpes/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/exampleBuild/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/monte/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/phitsProcess/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/R3Common/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/filter/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/Linac/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/epbBuild/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/vor/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/snsBuild/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/beer/*.cxx 
     ${PROJECT_SOURCE_DIR}/scatMat/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/miracles/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/log/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/danmax/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/saxs/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/flukaProcess/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/phitsPhysics/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/generalProcess/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/modelSupport/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/estia/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/cspec/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/constructVar/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/R1Common/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/xrayHutch/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/weights/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/flukaTally/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/commonVar/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/species/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/essConstruct/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/odin/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/loki/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/trex/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/tally/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/t1Build/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/xml/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/funcBase/*.cxx 
     ${PROJECT_SOURCE_DIR}/instrument/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ralBuild/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/cuBlock/*.cxx 
     ${PROJECT_SOURCE_DIR}/global/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/commonBeam/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/commonGenerator/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/simpleItem/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/bibBuild/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/bifrost/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/pipeBuild/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/sinbadBuild/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/lensModel/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/world/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/maxpeem/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/mcnpProcess/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/poly/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/nnbar/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/magnetic/*.cxx 
     ${PROJECT_SOURCE_DIR}/test/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/construct/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/freia/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/simMC/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/maxivBuild/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/moderator/*.cxx 
     ${PROJECT_SOURCE_DIR}/beamline/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/essLinac/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/formax/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/skadi/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/t2Build/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/work/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/photon/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/objectMod/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/softimax/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/phitsSupport/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/nmx/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/support/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/singleItemBuild/*.cxx 
     ${PROJECT_SOURCE_DIR}/System/visit/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/common/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/heimdal/*.cxx 
     ${PROJECT_SOURCE_DIR}/Model/d4cModel/*.cxx 
     ${PROJECT_SOURCE_DIR}/src/*.cxx 
     ${PROJECT_SOURCE_DIR}/include/*.h 
     ${PROJECT_SOURCE_DIR}/beamlineInc/*.h 
     ${PROJECT_SOURCE_DIR}/globalInc/*.h 
     ${PROJECT_SOURCE_DIR}/instrumentInc/*.h 
     ${PROJECT_SOURCE_DIR}/scatMatInc/*.h 
     ${PROJECT_SOURCE_DIR}/specialInc/*.h 
     ${PROJECT_SOURCE_DIR}/transportInc/*.h 
     ${PROJECT_SOURCE_DIR}/testInclude/*.h 
     ${PROJECT_SOURCE_DIR}/System/attachCompInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/compWeightsInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/constructInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/constructVarInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/crystalInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/endfInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/flukaPhysicsInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/flukaProcessInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/flukaTallyInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/funcBaseInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/generalProcessInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/geomInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/inputInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/insertUnitInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/logInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/magneticInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/mcnpProcessInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/md5Inc/*.h 
     ${PROJECT_SOURCE_DIR}/System/modelSupportInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/monteInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/objectModInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/phitsPhysicsInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/phitsProcessInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/phitsSupportInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/phitsTallyInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/physicsInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/polyInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/simMCInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/sourceInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/supportInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/tallyInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/visitInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/weightsInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/workInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/worldInc/*.h 
     ${PROJECT_SOURCE_DIR}/System/xmlInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/bibBuildInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/cuBlockInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/d4cModelInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/delftInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/epbBuildInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/essBuildInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/essConstructInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/essLinacInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/exampleBuildInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/filterInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/gammaBuildInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/lensModelInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/maxivBuildInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/moderatorInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/muonInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/photonInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/pipeBuildInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ralBuildInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/saxsInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/sinbadBuildInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/singleItemBuildInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/snsBuildInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/t1BuildInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/t2BuildInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/xrayHutchInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/beerInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/bifrostInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/commonInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/commonVarInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/cspecInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/dreamInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/estiaInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/freiaInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/heimdalInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/lokiInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/magicInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/miraclesInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/nmxInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/nnbarInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/odinInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/simpleItemInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/skadiInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/testBeamInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/trexInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/vespaInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/ESSBeam/vorInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/balderInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/commonBeamInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/commonGeneratorInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/cosaxsInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/danmaxInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/flexpesInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/formaxInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/LinacInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/maxpeemInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/micromaxInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/R1CommonInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/R3CommonInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/softimaxInc/*.h 
     ${PROJECT_SOURCE_DIR}/Model/MaxIV/speciesInc/*.h 
 )

