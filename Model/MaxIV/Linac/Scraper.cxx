/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Linac/Scaper.cxx
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

#include "Scraper.h"

namespace tdcSystem
{

Scraper::Scraper(const std::string& Key) :
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


Scraper::~Scraper()
  /*!
    Destructor
  */
{}

void
Scraper::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("Scraper","populate");

  FixedOffset::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  outerThick=Control.EvalVar<double>(keyName+"OuterThick");

  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");

  tubeAYStep=Control.EvalVar<double>(keyName+"TubeAYStep");
  tubeBYStep=Control.EvalVar<double>(keyName+"TubeBYStep");
  tubeRadius=Control.EvalVar<double>(keyName+"TubeRadius");
  tubeLength=Control.EvalVar<double>(keyName+"TubeLength");
  tubeThick=Control.EvalVar<double>(keyName+"TubeThick");
  tubeFlangeRadius=Control.EvalVar<double>(keyName+"TubeFlangeRadius");
  tubeFlangeLength=Control.EvalVar<double>(keyName+"TubeFlangeLength");

  scraperRadius=Control.EvalVar<double>(keyName+"ScraperRadius");
  scraperHeight=Control.EvalVar<double>(keyName+"ScraperHeight");
  scraperZLift=Control.EvalVar<double>(keyName+"ScraperZLift");

  driveRadius=Control.EvalVar<double>(keyName+"DriveRadius");

  topBoxWidth=Control.EvalVar<double>(keyName+"TopBoxWidth");
  topBoxHeight=Control.EvalVar<double>(keyName+"TopBoxHeight");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  tubeMat=ModelSupport::EvalMat<int>(Control,keyName+"TubeMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
  scraperMat=ModelSupport::EvalMat<int>(Control,keyName+"ScraperMat");
  driveMat=ModelSupport::EvalMat<int>(Control,keyName+"DriveMat");
  topMat=ModelSupport::EvalMat<int>(Control,keyName+"TopMat");

  return;
}


void
Scraper::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Scraper","createSurfaces");

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

  // main pipe and thickness
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+outerThick);

  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeRadius);
  ModelSupport::buildPlane(SMap,buildIndex+101,
			   Origin-Y*(length/2.0-flangeLength),Y);
  ModelSupport::buildPlane(SMap,buildIndex+202,
			   Origin+Y*(length/2.0-flangeLength),Y);

  // Main Top/Down Tubes
  const Geometry::Vec3D aOrg(Origin+Y*tubeAYStep);
  const Geometry::Vec3D bOrg(Origin+Y*tubeAYStep);
  ModelSupport::buildCylinder(SMap,buildIndex+307,aOrg,Z,tubeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+317,aOrg,Z,tubeRadius+tubeThick);

  ModelSupport::buildCylinder(SMap,buildIndex+305,aOrg-Z*tubeLength,Z);
  ModelSupport::buildCylinder
    (SMap,buildIndex+315,aOrg-Z*(tubeLength-tubeFlangeLength),Z);


  return;
}

void
Scraper::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Scraper","createObjects");

  std::string Out;

  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");


  Out=ModelSupport::getComposite
      (SMap,buildIndex," -411 -412 -413 -414 -415 -416 -417 -418 ");
  addOuterSurf(Out+frontStr+backStr);

  return;
}

void
Scraper::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("Scraper","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back

  return;
}

void
Scraper::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("Scraper","createAll");

  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,length);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE tdcSystem
