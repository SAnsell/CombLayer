/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacility/GTFWall.cxx
 *
 * Copyright (c) 2004-2024 by Konstantin Batkov
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

#include "GTFWall.h"

namespace MAXIV::GunTestFacility
{

GTFWall::GTFWall(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

GTFWall::GTFWall(const GTFWall& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  length(A.length),width(A.width),height(A.height),
  depth(A.depth),
  cornerLength(A.cornerLength),
  cornerHeight(A.cornerHeight),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: GTFWall to copy
  */
{}

GTFWall&
GTFWall::operator=(const GTFWall& A)
  /*!
    Assignment operator
    \param A :: GTFWall to copy
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
      depth=A.depth;
      cornerLength=A.cornerLength;
      cornerHeight=A.cornerHeight;
      mat=A.mat;
    }
  return *this;
}

GTFWall*
GTFWall::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new GTFWall(*this);
}

GTFWall::~GTFWall()
  /*!
    Destructor
  */
{}

void
GTFWall::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("GTFWall","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  cornerLength=Control.EvalVar<double>(keyName+"CornerLength");
  cornerHeight=Control.EvalVar<double>(keyName+"CornerHeight");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}

void
GTFWall::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("GTFWall","createSurfaces");

  SurfMap::makePlane("back",SMap,buildIndex+1,Origin,Y);
  SurfMap::makePlane("front",SMap,buildIndex+2,Origin+Y*(length),Y);

  SurfMap::makePlane("left",SMap,buildIndex+3,Origin-X*(width),X);
  SurfMap::makePlane("right",SMap,buildIndex+4,Origin,X);

  SurfMap::makePlane("bottom",SMap,buildIndex+5,Origin-Z*(depth),Z);
  SurfMap::makePlane("top",SMap,buildIndex+6,Origin+Z*(height),Z);

  SurfMap::makeShiftedPlane("cornerSide",SMap,buildIndex+12,buildIndex+1,Y,cornerLength);
  SurfMap::makeShiftedPlane("cornerTop",SMap,buildIndex+15,buildIndex+5,Z,cornerHeight);

  return;
}

void
GTFWall::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("GTFWall","createObjects");

  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 12 -2 3 -4 5 -6 ");
  makeCell("MainCell",System,cellIndex++,mat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -12 3 -4 15 -6 ");
  makeCell("AboveCorner",System,cellIndex++,mat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -12 3 -4 5 -15 ");
  makeCell("Corner",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 3 -4 5 -6 ");
  addOuterSurf(HR);

  return;
}


void
GTFWall::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("GTFWall","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setNamedLinkSurf(0,"Back",SurfMap::getSignedSurf("#back"));

  FixedComp::setConnect(1,Origin+Y*(length),Y);
  FixedComp::setNamedLinkSurf(1,"Front",SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin-X*(width),-X);
  FixedComp::setNamedLinkSurf(2,"Left",-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin,X);
  FixedComp::setNamedLinkSurf(3,"Right",SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(depth),-Z);
  FixedComp::setNamedLinkSurf(4,"Bottom",-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(height),Z);
  FixedComp::setNamedLinkSurf(5,"Top",SMap.realSurf(buildIndex+6));

  return;
}

void
GTFWall::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("GTFWall","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // MAXIV::GunTestFacility
