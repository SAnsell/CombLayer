/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/FixedMaskHybrid.cxx
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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

#include "FixedMaskHybrid.h"

namespace xraySystem
{

FixedMaskHybrid::FixedMaskHybrid(const std::string& Key)  :
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

FixedMaskHybrid::FixedMaskHybrid(const FixedMaskHybrid& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),radius(A.radius),flangeLength(A.flangeLength),
  flangeRadius(A.flangeRadius),
  mat(A.mat),flangeMat(A.flangeMat)
  /*!
    Copy constructor
    \param A :: FixedMaskHybrid to copy
  */
{}

FixedMaskHybrid&
FixedMaskHybrid::operator=(const FixedMaskHybrid& A)
  /*!
    Assignment operator
    \param A :: FixedMaskHybrid to copy
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
      flangeLength=A.flangeLength;
      flangeRadius=A.flangeRadius;
      mat=A.mat;
      flangeMat=A.flangeMat;
    }
  return *this;
}

FixedMaskHybrid*
FixedMaskHybrid::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new FixedMaskHybrid(*this);
}

FixedMaskHybrid::~FixedMaskHybrid()
  /*!
    Destructor
  */
{}

void
FixedMaskHybrid::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("FixedMaskHybrid","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");

  return;
}

void
FixedMaskHybrid::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("FixedMaskHybrid","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+11,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+11));

      ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*(flangeRadius),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+1,
	      SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),
				      flangeRadius);
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length+flangeRadius),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+12));

      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+2,
	      SMap.realPtr<Geometry::Plane>(getBackRule().getPrimarySurface()),
				      -flangeRadius);
    }

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);

  return;
}

void
FixedMaskHybrid::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("FixedMaskHybrid","createObjects");

  HeadRule HR;
  const HeadRule front(frontRule());
  const HeadRule back(backRule());

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -7 ");
  makeCell("MainCell",System,cellIndex++,mat,0.0,HR*front*back);

  addOuterSurf(HR*front*back);

  return;
}


void
FixedMaskHybrid::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("FixedMaskHybrid","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  return;
}

void
FixedMaskHybrid::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("FixedMaskHybrid","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
