/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/FlangePlate.cxx
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
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "FlangePlate.h"

namespace constructSystem
{

FlangePlate::FlangePlate(const std::string& Key) :
  attachSystem::FixedRotate(Key,2),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

FlangePlate::FlangePlate(const FlangePlate& A) :
  attachSystem::FixedRotate(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  innerRadius(A.innerRadius),flangeRadius(A.flangeRadius),
  flangeLength(A.flangeLength),
  innerMat(A.innerMat),flangeMat(A.flangeMat)
  /*!
    Copy constructor
    \param A :: FlangePlate to copy
  */
{}

FlangePlate&
FlangePlate::operator=(const FlangePlate& A)
  /*!
    Assignment operator
    \param A :: FlangePlate to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      innerRadius=A.innerRadius;
      flangeRadius=A.flangeRadius;
      flangeLength=A.flangeLength;
      innerMat=A.innerMat;
      flangeMat=A.flangeMat;
    }
  return *this;
}

FlangePlate::~FlangePlate()
  /*!
    Destructor
  */
{}

void
FlangePlate::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("FlangePlate","populate");

  FixedRotate::populate(Control);

  // Void + Fe special:
  innerRadius=Control.EvalDefVar<double>(keyName+"InnerRadius",-1.0);
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");

  innerMat=ModelSupport::EvalDefMat(Control,keyName+"InnerMat",0);
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");

  return;
}

void
FlangePlate::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("FlangePlate","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,
			       Origin-Y*(flangeLength/2.0),Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,
			       Origin+Y*(flangeLength/2.0),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
    }
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,flangeRadius);

  if (innerRadius>Geometry::zeroTol &&
      innerRadius<flangeRadius-Geometry::zeroTol)
    ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,innerRadius);

  return;
}

void
FlangePlate::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("FlangePlate","createObjects");

  std::string Out;

  const std::string frontStr=frontRule();
  const std::string backStr=backRule();

  if (innerRadius>Geometry::zeroTol)
    {
      Out=ModelSupport::getSetComposite(SMap,buildIndex," 107 -7 ");
      makeCell("Main",System,cellIndex++,flangeMat,0.0,Out+frontStr+backStr);

      Out=ModelSupport::getSetComposite(SMap,buildIndex," -107 ");
      makeCell("Inner",System,cellIndex++,innerMat,0.0,Out+frontStr+backStr);
    }
  else
    {
      Out=ModelSupport::getSetComposite(SMap,buildIndex," -7 ");
      makeCell("Main",System,cellIndex++,flangeMat,0.0,Out+frontStr+backStr);
    }

  Out=ModelSupport::getSetComposite(SMap,buildIndex," -7 ");
  addOuterSurf(Out+backStr+frontStr);

  return;
}


void
FlangePlate::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("FlangePlate","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);  //front and back
  return;
}


void
FlangePlate::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("FlangePlate","createAll(FC)");

  populate(System.getDataBase());
  createCentredUnitVector(FC,FIndex,flangeLength/2.0);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE constructSystem
