/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/PBIP.cxx
 *
 * Copyright (c) 2017-2022 by Konstantin Batkov
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
#include "Quaternion.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
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
#include "ExternalCut.h"

#include "PBIP.h"

namespace essSystem
{

PBIP::PBIP(const std::string& Key)  :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedGroup("before","main","after"),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PBIP::PBIP(const PBIP& A) :
  attachSystem::FixedRotate(A),
  attachSystem::ContainedGroup(A),
  attachSystem::ExternalCut(A),
  engActive(A.engActive),
  length(A.length),width(A.width),height(A.height),
  wallThick(A.wallThick),
  mainMat(A.mainMat),wallMat(A.wallMat),
  pipeBeforeHeight(A.pipeBeforeHeight),
  pipeBeforeWidthLeft(A.pipeBeforeWidthLeft),
  pipeBeforeAngleLeft(A.pipeBeforeAngleLeft),
  pipeBeforeWidthRight(A.pipeBeforeWidthRight),
  pipeAfterHeight(A.pipeAfterHeight),
  pipeAfterWidthLeft(A.pipeAfterWidthLeft),
  pipeAfterWidthRight(A.pipeAfterWidthRight),
  pipeAfterAngleRight(A.pipeAfterAngleRight),
  foilOffset(A.foilOffset),
  foilThick(A.foilThick),
  foilMat(A.foilMat)
  /*!
    Copy constructor
    \param A :: PBIP to copy
  */
{}

PBIP&
PBIP::operator=(const PBIP& A)
  /*!
    Assignment operator
    \param A :: PBIP to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      engActive=A.engActive;
      length=A.length;
      width=A.width;
      height=A.height;
      wallThick=A.wallThick;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
      pipeBeforeHeight=A.pipeBeforeHeight;
      pipeBeforeWidthLeft=A.pipeBeforeWidthLeft;
      pipeBeforeAngleLeft=A.pipeBeforeAngleLeft;
      pipeBeforeWidthRight=A.pipeBeforeWidthRight;
      pipeAfterHeight=A.pipeAfterHeight;
      pipeAfterWidthLeft=A.pipeAfterWidthLeft;
      pipeAfterWidthRight=A.pipeAfterWidthRight;
      pipeAfterAngleRight=A.pipeAfterAngleRight;
      foilOffset=A.foilOffset;
      foilThick=A.foilThick;
      foilMat=A.foilMat;
    }
  return *this;
}

PBIP*
PBIP::clone() const
/*!
  Clone self
  \return new (this)
 */
{
  return new PBIP(*this);
}

PBIP::~PBIP()
  /*!
    Destructor
  */
{}

void
PBIP::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("PBIP","populate");

  FixedRotate::populate(Control);
  engActive=Control.EvalTail<int>(keyName,"","EngineeringActive");

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  pipeBeforeHeight=Control.EvalVar<double>(keyName+"PipeBeforeHeight");
  pipeBeforeWidthLeft=Control.EvalVar<double>(keyName+"PipeBeforeWidthLeft");
  pipeBeforeAngleLeft=Control.EvalVar<double>(keyName+"PipeBeforeAngleLeft");
  pipeBeforeWidthRight=Control.EvalVar<double>(keyName+"PipeBeforeWidthRight");
  pipeAfterHeight=Control.EvalVar<double>(keyName+"PipeAfterHeight");
  pipeAfterWidthLeft=Control.EvalVar<double>(keyName+"PipeAfterWidthLeft");
  pipeAfterWidthRight=Control.EvalVar<double>(keyName+"PipeAfterWidthRight");
  pipeAfterAngleRight=Control.EvalVar<double>(keyName+"PipeAfterAngleRight");
  foilOffset=Control.EvalVar<double>(keyName+"FoilOffset");
  foilThick=Control.EvalVar<double>(keyName+"FoilThick");
  foilMat=ModelSupport::EvalMat<int>(Control,keyName+"FoilMat");

  return;
}

void
PBIP::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PBIP","createSurfaces");

  // main
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*wallThick,Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length+wallThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(width/2.0+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(width/2.0+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(height/2.0+wallThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height/2.0+wallThick),Z);

  // foil
  ModelSupport::buildPlane(SMap,buildIndex+21,Origin+Y*(foilOffset),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*(foilOffset+foilThick),Y);

  // pipe before
  Geometry::Vec3D leftNorm(X);
  Geometry::Quaternion::calcQRotDeg(-pipeBeforeAngleLeft,Z).rotate(leftNorm);

  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(pipeBeforeWidthRight),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(pipeBeforeWidthLeft),leftNorm);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(pipeBeforeHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(pipeBeforeHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+113,
			   Origin-X*(pipeBeforeWidthRight+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+114,
			   Origin+X*(pipeBeforeWidthLeft+wallThick),leftNorm);
  ModelSupport::buildPlane(SMap,buildIndex+115,
			   Origin-Z*(pipeBeforeHeight/2.0+wallThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+116,
			   Origin+Z*(pipeBeforeHeight/2.0+wallThick),Z);

  // pipe after
  Geometry::Vec3D rightNorm(X);
  Geometry::Quaternion::calcQRotDeg(-pipeAfterAngleRight,Z).rotate(rightNorm);
  ModelSupport::buildPlane(SMap,buildIndex+203,
			   Origin+Y*(length)-X*(pipeAfterWidthRight),rightNorm);
  ModelSupport::buildPlane(SMap,buildIndex+204,Origin+X*(pipeAfterWidthLeft),X);
  ModelSupport::buildPlane(SMap,buildIndex+205,Origin-Z*(pipeAfterHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+206,Origin+Z*(pipeAfterHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+213,
		 Origin+Y*(length)-X*(pipeAfterWidthRight+wallThick),rightNorm);
  ModelSupport::buildPlane(SMap,buildIndex+214,
			   Origin+X*(pipeAfterWidthLeft+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+215,
			   Origin-Z*(pipeAfterHeight/2.0+wallThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+216,
			   Origin+Z*(pipeAfterHeight/2.0+wallThick),Z);

  return;
}

void
PBIP::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param FCstart :: FC at the start (Bulk)
    \param lpBulk :: side link point of start
    \param FCend :: FC at the end (BeRef)
    \param lpBulk :: side link point at FCend
  */
{
  ELog::RegMethod RegA("PBIP","createObjects");

  const HeadRule frontHR=getRule("front");
  const HeadRule backHR=getRule("back");

  //  FixedComp::setLinkCopy(0,FCend,-lpEnd);

  HeadRule HR;
  // main
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -21 3 -4 5 -6");
  System.addCell(cellIndex++,mainMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"21 -22 3 -4 5 -6");
  System.addCell(cellIndex++,foilMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"22 -2 3 -4 5 -6");
  System.addCell(cellIndex++,mainMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				"11 -12 13 -14 15 -16"
				"(-1:2:-3:4:-5:6) (1:-103:104:-105:106)"
				"(-2:-203:204:-205:206)");
  System.addCell(cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 13 -14 15 -16");
  addOuterSurf("main", HR);

  // before
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 103 -104 105 -106");
  System.addCell(cellIndex++,mainMat,0.0,HR*frontHR);
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-11 113 -114 115 -116 (-103:104:-105:106)");
  System.addCell(cellIndex++,wallMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 113 -114 115 -116");
  addOuterSurf("before",HR*frontHR);

  // after
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 203 -204 205 -206");
  System.addCell(cellIndex++,mainMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
		      "12 213 -214 215 -216 (-2:-203:204:-205:206)");
  System.addCell(cellIndex++,wallMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 213 -214 215 -216");
  addOuterUnionSurf("after",HR*backHR);

  return;
}


void
PBIP::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("PBIP","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}




void
PBIP::createAll(Simulation& System,
		const attachSystem::FixedComp& FC,const long int linkIndex)
//		const attachSystem::FixedComp& FCstart,const long int lpStart,
//		const attachSystem::FixedComp& FCend,const long int lpEnd)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param linkIndex :: link point for axis orientation
    \param FCstart :: FC at the start (Bulk)
    \param lpBulk :: side link point of start
    \param FCend :: FC at the end (BeRef)
    \param lpBulk :: side link point at FCend
  */
{
  ELog::RegMethod RegA("PBIP","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,linkIndex);
  createSurfaces();
  createLinks();
  createObjects(System);
  insertObjects(System);

  return;
}

}  // essSystem essSystem
