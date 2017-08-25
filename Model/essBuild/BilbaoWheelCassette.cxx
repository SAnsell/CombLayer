/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/BilbaoWheelCassette.cxx
 *
 * Copyright (c) 2017 by Stuart Ansell / Konstantin Batkov
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
#include "BaseMap.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"
#include "AttachSupport.h"

#include "BilbaoWheelCassette.h"

namespace essSystem
{

  BilbaoWheelCassette::BilbaoWheelCassette(const std::string& baseKey,
					   const std::string& extraKey,
					   const size_t& Index)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(baseKey+extraKey+std::to_string(Index),6),
  baseName(baseKey),
  commonName(baseKey+extraKey),
  surfIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BilbaoWheelCassette::BilbaoWheelCassette(const BilbaoWheelCassette& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  baseName(A.baseName),
  commonName(A.commonName),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  engActive(A.engActive),
  bricksActive(A.bricksActive),
  wallThick(A.wallThick),delta(A.delta),temp(A.temp),
  mainMat(A.mainMat),wallMat(A.wallMat),
  floor(A.floor),
  roof(A.roof),
  back(A.back),
  front(A.front)
  /*!
    Copy constructor
    \param A :: BilbaoWheelCassette to copy
  */
{}

BilbaoWheelCassette&
BilbaoWheelCassette::operator=(const BilbaoWheelCassette& A)
  /*!
    Assignment operator
    \param A :: BilbaoWheelCassette to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      engActive=A.engActive;
      bricksActive=A.bricksActive;
      wallThick=A.wallThick;
      delta=A.delta;
      temp=A.temp;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
      floor=A.floor;
      roof=A.roof;
      back=A.back;
      front=A.front;
    }
  return *this;
}

BilbaoWheelCassette*
BilbaoWheelCassette::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new BilbaoWheelCassette(*this);
}

BilbaoWheelCassette::~BilbaoWheelCassette()
  /*!
    Destructor
  */
{}

void
BilbaoWheelCassette::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","populate");

  FixedOffset::populate(Control);


  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");
  bricksActive=Control.EvalDefVar<int>(keyName+"BricksActive", 0);

  const double nSectors = Control.EvalVar<double>(baseName+"NSectors");
  delta = 360.0/nSectors;

  wallThick=Control.EvalVar<double>(commonName+"WallThick");
  wallMat=ModelSupport::EvalMat<int>(Control,commonName+"WallMat");
  mainMat=ModelSupport::EvalMat<int>(Control,baseName+"WMat");
  temp=Control.EvalVar<double>(baseName+"Temp");

  /*wallThick=Control.EvalVar<double>(keyName+"WallThick");
  delta=Control.EvalVar<double>(keyName+"Delta");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  */
  return;
}

void
BilbaoWheelCassette::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
BilbaoWheelCassette::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createSurfaces");

  // divider
  ModelSupport::buildPlane(SMap,surfIndex+1,Origin,Y);
  // outer sides
  ModelSupport::buildPlaneRotAxis(SMap,surfIndex+3,Origin,X,Z,-delta/2.0);
  ModelSupport::buildPlaneRotAxis(SMap,surfIndex+4,Origin,X,Z,delta/2.0);

  ModelSupport::buildPlaneRotAxis(SMap,surfIndex+13,Origin+X*wallThick,X,Z,-delta/2.0);
  ModelSupport::buildPlaneRotAxis(SMap,surfIndex+14,Origin-X*wallThick,X,Z,delta/2.0);

  return;
}

void
BilbaoWheelCassette::createObjects(Simulation& System,
				   const attachSystem::FixedComp& FC)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param FC :: Tungsten layer rule
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createObjects");

  const std::string outer = FC.getLinkString(floor) +
    FC.getLinkString(roof) +FC.getLinkString(back) + FC.getLinkString(front);

  std::string Out;
  Out=ModelSupport::getComposite(SMap,surfIndex," 3 -13 -1");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,temp,Out+outer));

  Out=ModelSupport::getComposite(SMap,surfIndex," 13 -14 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mainMat,temp,Out+outer));

  Out=ModelSupport::getComposite(SMap,surfIndex," 14 -4 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,temp,Out+outer));

  Out=ModelSupport::getComposite(SMap,surfIndex," 3 -4 -1 ");
  addOuterSurf(Out+outer);

  return;
}


void
BilbaoWheelCassette::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createLinks");

  //  FixedComp::setConnect(0,Origin,-Y);
  //  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));

  return;
}




void
BilbaoWheelCassette::createAll(Simulation& System,
			       const attachSystem::FixedComp& FC,
			       const long int sideIndex,
			       const size_t lpFloor,
			       const size_t lpRoof,
			       const size_t lpBack,
			       const size_t lpFront,
			       const double& theta)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
    \param outer :: Tungsten layer rule
    \param angle :: angular coordinate of the given sector
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createAll");

  floor = lpFloor;
  roof = lpRoof;
  back = lpBack;
  front = lpFront;
  
  populate(System.getDataBase());
  xyAngle=theta;
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System,FC);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
