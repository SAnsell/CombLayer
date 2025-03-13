/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/OffsetFlangePipe.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include "Vec3D.h"
#include "Quaternion.h"
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
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"

#include "GeneralPipe.h"
#include "OffsetFlangePipe.h"

namespace constructSystem
{

OffsetFlangePipe::OffsetFlangePipe(const std::string& Key) :
  GeneralPipe(Key,11)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  nameSideIndex(9,"FlangeACentre");
  nameSideIndex(10,"FlangeBCentre");
}

OffsetFlangePipe::OffsetFlangePipe(const OffsetFlangePipe& A) :
  GeneralPipe(A),
  flangeAXStep(A.flangeAXStep),flangeAZStep(A.flangeAZStep),
  flangeAXYAngle(A.flangeAXYAngle),flangeAZAngle(A.flangeAZAngle),
  flangeARadius(A.flangeARadius),flangeALength(A.flangeALength),
  flangeBXStep(A.flangeBXStep),flangeBZStep(A.flangeBZStep),
  flangeBXYAngle(A.flangeBXYAngle),flangeBZAngle(A.flangeBZAngle),
  flangeBRadius(A.flangeBRadius),flangeBLength(A.flangeBLength),
  flangeAYAxis(A.flangeAYAxis),flangeBYAxis(A.flangeBYAxis)
  /*!
    Copy constructor
    \param A :: OffsetFlangePipe to copy
  */
{}

OffsetFlangePipe&
OffsetFlangePipe::operator=(const OffsetFlangePipe& A)
  /*!
    Assignment operator
    \param A :: OffsetFlangePipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      constructSystem::GeneralPipe::operator=(A);
      flangeAXStep=A.flangeAXStep;
      flangeAZStep=A.flangeAZStep;
      flangeAXYAngle=A.flangeAXYAngle;
      flangeAZAngle=A.flangeAZAngle;
      flangeARadius=A.flangeARadius;
      flangeALength=A.flangeALength;
      flangeBXStep=A.flangeBXStep;
      flangeBZStep=A.flangeBZStep;
      flangeBXYAngle=A.flangeBXYAngle;
      flangeBZAngle=A.flangeBZAngle;
      flangeBRadius=A.flangeBRadius;
      flangeBLength=A.flangeBLength;
      flangeAYAxis=A.flangeAYAxis;
      flangeBYAxis=A.flangeBYAxis;
    }
  return *this;
}

OffsetFlangePipe::~OffsetFlangePipe()
  /*!
    Destructor
  */
{}

void
OffsetFlangePipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("OffsetFlangePipe","populate");

  GeneralPipe::populate(Control);

  flangeARadius=Control.EvalPair<double>(keyName+"FlangeARadius",
					 keyName+"FlangeRadius");
  flangeBRadius=Control.EvalPair<double>(keyName+"FlangeBRadius",
					 keyName+"FlangeRadius");

  flangeALength=flangeA.type ? Control.EvalPair<double>(keyName+"FlangeALength",
							keyName+"FlangeLength") : 0.0;
  flangeBLength=flangeB.type ? Control.EvalPair<double>(keyName+"FlangeBLength",
							keyName+"FlangeLength") : 0.0;

  flangeAXStep=Control.EvalDefPair<double>(keyName+"FlangeAXStep",
					   keyName+"FlangeXStep",0.0);
  flangeAZStep=Control.EvalDefPair<double>(keyName+"FlangeAZStep",
					   keyName+"FlangeZStep",0.0);
  flangeAXYAngle=Control.EvalDefVar<double>
    (keyName+"FlangeAXYAngle",0.0);

  flangeAZAngle=Control.EvalDefVar<double>
    (keyName+"FlangeAZAngle",0.0);

  flangeBXStep=Control.EvalDefPair<double>(keyName+"FlangeBXStep",
					   keyName+"FlangeXStep",0.0);
  flangeBZStep=Control.EvalDefPair<double>(keyName+"FlangeBZStep",
					   keyName+"FlangeZStep",0.0);
  flangeBXYAngle=Control.EvalDefVar<double>
    (keyName+"FlangeBXYAngle",0.0);

  flangeBZAngle=Control.EvalDefVar<double>
    (keyName+"FlangeBZAngle",0.0);


  return;
}

void
OffsetFlangePipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("OffsetFlangePipe","createSurfaces");

  // Inner void
  flangeAYAxis=Y;
  flangeBYAxis=Y;

  if (!frontActive())
    {
      const Geometry::Quaternion Qz=
	Geometry::Quaternion::calcQRotDeg(flangeAZAngle,X);
      const Geometry::Quaternion Qxy=
	Geometry::Quaternion::calcQRotDeg(flangeAXYAngle,Z);
      Qz.rotate(flangeAYAxis);
      Qxy.rotate(flangeAYAxis);
      ModelSupport::buildPlane(SMap,buildIndex+1,
			       Origin-Y*(length/2.0),flangeAYAxis);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    }
  getShiftedFront(SMap,buildIndex+101,flangeAYAxis,flangeALength);


  if (!backActive())
    {
      const Geometry::Quaternion Qz=
	Geometry::Quaternion::calcQRotDeg(flangeBZAngle,X);
      const Geometry::Quaternion Qxy=
	Geometry::Quaternion::calcQRotDeg(flangeBXYAngle,Z);
      Qz.rotate(flangeBYAxis);
      Qxy.rotate(flangeBYAxis);
      ModelSupport::buildPlane(SMap,buildIndex+2,
			       Origin+Y*(length/2.0),flangeBYAxis);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
    }
  getShiftedBack(SMap,buildIndex+102,flangeBYAxis,-flangeBLength);

  makeCylinder("InnerCyl",SMap,buildIndex+7,Origin,Y,radius);
  makeCylinder("PipeCyl",SMap,buildIndex+17,Origin,Y,radius+pipeThick);
  addSurf("OuterRadius",SMap.realSurf(buildIndex+17));

  // FLANGE SURFACES FRONT/BACK:
  ModelSupport::buildCylinder(SMap,buildIndex+107,
			      Origin+X*flangeAXStep+Z*flangeAZStep,flangeAYAxis,
			      flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,
			      Origin+X*flangeBXStep+Z*flangeBZStep,flangeBYAxis,
			      flangeBRadius);
  return;
}

void
OffsetFlangePipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("OffsetFlangePipe","createObjects");

  HeadRule HR;
  const HeadRule& frontHR=getFrontRule();
  const HeadRule& backHR=getBackRule();

  // Void
  HR=HeadRule(SMap,buildIndex,-7);
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  // FLANGE Front:
  if (flangeA.type) {
    HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 -107 7");
    makeCell("FrontFlange",System,cellIndex++,pipeMat,0.0,HR*frontHR);
  }

  // FLANGE Back:

  if (flangeB.type) {
    HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 -207 7");
    makeCell("BackFlange",System,cellIndex++,pipeMat,0.0,HR*backHR);
  }

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 -17 7");
  makeCell("MainPipe",System,cellIndex++,pipeMat,0.0,HR);

  // outer boundary [flange front/back]
  if (flangeA.type) {
    HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 -107");
    addOuterSurf("FlangeA",HR*frontHR);
  }
  if (flangeB.type) {
    HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 -207");
    addOuterSurf("FlangeB",HR*backHR);
  }
  // outer boundary mid tube
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 -17");
  addOuterSurf("Main",HR);

  return;
}

void
OffsetFlangePipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("OffsetFlangePipe","createLinks");

  //stuff for intersection
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back
  FixedComp::setConnect(2,Origin-X*radius,-X);
  FixedComp::setConnect(3,Origin+X*radius,X);
  FixedComp::setConnect(4,Origin-Z*radius,-Z);
  FixedComp::setConnect(5,Origin+Z*radius,Z);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+7));

  // pipe wall
  FixedComp::setConnect(7,Origin-Z*(radius+pipeThick),-Z);
  FixedComp::setConnect(8,Origin+Z*(radius+pipeThick),Z);
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+17));

  FixedComp::nameSideIndex(7,"outerPipe");
  FixedComp::nameSideIndex(7,"pipeOuterBase");
  FixedComp::nameSideIndex(8,"pipeOuterTop");

  // flange mid point
  FixedComp::setLinkCopy(9,*this,1);
  FixedComp::setLinkCopy(10,*this,2);
  FixedComp::setConnect
    (9,FixedComp::getLinkPt(1)+X*flangeAXStep+Z*flangeAZStep,flangeAYAxis);
  FixedComp::setConnect
    (10,FixedComp::getLinkPt(2)+X*flangeBXStep+Z*flangeBZStep,flangeBYAxis);

  return;
}

void
OffsetFlangePipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("OffsetFlangePipe","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE constructSystem
