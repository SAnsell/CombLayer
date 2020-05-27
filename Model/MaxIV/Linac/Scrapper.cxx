/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Linac/Scrapper.cxx
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
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"

#include "Scrapper.h"

namespace tdcSystem
{

Scrapper::Scrapper(const std::string& Key) :
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


Scrapper::~Scrapper()
  /*!
    Destructor
  */
{}

void
Scrapper::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("Scrapper","populate");

  FixedOffset::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");

  tubeAYStep=Control.EvalVar<double>(keyName+"TubeAYStep");
  tubeBYStep=Control.EvalVar<double>(keyName+"TubeBYStep");
  tubeRadius=Control.EvalVar<double>(keyName+"TubeRadius");
  tubeLength=Control.EvalVar<double>(keyName+"TubeLength");
  tubeThick=Control.EvalVar<double>(keyName+"TubeThick");
  tubeFlangeRadius=Control.EvalVar<double>(keyName+"TubeFlangeRadius");
  tubeFlangeLength=Control.EvalVar<double>(keyName+"TubeFlangeLength");

  scrapperRadius=Control.EvalVar<double>(keyName+"ScrapperRadius");
  scrapperHeight=Control.EvalVar<double>(keyName+"ScrapperHeight");
  scrapperZLift=Control.EvalVar<double>(keyName+"ScrapperZLift");

  driveRadius=Control.EvalVar<double>(keyName+"DriveRadius");
  driveFlangeRadius=Control.EvalVar<double>(keyName+"DriveFlangeRadius");
  driveFlangeLength=Control.EvalVar<double>(keyName+"DriveFlangeLength");
  
  supportRadius=Control.EvalVar<double>(keyName+"SupportRadius");
  supportThick=Control.EvalVar<double>(keyName+"SupportThick");
  supportHeight=Control.EvalVar<double>(keyName+"SupportHeight");
    
  topBoxWidth=Control.EvalVar<double>(keyName+"TopBoxWidth");
  topBoxHeight=Control.EvalVar<double>(keyName+"TopBoxHeight");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  tubeMat=ModelSupport::EvalMat<int>(Control,keyName+"TubeMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
  scrapperMat=ModelSupport::EvalMat<int>(Control,keyName+"ScrapperMat");
  driveMat=ModelSupport::EvalMat<int>(Control,keyName+"DriveMat");
  topMat=ModelSupport::EvalMat<int>(Control,keyName+"TopMat");

  return;
}


void
Scrapper::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Scrapper","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  // mid divider
  ModelSupport::buildPlane(SMap,buildIndex+10,Origin,Z);
  
  // main pipe and thickness
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);

  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeRadius);
  ModelSupport::buildPlane(SMap,buildIndex+101,
			   Origin-Y*(length/2.0-flangeLength),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,
			   Origin+Y*(length/2.0-flangeLength),Y);

  // Main Top/Down Tubes
  Geometry::Vec3D aOrg(Origin+Y*tubeAYStep);
  Geometry::Vec3D bOrg(Origin+Y*tubeBYStep);


  ModelSupport::buildCylinder(SMap,buildIndex+307,aOrg,Z,tubeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+317,aOrg,Z,tubeRadius+tubeThick);

  ModelSupport::buildCylinder(SMap,buildIndex+327,aOrg,Z,tubeFlangeRadius);

  ModelSupport::buildPlane(SMap,buildIndex+305,aOrg-Z*tubeLength,Z);
  ModelSupport::buildPlane(SMap,buildIndex+315,
			   aOrg-Z*(tubeLength-tubeFlangeLength),Z);
  ModelSupport::buildPlane(SMap,buildIndex+325,
			   aOrg-Z*(tubeLength+tubeFlangeLength),Z);

  // scrapper (A)
  ModelSupport::buildCylinder(SMap,buildIndex+407,aOrg,Z,scrapperRadius);
  ModelSupport::buildPlane(SMap,buildIndex+405,
			   aOrg-Z*(scrapperZLift),Z);
  ModelSupport::buildPlane(SMap,buildIndex+415,
			   aOrg-Z*(scrapperZLift+scrapperHeight),Z);

  // drive / support column
  ModelSupport::buildCylinder(SMap,buildIndex+507,aOrg,Z,driveRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+517,aOrg,Z,supportRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+527,
			      aOrg,Z,supportRadius+supportThick);
  ModelSupport::buildCylinder(SMap,buildIndex+537,aOrg,Z,driveFlangeRadius);
  
  // Top box
  aOrg -= Z*(tubeLength+tubeFlangeLength);
  ModelSupport::buildPlane
    (SMap,buildIndex+505,aOrg-Z*driveFlangeLength,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+515,aOrg-Z*supportHeight,Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+601,aOrg-Y*(topBoxWidth/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+602,aOrg+Y*(topBoxWidth/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+603,aOrg-X*(topBoxWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+604,aOrg+X*(topBoxWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+605,aOrg-Z*(supportHeight+topBoxHeight),Z);


  ModelSupport::buildCylinder(SMap,buildIndex+1307,bOrg,Z,tubeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+1317,bOrg,Z,tubeRadius+tubeThick);
  ModelSupport::buildCylinder(SMap,buildIndex+1327,bOrg,Z,tubeFlangeRadius);
  
  ModelSupport::buildPlane(SMap,buildIndex+1305,bOrg+Z*tubeLength,Z);
  ModelSupport::buildPlane(SMap,buildIndex+1315,
			   bOrg+Z*(tubeLength-tubeFlangeLength),Z);
  ModelSupport::buildPlane(SMap,buildIndex+1325,
			   bOrg+Z*(tubeLength+tubeFlangeLength),Z);

  ModelSupport::buildCylinder(SMap,buildIndex+1407,bOrg,Z,scrapperRadius);
  ModelSupport::buildPlane(SMap,buildIndex+1405,
			   bOrg+Z*(scrapperZLift),Z);
  ModelSupport::buildPlane(SMap,buildIndex+1415,
			   bOrg+Z*(scrapperZLift+scrapperHeight),Z);

  ModelSupport::buildCylinder(SMap,buildIndex+1507,bOrg,Z,driveRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+1517,bOrg,Z,supportRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+1527,
			      bOrg,Z,supportRadius+supportThick);
  ModelSupport::buildCylinder(SMap,buildIndex+1537,bOrg,Z,driveFlangeRadius);
  
  bOrg += Z*(tubeLength+tubeFlangeLength);
  ModelSupport::buildPlane
    (SMap,buildIndex+1505,bOrg+Z*driveFlangeLength,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+1515,bOrg+Z*supportHeight,Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+1601,bOrg-Y*(topBoxWidth/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+1602,bOrg+Y*(topBoxWidth/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+1603,bOrg-X*(topBoxWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+1604,bOrg+X*(topBoxWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+1605,bOrg+Z*(supportHeight+topBoxHeight),Z);

  return;
}

void
Scrapper::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Scrapper","createObjects");


  std::string Out;

  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");

  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  /// does scrapper exist in main void
  if (scrapperZLift<radius-Geometry::zeroTol)
    Out+=ModelSupport::getComposite(SMap,buildIndex," (407:405) (1407:-1405) ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr+backStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 7 -17 (10 : 307) (-10: 1307) ");
  makeCell("Wall",System,cellIndex++,tubeMat,0.0,Out+frontStr+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 17 -107 -101 ");
  makeCell("FlangeA",System,cellIndex++,flangeMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 17 -107 102 ");
  makeCell("FlangeB",System,cellIndex++,flangeMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"101 -102 -107 17 (10:327) (-10:1327) ");
  makeCell("Outer",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -10 7 -307 305 (-415:407) 507 ");
  makeCell("TubeA",System,cellIndex++,voidMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -10 17 307 -317 305 ");
  makeCell("TubeAWall",System,cellIndex++,tubeMat,0.0,Out);
 
  Out=ModelSupport::getComposite(SMap,buildIndex," 317 -305 315 -327 ");
  makeCell("TubeFlangeA",System,cellIndex++,flangeMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-327 -305 325 507 ");
  makeCell("TubeTopA",System,cellIndex++,flangeMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -10 17 -327 317 305 ");
  makeCell("TubeAOuter",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 10 7 -1307 -1305 (1415:1407) 1507 ) ");
  makeCell("TubeB",System,cellIndex++,voidMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 10 17 1307 -1317 -1305 ");
  makeCell("TubeBWall",System,cellIndex++,tubeMat,0.0,Out);
 
  Out=ModelSupport::getComposite(SMap,buildIndex," 1317 -1305 1315 -1327 ");
  makeCell("TubeFlangeB",System,cellIndex++,flangeMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1327 1305 -1325 1507 ");
  makeCell("TubeTopB",System,cellIndex++,flangeMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 10 17 -1327 1317 -1315 ");
  makeCell("TubeBOuter",System,cellIndex++,0,0.0,Out);

  // scrappers thenmselves
  Out=ModelSupport::getComposite(SMap,buildIndex," -407 -405 415 ");
  makeCell("ScrapperA",System,cellIndex++,scrapperMat,0.0,Out);

  // scrappers thenmselves
  Out=ModelSupport::getComposite(SMap,buildIndex," -1407 1405 -1415 ");
  makeCell("ScrapperB",System,cellIndex++,scrapperMat,0.0,Out);


  Out=ModelSupport::getComposite(SMap,buildIndex," -415 -507 515 ");
  makeCell("DriveA",System,cellIndex++,driveMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -325 505 -537 507 ");
  makeCell("DriveFlangeA",System,cellIndex++,flangeMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -505 -517 515 507");
  makeCell("SupportA",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -505 517 -527 515");
  makeCell("SupportATube",System,cellIndex++,tubeMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," (-505:537) -325 -327 527 515");
  makeCell("SupportAOut",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 601 -602 603 -604 605 -515 ");
  makeCell("PlateA",System,cellIndex++,topMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -327 (-601 : 602 : -603 : 604) 605 -515 ");
  makeCell("PlateAOut",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1415 -1507 -1515 ");
  makeCell("DriveB",System,cellIndex++,driveMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1325 -1505 -1537 1507 ");
  makeCell("DriveFlangeB",System,cellIndex++,flangeMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1505 -1517 -1515 1507");
  makeCell("SupportB",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1505 1517 -1527 -1515");
  makeCell("SupportBTube",System,cellIndex++,tubeMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," (1505:1537) 1325 -1327 1527 -1515");
  makeCell("SupportBOut",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1601 -1602 1603 -1604 -1605 1515 ");
  makeCell("PlateB",System,cellIndex++,topMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1327 (-1601 : 1602 : -1603 : 1604) -1605 1515 ");
  makeCell("PlateBOut",System,cellIndex++,0,0.0,Out);
  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," (-107 : (-10 -327 605) : (10 -1327 -1605)) ");
  addOuterSurf(Out+frontStr+backStr);

  return;
}

void
Scrapper::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("Scrapper","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back

  return;
}

void
Scrapper::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("Scrapper","createAll");

  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,length);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE tdcSystem
