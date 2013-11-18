/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/OldHutch.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "PhysCut.h"
#include "PhysImp.h"
#include "KGroup.h"
#include "Source.h"
#include "PhysicsCards.h"
#include "shutterInfo.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "SpecialSurf.h"
#include "ModelSupport.h"
#include "chipDataStore.h"
#include "GeneralShutter.h"
#include "LinearComp.h"
#include "Hutch.h"


namespace hutchSystem
{

ChipIRHutch::ChipIRHutch(const int N,const std::string& Key)  : 
  LinearComp(),hutchIndex(N),keyName(Key),cellIndex(N+1),populated(0),
  collimatorVoid(0),beamStopCell(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param N :: Number for surface/cell offset
    \param Key :: Keyname for variables
  */
{}

ChipIRHutch::~ChipIRHutch() 
  /*!
    Destructor
  */
{}

ChipIRHutch::ChipIRHutch(const ChipIRHutch& A) : 
  LinearComp(A),hutchIndex(A.hutchIndex),keyName(A.keyName)  /*!
    Copy constructor
    \param A :: ChipIRHutch to copy
  */
{};

ChipIRHutch&
ChipIRHutch::operator=(const ChipIRHutch& A)
  /*!
    Assignment operator
    \param A :: ChipIRHutch to copy
    \return *this
  */
{
  if (this!=&A)
    {
      LinearComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      CentPoint=A.CentPoint;
    }
  return *this;
}

void
ChipIRHutch::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  const FuncDataBase& Control=System.getDataBase();

  beamAngle=Control.EvalVar<double>("chipSndAngle");
  hYLen=Control.EvalVar<double>(keyName+"YLen");
  hYFlat=Control.EvalVar<double>(keyName+"YFlat");
  hYNose=Control.EvalVar<double>(keyName+"YNose");
  hGuideWidth=Control.EvalVar<double>(keyName+"GuideWidth");
  hNoseOutW=Control.EvalVar<double>(keyName+"NoseOutW");
  hFLWidth=Control.EvalVar<double>(keyName+"FrontWidth");
  hFLWidth=Control.EvalVar<double>(keyName+"FrontWidth");
  hFullWidth=Control.EvalVar<double>(keyName+"FullWidth");
  hYSlopeDist=Control.EvalVar<double>(keyName+"YSlopeDist");
  hRXStep=Control.EvalVar<double>(keyName+"RXStep");
  hRoof=Control.EvalVar<double>(keyName+"Roof");
  hCollRoof=Control.EvalVar<double>(keyName+"CollRoof");
  hFloor=Control.EvalVar<double>(keyName+"Floor");
  hFloorDepth=Control.EvalVar<double>(keyName+"FloorDepth");
  hHangSteel=Control.EvalVar<double>(keyName+"HangSteel");
  hHangDepth=Control.EvalVar<double>(keyName+"HangDepth");
  hHangLift=Control.EvalVar<double>(keyName+"HangLift");
  hRoofSteel=Control.EvalVar<double>(keyName+"RoofSteel");
  hRWallSteel=Control.EvalVar<double>(keyName+"RightWallSteel");
  hLWallSteel=Control.EvalVar<double>(keyName+"LeftWallSteel");
  hBWallSteel=Control.EvalVar<double>(keyName+"BackWallSteel");
  hSideAngle=Control.EvalVar<double>(keyName+"SideAngle");

  iFrontTrimLen=Control.EvalVar<double>(keyName+"FrontTrimLen");
  iFrontTrimWidth=Control.EvalVar<double>(keyName+"FrontTrimWidth");
  iFrontTrimHeight=Control.EvalVar<double>(keyName+"FrontTrimHeight");
  iFrontTrimDepth=Control.EvalVar<double>(keyName+"FrontTrimDepth");
  screenY=Control.EvalVar<double>(keyName+"ScreenY");
  // Tables:
  t1Height=Control.EvalVar<double>(keyName+"Table1Height");
  t1Width=Control.EvalVar<double>(keyName+"Table1Width");
  t1Len=Control.EvalVar<double>(keyName+"Table1Length");
  t1MidY=Control.EvalVar<double>(keyName+"Table1MidY");

  t2Height=Control.EvalVar<double>(keyName+"Table2Height");
  t2Width=Control.EvalVar<double>(keyName+"Table2Width");
  t2Len=Control.EvalVar<double>(keyName+"Table2Length");
  t2MidY=Control.EvalVar<double>(keyName+"Table2MidY");

  roofMat=Control.EvalVar<int>(keyName+"RoofMat");
  wallMat=Control.EvalVar<int>(keyName+"WallMat");  
  floorMat=Control.EvalVar<int>(keyName+"FloorMat");
  tableMat=Control.EvalVar<int>(keyName+"TableMat");
  trimMat=Control.EvalVar<int>(keyName+"TrimMat");
  bsWallMat=Control.EvalVar<int>(keyName+"BSWallMat");
  bsMat=Control.EvalVar<int>(keyName+"BSMat");
  screenMat=Control.EvalVar<int>(keyName+"ScreenMat");

  // Beam stop:
  wYDist=Control.EvalVar<double>(keyName+"WalkY");
  wXWrap=Control.EvalVar<double>(keyName+"WalkXWrap");
  wWallThick=Control.EvalVar<double>(keyName+"WalkWallThick");
  wXSnd=Control.EvalVar<double>(keyName+"WalkSndX");
  bsYStart=Control.EvalVar<double>(keyName+"BeamStopYStart");
  bsYFullLen=Control.EvalVar<double>(keyName+"BeamStopFullLen");
  bsYLen=Control.EvalVar<double>(keyName+"BeamStopLen");
  bsYMidThick=Control.EvalVar<double>(keyName+"BeamStopMidThick");
  bsXWidth=Control.EvalVar<double>(keyName+"BeamStopXWidth");
  bsYBackWall=Control.EvalVar<double>(keyName+"BeamStopYBackWall");
  bsXSideWall=Control.EvalVar<double>(keyName+"BeamStopXSideWall");
  bsRoof=Control.EvalVar<double>(keyName+"BeamStopRoof");
  bsRoofThick=Control.EvalVar<double>(keyName+"BeamStopRoofThick");
  bsBase=Control.EvalVar<double>(keyName+"BeamStopBase");

  // Wall dividers:
  nWallDivide=Control.EvalVar<int>(keyName+"WallNDivide");
  nFloorDivide=Control.EvalVar<int>(keyName+"FloorNDivide");
  nRoofDivide=Control.EvalVar<int>(keyName+"RoofNDivide");


  ModelSupport::populateDivide(Control,nWallDivide,
			      keyName+"WallMat_",wallMat,wallMatList);
  ModelSupport::populateDivide(Control,nFloorDivide,
			      keyName+"FloorMat_",floorMat,floorMatList);
  ModelSupport::populateDivide(Control,nRoofDivide,
			      keyName+"RoofMat_",roofMat,roofMatList);
  
  ModelSupport::populateDivide(Control,nWallDivide,
			       keyName+"WallFrac_",wallFrac);
  ModelSupport::populateDivide(Control,nFloorDivide,
			       keyName+"FloorFrac_",floorFrac);
  ModelSupport::populateDivide(Control,nRoofDivide,
			       keyName+"RoofFrac_",roofFrac);
  
  populated=1;
  return;
}

void
ChipIRHutch::createUnitVector(const shutterSystem::GeneralShutter& GI,
			      const zoomSystem::LinearComp& LC)
/*!
    Create the unit vectors
    \param GI :: shutter direction
    \param LC :: connectin linear component
  */
{
  ELog::RegMethod RegA("ChipIRHutch","createUnitVector");

  //  const masterRotate& MI=masterRotate::Instance();

  Z=Geometry::Vec3D(-1,0,0);               // Gravity axis [up]
  X=LC.getX();                             // horrizontal axis [across]
  Y=(Z*X);                                 // forward axis [centre line]
  entryY=LC.getExitNorm(); 

  //  CentPoint=GI.getOrigin();           // Shutter view point
  CentPoint=LC.getExit();           // Shutter view point
  
  BeamAxis=LC.getExitNorm();

  Geometry::Quaternion::calcQRotDeg(beamAngle,X).rotate(BeamAxis);

  ImpactPoint=GI.getFrontPt();
  //  Geometry::Quaternion::calcQRotDeg(beamAngle,X).rotate(Z);
  Geometry::Quaternion::calcQRotDeg(beamAngle,X).rotate(Y);

  const Geometry::Vec3D Saxis=
    masterRotate::Instance().calcAxisRotate(GI.getAxis());
  chipIRDatum::chipDataStore::Instance().
    setCNum(chipIRDatum::shutterAxis,Saxis);

  return;
}

void 
ChipIRHutch::createWallObjects(Simulation& System)
  /*!
    Create the walls for the chipIR hutch
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("ChipIRHutch","createWallObjects");
  // Create outer virtual void
  std::string Out;
  // ---------------
  // OUTER FIXTURES:
  // ----------------
  
  // Nose area [void] 11 -1 333 -334 -36 35
  Out=ModelSupport::getComposite(SMap,hutchIndex,"31 -1 13 -14 5 -26");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  collimatorVoid=cellIndex-1;

  // void in hanging space
  Out=ModelSupport::getComposite(SMap,hutchIndex,"-36 35 -31 11 333 -334");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  //  // // Nose area [void] 11 -1 333 -334 -36 35
  //  Out=ModelSupport::getComposite(SMap,hutchIndex,"11 -1 13 -14 5 -36");
  //  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Roof
  Out=ModelSupport::getComposite(SMap,hutchIndex,"-6 26 11 -2 23  4 -24 (-3 : 12)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,roofMat,0.0,Out));
  roofCell=cellIndex-1;

  // Floor                                        		    
  Out=ModelSupport::getComposite(SMap,hutchIndex,"55 -5 11 23 4 -24 (-304:-2) (-204:-302) (-3:12) -202");
  System.addCell(MonteCarlo::Qhull(cellIndex++,floorMat,0.0,Out));
  
  // Front Hangdown
  Out=ModelSupport::getComposite(SMap,hutchIndex,
	       "5 -26 11 -31 13 -14 (36 : -333 : 334 : -35)");  
  System.addCell(MonteCarlo::Qhull(cellIndex++,roofMat,0.0,Out));

  // Wall Sections:
  // LEFT:
  Out=ModelSupport::getComposite(SMap,hutchIndex,"5 -26 11 (14 : 21) (44 : (-34  -32)) -2  4  -24 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  leftWallCell=cellIndex-1;

  // Create Wall Sections:
  // RIGHT:
  //  std::string Out=ModelSupport::getComposite(hutchIndex,"-5 6 11 -2  23 4 -24 (-3:12)");
  Out=ModelSupport::getComposite(SMap,hutchIndex,"5 -26 11 -2 23 (-3:12) (-13 : 21)  (-22 : -43) 33");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  rightWallCell=cellIndex-1;

  // Back Wall with hole for beamstop
  Out=ModelSupport::getComposite(SMap,hutchIndex,"5 -26 -2 32 43 -44  34 (-303:304:306:302)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  backWallCell=cellIndex-1;

  // ---------------
  // INNER FIXTURES:
  // ----------------
  // First trimmer:             
  Out=ModelSupport::getComposite(SMap,hutchIndex,"5 -26 (-33 : 13)  (34 : -14) 1 -21 (-45 : 46 : -53 : 54)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,trimMat,0.0,Out));

  // Void in trimmer
  Out=ModelSupport::getComposite(SMap,hutchIndex,"45 -46 53 -54 811 -21");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  if (screenY>0)
    {
      // Screen infront of trimmer
      Out=ModelSupport::getComposite(SMap,hutchIndex,"45 -46 53 -54 1 -811");
      System.addCell(MonteCarlo::Qhull(cellIndex++,screenMat,0.0,Out));
    } 

  // Table one: 
  Out=ModelSupport::getComposite(SMap,hutchIndex,"5 -106 101 -102 103 -104");
  System.addCell(MonteCarlo::Qhull(cellIndex++,tableMat,0.0,Out));

  // Table two:
  Out=ModelSupport::getComposite(SMap,hutchIndex,"5 -116 111 -112 113 -114");
  System.addCell(MonteCarlo::Qhull(cellIndex++,tableMat,0.0,Out));

  // Void around tables:
  Out=ModelSupport::getComposite(SMap,hutchIndex,"5 -26 21 (-33 : 22 43) 34 -44 -301 "
		   "(-101 : 102 : -103 : 104 : 106) "
		   "(-111 : 112 : -113 : 114 : 116) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  //back end void [uses beamstop cutter]
  // Special case if beamstop has intercepted:
  Out=ModelSupport::getComposite(SMap,hutchIndex,"5 -26 34 -44 301 -32 (-33 : 22) 43");
  System.addCell(MonteCarlo::Qhull(cellIndex,0,0.0,Out));
  MonteCarlo::Qhull* Pcell=System.findQhull(cellIndex);
  Pcell->addSurfString(" #150000 ");
  cellIndex++;

  //  Out=ModelSupport::getComposite(SMap,hutchIndex,"-5");
  return;
}

void
ChipIRHutch::addOuterVoid(Simulation& System)
  /*!
    Create outer virtual space that includes the beamstop etc
    \param System :: Simulation to add to 
   */
{
  ELog::RegMethod RegA("ChipIRHutch","addOuterVoid");

  std::string Out=ModelSupport::getComposite(SMap,hutchIndex,
		    "55 -6 11 23 4 -24 (-304:-2) (-204:-302) (-3:12) -202");
  
  System.addCell(MonteCarlo::Qhull(100000,0,0.0,Out));
  System.makeVirtual(100000);        

  MonteCarlo::Qhull* OuterVoid=System.findQhull(74123);
  if (OuterVoid)
    OuterVoid->addSurfString(" #100000 "); 
  else
    ELog::EM.error("Failed to find OuterVoid ");
  return;
}

void
ChipIRHutch::createBeamStopSurfaces()
  /*!
    Create the beamstop area
  */
{
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  Geometry::Plane* PX;
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+203);   // Inside number
  PX->setPlane(CentPoint-X*(hRXStep+hFullWidth/2.0-wWallThick),X);
  SMap.registerSurf(hutchIndex+203,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+204);   // Outside wall
  PX->setPlane(CentPoint-X*(hRXStep+hFullWidth/2.0-wXSnd),X);
  SMap.registerSurf(hutchIndex+204,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+214);   // Outside wall
  PX->setPlane(CentPoint-X*(hRXStep+hFullWidth/2.0+wWallThick-wXSnd),X);
  SMap.registerSurf(hutchIndex+214,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+213);   // Door point
  PX->setPlane(CentPoint-X*(hRXStep+hFullWidth/2.0-wXWrap),X);
  SMap.registerSurf(hutchIndex+213,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+202);   // outside wall
  PX->setPlane(CentPoint+Y*(hYLen+wYDist),Y);
  SMap.registerSurf(hutchIndex+202,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+201);   // Inside number
  PX->setPlane(CentPoint+Y*(hYLen+wYDist-wWallThick),Y);
  SMap.registerSurf(hutchIndex+201,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+301);   // Front beamstop
  PX->setPlane(CentPoint+Y*bsYStart,Y);
  SMap.registerSurf(hutchIndex+301,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+302);   // Back beamstop
  PX->setPlane(CentPoint+Y*(bsYStart+bsYFullLen),Y);
  SMap.registerSurf(hutchIndex+302,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+303);   // -ve BS side
  PX->setPlane(CentPoint-X*(bsXWidth/2.0),X);
  SMap.registerSurf(hutchIndex+303,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+304);   // +ve BS side
  PX->setPlane(CentPoint+X*(bsXWidth/2.0),X);
  SMap.registerSurf(hutchIndex+304,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+306);   // BS roof top
  //  PX->setPlane(Z*bsRoof,Z);
  PX->setPlane(CentPoint+Z*bsRoof,Z);
  SMap.registerSurf(hutchIndex+306,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+316);   // BS roof inner
  // PX->setPlane(Z*(bsRoof-bsRoofThick),Z);  
  PX->setPlane(CentPoint+Z*(bsRoof-bsRoofThick),Z);
  SMap.registerSurf(hutchIndex+316,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+311);   // beamstop mid wall
  PX->setPlane(CentPoint+Y*(bsYStart+bsYLen),Y);
  SMap.registerSurf(hutchIndex+311,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+312);   // Back beamstop
  PX->setPlane(CentPoint+Y*(bsYStart+bsYFullLen-bsYBackWall),Y);
  SMap.registerSurf(hutchIndex+312,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+322);   // beamstop mid wall other si
  PX->setPlane(CentPoint+Y*(bsYStart+bsYLen+bsYMidThick),Y);
  SMap.registerSurf(hutchIndex+322,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+313);   // -ve BS side wall
  PX->setPlane(CentPoint-X*(-bsXSideWall+bsXWidth/2.0),X);
  SMap.registerSurf(hutchIndex+313,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+314);   // +ve BS side wsll
  PX->setPlane(CentPoint+X*(-bsXSideWall+bsXWidth/2.0),X);
  SMap.registerSurf(hutchIndex+314,PX);


  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+315);   // BS roof inner  
  PX->setPlane(CentPoint-Z*bsBase,Z);
  //  PX->setPlane(-Z*(hFloor-bsBase),Z);
  SMap.registerSurf(hutchIndex+315,PX);

  // Calc centroid
  serverCent=calcCentroid(316,313,312,315,322,314);

  return;
}

void 
ChipIRHutch::createWallSurfaces(const zoomSystem::LinearComp& Guide)
  /*!
    Create the walls for the hutch
    \param Guide :: Object this is joined to
  */
{
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  const masterRotate& MR=masterRotate::Instance();
  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();

  Geometry::Plane* PX;
  // ---------------------------------------------------
  // OUTER WALLS:
  // ---------------------------------------------------
  SMap.addMatch(hutchIndex+11,Guide.getExitSurface());

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+1);   // front face 
  PX->setPlane(CentPoint+Y*hYNose,Y);
  SMap.registerSurf(hutchIndex+1,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+811);   // screen material
  PX->setPlane(CentPoint+Y*(hYNose+screenY),Y);
  SMap.registerSurf(hutchIndex+811,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+2);    // back wall
  PX->setPlane(CentPoint+Y*hYLen,Y);
  SMap.registerSurf(hutchIndex+2,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+12);   // Instep-right
  PX->setPlane(CentPoint+Y*hYSlopeDist,Y);
  SMap.registerSurf(hutchIndex+2,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+32);    // back wall inner
  PX->setPlane(CentPoint+Y*(hYLen-hBWallSteel),Y);
  SMap.registerSurf(hutchIndex+32,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+333);   // Nose on right
  PX->setPlane(CentPoint-X*hGuideWidth/2.0,X);
  SMap.registerSurf(hutchIndex+333,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+334);   // Nose on left
  PX->setPlane(CentPoint+X*hGuideWidth/2.0,X);
  SMap.registerSurf(hutchIndex+334,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+13);   // Nose on right
  PX->setPlane(CentPoint-X*hNoseWidth/2.0,X);
  SMap.registerSurf(hutchIndex+13,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+14);   // Nose on left
  PX->setPlane(CentPoint+X*hNoseWidth/2.0,X);
  SMap.registerSurf(hutchIndex+14,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+23);   // Flat on right
  PX->setPlane(CentPoint-X*(hRXStep+hFullWidth/2.0),X);
  SMap.registerSurf(hutchIndex+23,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+43);   // Flat on right (inner)
  PX->setPlane(CentPoint-X*(hRXStep+hFullWidth/2.0-hRWallSteel),X);
  SMap.registerSurf(hutchIndex+43,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+24);   // Flat on left
  PX->setPlane(CentPoint+X*hFullWidth/2.0,X);
  SMap.registerSurf(hutchIndex+24,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+44);   // Flat on left (inner)
  PX->setPlane(CentPoint+X*(hFullWidth/2.0-hLWallSteel),X);
  SMap.registerSurf(hutchIndex+44,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+22);   // Inside number
  PX->setPlane(CentPoint+Y*(hYSlopeDist+hRWallSteel),Y);
  SMap.registerSurf(hutchIndex+22,PX);

  // LEFT SIDE ANGLE:
  Geometry::Vec3D APt=CentPoint+X*hFLWidth;
  Geometry::Vec3D BPt=CentPoint+X*(hFLWidth+hFullLWidth);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+4);  
  PX->setPlane(APt,BPt,BPt+Z);
  SMap.registerSurf(hutchIndex+4,PX);
  // Left side Inner: (make from previous -normal)
  APt-=PX->getNormal()*hLWallSteel;
  BPt-=PX->getNormal()*hLWallSteel;
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+34);  
  PX->setPlane(APt,BPt,BPt+Z);
  SMap.registerSurf(hutchIndex+34,PX);
  
  // RIGHT SIDE ANGLE:
  sidePt=CentPoint-X*(hFrontWidth/2.0)+Y*(hYNose+iFrontTrimLen);
  YA=X;
  Geometry::Quaternion::calcQRotDeg(hSideAngle/2.0,Z).rotate(YA);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+33);  
  PX->setPlane(sidePt,-YA);
  SMap.registerSurf(hutchIndex+33,PX);
  CS.setDNum(chipIRDatum::hutRApex,MR.calcRotate(sidePt));
  sidePt-=YA*hRWallSteel;
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+3);  
  PX->setPlane(sidePt,-YA);
  SMap.registerSurf(hutchIndex+3,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+55);   // subFloor
  PX->setPlane(-Z*(hFloor+hFloorDepth),Z);
  SMap.registerSurf(hutchIndex+55,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+5);   // Floor
  PX->setPlane(-Z*hFloor,Z);
  SMap.registerSurf(hutchIndex+5,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+6);   // Roof
  PX->setPlane(Z*hRoof,Z);
  SMap.registerSurf(hutchIndex+6,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+26);   // Roof steel
  PX->setPlane(Z*(hRoof-hRoofSteel),Z);
  SMap.registerSurf(hutchIndex+26,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+36);   // Roof Hang down
  PX->setPlane(Z*(hRoof-hRoofSteel-hHangDepth),Z);
  SMap.registerSurf(hutchIndex+36,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+35);   // Floor Hang up
  PX->setPlane(Z*(-hFloor+hHangLift),Z);
  SMap.registerSurf(hutchIndex+35,PX);
  // ---------------
  // INNER FIXTURES:
  // ----------------
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+31);   // front hangdown
  PX->setPlane(CentPoint+Y*hHangSteel,Y);
  SMap.registerSurf(hutchIndex+31,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+21);   // front trimmer
  PX->setPlane(CentPoint+Y*(hYNose+iFrontTrimLen),Y);
  SMap.registerSurf(hutchIndex+21,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+53);   // front trimmer inset
  PX->setPlane(CentPoint-X*iFrontTrimWidth/2.0,X);
  SMap.registerSurf(hutchIndex+53,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+54);   // front trimmer inset
  PX->setPlane(CentPoint+X*iFrontTrimWidth/2.0,X);
  SMap.registerSurf(hutchIndex+54,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+46);   // Roof of trimmer
  PX->setPlane(Z*iFrontTrimHeight,Z);
  SMap.registerSurf(hutchIndex+46,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+45);   //  Floor of trimmer
  PX->setPlane(-Z*iFrontTrimDepth,Z);
  SMap.registerSurf(hutchIndex+45,PX);
  //------------------
  //    TABLES
  //------------------
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+101);   
  PX->setPlane(CentPoint+Y*(t1MidY-(t1Len/2.0)),Y);
  SMap.registerSurf(hutchIndex+101,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+102);   
  PX->setPlane(CentPoint+Y*(t1MidY+(t1Len/2.0)),Y);
  SMap.registerSurf(hutchIndex+102,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+103);   
  PX->setPlane(CentPoint-X*(t1Width/2.0),X);
  SMap.registerSurf(hutchIndex+103,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+104);   
  PX->setPlane(CentPoint+X*(t1Width/2.0),X);
  SMap.registerSurf(hutchIndex+104,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+106);   
  PX->setPlane(CentPoint+Z*t1Height,Z);
  SMap.registerSurf(hutchIndex+106,PX);
  // Upmove to hit correct z:


  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+111);   
  PX->setPlane(CentPoint+Y*(t2MidY-(t2Len/2.0)),Y);
  SMap.registerSurf(hutchIndex+111,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+112);   
  PX->setPlane(CentPoint+Y*(t2MidY+t2Len/2.0),Y);
  SMap.registerSurf(hutchIndex+112,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+113);   
  PX->setPlane(CentPoint-X*(t2Width/2.0),X);
  SMap.registerSurf(hutchIndex+113,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+114);   
  PX->setPlane(CentPoint+X*(t2Width/2.0),X);
  SMap.registerSurf(hutchIndex+114,PX);
  PX=SurI.createUniqSurf<Geometry::Plane>(hutchIndex+116);   
  PX->setPlane(CentPoint+Z*t2Height,Z);
  SMap.registerSurf(hutchIndex+116,PX);

  // Create Table Output
  return;

}

void
ChipIRHutch::createBeamStopObjects(Simulation& System)
  /*!
    Create the beamstop objects
    \param System :: Simulation to set
   */
{
  // Create Virtual hole for beamstop
  
  // Create outer virtual void
  std::string Out=ModelSupport::getComposite(SMap,hutchIndex,"301 -302 303 -304 5 -306");
  System.addCell(MonteCarlo::Qhull(150000,0,0.0,Out));
  System.makeVirtual(150000);        
  
  // Create Beamsop outer walls [not roof]
  Out=ModelSupport::getComposite(SMap,hutchIndex,"301 -302 303 -304 5 -306 (312:314:-313:-315:316)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,bsWallMat,0.0,Out));
  
  // Create Beamsop Mid wall
  Out=ModelSupport::getComposite(SMap,hutchIndex,"311 -322 313 -314 315 -316");
  System.addCell(MonteCarlo::Qhull(cellIndex++,bsWallMat,0.0,Out));
  // Void in Beamstop
  Out=ModelSupport::getComposite(SMap,hutchIndex,"322 -312 313 -314 315 -316");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  // Beamstop
  Out=ModelSupport::getComposite(SMap,hutchIndex,"301 -311 313 -314 315 -316");
  System.addCell(MonteCarlo::Qhull(cellIndex++,bsMat,0.0,Out));
  beamStopCell=cellIndex-1;
  // Get the centroid point:
  beamStopCent=calcCentroid(301,313,315,311,314,316);
    
  // Create walkway
  Out=ModelSupport::getComposite(SMap,hutchIndex,"23  (-203:201) 2 -202 -213 5 -316");
  System.addCell(MonteCarlo::Qhull(cellIndex++,bsWallMat,0.0,Out));

 // Walkway second bit
  Out=ModelSupport::getComposite(SMap,hutchIndex,"-204 214 -202 302 5 -316");
  System.addCell(MonteCarlo::Qhull(cellIndex++,bsWallMat,0.0,Out));

 // Door
  Out=ModelSupport::getComposite(SMap,hutchIndex,"-214 213 201 -202 5 -316");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // BS roof
  Out=ModelSupport::getComposite(SMap,hutchIndex,"2 -202 23  316 -306 -204 (302:-303)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,bsWallMat,0.0,Out));

  // Void at walkway
  Out=ModelSupport::getComposite(SMap,hutchIndex,"2 -201 203 5 -316 (302:-303) -214 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  // Void at above blockhouse
  Out=ModelSupport::getComposite(SMap,hutchIndex,"2 -202 23 -304 (-204:-302) 306 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  return;
}

void
ChipIRHutch::writeMasterPoints() const
  /*!
    Write out the master points
  */
{
  ELog::RegMethod RegA("ChipIRHutch","writeMasterPoints");
  const masterRotate& MR=masterRotate::Instance();
  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();
  //.setCNum(chipIRDatum::shutterAxis,Saxis);

  CS.setCNum(chipIRDatum::tubeAxis,MR.calcAxisRotate(BeamAxis));
  CS.setCNum(chipIRDatum::secScatImpact,MR.calcRotate(ImpactPoint));
  CS.setCNum(chipIRDatum::collCentreStart,MR.calcRotate(CentPoint));
  CS.setCNum(chipIRDatum::table1Cent,MR.calcRotate(calcTable(0)));
  CS.setCNum(chipIRDatum::table2Cent,MR.calcRotate(calcTable(1)));
  const Geometry::Vec3D BStop=CentPoint+Y*bsYStart;
  CS.setCNum(chipIRDatum::beamStop,MR.calcRotate(BStop));

  // Calculate the points that the beamstop holds:
  const int aPln=SMap.realSurf(hutchIndex+301);  
  int sPln[4];
  for(int i=0;i<4;i++)
    sPln[i]=SMap.realSurf(hutchIndex+313+i);  

  std::vector<Geometry::Vec3D> Inter;

  ModelSupport::calcVertex(aPln,sPln[0],sPln[2],Inter,0);
  CS.setDNum(chipIRDatum::flood1,MR.calcRotate(Inter.back()));
  Inter.clear();

  ModelSupport::calcVertex(aPln,sPln[0],sPln[3],Inter,0);
  CS.setDNum(chipIRDatum::flood2,MR.calcRotate(Inter.back()));
  Inter.clear();

  ModelSupport::calcVertex(aPln,sPln[1],sPln[2],Inter,0);
  CS.setDNum(chipIRDatum::flood3,MR.calcRotate(Inter.back()));
  Inter.clear();

  ModelSupport::calcVertex(aPln,sPln[1],sPln[3],Inter,0);
  CS.setDNum(chipIRDatum::flood4,MR.calcRotate(Inter.back()));
  Inter.clear();

  // Calculate DPoints etc:
  
  return;
}

int
ChipIRHutch::isObjectContained(Simulation& System,
			       const int PrimaryObj,const int SecondaryObj)
  /*!
    Compare two object to find if they intersect.
    \param System :: Simulation to use
    \param PrimaryObj :: Object to calculate if extents to Secondary
    \param SecondaryObj :: Object to check points exist in
    \return 1 on intersection / 0 on no intersection
    \todo to be completed
  */
{
  ELog::RegMethod RegA("ChipIRHutch","isObjectContained");

  MonteCarlo::Qhull* POptr=System.findQhull(PrimaryObj);
  MonteCarlo::Qhull* SOptr=System.findQhull(SecondaryObj);
  if (!POptr || !SOptr)
    {
      ELog::EM<<"Failed to find object "<<PrimaryObj<<" "
	      <<SecondaryObj<<ELog::endErr;
      return 0;
    }

  if (!POptr->hasIntersections())
    POptr->calcIntersections();

  return 0;
}

Geometry::Vec3D
ChipIRHutch::calcTable(const int Index) const
  /*!
    Caclulate the table position
    \param Index :: index of table  [0/1]
    \return Point of the table
   */
{
  switch (Index) 
    {
    case 0:
      return CentPoint+Y*t1MidY;
    case 1:
      return CentPoint+Y*t2MidY;
    }
  throw ColErr::IndexError<int>(Index,2,"ChipIRHutch::caclTable");
} 

void 
ChipIRHutch::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("Hutch","LayerProcess");
  
  if (nRoofDivide>1)
    {
      ModelSupport::surfDivide DA;
      // Generic
      for(int i=0;i<nRoofDivide-1;i++)
	{
	  DA.addFrac(roofFrac[i]);
	  DA.addMaterial(roofMatList[i]);
	}
      DA.addMaterial(roofMatList.back());

      DA.init();
      
      DA.setCellN(roofCell);
      DA.setOutNum(cellIndex,hutchIndex+501);
      DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+26),
				   -SMap.realSurf(hutchIndex+6));
      DA.activeDivide(System);
      cellIndex=DA.getCellNum();
    }

  if (nWallDivide>1)
    {
      // PROCESS WALLS:
      ModelSupport::surfDivide DA;
      
      // Generic
      for(int i=0;i<nWallDivide-1;i++)
	{
	  DA.addFrac(wallFrac[i]);
	  DA.addMaterial(wallMatList[i]);
	}
      DA.addMaterial(wallMatList.back());

      // Cell Specific:
      // BACK WALL:
      DA.setCellN(backWallCell);
      DA.setOutNum(cellIndex,hutchIndex+601);
      DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+32),
				   -SMap.realSurf(hutchIndex+2));
      DA.activeDivide(System);

      // LEFT WALL:
      DA.init();
      DA.setCellN(leftWallCell);
      DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+34),
				   SMap.realSurf(hutchIndex+4));
      DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+44),
				   -SMap.realSurf(hutchIndex+24));
      DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+14),
				   -SMap.realSurf(hutchIndex+64));
      DA.addInnerSingle(SMap.realSurf(hutchIndex+21));
      DA.activeDivide(System);
      
      // RIGHT WALL
      DA.init();
      DA.setCellN(rightWallCell);
      DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+33),
				   -SMap.realSurf(hutchIndex+3));
      DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+43),
				   SMap.realSurf(hutchIndex+23));
      DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+13),
				   SMap.realSurf(hutchIndex+63));
      DA.makePair<Geometry::Plane>(SMap.realSurf(hutchIndex+22),
				   SMap.realSurf(hutchIndex+12));
      DA.addInnerSingle(SMap.realSurf(hutchIndex+21));
      DA.activeDivide(System);
    }
  return;
}

int
ChipIRHutch::exitWindow(const double,std::vector<int>&,Geometry::Vec3D&) const
{
  throw ColErr::AbsObjMethod("ChipIRHutch::exitWindow");
}

void
ChipIRHutch::createAll(Simulation& System,
		       const shutterSystem::GeneralShutter& GI,
		       const zoomSystem::LinearComp& LC) 
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param GI :: Shutter to build beam along
    \param LC :: Linear Comp
  */
{
  ELog::RegMethod RegA("ChipIRHutch","createAll");

  populate(System);
  createUnitVector(GI,LC);
  createWallSurfaces(LC);

  createBeamStopSurfaces();
  createBeamStopObjects(System);
  createWallObjects(System);
  addOuterVoid(System);
  layerProcess(System);
  writeMasterPoints();

  return;
}

Geometry::Vec3D
ChipIRHutch::calcCentroid(const int pA,const int pB,const int pC,
			  const int pX,const int pY,const int pZ) const
  /*!
    Calculate the centroid between the two intersecting corners
    \param pA :: Plane on first corner
    \param pB :: Plane on first corner
    \param pC :: Plane on first corner
    \param pX :: Plane on second corner
    \param pY :: Plane on second corner
    \param pZ :: Plane on second corner
    \return Centroid
   */
{
  std::vector<Geometry::Vec3D> OutVec;
  OutVec=SurInter::makePoint(dynamic_cast<Geometry::Plane*>(SMap.realSurfPtr(hutchIndex+pA)),
			     dynamic_cast<Geometry::Plane*>(SMap.realSurfPtr(hutchIndex+pB)),
			     dynamic_cast<Geometry::Plane*>(SMap.realSurfPtr(hutchIndex+pC)));
  Geometry::Vec3D OutPt=OutVec.front();
  OutVec=SurInter::makePoint(dynamic_cast<Geometry::Plane*>(SMap.realSurfPtr(hutchIndex+pX)),
			     dynamic_cast<Geometry::Plane*>(SMap.realSurfPtr(hutchIndex+pY)),
			     dynamic_cast<Geometry::Plane*>(SMap.realSurfPtr(hutchIndex+pZ)));
  OutPt+=OutVec.front();
  OutPt/=2.0;
  return OutPt;
}
  
}  // NAMESPACE shutterSystem

