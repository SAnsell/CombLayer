/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacility/Solenoid.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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

#include "Solenoid.h"

namespace xraySystem
{

Solenoid::Solenoid(const std::string& Key)  :
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

Solenoid::Solenoid(const Solenoid& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),
  frameWidth(A.frameWidth),frameThick(A.frameThick),
  coilThick(A.coilThick),
  coilRadius(A.coilRadius),
  coilGap(A.coilGap),
  penRadius(A.penRadius),
  shellRadius(A.shellRadius),
  shellThick(A.shellThick),
  shellLength(A.shellLength),
  frameMat(A.frameMat),coilMat(A.coilMat),
  voidMat(A.voidMat),
  shellMat(A.shellMat),
  nCoils(A.nCoils),
  nFrameFacets(A.nFrameFacets)
  /*!
    Copy constructor
    \param A :: Solenoid to copy
  */
{}

Solenoid&
Solenoid::operator=(const Solenoid& A)
  /*!
    Assignment operator
    \param A :: Solenoid to copy
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
      frameWidth=A.frameWidth;
      coilThick=A.coilThick;
      frameThick=A.frameThick;
      coilRadius=A.coilRadius;
      coilGap=A.coilGap;
      penRadius=A.penRadius;
      shellRadius=A.shellRadius;
      shellThick=A.shellThick;
      shellLength=A.shellLength;
      frameMat=A.frameMat;
      coilMat=A.coilMat;
      voidMat=A.voidMat;
      shellMat=A.shellMat;
      nCoils=A.nCoils;
      nFrameFacets=A.nFrameFacets;
    }
  return *this;
}

Solenoid*
Solenoid::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new Solenoid(*this);
}

Solenoid::~Solenoid()
  /*!
    Destructor
  */
{}

double
Solenoid::getFrameRadius() const
/*!
  Return the radius of the circumscribed circle around the frame
 */
{
  ELog::RegMethod RegA("Solenoid","getFrameRadius");

  const double n = static_cast<double>(nFrameFacets);
  const double angle = M_PI/n; // half of angle between the facets
  const double a = frameWidth*tan(angle); // facet length (2D)

  return a/(2.0 * sin(M_PI/n));
}

void
Solenoid::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("Solenoid","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  frameWidth=Control.EvalVar<double>(keyName+"FrameWidth");
  frameThick=Control.EvalVar<double>(keyName+"FrameThick");
  coilThick=Control.EvalVar<double>(keyName+"CoilThick");
  coilRadius=Control.EvalVar<double>(keyName+"CoilRadius");
  coilGap=Control.EvalVar<double>(keyName+"CoilGap");
  penRadius=Control.EvalVar<double>(keyName+"PenetraionRadius");
  shellRadius=Control.EvalVar<double>(keyName+"ShellRadius");
  shellThick=Control.EvalVar<double>(keyName+"ShellThick");
  shellLength=Control.EvalVar<double>(keyName+"ShellLength");

  frameMat=ModelSupport::EvalMat<int>(Control,keyName+"FrameMat");
  coilMat=ModelSupport::EvalMat<int>(Control,keyName+"CoilMat");
  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  shellMat=ModelSupport::EvalMat<int>(Control,keyName+"ShellMat");
  nCoils=Control.EvalVar<size_t>(keyName+"NCoils");
  nFrameFacets=Control.EvalVar<size_t>(keyName+"NFrameFacets");

  const double R = getFrameRadius() + shellThick + Geometry::zeroTol;
  if (shellRadius < R)
    throw ColErr::RangeError<double_t>(shellRadius,R,std::numeric_limits<double>::infinity(),
				       "shellRadius must exceed the one of the circumscribed frame circle + shellThick.");

  return;
}

void
Solenoid::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Solenoid","createSurfaces");

  double dy = (shellLength - length)/2.0;

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+11,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+11));

      ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*(frameThick+dy),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+1,
	      SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),
				      frameThick+dy);
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(shellLength),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+12));

      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length+dy-frameThick),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+2,
	      SMap.realPtr<Geometry::Plane>(getBackRule().getPrimarySurface()),
				      -frameThick);
    }

  ModelSupport::buildPlane(SMap,buildIndex+301,Origin+Y*(shellThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+302,Origin+Y*(shellLength-shellThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+311,Origin+Y*(dy),Y);
  ModelSupport::buildShiftedPlane(SMap, buildIndex+312,buildIndex+311,Y,length);


  int CN(buildIndex+1000);
  double angle(0.0);
  const double dangle = 2.0*M_PI/static_cast<double>(nFrameFacets);
  for (size_t i=0; i<nFrameFacets; ++i) {
    const Geometry::Vec3D QR=X*cos(angle)+Z*sin(angle);
    ModelSupport::buildPlane(SMap,CN+51,Origin+QR*(frameWidth/2.0),QR);
    angle+=dangle;
    CN++;
  }

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,penRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,coilRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,coilRadius+coilGap);
  ModelSupport::buildCylinder(SMap,buildIndex+37,Origin,Y,shellRadius-shellThick);
  ModelSupport::buildCylinder(SMap,buildIndex+47,Origin,Y,shellRadius);

  int SI(buildIndex+20);
  dy += frameThick+coilThick;
  const double spacerThick = (length-2*frameThick - coilThick*static_cast<double>(nCoils))/(static_cast<double>(nCoils-1));
  if (spacerThick<0.0)
    throw ColErr::ExitAbort("Total length is less than the sum of coils/spacers");

  // planes for the coil spacers
  for (size_t i=1; i<nCoils; ++i) {
    ModelSupport::buildPlane(SMap,SI+1,Origin+Y*dy,Y);
    dy+=spacerThick;
    ModelSupport::buildPlane(SMap,SI+2,Origin+Y*dy,Y);
    dy+=coilThick;
    SI+=10;
  }

  return;
}

void
Solenoid::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Solenoid","createObjects");

  const HeadRule ICellHR=getRule("Inner");

  HeadRule HR;
  const HeadRule frontStr(frontRule());
  const HeadRule backStr(backRule());

  HR = ModelSupport::getHeadRule(SMap,buildIndex,"-301 7 -37");
  makeCell("ShellFront",System,cellIndex++,shellMat,0.0,HR*frontStr);

  HR = ModelSupport::getHeadRule(SMap,buildIndex,"301 -311 7 -37");
  makeCell("ShellGapFront",System,cellIndex++,voidMat,0.0,HR);

  HR = ModelSupport::getHeadRule(SMap,buildIndex,"312 -302 7 -37");
  makeCell("ShellGapBack",System,cellIndex++,voidMat,0.0,HR);

  HR = ModelSupport::getHeadRule(SMap,buildIndex,"302 7 -37");
  makeCell("ShellBack",System,cellIndex++,shellMat,0.0,HR*backStr);


  const std::string strFrame=
    ModelSupport::getSeqIntersection(-51, -(50+static_cast<int>(nFrameFacets)),1);
  const HeadRule HRFrame=ModelSupport::getHeadRule(SMap,buildIndex+1000,strFrame);

  HR = ModelSupport::getHeadRule(SMap,buildIndex,"311 -312 27");
  HR *= HRFrame;
  makeCell("Frame",System,cellIndex++,frameMat,0.0,HR);

  HR = ModelSupport::getHeadRule(SMap,buildIndex,"311 -312 -37");
  HR *= HRFrame.complement();
  makeCell("ShellGapSide",System,cellIndex++,voidMat,0.0,HR);

  HR = ModelSupport::getHeadRule(SMap,buildIndex,"37 -47");
  makeCell("Shell",System,cellIndex++,shellMat,0.0,HR*frontStr*backStr);

  // first coil
  HR=ModelSupport::getHeadRule(SMap,buildIndex,buildIndex,"1 -21 7 -17");
  makeCell("Coil",System,cellIndex++,coilMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,buildIndex,"1 -21 17 -27");
  makeCell("CoilGap",System,cellIndex++,voidMat,0.0,HR);

  int SI(buildIndex+20);
  for (size_t i=1; i<nCoils; ++i) {
    HR=ModelSupport::getHeadRule(SMap,buildIndex,SI,"1M -2M 7 -27");
    makeCell("Spacer",System,cellIndex++,frameMat,0.0,HR);

    if (i<nCoils-1)
      HR=ModelSupport::getHeadRule(SMap,buildIndex,SI,SI+10,"2M -1N 7 -17");
    else
      HR=ModelSupport::getHeadRule(SMap,buildIndex,SI,"2M -2 7 -17");
    makeCell("Coil",System,cellIndex++,coilMat,0.0,HR);

    if (i<nCoils-1)
      HR=ModelSupport::getHeadRule(SMap,buildIndex,SI,SI+10,"2M -1N 17 -27");
    else
      HR=ModelSupport::getHeadRule(SMap,buildIndex,SI,"2M -2 17 -27");
    makeCell("CoilGap",System,cellIndex++,voidMat,0.0,HR);

    SI+=10;
  }

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"311 -1 7 -27");
  makeCell("FrameFront",System,cellIndex++,frameMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -312 7 -27");
  makeCell("FrameBack",System,cellIndex++,frameMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7");
  makeCell("AxialPenetration",System,cellIndex++,voidMat,0.0,HR*frontStr*backStr*ICellHR);

  HR = ModelSupport::getHeadRule(SMap,buildIndex,"-47");
  addOuterSurf(HR*frontStr*backStr);

  return;
}


void
Solenoid::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("Solenoid","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  FixedComp::setConnect(2,Origin-X*(frameWidth/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(frameWidth/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(coilThick/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(coilThick/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
Solenoid::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("Solenoid","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
