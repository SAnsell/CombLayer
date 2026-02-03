/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/BladeBPM.cxx
 *
 * Copyright (c) 2004-2026 by Konstantin Batkov
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

#include "BladeBPM.h"

namespace xraySystem
{

BladeBPM::BladeBPM(const std::string& Key)  :
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

BladeBPM::BladeBPM(const BladeBPM& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),chamberRadius(A.chamberRadius),chamberFlangeRadius(A.chamberFlangeRadius),
  chamberLength(A.chamberLength),
  chamberWallThick(A.chamberWallThick),
  chamberFlangeLength(A.chamberFlangeLength),
  chamberFlangeMat(A.chamberFlangeMat),chamberWallMat(A.chamberWallMat),
  voidMat(A.voidMat)
  /*!
    Copy constructor
    \param A :: BladeBPM to copy
  */
{}

BladeBPM&
BladeBPM::operator=(const BladeBPM& A)
  /*!
    Assignment operator
    \param A :: BladeBPM to copy
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
      chamberLength=A.chamberLength;
      chamberRadius=A.chamberRadius;
      chamberWallThick=A.chamberWallThick;
      chamberFlangeRadius=A.chamberFlangeRadius;
      chamberFlangeLength=A.chamberFlangeLength;
      chamberFlangeMat=A.chamberFlangeMat;
      chamberWallMat=A.chamberWallMat;
      voidMat=A.voidMat;
    }
  return *this;
}

BladeBPM*
BladeBPM::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new BladeBPM(*this);
}

BladeBPM::~BladeBPM()
  /*!
    Destructor
  */
{}

void
BladeBPM::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BladeBPM","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  chamberLength=Control.EvalVar<double>(keyName+"ChamberLength");
  chamberRadius=Control.EvalVar<double>(keyName+"ChamberRadius");
  chamberWallThick=Control.EvalVar<double>(keyName+"ChamberWallThick");
  chamberFlangeRadius=Control.EvalVar<double>(keyName+"ChamberFlangeRadius");
  chamberFlangeLength=Control.EvalVar<double>(keyName+"ChamberFlangeLength");

  chamberFlangeMat=ModelSupport::EvalMat<int>(Control,keyName+"ChamberFlangeMat");
  chamberWallMat=ModelSupport::EvalMat<int>(Control,keyName+"ChamberWallMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");

  return;
}

void
BladeBPM::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BladeBPM","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));

      ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*(chamberFlangeLength),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+1,
	      SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),
				      chamberFlangeLength);
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length+chamberFlangeLength),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));

      ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+2,
	      SMap.realPtr<Geometry::Plane>(getBackRule().getPrimarySurface()),
				      -chamberFlangeLength);
    }

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,chamberRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,chamberRadius+chamberWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,chamberFlangeRadius);

  // ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(chamberRadius/2.0),X);
  // ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(chamberRadius/2.0),X);


  return;
}

void
BladeBPM::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BladeBPM","createObjects");

  HeadRule HR;
  const HeadRule front(frontRule());
  const HeadRule back(backRule());

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -7 ");
  makeCell("ChamberVoid",System,cellIndex++,voidMat,0.0,HR*front*back);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 11 -12 7 -17 ");
  makeCell("ChamberWall",System,cellIndex++,chamberWallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -11 7 -27 ");
  makeCell("FlangeA",System,cellIndex++,chamberFlangeMat,0.0,HR*front);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 11 -12 17 -27 ");
  makeCell("FlangeBetween",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 12 7 -27 ");
  makeCell("FlangeB",System,cellIndex++,chamberFlangeMat,0.0,HR*back);

  // HR=ModelSupport::getHeadRule(SMap,buildIndex,
  // 				 " 13 -14 15 -16 (-1:2:-3:4:-5:6) ");
  // makeCell("Wall",System,cellIndex++,chamberWallMat,0.0,HR*front*back);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -27 ");
  addOuterSurf(HR*front*back);

  return;
}


void
BladeBPM::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("BladeBPM","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);


  return;
}

void
BladeBPM::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("BladeBPM","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
