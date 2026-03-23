/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/WhiteBeamStop.cxx
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
#include "Quaternion.h"

#include "WhiteBeamStop.h"

namespace xraySystem
{

WhiteBeamStop::WhiteBeamStop(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

WhiteBeamStop::WhiteBeamStop(const WhiteBeamStop& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  length(A.length),width(A.width),height(A.height),
  angle(A.angle),
  inBeam(A.inBeam),
  offBeamOffset(A.offBeamOffset),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: WhiteBeamStop to copy
  */
{}

WhiteBeamStop&
WhiteBeamStop::operator=(const WhiteBeamStop& A)
  /*!
    Assignment operator
    \param A :: WhiteBeamStop to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      length=A.length;
      width=A.width;
      height=A.height;
      angle=A.angle;
      inBeam=A.inBeam;
      offBeamOffset=A.offBeamOffset;
      mat=A.mat;
    }
  return *this;
}

WhiteBeamStop*
WhiteBeamStop::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new WhiteBeamStop(*this);
}

WhiteBeamStop::~WhiteBeamStop()
  /*!
    Destructor
  */
{}

void
WhiteBeamStop::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("WhiteBeamStop","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  angle=Control.EvalVar<double>(keyName+"Angle");
  inBeam=Control.EvalVar<int>(keyName+"InBeam");
  offBeamOffset=Control.EvalVar<double>(keyName+"OffBeamOffset");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}

void
WhiteBeamStop::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("WhiteBeamStop","createSurfaces");

  const Geometry::Quaternion Qxy=Geometry::Quaternion::calcQRotDeg(angle,Z);
  const Geometry::Vec3D PX=Qxy.rotate(X);

  const double cosa = cos(angle*M_PI/180.0);
  const double offset = inBeam ? 0.0 : offBeamOffset;
  const double dl = cosa * length/2.0;

  SurfMap::makePlane("back",SMap,buildIndex+1,Origin-Y*(dl),Y);
  SurfMap::makePlane("front",SMap,buildIndex+2,Origin+Y*(dl),Y);

  SurfMap::makePlane("left",SMap,buildIndex+3,Origin+X*(offset),PX);
  SurfMap::makePlane("right",SMap,buildIndex+4,Origin+PX*(width)+X*(offset),PX);

  SurfMap::makePlane("bottom",SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  SurfMap::makePlane("top",SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  return;
}

void
WhiteBeamStop::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("WhiteBeamStop","createObjects");

  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 3 -4 5 -6 ");
  makeCell("BeamStop",System,cellIndex++,mat,0.0,HR);

  addOuterSurf(HR);

  return;
}


void
WhiteBeamStop::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("WhiteBeamStop","createLinks");

  // const Geometry::Quaternion Qxy=Geometry::Quaternion::calcQRotDeg(angle,Z);
  // const Geometry::Vec3D PX=Qxy.rotate(X);

  // const double cosa = cos(angle*M_PI/180.0);
  // const double offset = inBeam ? 0.0 : offBeamOffset;
  // const double dl = cosa * length/2.0;
  // const double dw = cosa * width/2.0;

  // ELog::EM << "Origin: " << Origin << ELog::endDiag;
  // ELog::EM << "PX: " << PX << ELog::endDiag;

  // FixedComp::setConnect(0,Origin+X*(offset),-PX);
  // FixedComp::setNamedLinkSurf(0,"Left",SurfMap::getSignedSurf("#left"));

  // FixedComp::setConnect(1,Origin+PX*(width)+X*(offset),PX);
  // FixedComp::setNamedLinkSurf(1,"Right",SurfMap::getSignedSurf("right"));

  // ELog::EM<<"*** "<<keyName<<": "<<getLinkPt("Right")<<" "<<getLinkSurf("Right")<<ELog::endDiag;

  // FixedComp::setConnect(2,Origin-Y*(dl)+X*(dw),-Y);
  // FixedComp::setNamedLinkSurf(2,"Back",SurfMap::getSignedSurf("#back"));

  // FixedComp::setConnect(3,Origin+Y*(dl)+X*(dw),X);
  // FixedComp::setNamedLinkSurf(3,"Front",SurfMap::getSignedSurf("front"));

  // FixedComp::setConnect(4,Origin-Z*(height/2.0)+X*(dw),-Z);
  // FixedComp::setNamedLinkSurf(4,"Bottom",SurfMap::getSignedSurf("#bottom"));

  // FixedComp::setConnect(5,Origin+Z*(height/2.0)+X*(dw),Z);
  // FixedComp::setNamedLinkSurf(5,"Top",SurfMap::getSignedSurf("#bottom"));


  return;
}

void
WhiteBeamStop::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("WhiteBeamStop","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
