/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/formax/formaxDetectorTube.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "BlockZone.h"
#include "FrontBackCut.h"
#include "ContainedGroup.h"
#include "generalConstruct.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "portItem.h"
#include "portSet.h"
#include "FlangeDome.h"
#include "GateValveCylinder.h"
#include "MonoBeamStop.h"
#include "AreaDetector.h"

#include "formaxDetectorTube.h"

namespace xraySystem
{

formaxDetectorTube::formaxDetectorTube(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut(),

  buildZone(Key+"BuildZone"),
  
  mainTube
  ({
    std::make_shared<constructSystem::PipeTube>(keyName+"Segment0"),
    std::make_shared<constructSystem::PipeTube>(keyName+"Segment1"),
    std::make_shared<constructSystem::PipeTube>(keyName+"Segment2"),
    std::make_shared<constructSystem::PipeTube>(keyName+"Segment3"),
    std::make_shared<constructSystem::PipeTube>(keyName+"Segment4"),
    std::make_shared<constructSystem::PipeTube>(keyName+"Segment5"),
    std::make_shared<constructSystem::PipeTube>(keyName+"Segment6"),
    std::make_shared<constructSystem::PipeTube>(keyName+"Segment7")
  }),
  frontDome(new constructSystem::FlangeDome(keyName+"FrontDome")),
  backDome(new constructSystem::FlangeDome(keyName+"BackDome")),
  monoBeamStop(new xraySystem::MonoBeamStop(keyName+"BeamStop")),
  waxs(new xraySystem::AreaDetector(keyName+"WAXS"))
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
 */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  for(size_t i=0;i<8;i++)
    OR.addObject(mainTube[i]);
  OR.addObject(frontDome);
  OR.addObject(backDome);
  OR.addObject(waxs);
  OR.addObject(monoBeamStop);
}

formaxDetectorTube::~formaxDetectorTube()
  /*!
    Destructor
  */
{}

void
formaxDetectorTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("formaxDetectorTube","populate");

  FixedRotate::populate(Control);

  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  outerMat=ModelSupport::EvalDefMat(Control,keyName+"OuterMat",0);
  
  return;
}

void
formaxDetectorTube::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("formaxDetectorTube","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }

  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*outerLength,Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,outerRadius);

  const std::string Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  const HeadRule HR(Out);

  buildZone.setSurround(HR);
  buildZone.setFront(getRule("front"));
  buildZone.setMaxExtent(getRule("back"));
  buildZone.setInnerMat(outerMat);
  return;
}

void
formaxDetectorTube::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("formaxDetectorTube","createObjects");

  int outerCell;
  
  buildZone.addInsertCells(this->getInsertCells());

  mainTube[0]->createAll(System,*this,0);
  outerCell=buildZone.createUnit(System,*mainTube[0],-1);
  
  frontDome->setCutSurf("plate",*mainTube[0],"front");
  frontDome->createAll(System,*mainTube[0],1);
  frontDome->insertInCell(System,outerCell);
  
  outerCell=buildZone.createUnit(System,*mainTube[0],2);
  mainTube[0]->insertAllInCell(System,outerCell);

  for(size_t i=1;i<8;i++)
    {
      constructSystem::constructUnit
	(System,buildZone,*mainTube[i-1],"back",*mainTube[i]);
    }  

  backDome->setCutSurf("plate",*mainTube[7],"back");
  backDome->createAll(System,*mainTube[7],2);
  const constructSystem::portItem& BPI=backDome->getPort(1);
  outerCell=buildZone.createUnit(System,BPI,"OuterPlate");  
  backDome->insertInCell(System,outerCell);

  // Construct inner stuff
  monoBeamStop->createAll(System,*this,0);
  waxs->createAll(System,*this,0);
  const Geometry::Vec3D BPoint(monoBeamStop->getLinkPt(0));
  const Geometry::Vec3D CPoint(waxs->getLinkPt(0));
  for(size_t i=0;i<8;i++)
    {
      if (mainTube[i]->getCellHR(System,"Void").isValid(BPoint))
	{
	  monoBeamStop->insertInCell(*mainTube[i]->getCellObject(System,"Void"));
	}
      if (mainTube[i]->getCellHR(System,"Void").isValid(CPoint))
	{
	  waxs->insertInCell(*mainTube[i]->getCellObject(System,"Void"));
	}
    }
  
  buildZone.createUnit(System);
  buildZone.rebuildInsertCells(System);
  setCell("FirstVoid",buildZone.getCell("Unit"));
  setCell("LastVoid",buildZone.getLastCell("Unit"));
  lastComp=mainTube[7];
  return;
}

void
formaxDetectorTube::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("formaxDetectorTube","createLinks");

  const constructSystem::portItem& API=frontDome->getPort(0);
  setLinkCopy(0,API,"OuterPlate");

  setLinkCopy(1,*backDome,2);
  return;
}

void
formaxDetectorTube::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("formaxDetectorTube","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem


