/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/DipoleChamber.cxx
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
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "SimProcess.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "LineIntersectVisit.h" 

#include "DipoleChamber.h"

namespace xraySystem
{

DipoleChamber::DipoleChamber(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedGroup("Main","Exit"),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


DipoleChamber::~DipoleChamber() 
  /*!
    Destructor
  */
{}

void
DipoleChamber::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("DipoleChamber","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  outWidth=Control.EvalVar<double>(keyName+"OutWidth");
  ringWidth=Control.EvalVar<double>(keyName+"RingWidth");
  curveRadius=Control.EvalVar<double>(keyName+"CurveRadius");
  curveAngle=Control.EvalVar<double>(keyName+"CurveAngle");
  height=Control.EvalVar<double>(keyName+"Height");

  exitWidth=Control.EvalVar<double>(keyName+"ExitWidth");
  exitHeight=Control.EvalVar<double>(keyName+"ExitHeight");
  exitLength=Control.EvalVar<double>(keyName+"ExitLength");
  
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  innerXFlat=Control.EvalVar<double>(keyName+"InnerXFlat");
  innerXOut=Control.EvalVar<double>(keyName+"InnerXOut");

  elecXFlat=Control.EvalVar<double>(keyName+"ElecXFlat");
  elecXCut=Control.EvalVar<double>(keyName+"ElecXCut");
  elecXFull=Control.EvalVar<double>(keyName+"ElecXFull");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");


  return;
}

void
DipoleChamber::createUnitVector(const attachSystem::FixedComp& FC,
    	                     const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: FixedComp to attach to
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("DipoleChamber","createUnitVector");
  
  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}

void
DipoleChamber::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("DipoleChamber","createSurface");

  if (!ExternalCut::isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }


  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*outWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  const double cAng(M_PI*curveAngle/180.0);
  // all curve +/- to this one
  const Geometry::Vec3D curvePt(Origin+X*(curveRadius+ringWidth));
  
  ModelSupport::buildCylinder(SMap,buildIndex+7,curvePt,Z,curveRadius);
  // construct cut plane
  const double xVal=curveRadius*(1.0-cos(cAng));
  const double yVal=curveRadius*sin(cAng);
  const Geometry::Vec3D CPt(Origin+X*(ringWidth+xVal)+Y*yVal);
  const Geometry::Vec3D BAxis(X*sin(cAng)+Y*cos(cAng));

  
  ModelSupport::buildPlane(SMap,buildIndex+2,CPt,BAxis);
  ModelSupport::buildPlane(SMap,buildIndex+12,CPt+BAxis*wallThick,BAxis);
  setCutSurf("back",-SMap.realSurf(buildIndex+12));
  // Walls

  // low/left counter clockwise points (starting from
  // outer point
  
  ModelSupport::buildPlane
    (SMap,buildIndex+13,Origin-X*(outWidth+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(wallThick+height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(wallThick+height/2.0),Z);

  ModelSupport::buildCylinder
    (SMap,buildIndex+17,curvePt+X*wallThick,Z,curveRadius);  


  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(exitWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(exitWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(exitHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(exitHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+113,
			   Origin-X*(wallThick+exitWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+114,
			   Origin+X*(wallThick+exitWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+115,
			   Origin-Z*(wallThick+exitHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+116,
			   Origin+Z*(wallThick+exitHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+101,
			   Origin+Y*(yVal+length-flangeLength),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeRadius);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(yVal+length),Y);
  setCutSurf("exit",-SMap.realSurf(buildIndex+102));


  // Inner objects:
  ModelSupport::buildPlane
    (SMap,buildIndex+204,Origin-X*innerXFlat,X);  
  ModelSupport::buildCylinder                          // Outside of 204
    (SMap,buildIndex+207,Origin-X*(innerXOut-curveRadius),Z,curveRadius);  

  // electron cut objects
    ModelSupport::buildPlane
    (SMap,buildIndex+304,Origin+X*elecXFlat,X);  
  ModelSupport::buildCylinder                          // Outside of 204
    (SMap,buildIndex+307,Origin-X*(elecXCut-curveRadius),Z,curveRadius);  
  ModelSupport::buildCylinder                          // Outside of 204
    (SMap,buildIndex+317,Origin-X*(elecXFull-curveRadius),Z,curveRadius);  

  
  return;
}

void
DipoleChamber::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("DipoleChamber","createObjects");

  const std::string frontStr=getRuleStr("front");
  //
  std::string Out;
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"  3 -2 7 5 -6 (207 : 204) (-304:307:-317) ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"  13 -2 17 15 -16 (-3:-7:-5:6)");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -12 2 13 17 15 -16 (-113:114:-115:116)");
  makeCell("BackPlate",System,cellIndex++,wallMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -102 103 -104 105 -106 ");
  makeCell("ExitVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 2 -102 113 -114 115 -116 (-103:104:-105:106) ");
  makeCell("ExitWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -107 101 -102 (-113:114:-115:116) ");
  makeCell("Flange",System,cellIndex++,wallMat,0.0,Out);

  // Inner components
  Out=ModelSupport::getComposite
    (SMap,buildIndex," -207 -204 5 -6");
  makeCell("InnerCut",System,cellIndex++,innerMat,0.0,Out+frontStr);

  // electorn inner componnents
  Out=ModelSupport::getComposite
    (SMap,buildIndex," -2 304 -307 317 5 -6");
  makeCell("ElectCut",System,cellIndex++,innerMat,0.0,Out+frontStr);
  

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"  13 -12 17 15 -16 ");
  addOuterSurf("Main",Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -101 113 -114 115 -116 ");
  addOuterSurf("Exit",Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"  101 -102 -107 ");
  addOuterUnionSurf("Exit",Out);


  return;
}

void 
DipoleChamber::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("DipoleChamber","createLinks");
  ExternalCut::createLink("front",*this,0,Origin,Y);
  ExternalCut::createLink("back",*this,1,Origin,Y);
  ExternalCut::createLink("exit",*this,2,Origin,Y);
  //  ExternalCut::createLink("back",*this,1,Origin,Y);

  // construct from Surface 12:


  const double cAng(M_PI*curveAngle/180.0);
  const double xVal=curveRadius*(1.0-cos(cAng));
  const double yVal=curveRadius*sin(cAng);

  const Geometry::Vec3D BAxis(X*sin(cAng)+Y*cos(cAng));
  const Geometry::Vec3D CAxis(Y*sin(cAng)+X*cos(cAng));
  const Geometry::Vec3D CPt(Origin+X*(ringWidth+xVal)+Y*yVal);

  // Line goes along plane (build+12):
  MonteCarlo::LineIntersectVisit LI(CPt+BAxis*wallThick,CAxis);
  const Geometry::Vec3D CPA=
    LI.getPoint(SMap.realPtr<Geometry::Cylinder>(buildIndex+7));
  const Geometry::Vec3D CPB=
    LI.getPoint(SMap.realPtr<Geometry::Cylinder>(buildIndex+317));
  
  // dipole exit
  Fixedcomp::setConnectPt(3,(CPA+CPB)/2.0,BAxis);
  Fixedcomp::setLineSurf(3,SMap.realSurf(buildIndex+12));
  
  FixedComp::nameSideIndex(2,"exit");
  FixedComp::nameSideIndex(3,"dipoleExit");
  return;
}


void
DipoleChamber::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("DipoleChamber","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE epbSystem
