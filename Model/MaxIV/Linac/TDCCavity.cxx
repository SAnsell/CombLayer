/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/TDCCavity.cxx
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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

#include "TDCCavity.h"

namespace tdcSystem
{

TDCCavity::TDCCavity(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

TDCCavity::TDCCavity(const TDCCavity& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),radius(A.radius),height(A.height),
  wallThick(A.wallThick),
  mainMat(A.mainMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: TDCCavity to copy
  */
{}

TDCCavity&
TDCCavity::operator=(const TDCCavity& A)
  /*!
    Assignment operator
    \param A :: TDCCavity to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      length=A.length;
      radius=A.radius;
      height=A.height;
      wallThick=A.wallThick;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
    }
  return *this;
}

TDCCavity*
TDCCavity::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new TDCCavity(*this);
}

TDCCavity::~TDCCavity()
  /*!
    Destructor
  */
{}

void
TDCCavity::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("TDCCavity","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");
  height=Control.EvalVar<double>(keyName+"Height");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
TDCCavity::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("TDCCavity","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }

  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+2,
	      SMap.realPtr<Geometry::Plane>(getBackRule().getPrimarySurface()),
				      -wallThick);
    }

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);

  // ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(radius/2.0),X);
  // ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(radius/2.0),X);

  // ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  // ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  // ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(radius/2.0+wallThick),X);
  // ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(radius/2.0+wallThick),X);

  // ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(height/2.0+wallThick),Z);
  // ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height/2.0+wallThick),Z);

  return;
}

void
TDCCavity::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("TDCCavity","createObjects");

  std::string Out;
  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");

  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ")+frontStr+backStr;
  makeCell("MainCell",System,cellIndex++,mainMat,0.0,Out);

  addOuterSurf(Out);

  return;
}


void
TDCCavity::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("TDCCavity","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);
  ExternalCut::createLink("back",*this,1,Origin,Y);

  return;
}

void
TDCCavity::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("TDCCavity","createAll");

  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,length);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
