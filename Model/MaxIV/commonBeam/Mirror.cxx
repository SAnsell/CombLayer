/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/Mirror.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "Mirror.h"


namespace xraySystem
{

Mirror::Mirror(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,8),
  attachSystem::CellMap(),attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}

Mirror::Mirror(const Mirror& A) :
  attachSystem::ContainedComp(A),attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  theta(A.theta),phi(A.phi),radius(A.radius),width(A.width),
  thick(A.thick),length(A.length),baseTop(A.baseTop),
  baseDepth(A.baseDepth),baseOutWidth(A.baseOutWidth),
  baseGap(A.baseGap),mirrMat(A.mirrMat),baseMat(A.baseMat)
  /*!
    Copy constructor
    \param A :: Mirror to copy
  */
{}

Mirror&
Mirror::operator=(const Mirror& A)
  /*!
    Assignment operator
    \param A :: Mirror to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      theta=A.theta;
      phi=A.phi;
      radius=A.radius;
      width=A.width;
      thick=A.thick;
      length=A.length;
      baseTop=A.baseTop;
      baseDepth=A.baseDepth;
      baseOutWidth=A.baseOutWidth;
      baseGap=A.baseGap;
      mirrMat=A.mirrMat;
      baseMat=A.baseMat;
    }
  return *this;
}



Mirror::~Mirror()
  /*!
    Destructor
   */
{}

void
Mirror::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("Mirror","populate");

  FixedRotate::populate(Control);

  theta=Control.EvalVar<double>(keyName+"Theta");
  phi=Control.EvalDefVar<double>(keyName+"Phi",0.0);

  radius=Control.EvalDefVar<double>(keyName+"Radius",0.0);
  width=Control.EvalVar<double>(keyName+"Width");
  thick=Control.EvalVar<double>(keyName+"Thick");
  length=Control.EvalVar<double>(keyName+"Length");

  baseTop=Control.EvalVar<double>(keyName+"BaseTop");
  baseDepth=Control.EvalVar<double>(keyName+"BaseDepth");
  baseOutWidth=Control.EvalVar<double>(keyName+"BaseOutWidth");
  baseGap=Control.EvalVar<double>(keyName+"BaseGap");

  mirrMat=ModelSupport::EvalMat<int>(Control,keyName+"MirrorMat");
  baseMat=ModelSupport::EvalMat<int>(Control,keyName+"BaseMat");

  return;
}


void
Mirror::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("Mirror","createSurfaces");

  // main xstal CENTRE AT ORIGIN
  const Geometry::Quaternion QXA
    (Geometry::Quaternion::calcQRotDeg(-theta,X));

  Geometry::Vec3D PX(X);
  Geometry::Vec3D PY(Y);
  Geometry::Vec3D PZ(Z);

  QXA.rotate(PY);
  QXA.rotate(PZ);

  const Geometry::Quaternion QYA
    (Geometry::Quaternion::calcQRotDeg(phi,PY));

  QYA.rotate(PX);
  QYA.rotate(PZ);

  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-PY*(length/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+PY*(length/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-PX*(width/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+PX*(width/2.0),PX);

  if (std::abs(radius)>Geometry::zeroTol)
    {
      // calc edge cut
      const double tAngle = length/(2.0*radius);  // cos(-a) == cos(a)
      const double lift = radius*(1.0-cos(tAngle));
      if (radius<0)
	ModelSupport::buildPlane(SMap,buildIndex+105,Origin-PZ*lift,-PZ);
      else
	ModelSupport::buildPlane(SMap,buildIndex+105,Origin-PZ*lift,PZ);
      ModelSupport::buildCylinder(SMap,buildIndex+107,
				  Origin-PZ*radius,PX,std::abs(radius));
      ModelSupport::buildCylinder(SMap,buildIndex+117,
				  Origin-PZ*radius,PX,std::abs(radius)+thick);
    }
  else
    {
      ModelSupport::buildPlane(SMap,buildIndex+105,Origin-PZ*thick,PZ);
      ModelSupport::buildPlane(SMap,buildIndex+106,Origin,PZ);
    }


  // support
  ModelSupport::buildPlane(SMap,buildIndex+203,
			   Origin-PX*(baseOutWidth+width/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+204,
			   Origin+PX*(baseOutWidth+width/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+205,Origin+PZ*baseTop,PZ);
  ModelSupport::buildPlane(SMap,buildIndex+206,Origin-PZ*baseDepth,PZ);


  ModelSupport::buildPlane(SMap,buildIndex+216,Origin-PZ*(thick+baseGap),PZ);

  /// create the link point towards the reflected beam
  // (do it here to avoid re-definition of variables in createLinks()

  Geometry::Vec3D Yrefl(PY);
  Geometry::Quaternion::calcQRotDeg(-theta,X).rotate(Yrefl);

  FixedComp::setConnect(1,Origin+PZ*baseTop,Yrefl);
  FixedComp::setLinkSurf(1,-SMap.realSurf(buildIndex+205));

  return;
}

void
Mirror::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("Mirror","createObjects");

  HeadRule HR;
  // xstal
  if (std::abs(radius)<Geometry::zeroTol)
    HR=ModelSupport::getHeadRule(SMap,buildIndex,
				   " 101 -102 103 -104 105 -106 ");
  else
    HR=ModelSupport::getHeadRule
      (SMap,buildIndex," 103 -104 107 -117 105 ");

  makeCell("Mirror",System,cellIndex++,mirrMat,0.0,HR);

  // Make sides
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 101 -102 -103 203 -205 206 ");
  makeCell("LeftSide",System,cellIndex++,baseMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 101 -102 104 -204 -205 206 ");
  makeCell("RightSide",System,cellIndex++,baseMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 " 101 -102 103 -104 -216 206 ");
  makeCell("Base",System,cellIndex++,baseMat,0.0,HR);

  // vacuum units:
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex," 101 -102 103 -104 -105 216" );
  makeCell("BaseVac",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex," 101 -102 103 -104 -205 106 " );
  makeCell("TopVac",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex," 101 -102 203 -204 -205 206" );
  addOuterSurf(HR);

  return;
}

void
Mirror::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("Mirror","createLinks");

  // link points are defined in the end of createSurfaces

  return;
}

void
Mirror::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("Mirror","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
