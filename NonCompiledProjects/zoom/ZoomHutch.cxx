/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoom/ZoomHutch.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedGroup.h"
#include "LinearComp.h"
#include "ContainedComp.h"
#include "ZoomTank.h"
#include "ZoomHutch.h"

namespace zoomSystem
{

ZoomHutch::ZoomHutch(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedGroup(Key,"Main",6,"Beam",2),
  tank("zoomTank"),innerVoid(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ZoomHutch::ZoomHutch(const ZoomHutch& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedGroup(A),
  tank(A.tank),xStep(A.xStep),yStep(A.yStep),
  zStep(A.zStep),xyAngle(A.xyAngle),zAngle(A.zAngle),
  frontLeftWidth(A.frontLeftWidth),
  midLeftWidth(A.midLeftWidth),backLeftWidth(A.backLeftWidth),
  frontRightWidth(A.frontRightWidth),midRightWidth(A.midRightWidth),
  backRightWidth(A.backRightWidth),frontLeftLen(A.frontLeftLen),
  midLeftLen(A.midLeftLen),frontRightLen(A.frontRightLen),
  midRightLen(A.midRightLen),fullLen(A.fullLen),wallThick(A.wallThick),
  floorThick(A.floorThick),roofThick(A.roofThick),floorDepth(A.floorDepth),
  roofHeight(A.roofHeight),wallMat(A.wallMat),roofMat(A.roofMat),
  floorMat(A.floorMat),innerVoid(A.innerVoid)
  /*!
    Copy constructor
    \param A :: ZoomHutch to copy
  */
{}

ZoomHutch&
ZoomHutch::operator=(const ZoomHutch& A)
  /*!
    Assignment operator
    \param A :: ZoomHutch to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedGroup::operator=(A);
      tank=A.tank;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      frontLeftWidth=A.frontLeftWidth;
      midLeftWidth=A.midLeftWidth;
      backLeftWidth=A.backLeftWidth;
      frontRightWidth=A.frontRightWidth;
      midRightWidth=A.midRightWidth;
      backRightWidth=A.backRightWidth;
      frontLeftLen=A.frontLeftLen;
      midLeftLen=A.midLeftLen;
      frontRightLen=A.frontRightLen;
      midRightLen=A.midRightLen;
      fullLen=A.fullLen;
      wallThick=A.wallThick;
      floorThick=A.floorThick;
      roofThick=A.roofThick;
      floorDepth=A.floorDepth;
      roofHeight=A.roofHeight;
      wallMat=A.wallMat;
      roofMat=A.roofMat;
      floorMat=A.floorMat;
      innerVoid=A.innerVoid;
    }
  return *this;
}

ZoomHutch::~ZoomHutch() 
 /*!
   Destructor
 */
{}

void
ZoomHutch::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase to use
 */
{
  ELog::RegMethod RegA("ZoomHutch","populate");
  

  // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
  zAngle=Control.EvalVar<double>(keyName+"ZAngle");

  frontLeftWidth=Control.EvalVar<double>(keyName+"FrontLeftWidth");
  midLeftWidth=Control.EvalVar<double>(keyName+"MidLeftWidth");
  backLeftWidth=Control.EvalVar<double>(keyName+"BackLeftWidth");
  frontRightWidth=Control.EvalVar<double>(keyName+"FrontRightWidth");
  midRightWidth=Control.EvalVar<double>(keyName+"MidRightWidth");
  backRightWidth=Control.EvalVar<double>(keyName+"BackRightWidth");

  frontLeftLen=Control.EvalVar<double>(keyName+"FrontLeftLen");
  midLeftLen=Control.EvalVar<double>(keyName+"MidLeftLen");
  frontRightLen=Control.EvalVar<double>(keyName+"FrontRightLen");
  midRightLen=Control.EvalVar<double>(keyName+"MidRightLen");

  floorDepth=Control.EvalVar<double>(keyName+"Floor");
  roofHeight=Control.EvalVar<double>(keyName+"Roof");
  fullLen=Control.EvalVar<double>(keyName+"FullLen");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");

  portRadius=Control.EvalVar<double>(keyName+"PortRadius");

  // Material
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  floorMat=ModelSupport::EvalMat<int>(Control,keyName+"FloorMat");
  roofMat=ModelSupport::EvalMat<int>(Control,keyName+"RoofMat");

  return;
} 
  
void
ZoomHutch::createUnitVector(const attachSystem::FixedGroup& TC,
			    const long int)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param TC :: Linked object
  */
{
  ELog::RegMethod RegA("ZoomHutch","createUnitVector");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  mainFC.createUnitVector(TC.getKey("Main"));
  beamFC.createUnitVector(TC.getKey("Beam"));

  mainFC.setCentre(beamFC.getCentre());
  mainFC.applyShift(xStep,yStep,zStep);
  beamFC.applyShift(xStep,yStep,zStep);
  mainFC.applyAngleRotate(xyAngle,zAngle);
  beamFC.applyAngleRotate(xyAngle,zAngle);

  setDefault("Main","Beam");
  
  return;
}

void
ZoomHutch::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ZoomHutch","createSurface");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*fullLen,Y);
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*frontLeftLen,Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*frontRightLen,Y);
  ModelSupport::buildPlane(SMap,buildIndex+21,Origin+Y*midLeftLen,Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*midRightLen,Y);
  // Widths:
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*frontLeftWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*midLeftWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*backLeftWidth,X);

  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*frontRightWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*midRightWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*backRightWidth,X);
  
  // Floors/Roof:
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*floorDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*roofHeight,Z);

  // Port:
  const attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  ModelSupport::buildCylinder(SMap,buildIndex+77,
			      beamFC.getCentre(),beamFC.getY(),portRadius);
  
  // INNER WALLS:

  ModelSupport::buildPlane(SMap,buildIndex+31,Origin+Y*wallThick,Y);
  ModelSupport::buildPlane(SMap,buildIndex+32,
			   Origin+Y*(fullLen-wallThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+41,
			   Origin+Y*(frontLeftLen+wallThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+42,
			   Origin+Y*(frontRightLen+wallThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+51,
			   Origin+Y*(midRightLen+wallThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+52,
			   Origin+Y*(midRightLen+wallThick),Y);

  // Floors/Roof:
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(floorDepth-floorThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(roofHeight-roofThick),Z);

  // Widths:

  ModelSupport::buildPlane(SMap,buildIndex+33,
			   Origin-X*(frontLeftWidth-wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+43,
			   Origin-X*(midLeftWidth-wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+53,
			   Origin-X*(backLeftWidth-wallThick),X);

  ModelSupport::buildPlane(SMap,buildIndex+34,
			   Origin+X*(frontRightWidth-wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+44,
			   Origin+X*(midRightWidth-wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+54,
			   Origin+X*(backRightWidth-wallThick),X);
  return;
}

void
ZoomHutch::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ZoomHutch","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"1 -2 13 (3 : 11) (-4 : 12) (-14 : 22) -24  5 -6");
  addOuterSurf(Out);
  // OUTER WALLS
  // Front
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -31 3  -4  15 -16 77");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  // port
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -31 -77");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  
  // left
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "31 -41 (11:3) 13 -33 15 -16");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  // Add door here:
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "41 -2 13 -43 15 -16");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  // Right
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "31 -42 (12:-4) -14 34 15 -16");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "42 -52 (22:-14) -24 44 15 -16");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "52 -2 -24 54 15 -16");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  // Back wall
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "32 -2 43 -54 15 -16");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));

  // ROOF:
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 13 (3 : 11) "
				 "(-4 : 12) (-14 : 22) -24  16 -6");
  System.addCell(MonteCarlo::Object(cellIndex++,roofMat,0.0,Out));

  // Floor:
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 13 (3 : 11) "
				 "(-4 : 12) (-14 : 22) -24  -15 5");
  System.addCell(MonteCarlo::Object(cellIndex++,floorMat,0.0,Out));

  // INNER VOID
  Out=ModelSupport::getComposite(SMap,buildIndex,
   				 "31 -32 43 (33 : 41) "
   				 "(-34 : 42) (-44 : 52) -54  15 -16");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  innerVoid=cellIndex-1;
  return;
}

void
ZoomHutch::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("ZoomHutch","createLinks");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  
  // // set Links:
  mainFC.setConnect(0,Origin,-Y);
  mainFC.setConnect(1,Origin+Y*fullLen,Y);
  mainFC.setConnect(2,Origin-X*frontLeftWidth,-X);
  mainFC.setConnect(3,Origin+X*frontRightWidth,X);
  mainFC.setConnect(4,Origin+Z*floorDepth,X);
  mainFC.setConnect(5,Origin+Z*roofHeight,X);
  
  for(size_t i=0;i<6;i++)
    {
      int ii(static_cast<int>(i));
      mainFC.setLinkSurf(i,SMap.realSurf(buildIndex+1+ii));
    }
  
  beamFC.setExit(buildIndex+2,bOrigin,bY);


  return;
}

void
ZoomHutch::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Linear object to component
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("ZoomHutch","createAll");
  populate(System.getDataBase());

  const attachSystem::FixedGroup* FGPtr=
    dynamic_cast<const attachSystem::FixedGroup*>(&FC);
  if (!FGPtr)
    throw ColErr::DynamicConv("FixedComp","FixedGroup","FC");
  
  
  createUnitVector(*FGPtr,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  
  tank.addInsertCell(innerVoid);
  tank.createAll(System,*FGPtr,0);
  
  return;
}

  
}  // NAMESPACE moderatorSystem
