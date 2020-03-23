/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/R3ChokeInsert.cxx
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "SurfMap.h"

#include "R3ChokeInsert.h"

namespace xraySystem
{

R3ChokeInsert::R3ChokeInsert(const std::string& Key) :
  attachSystem::FixedOffset(Key,3),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


R3ChokeInsert::~R3ChokeInsert()
  /*!
    Destructor
  */
{}

void
R3ChokeInsert::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("R3ChokeInsert","populate");

  FixedOffset::populate(Control);

  // Void + Fe special:
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");
  plateGap=Control.EvalVar<double>(keyName+"PlateGap");
  plateDepth=Control.EvalVar<double>(keyName+"PlateDepth");
  plateLength=Control.EvalVar<double>(keyName+"PlateLength");

  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");


  return;
}


void
R3ChokeInsert::createSurfaces()
  /*!
    Create the surfaces
    If front/back given it is at portLen from the wall and
    length/2+portLen from origin.
  */
{
  ELog::RegMethod RegA("R3ChokeInsert","createSurfaces");

  // front planes
  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }

  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*plateLength,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(plateThick/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(plateThick/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(plateGap/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(plateGap/2.0),Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+15,Origin-Z*(plateDepth+plateGap/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+16,Origin+Z*(plateDepth+plateGap/2.0),Z);

  return;
}

void
R3ChokeInsert::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("R3ChokeInsert","createObjects");

  std::string Out;

  const std::string frontStr=ExternalCut::getRuleStr("front");

  Out=ModelSupport::getComposite(SMap,buildIndex," -2 3 -4 5 -6 ");
  makeCell("Gap",System,cellIndex++,0,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -2 3 -4 -5 15 ");
  makeCell("TopPlate",System,cellIndex++,plateMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -2 3 -4 6 -16 ");
  makeCell("BasePlate",System,cellIndex++,plateMat,0.0,Out+frontStr);
  

  Out=ModelSupport::getComposite(SMap,buildIndex," -2 3 -4 15 -16 ");
  addOuterSurf(Out);


  return;
}

void
R3ChokeInsert::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("R3ChokeInsert","createLinks");

  return;
}

void
R3ChokeInsert::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("R3ChokeInsert","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
