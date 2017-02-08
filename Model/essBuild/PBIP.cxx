/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/PBIP.cxx
 *
 * Copyright (c) 2017 by Konstantin Batkov
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
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "FixedOffset.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "PBIP.h"

namespace essSystem
{

PBIP::PBIP(const std::string& Key)  :
  attachSystem::ContainedGroup("before","main","after"),
  attachSystem::FixedOffset(Key,6),
  surfIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PBIP::PBIP(const PBIP& A) :
  attachSystem::ContainedGroup(A),
  attachSystem::FixedOffset(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
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
  pipeAfterAngleRight(A.pipeAfterAngleRight)
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
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
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

  FixedOffset::populate(Control);
  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");

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

  return;
}

void
PBIP::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: object for origin
  */
{
  ELog::RegMethod RegA("PBIP","createUnitVector");

  FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

  return;
}

void
PBIP::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PBIP","createSurfaces");

  ModelSupport::buildPlane(SMap,surfIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(length),Y);
  ModelSupport::buildPlane(SMap,surfIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,surfIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane(SMap,surfIndex+11,Origin-Y*wallThick,Y);
  ModelSupport::buildPlane(SMap,surfIndex+12,Origin+Y*(length+wallThick),Y);
  ModelSupport::buildPlane(SMap,surfIndex+13,Origin-X*(width/2.0+wallThick),X);
  ModelSupport::buildPlane(SMap,surfIndex+14,Origin+X*(width/2.0+wallThick),X);
  ModelSupport::buildPlane(SMap,surfIndex+15,Origin-Z*(height/2.0+wallThick),Z);
  ModelSupport::buildPlane(SMap,surfIndex+16,Origin+Z*(height/2.0+wallThick),Z);

  // pipe before
  Geometry::Vec3D leftNorm(X);
  Geometry::Quaternion::calcQRotDeg(-pipeBeforeAngleLeft,Z).rotate(leftNorm);

  ModelSupport::buildPlane(SMap,surfIndex+103,Origin-X*(pipeBeforeWidthRight),X);
  ModelSupport::buildPlane(SMap,surfIndex+104,Origin+X*(pipeBeforeWidthLeft),leftNorm);
  ModelSupport::buildPlane(SMap,surfIndex+105,Origin-Z*(pipeBeforeHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,surfIndex+106,Origin+Z*(pipeBeforeHeight/2.0),Z);

  // pipe after
  Geometry::Vec3D rightNorm(X);
  Geometry::Quaternion::calcQRotDeg(-pipeAfterAngleRight,Z).rotate(rightNorm);
  ModelSupport::buildPlane(SMap,surfIndex+203,
			   Origin+Y*(length)-X*(pipeAfterWidthRight),rightNorm);
  ModelSupport::buildPlane(SMap,surfIndex+204,Origin+X*(pipeAfterWidthLeft),X);
  ModelSupport::buildPlane(SMap,surfIndex+205,Origin-Z*(pipeAfterHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,surfIndex+206,Origin+Z*(pipeAfterHeight/2.0),Z);

  return;
}

void
PBIP::createObjects(Simulation& System,
		    const attachSystem::FixedComp& FCstart,const long int& lpStart,
		    const attachSystem::FixedComp& FCend,const long int& lpEnd)
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

  const size_t lIndex(static_cast<size_t>(std::abs(lpEnd)-1));
  std::string BSurf=(lpEnd>0) ?
    FCend.getLinkString(lIndex) : FCend.getBridgeComplement(lIndex) ;
  FixedComp::setLinkComponent(0,FCend,lIndex);

  std::string Out,before,after;
  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mainMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex,
	     " 11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6) (1:-103:104:-105:106)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  
  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -12 13 -14 15 -16 ");
  addOuterSurf("main", Out);

  before=ModelSupport::getComposite(SMap,surfIndex," -1 103 -104 105 -106 ") +
    std::to_string(-FCstart.getLinkSurf(static_cast<size_t>(lpStart)));
  System.addCell(MonteCarlo::Qhull(cellIndex++,mainMat,0.0,before));
  addOuterSurf("before",before);

  after=ModelSupport::getComposite(SMap,surfIndex," 2 203 -204 205 -206 ") + BSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,mainMat,0.0,after));
  addOuterUnionSurf("after", after);

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
  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(surfIndex+2));

  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(surfIndex+3));

  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(surfIndex+4));

  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(surfIndex+5));

  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(surfIndex+6));

  return;
}




void
PBIP::createAll(Simulation& System,
		const attachSystem::FixedComp& FC,const long int& lp,
		const attachSystem::FixedComp& FCstart,const long int& lpStart,
		const attachSystem::FixedComp& FCend,const long int& lpEnd)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param lp :: link point
    \param FCstart :: FC at the start (Bulk)
    \param lpBulk :: side link point of start
    \param FCend :: FC at the end (BeRef)
    \param lpBulk :: side link point at FCend
  */
{
  ELog::RegMethod RegA("PBIP","createAll");

  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createLinks();
  createObjects(System,FCstart,lpStart,FCend,lpEnd);
  insertObjects(System);

  return;
}

}  // essSystem essSystem
