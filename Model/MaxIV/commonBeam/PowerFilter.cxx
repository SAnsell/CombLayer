/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/PowerFilter.cxx
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
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Quaternion.h"
#include "Line.h"
#include "BeamAxis.h"

#include "PowerFilter.h"

namespace xraySystem
{

PowerFilter::PowerFilter(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  constructSystem::BeamAxis()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PowerFilter::PowerFilter(const PowerFilter& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  constructSystem::BeamAxis(A),
  maxLength(A.maxLength),width(A.width),height(A.height),
  baseLength(A.baseLength),
  wedgeAngle(A.wedgeAngle),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: PowerFilter to copy
  */
{}

PowerFilter&
PowerFilter::operator=(const PowerFilter& A)
  /*!
    Assignment operator
    \param A :: PowerFilter to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      constructSystem::BeamAxis::operator=(A);
      maxLength=A.maxLength;
      baseLength=A.baseLength;
      wedgeAngle=A.wedgeAngle;
      width=A.width;
      height=A.height;
      mat=A.mat;
    }
  return *this;
}

PowerFilter*
PowerFilter::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new PowerFilter(*this);
}

PowerFilter::~PowerFilter()
  /*!
    Destructor
  */
{}

void
PowerFilter::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("PowerFilter","populate");

  FixedRotate::populate(Control);

  maxLength=Control.EvalVar<double>(keyName+"MaxLength");
  baseLength=Control.EvalVar<double>(keyName+"BaseLength");
  wedgeAngle=Control.EvalVar<double>(keyName+"WedgeAngle");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  baseHeight=Control.EvalVar<double>(keyName+"BaseHeight");
    filterZOffset=Control.EvalVar<double>(keyName+"FilterZOffset");
    filterGap=Control.EvalVar<double>(keyName+"FilterGap");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}

void
PowerFilter::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PowerFilter","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(filterGap/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(baseLength+filterGap/2.0),Y);

  Geometry::Vec3D v(Y);
  const double totalHeight = height+baseHeight;
  Geometry::Quaternion::calcQRotDeg(-wedgeAngle,X).rotate(v);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(maxLength-filterGap/2.0)+Z*(totalHeight/2.0),v);

  SurfMap::makePlane("left",SMap,buildIndex+3,Origin-X*(width/2.0),X);
  SurfMap::makePlane("right",SMap,buildIndex+4,Origin+X*(width/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(totalHeight/2.0-filterZOffset),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(filterZOffset+totalHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(filterZOffset+totalHeight/2.0-baseHeight),Z);

  return;
}

void
PowerFilter::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PowerFilter","createObjects");

  const HeadRule lrHR = ModelSupport::getHeadRule(SMap,buildIndex,"3 -4");

  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 11 -1 16 -6 ");
  makeCell("Base",System,cellIndex++,mat,0.0,HR*lrHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 11 -1 5 -16 ");
  makeCell("VoidBeforeBlade",System,cellIndex++,0,0.0,HR*lrHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 5 -6 ");
  makeCell("Blade",System,cellIndex++,mat,0.0,HR*lrHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 11 -2 5 -6 ");
  addOuterSurf(HR*lrHR);

  return;
}


void
PowerFilter::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("PowerFilter","createLinks");

  // FixedComp::setConnect(0,Origin-Y*(maxLength/2.0),-Y);
  // FixedComp::setNamedLinkSurf(0,"Back",SurfMap::getSignedSurf("#back"));

  // // TODO: Check and use names for the links below:

  // FixedComp::setConnect(1,Origin+Y*(maxLength/2.0),Y);
  // FixedComp::setNamedLinkSurf(1,"Front",SMap.realSurf(buildIndex+2));

  // FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  // FixedComp::setNamedLinkSurf(2,"Left",-SMap.realSurf(buildIndex+3));

  // FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  // FixedComp::setNamedLinkSurf(3,"Right",SMap.realSurf(buildIndex+4));

  // FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  // FixedComp::setNamedLinkSurf(4,"Bottom",-SMap.realSurf(buildIndex+5));

  // FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  // FixedComp::setNamedLinkSurf(5,"Top",SMap.realSurf(buildIndex+6));

  return;
}

void
PowerFilter::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("PowerFilter","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
