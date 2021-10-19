/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/BeamScrapper.cxx
 *
 * Copyright (c) 2004-2021 Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Line.h"
#include "surfRegister.h"
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Quaternion.h"

#include "BeamScrapper.h"

namespace xraySystem
{

BeamScrapper::BeamScrapper(const std::string& Key)  :
  attachSystem::ContainedGroup("Payload","Connect","Outer"),
  attachSystem::FixedRotate(Key,6),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


BeamScrapper::~BeamScrapper()
  /*!
    Destructor
  */
{}

void
BeamScrapper::calcImpactVector()
  /*!
    Calculate the impact points of the main beam  on the screen surface:
    We have the beamAxis this must intersect the screen and mirror closest to 
    their centre points. It DOES NOT need to hit the centre points as the mirror
    system is confined to moving down the Y axis of the object. 
    [-ve Y from flange  to beam centre]
  */
{
  ELog::RegMethod RegA("BeamScrapper","calcImpactVector");

  // defined points:

  // This point is the beam centre point between the main axis:

  std::tie(std::ignore,beamCentre)=
    beamAxis.closestPoints(Geometry::Line(Origin,Y));

  // Thread point
  plateCentre=beamCentre-Y*plateOffset;
  return;
}


void
BeamScrapper::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BeamScrapper","populate");

  FixedRotate::populate(Control);

  tubeRadius=Control.EvalVar<double>(keyName+"TubeRadius");
  tubeOffset=Control.EvalVar<double>(keyName+"TubeOffset");
  tubeWall=Control.EvalVar<double>(keyName+"TubeWall");
  plateOffset=Control.EvalVar<double>(keyName+"PlateOffset");
  plateAngle=Control.EvalVar<double>(keyName+"PlateAngle");
  plateLength=Control.EvalVar<double>(keyName+"PlateLength");
  plateHeight=Control.EvalVar<double>(keyName+"PlateHeight");
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");

  tubeWidth=Control.EvalVar<double>(keyName+"TubeWidth");
  tubeHeight=Control.EvalVar<double>(keyName+"TubeHeight");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");
  copperMat=ModelSupport::EvalMat<int>(Control,keyName+"CopperMat");

  return;
}

void
BeamScrapper::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BeamScrapper","createSurfaces");

  calcImpactVector();

  if (!isActive("FlangePlate"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("FlangePlate",SMap.realSurf(buildIndex+1));
    }
  
  ModelSupport::buildCylinder
    (SMap,buildIndex+7,beamCentre-X*tubeOffset,Y,tubeRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+8,beamCentre+X*tubeOffset,Y,tubeRadius);

  ModelSupport::buildCylinder
    (SMap,buildIndex+17,beamCentre-X*tubeOffset,Y,tubeRadius+tubeWall);
  ModelSupport::buildCylinder
    (SMap,buildIndex+18,beamCentre+X*tubeOffset,Y,tubeRadius+tubeWall);
  
  // screen+mirror thread
  ModelSupport::buildPlane(SMap,buildIndex+201,plateCentre,Y);

  return;
}

void
BeamScrapper::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BeamScrapper","createObjects");

  const HeadRule plateHR=getRule("FlangePlate");
  HeadRule HR;

  // linear pneumatics feedthrough
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 -201");
  makeCell("water",System,cellIndex++,waterMat,0.0,HR*plateHR);
  addOuterSurf("Payload",HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-8 -201");
  makeCell("water",System,cellIndex++,waterMat,0.0,HR*plateHR);


  addOuterUnionSurf("Payload",HR);      
      
  return;
}


void
BeamScrapper::createLinks()
  /*!
    Create all the linkes [need front/back to join/use InnerZone]
  */
{
  ELog::RegMethod RegA("BeamScrapper","createLinks");

  return;
}

void
BeamScrapper::setBeamAxis(const attachSystem::FixedComp& FC,
			    const long int sIndex)
  /*!
    Set the screen centre
    \param FC :: FixedComp to use
    \param sIndex :: Link point index
  */
{
  ELog::RegMethod RegA("BeamScrapper","setBeamAxis(FC)");

  beamAxis=Geometry::Line(FC.getLinkPt(sIndex),
			  FC.getLinkAxis(sIndex));


  return;
}

void
BeamScrapper::setBeamAxis(const Geometry::Vec3D& Org,
		       const Geometry::Vec3D& Axis)
  /*!
    Set the screen centre
    \param Org :: Origin point for line
    \param Axis :: Axis of line
  */
{
  ELog::RegMethod RegA("BeamScrapper","setBeamAxis(Vec3D)");

  beamAxis=Geometry::Line(Org,Axis);
  return;
}

void
BeamScrapper::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("BeamScrapper","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
