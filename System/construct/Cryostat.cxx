/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/Cryostat.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <memory>
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
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
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Cryostat.h"

namespace constructSystem
{

Cryostat::Cryostat(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  cryIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(cryIndex+1),active(1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

Cryostat::Cryostat(const Cryostat& A) : 
  attachSystem::FixedOffset(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  cryIndex(A.cryIndex),cellIndex(A.cellIndex),
  active(A.active),sampleZOffset(A.sampleZOffset),
  sampleRadius(A.sampleRadius),sampleHeight(A.sampleHeight),
  voidRadius(A.voidRadius),voidDepth(A.voidDepth),
  voidHeight(A.voidHeight),voidWallThick(A.voidWallThick),
  stickLen(A.stickLen),stickRadius(A.stickRadius),
  stickBoreRadius(A.stickBoreRadius),
  stickBoreHeight(A.stickBoreHeight),
  stickBoreThick(A.stickBoreThick),heatRadius(A.heatRadius),
  heatDepth(A.heatDepth),heatHeight(A.heatHeight),
  heatThick(A.heatThick),heatOuterRadius(A.heatOuterRadius),
  heatOuterLift(A.heatOuterLift),
  liqHeInnerRadius(A.liqHeInnerRadius),
  liqHeOuterRadius(A.liqHeOuterRadius),
  liqHeWallThick(A.liqHeWallThick),liqHeHeight(A.liqHeHeight),
  liqHeExtra(A.liqHeExtra),
  liqN2InnerRadius(A.liqN2InnerRadius),
  liqN2OuterRadius(A.liqN2OuterRadius),
  liqN2WallThick(A.liqN2WallThick),liqN2Height(A.liqN2Height),
  tailRadius(A.tailRadius),tailDepth(A.tailDepth),
  tailHeight(A.tailHeight),tailThick(A.tailThick),
  tailOuterRadius(A.tailOuterRadius),
  tailOuterHeight(A.tailOuterHeight),
  tailOuterLift(A.tailOuterLift),mainThick(A.mainThick),
  roofThick(A.roofThick),sampleMat(A.sampleMat),
  liqN2Mat(A.liqN2Mat),liqHeMat(A.liqHeMat),
  wallMat(A.wallMat),stickMat(A.stickMat)
  /*!
    Copy constructor
    \param A :: Cryostat to copy
  */
{}

Cryostat&
Cryostat::operator=(const Cryostat& A)
  /*!
    Assignment operator
    \param A :: Cryostat to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      active=A.active;
      sampleZOffset=A.sampleZOffset;
      sampleRadius=A.sampleRadius;
      sampleHeight=A.sampleHeight;
      voidRadius=A.voidRadius;
      voidDepth=A.voidDepth;
      voidHeight=A.voidHeight;
      voidWallThick=A.voidWallThick;
      stickLen=A.stickLen;
      stickRadius=A.stickRadius;
      stickBoreRadius=A.stickBoreRadius;
      stickBoreHeight=A.stickBoreHeight;
      stickBoreThick=A.stickBoreThick;
      heatRadius=A.heatRadius;
      heatDepth=A.heatDepth;
      heatHeight=A.heatHeight;
      heatThick=A.heatThick;
      heatOuterRadius=A.heatOuterRadius;
      heatOuterLift=A.heatOuterLift;
      liqHeInnerRadius=A.liqHeInnerRadius;
      liqHeOuterRadius=A.liqHeOuterRadius;
      liqHeWallThick=A.liqHeWallThick;
      liqHeHeight=A.liqHeHeight;
      liqHeExtra=A.liqHeExtra;
      liqN2InnerRadius=A.liqN2InnerRadius;
      liqN2OuterRadius=A.liqN2OuterRadius;
      liqN2WallThick=A.liqN2WallThick;
      liqN2Height=A.liqN2Height;
      tailRadius=A.tailRadius;
      tailDepth=A.tailDepth;
      tailHeight=A.tailHeight;
      tailThick=A.tailThick;
      tailOuterRadius=A.tailOuterRadius;
      tailOuterHeight=A.tailOuterHeight;
      tailOuterLift=A.tailOuterLift;
      mainThick=A.mainThick;
      roofThick=A.roofThick;
      sampleMat=A.sampleMat;
      liqN2Mat=A.liqN2Mat;
      liqHeMat=A.liqHeMat;
      wallMat=A.wallMat;
      stickMat=A.stickMat;
    }
  return *this;
}


Cryostat::~Cryostat() 
  /*!
    Destructor
  */
{}

void
Cryostat::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("Cryostat","populate");

  FixedOffset::populate(Control);
  
  //  Activation
  active=Control.EvalDefVar<int>(keyName+"Active",1);
  
  sampleZOffset=Control.EvalDefVar<double>(keyName+"SampleZOffset",0.0);
  ELog::EM<<"Sample Z Off "<<sampleZOffset<<ELog::endDiag;
  sampleRadius=Control.EvalVar<double>(keyName+"SampleRadius");
  sampleHeight=Control.EvalVar<double>(keyName+"SampleHeight");

  voidRadius=Control.EvalVar<double>(keyName+"VoidRadius");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidWallThick=Control.EvalVar<double>(keyName+"VoidWallThick");
  
  stickLen=Control.EvalVar<double>(keyName+"StickLen");
  stickRadius=Control.EvalVar<double>(keyName+"StickRadius");
  
  stickBoreRadius=Control.EvalVar<double>(keyName+"StickBoreRadius");
  stickBoreHeight=Control.EvalVar<double>(keyName+"StickBoreHeight");
  stickBoreThick=Control.EvalVar<double>(keyName+"StickBoreThick");

  heatRadius=Control.EvalVar<double>(keyName+"HeatRadius");
  heatHeight=Control.EvalVar<double>(keyName+"HeatHeight");
  heatDepth=Control.EvalVar<double>(keyName+"HeatDepth");
  heatThick=Control.EvalVar<double>(keyName+"HeatThick");

  heatOuterRadius=Control.EvalVar<double>(keyName+"HeatOuterRadius");
  heatOuterLift=Control.EvalVar<double>(keyName+"HeatOuterLift");

  liqN2InnerRadius=Control.EvalVar<double>(keyName+"LiqN2InnerRadius");
  liqN2OuterRadius=Control.EvalVar<double>(keyName+"LiqN2OuterRadius");
  liqN2WallThick=Control.EvalVar<double>(keyName+"LiqN2WallThick");
  liqN2Height=Control.EvalVar<double>(keyName+"LiqN2Height");

  liqHeInnerRadius=Control.EvalVar<double>(keyName+"LiqHeInnerRadius");
  liqHeOuterRadius=Control.EvalVar<double>(keyName+"LiqHeOuterRadius");
  liqHeWallThick=Control.EvalVar<double>(keyName+"LiqHeWallThick");
  liqHeHeight=Control.EvalVar<double>(keyName+"LiqHeHeight");
  liqHeExtra=Control.EvalVar<double>(keyName+"LiqHeExtraLift");

  tailRadius=Control.EvalVar<double>(keyName+"TailRadius");
  tailHeight=Control.EvalVar<double>(keyName+"TailHeight");
  tailDepth=Control.EvalVar<double>(keyName+"TailDepth");
  tailThick=Control.EvalVar<double>(keyName+"TailThick");

  mainThick=Control.EvalVar<double>(keyName+"MainThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");

  tailOuterRadius=Control.EvalVar<double>(keyName+"TailOuterRadius");
  tailOuterHeight=Control.EvalVar<double>(keyName+"TailOuterHeight");
  tailOuterLift=Control.EvalVar<double>(keyName+"TailOuterLift");

  
  sampleMat=ModelSupport::EvalMat<int>(Control,keyName+"SampleMat");
  liqN2Mat=ModelSupport::EvalMat<int>(Control,keyName+"LiqN2Mat");
  liqHeMat=ModelSupport::EvalMat<int>(Control,keyName+"LiqHeMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  stickMat=ModelSupport::EvalMat<int>(Control,keyName+"StickMat");

  return;
}

void
Cryostat::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors.
    - Main is considered the actual build with Z in gravity direction
    - Beam as the beam direction (can be sloped)
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("Cryostat","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();  

  return;
}


void
Cryostat::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("Cryostat","createSurfaces");

  // sample holder
  ModelSupport::buildCylinder(SMap,cryIndex+7,Origin,Z,sampleRadius);
  ModelSupport::buildPlane(SMap,cryIndex+5,Origin+Z*(sampleZOffset-sampleHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,cryIndex+6,Origin+Z*(sampleZOffset+sampleHeight/2.0),Z);

  // inner void
  ModelSupport::buildCylinder(SMap,cryIndex+17,Origin,Z,voidRadius);
  ModelSupport::buildPlane(SMap,cryIndex+15,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,cryIndex+16,Origin+Z*voidHeight,Z);

  // wall inner void
  ModelSupport::buildCylinder(SMap,cryIndex+27,Origin,Z,voidRadius+voidWallThick);
  ModelSupport::buildPlane(SMap,cryIndex+25,Origin-Z*(voidDepth+voidWallThick),Z);
  ModelSupport::buildPlane(SMap,cryIndex+26,Origin+Z*(voidHeight+voidWallThick),Z);

  // Stick

  ModelSupport::buildCylinder(SMap,cryIndex+107,Origin,Z,stickRadius);
  ModelSupport::buildPlane(SMap,cryIndex+106,Origin+Z*(stickLen+sampleZOffset),Z);

  // stick bore
  ModelSupport::buildCylinder(SMap,cryIndex+117,Origin,Z,stickBoreRadius);
  ModelSupport::buildPlane(SMap,cryIndex+116,Origin+Z*stickBoreHeight,Z);

  // stick bore walls
  ModelSupport::buildCylinder(SMap,cryIndex+127,Origin,Z,stickBoreRadius+stickBoreThick);
  ModelSupport::buildPlane(SMap,cryIndex+126,Origin+Z*(stickBoreHeight+stickBoreThick),Z);

  // Heat shield

  ModelSupport::buildCylinder(SMap,cryIndex+207,Origin,Z,heatRadius);
  ModelSupport::buildPlane(SMap,cryIndex+205,Origin-Z*heatDepth,Z);
  ModelSupport::buildPlane(SMap,cryIndex+206,Origin+Z*heatHeight,Z);

  ModelSupport::buildCylinder(SMap,cryIndex+217,Origin,Z,(heatRadius+heatThick));
  ModelSupport::buildPlane(SMap,cryIndex+215,Origin-Z*(heatDepth+heatThick),Z);

  // build cone to connect outer tail
  ModelSupport::buildCone(SMap,cryIndex+218,Origin,Z,
			  Origin+Y*heatRadius+Z*heatHeight,
			  Origin+Y*heatOuterRadius+Z*heatOuterLift);

  ModelSupport::buildCone(SMap,cryIndex+228,Origin,Z,
			  Origin+Y*(heatRadius+heatThick)+Z*heatHeight,
			  Origin+Y*(heatOuterRadius+heatThick)+Z*heatOuterLift);

  ModelSupport::buildPlane(SMap,cryIndex+226,Origin+Z*heatOuterLift,Z);
  
  // Tail

  ModelSupport::buildCylinder(SMap,cryIndex+307,Origin,Z,tailRadius);
  ModelSupport::buildPlane(SMap,cryIndex+305,Origin-Z*tailDepth,Z);
  ModelSupport::buildPlane(SMap,cryIndex+306,Origin+Z*tailHeight,Z);

  ModelSupport::buildCylinder(SMap,cryIndex+317,Origin,Z,(tailRadius+tailThick));
  ModelSupport::buildPlane(SMap,cryIndex+315,Origin-Z*(tailDepth+tailThick),Z);

  // tail outer
  ModelSupport::buildCylinder(SMap,cryIndex+327,Origin,Z,tailOuterRadius);
  ModelSupport::buildPlane(SMap,cryIndex+325,Origin+Z*tailOuterLift,Z);
  ModelSupport::buildPlane(SMap,cryIndex+326,Origin+Z*tailOuterHeight,Z);

  // Top plate
  ModelSupport::buildPlane(SMap,cryIndex+336,
			   Origin+Z*(tailOuterHeight+roofThick),Z);

  ModelSupport::buildCylinder(SMap,cryIndex+337,Origin,Z,
			      (tailOuterRadius+mainThick));
  // build cone to connect outer tail
  ModelSupport::buildCone(SMap,cryIndex+318,Origin,Z,
			  Origin+Y*tailRadius+Z*tailHeight,
			  Origin+Y*tailOuterRadius+Z*tailOuterLift);

  ModelSupport::buildCone(SMap,cryIndex+328,Origin,Z,
			  Origin+Y*(tailRadius+tailThick)+Z*tailHeight,
			  Origin+Y*(tailOuterRadius+tailThick)+Z*tailOuterLift);


  // Liquid N2 [400]
  // inner first
  ModelSupport::buildCylinder(SMap,cryIndex+407,Origin,Z,
                              liqN2InnerRadius-liqN2WallThick);
  ModelSupport::buildCylinder(SMap,cryIndex+417,Origin,Z,liqN2InnerRadius);
  ModelSupport::buildCylinder(SMap,cryIndex+427,Origin,Z,
                              liqN2OuterRadius);
  ModelSupport::buildCylinder(SMap,cryIndex+437,Origin,Z,
                              liqN2OuterRadius+liqN2WallThick);
  
  ModelSupport::buildPlane(SMap,cryIndex+405,
                           Origin+Z*(heatOuterLift+liqN2WallThick),Z);
  ModelSupport::buildPlane
    (SMap,cryIndex+406,Origin+Z*(heatOuterLift+liqN2WallThick+liqN2Height),Z);
  ModelSupport::buildPlane
    (SMap,cryIndex+416,Origin+Z*(heatOuterLift+2*liqN2WallThick+liqN2Height),Z);

  // Liquid He [500]
  // inner first
  ModelSupport::buildCylinder(SMap,cryIndex+507,Origin,Z,
                              liqHeInnerRadius-liqHeWallThick);
  ModelSupport::buildCylinder(SMap,cryIndex+517,Origin,Z,liqHeInnerRadius);
  ModelSupport::buildCylinder(SMap,cryIndex+527,Origin,Z,
                              liqHeOuterRadius);
  ModelSupport::buildCylinder(SMap,cryIndex+537,Origin,Z,
                              liqHeOuterRadius+liqHeWallThick);

  const double HE(heatOuterLift+liqHeExtra);
  ModelSupport::buildPlane(SMap,cryIndex+505,
                           Origin+Z*(HE+liqHeWallThick),Z);
  ModelSupport::buildPlane
    (SMap,cryIndex+506,Origin+Z*(HE+liqHeWallThick+liqHeHeight),Z);
  ModelSupport::buildPlane
    (SMap,cryIndex+515,Origin+Z*HE,Z);  
  ModelSupport::buildPlane
    (SMap,cryIndex+516,Origin+Z*(HE+2*liqHeWallThick+liqHeHeight),Z);
  
  
  return;
}

void
Cryostat::createNullObjects()
  /*!
    Replaces the cell (sampleVoid) with the same cell
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Cryostat","createNullObjects");
  
  addCells("SampleVoid",getInsertCells());

  return;
}
  
void
Cryostat::createObjects(Simulation& System)
  /*!
    Builds the whole crostate
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Cryostat","createObjects");

  std::string Out;


  // Sample
  Out=ModelSupport::getComposite(SMap,cryIndex," -7 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,sampleMat,0.0,Out));
  addCell("SampleHolder",cellIndex-1);

  // stick
  Out=ModelSupport::getComposite(SMap,cryIndex," -107  6 -106 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,stickMat,0.0,Out));

  // sample void
  Out=ModelSupport::getComposite(SMap,cryIndex," -17 15 -16 (-6:107) (-5:6:7) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("SampleVoid",cellIndex-1);
  // sample void skin
  Out=ModelSupport::getComposite(SMap,cryIndex," -27 25 -26 (17:-15:16) (-15:117) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  

  // stick bore
  Out=ModelSupport::getComposite(SMap,cryIndex," -117 16 -116 107  ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // stick bore walls
  Out=ModelSupport::getComposite(SMap,cryIndex," -127 -126 26 107  (117:116) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // heat shield
  Out=ModelSupport::getComposite(SMap,cryIndex," -207 205 -206  (127:-26) (27:26:-25) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,cryIndex," -217 215 -206 (207:-205) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // Heat shield to N2 tanks (on top of heat shield)

  Out=ModelSupport::getComposite(SMap,cryIndex," 405 -406 417 -427 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,liqN2Mat,0.0,Out));  

  Out=ModelSupport::getComposite(SMap,cryIndex," 226 -416 407 -437 (-405:406:-417:427) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));  

  // He tanks (inside N2 tanks)
  Out=ModelSupport::getComposite(SMap,cryIndex," 505 -506 517 -527 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,liqHeMat,0.0,Out));  

  Out=ModelSupport::getComposite(SMap,cryIndex," 515 -516 507 -537 (-505:506:-517:527) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));  
  
  // piece in cone
  Out=ModelSupport::getComposite(SMap,cryIndex," 206 -226 127 -218");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));  

  Out=ModelSupport::getComposite(SMap,cryIndex," 206 -226 218 -228 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));  

  
  // tail
  Out=ModelSupport::getComposite(SMap,cryIndex," -307 305 -306 (217:-215:206) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,cryIndex," -317 315 -306  (307:-305) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));  


  // piece in cone
  Out=ModelSupport::getComposite(SMap,cryIndex," 306 -226 -318 217 228 -327 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));  

  Out=ModelSupport::getComposite(SMap,cryIndex," 206 -318 127 -325");
  //  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));  

  Out=ModelSupport::getComposite(SMap,cryIndex," 306 -325 318 -328 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));  

  // Outer wall (veritcal)
  Out=ModelSupport::getComposite(SMap,cryIndex," 226 -326 -327 127 "
                                                " (437:-407:416) (537:-507:516:-515) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));  

  Out=ModelSupport::getComposite(SMap,cryIndex," 325 -326 -337 327 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));  

  // roof
  Out=ModelSupport::getComposite(SMap,cryIndex," 326 -336 -337 127 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));  

  // Heat protection layer
  
  // OUTER
  Out=ModelSupport::getComposite(SMap,cryIndex," 306 -325 -328  ");
  addOuterSurf(Out);

  Out=ModelSupport::getComposite(SMap,cryIndex," -317 315 -206 ");
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,cryIndex," 325 -336 -337 ");
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,cryIndex," -106 (-107:-126) -127 336 ");
  addOuterUnionSurf(Out);
  return;
}

void
Cryostat::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("Cryostat","createLinks");

  
  FixedComp::setConnect(0,Origin-Y*(tailRadius+tailThick),-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(cryIndex+307));

  FixedComp::setConnect(1,Origin+Y*(tailRadius+tailThick),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(cryIndex+307));

  return;
}

void
Cryostat::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp at the beam centre
    \param sideIndex :: side index
  */
{
  ELog::RegMethod RegA("Cryostat","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  if (active)
    createObjects(System);
  else
    createNullObjects();
  
  createLinks();
  insertObjects(System);   

  return;
}
  
}  // NAMESPACE constructSystem
