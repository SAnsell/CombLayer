/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/FrontEndBuilding.cxx
 *
 * Copyright (c) 2018 by Konstantin Batkov
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

#include "FrontEndBuilding.h"

namespace essSystem
{

FrontEndBuilding::FrontEndBuilding(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  surfIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

FrontEndBuilding::FrontEndBuilding(const FrontEndBuilding& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  length(A.length),
  widthLeft(A.widthLeft),
  widthRight(A.widthRight),
  height(A.height),
  wallThick(A.wallThick),
  mainMat(A.mainMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: FrontEndBuilding to copy
  */
{}

FrontEndBuilding&
FrontEndBuilding::operator=(const FrontEndBuilding& A)
  /*!
    Assignment operator
    \param A :: FrontEndBuilding to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      length=A.length;
      widthLeft=A.widthLeft;
      widthRight=A.widthRight;
      height=A.height;
      wallThick=A.wallThick;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
    }
  return *this;
}

FrontEndBuilding*
FrontEndBuilding::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new FrontEndBuilding(*this);
}

FrontEndBuilding::~FrontEndBuilding()
  /*!
    Destructor
  */
{}

void
FrontEndBuilding::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("FrontEndBuilding","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  widthLeft=Control.EvalVar<double>(keyName+"WidthLeft");
  widthRight=Control.EvalVar<double>(keyName+"WidthRight");
  height=Control.EvalVar<double>(keyName+"Height");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
FrontEndBuilding::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("FrontEndBuilding","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
FrontEndBuilding::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("FrontEndBuilding","createSurfaces");

  ModelSupport::buildPlane(SMap,surfIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(length),Y);

  ModelSupport::buildPlane(SMap,surfIndex+3,Origin-X*(widthLeft),X);
  ModelSupport::buildPlane(SMap,surfIndex+4,Origin+X*(widthRight),X);

  ModelSupport::buildPlane(SMap,surfIndex+11,Origin-Y*(wallThick),Y);
  ModelSupport::buildPlane(SMap,surfIndex+12,Origin+Y*(length+wallThick),Y);

  return;
}

void
FrontEndBuilding::createObjects(Simulation& System,
				const attachSystem::FixedComp &FC,
				const long int floorIndexLow,
				const long int floorIndexTop,
				const long int roofIndexLow,
				const long int roofIndexTop)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param floorIndexLow :: bottom floor lp
    \param floorIndexTop :: top floor lp
    \param roofIndexLow :: bottom roof lp
    \param roofIndexTop :: top roof lp
  */
{
  ELog::RegMethod RegA("FrontEndBuilding","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 3 -4 ") +
    FC.getLinkString(floorIndexTop) +
    FC.getLinkString(roofIndexLow);
  System.addCell(MonteCarlo::Qhull(cellIndex++,mainMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 3 -4 ") +
    FC.getLinkString(-floorIndexLow) + FC.getLinkString(-floorIndexTop);
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 3 -4 ") +
    FC.getLinkString(-roofIndexLow) + FC.getLinkString(-roofIndexTop);
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -1 3 -4 ") +
    FC.getLinkString(-floorIndexLow) +
    FC.getLinkString(-roofIndexTop);
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -2 3 -4 ") +
    FC.getLinkString(-floorIndexLow) +
    FC.getLinkString(-roofIndexTop);
  addOuterSurf(Out);

  return;
}


void
FrontEndBuilding::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("FrontEndBuilding","createLinks");

  //  FixedComp::setConnect(0,Origin,-Y);
  //  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));

  return;
}




void
FrontEndBuilding::createAll(Simulation& System,
			    const attachSystem::FixedComp& FC,
			    const long int sideIndex,
			    const long int floorIndexLow,
			    const long int floorIndexTop,
			    const long int roofIndexLow,
			    const long int roofIndexTop)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
    \param floorIndexLow :: bottom floor lp
    \param floorIndexTop :: top floor lp
    \param roofIndexLow :: bottom roof lp
    \param roofIndexTop :: top roof lp
  */
{
  ELog::RegMethod RegA("FrontEndBuilding","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System,FC,floorIndexLow,floorIndexTop,roofIndexLow,roofIndexTop);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
