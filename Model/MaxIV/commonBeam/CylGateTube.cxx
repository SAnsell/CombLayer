/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/CylGateTube.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell 
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"
#include "SurInter.h"

#include "CylGateTube.h" 

namespace xraySystem
{

CylGateTube::CylGateTube(const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut(),
  closed(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}



CylGateTube::~CylGateTube() 
  /*!
    Destructor
  */
{}

void
CylGateTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("CylGateTube","populate");
  
  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  depth=Control.EvalVar<double>(keyName+"Depth");
  height=Control.EvalVar<double>(keyName+"Height");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  portRadius=Control.EvalVar<double>(keyName+"PortRadius");
  portFlangeRadius=Control.EvalVar<double>(keyName+"PortFlangeRadius");
  portInner=Control.EvalVar<double>(keyName+"PortInner");
  portThick=Control.EvalVar<double>(keyName+"PortThick");

  topRadius=Control.EvalVar<double>(keyName+"TopRadius");
  topHoleRadius=Control.EvalVar<double>(keyName+"TopHoleRadius");
  topThick=Control.EvalVar<double>(keyName+"TopThick");

  liftHeight=Control.EvalVar<double>(keyName+"LiftHeight");
  liftRadius=Control.EvalVar<double>(keyName+"LiftRadius");
  liftThick=Control.EvalVar<double>(keyName+"LiftThick");
  liftPlate=Control.EvalVar<double>(keyName+"LiftPlate");

  driveRadius=Control.EvalVar<double>(keyName+"DriveRadius");
  bladeLift=Control.EvalVar<double>(keyName+"BladeLift");
  bladeThick=Control.EvalVar<double>(keyName+"BladeThick");
  bladeRadius=Control.EvalVar<double>(keyName+"BladeRadius");

  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  bladeMat=ModelSupport::EvalMat<int>(Control,keyName+"BladeMat");
  driveMat=ModelSupport::EvalMat<int>(Control,keyName+"DriveMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  closed=ModelSupport::EvalMat<int>(Control,keyName+"Closed");

  return;
}


void
CylGateTube::createSurfaces()
  /*!
    Create the surfaces
    Note that it is created in the vertical direction for the 
    main cylinder and the port view is in the Y directions.
  */
{
  ELog::RegMethod RegA("CylGateTube","createSurfaces");

  const double bladeZStep((!closed) ? bladeLift : 0.0);
    
  // front/back planes
  if (!isActive("front"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+1,Origin-Y*(radius+wallThick+portThick),Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }	
  if (!isActive("back"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+2,Origin+Y*(radius+wallThick+portThick),Y);
      setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }	
  // inner port radius on front / back
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,portRadius);
  // flanges on front / back
  ModelSupport::buildCylinder(SMap,buildIndex+117,Origin,Y,portFlangeRadius);
  // inner flange limits:
  ModelSupport::buildPlane
    (SMap,buildIndex+101,Origin-Y*(radius+wallThick-portInner),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+102,Origin+Y*(radius+wallThick-portInner),Y);

  
  // Main tube
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,radius+wallThick);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(depth+wallThick),Z);

  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);

  // Flange at top
  ModelSupport::buildPlane
    (SMap,buildIndex+106,Origin+Z*(height+topThick),Z);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Z,topRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+217,Origin,Z,topHoleRadius);
  
  // lift port
  ModelSupport::buildPlane
    (SMap,buildIndex+206,Origin+Z*(height+topThick+liftHeight),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+216,Origin+Z*(height+topThick+liftHeight+liftPlate),Z);

  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,Z,liftRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+317,Origin,Z,liftRadius+liftThick);
  // blade
  const Geometry::Vec3D bladeOrg(Origin+Z*bladeZStep);
  ModelSupport::buildPlane(SMap,buildIndex+405,bladeOrg,Z);
  ModelSupport::buildCylinder(SMap,buildIndex+407,bladeOrg,Y,bladeRadius);
  ModelSupport::buildPlane(SMap,buildIndex+401,bladeOrg-Y*(bladeThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+402,bladeOrg+Y*(bladeThick/2.0),Y);

  // blade support rod
  ModelSupport::buildCylinder(SMap,buildIndex+507,Origin,Z,driveRadius);

  return;
}

void
CylGateTube::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CylGateTube","createObjects");

  std::string Out;


  const std::string frontStr=getRuleStr("front"); // 1
  const std::string backStr=getRuleStr("back");    // -2


  // Main Void [exclude flange cylinder/ blade and blade tube]
  Out=ModelSupport::getComposite(SMap,buildIndex," 5 -6 -7 117 (507 : -405)");
  if (!closed)
    Out+=ModelSupport::getComposite(SMap,buildIndex," (407:-401:402)");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  // blade:
  Out=ModelSupport::getComposite(SMap,buildIndex,"-407 401 -402 ");
  makeCell("Blade",System,cellIndex++,bladeMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 5 -6 7 -17 117 ");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 15 -5 -17 ");
  makeCell("Base",System,cellIndex++,wallMat,0.0,Out);

  // Front/Back flanges:
  Out=ModelSupport::getComposite(SMap,buildIndex," -101 107 -117 ");
  makeCell("FlangeA",System,cellIndex++,wallMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 102 107 -117 ");
  makeCell("FlangeB",System,cellIndex++,wallMat,0.0,Out+backStr);

  if (closed)
    Out=ModelSupport::getComposite
      (SMap,buildIndex,"101 -102 -117 (407 : -401 : 402) (507:-405)");
  else
    Out=ModelSupport::getComposite(SMap,buildIndex,"101 -102 -117 ");
  makeCell("MidVoid",System,cellIndex++,voidMat,0.0,Out);
  // end caps
  Out=ModelSupport::getComposite(SMap,buildIndex,"-101 -107 ");
  makeCell("MidVoid",System,cellIndex++,voidMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"102 -107 ");
  makeCell("MidVoid",System,cellIndex++,voidMat,0.0,Out+backStr);

  // blade support
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"(-401:402:407) 405 -206 -507 ");
  makeCell("BladeSupport",System,cellIndex++,bladeMat,0.0,Out);

  ELog::EM<<"front == "<<frontStr<<ELog::endDiag;
  // top flange [artifical cut on port flanges for FLUKA convinence]
  Out=ModelSupport::getComposite(SMap,buildIndex,"6 -106 -207 217");
  makeCell("TopFlange",System,cellIndex++,wallMat,0.0,Out+frontStr+backStr);

  // top clearance
  if (topRadius-driveRadius>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,"6 -106 -217 507");
      makeCell("TopGap",System,cellIndex++,voidMat,0.0,Out);
    }

  Out=ModelSupport::getComposite(SMap,buildIndex,"106 -206 507 -307");
  makeCell("TopVoid",System,cellIndex++,voidMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"106 -206 -317 307 ");
  makeCell("TopWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-216 206 -317 ");
  makeCell("TopCap",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"106 -216 -207 317");
  makeCell("TopSpace",System,cellIndex++,0,0.0,Out+frontStr+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"117 17 -6 -207 15 ");
  makeCell("LowSpace",System,cellIndex++,0,0.0,Out+frontStr+backStr);


  Out=ModelSupport::getComposite(SMap,buildIndex," 15 -216 -207 ");
  addOuterSurf(Out+frontStr+backStr);

  return;
}
  
void
CylGateTube::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("CylGateTube","createLinks");

  //stuff for intersection


  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back

  return;
}
  
  
void
CylGateTube::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("CylGateTube","createAll(FC)");

  populate(System.getDataBase());
  const double offset(radius+wallThick+portThick);
  createCentredUnitVector(FC,FIndex,offset);
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
