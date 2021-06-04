/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/LocalShielding.cxx
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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

#include "LocalShielding.h"

namespace tdcSystem
{

LocalShielding::LocalShielding(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

LocalShielding::LocalShielding(const LocalShielding& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::ExternalCut(A),
  length(A.length),width(A.width),height(A.height),
  midHoleWidth(A.midHoleWidth),
  midHoleHeight(A.midHoleHeight),
  cornerWidth(A.cornerWidth),
  cornerHeight(A.cornerHeight),
  mainMat(A.mainMat),
  cType(A.cType),
  opt(A.opt)
  /*!
    Copy constructor
    \param A :: LocalShielding to copy
  */
{}

LocalShielding&
LocalShielding::operator=(const LocalShielding& A)
  /*!
    Assignment operator
    \param A :: LocalShielding to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      length=A.length;
      width=A.width;
      height=A.height;
      midHoleWidth=A.midHoleWidth;
      midHoleHeight=A.midHoleHeight;
      cornerWidth=A.cornerWidth;
      cornerHeight=A.cornerHeight;
      mainMat=A.mainMat;
      cType=A.cType;
      opt=A.opt;
    }
  return *this;
}

LocalShielding*
LocalShielding::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new LocalShielding(*this);
}

LocalShielding::~LocalShielding()
  /*!
    Destructor
  */
{}

void
LocalShielding::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("LocalShielding","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  
  midHoleWidth=Control.EvalVar<double>(keyName+"MidHoleWidth");
  midHoleHeight=Control.EvalVar<double>(keyName+"MidHoleHeight");
  cornerWidth=Control.EvalVar<double>(keyName+"CornerWidth");
  cornerHeight=Control.EvalVar<double>(keyName+"CornerHeight");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  cType=Control.EvalVar<std::string>(keyName+"CornerType");
  opt=Control.EvalVar<std::string>(keyName+"Option");

  return;
}

void
LocalShielding::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("LocalShielding","createSurfaces");

  makePlane("front",SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  makePlane("back",SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  makePlane("left",SMap,buildIndex+3,Origin-X*(width/2.0),X);
  makePlane("right",SMap,buildIndex+4,Origin+X*(width/2.0),X);

  makePlane("base",SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  makePlane("top",SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  // penetration for the beam pipe
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(midHoleWidth/2.0+xStep),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(midHoleWidth/2.0-xStep),X);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(midHoleHeight/2.0+zStep),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(midHoleHeight/2.0-zStep),Z);

  // cut corners
  ModelSupport::buildShiftedPlane(SMap,buildIndex+23,buildIndex+3,X,cornerWidth);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+24,buildIndex+4,X,-cornerWidth);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+26,buildIndex+6,Z,-cornerHeight);

  return;
}

void
LocalShielding::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("LocalShielding","createObjects");

  std::string Out;
  const std::string ICell=isActive("Inner") ? getRuleStr("Inner") : "";

  const std::string side=ModelSupport::getComposite(SMap,buildIndex," 1 -2 ");

  const bool isMidHole = (midHoleWidth>Geometry::zeroTol) && (midHoleHeight>Geometry::zeroTol);
  const bool isCorners = (cornerWidth>Geometry::zeroTol) && (cornerHeight>Geometry::zeroTol);

  const std::string top=ModelSupport::getComposite(SMap,buildIndex,
						   isCorners ? " -26 " : " -6 ");

  if ((!isMidHole) && (!isCorners))
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 ");
      makeCell("Wall",System,cellIndex++,mainMat,0.0,Out+side);
    }
  else if (isMidHole)
    {
      if (opt == "SideOnly") // build only side walls, hole height does not make sence
	{
	  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -13 5 ") + top;
	  makeCell("Wall",System,cellIndex++,mainMat,0.0,Out+side);

	  Out=ModelSupport::getComposite(SMap,buildIndex," 13 -14 5 ") + top;
	  makeCell("Hole",System,cellIndex++,0,0.0,Out+side+ICell);

	  Out=ModelSupport::getComposite(SMap,buildIndex," 14 -4 5 ") + top;
	  makeCell("Wall",System,cellIndex++,mainMat,0.0,Out+side);
	}
      else
	{
	  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -15 ");
	  makeCell("Wall",System,cellIndex++,mainMat,0.0,Out+side);

	  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -13 15 -16 ");
	  makeCell("Wall",System,cellIndex++,mainMat,0.0,Out+side);

	  Out=ModelSupport::getComposite(SMap,buildIndex," 13 -14 15 -16 ");
	  makeCell("Hole",System,cellIndex++,0,0.0,Out+side+ICell);

	  Out=ModelSupport::getComposite(SMap,buildIndex," 14 -4 15 -16 ");
	  makeCell("Wall",System,cellIndex++,mainMat,0.0,Out+side);

	  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 16 ") + top;
	  makeCell("Wall",System,cellIndex++,mainMat,0.0,Out+side);
	}
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 ") + top;
      makeCell("Wall",System,cellIndex++,mainMat,0.0,Out+side);
    }

  if (isCorners)
    {
      if (cType == "right") // left corner is not built
	{
	  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -23 26 -6 ");
	  makeCell("Corner",System,cellIndex++,0,0.0,Out+side);

	  Out=ModelSupport::getComposite(SMap,buildIndex," 23 -4 26 -6 ");
	  makeCell("Wall",System,cellIndex++,mainMat,0.0,Out+side);
	}
      else if (cType == "left") // right corner is not built
	{
	  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -24 26 -6 ");
	  makeCell("Wall",System,cellIndex++,mainMat,0.0,Out+side);

	  Out=ModelSupport::getComposite(SMap,buildIndex," 24 -4 26 -6 ");
	  makeCell("Corner",System,cellIndex++,0,0.0,Out+side);
	}
      else // both corners are built
	{
	  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -23 26 -6 ");
	  makeCell("Corner",System,cellIndex++,0,0.0,Out+side);

	  Out=ModelSupport::getComposite(SMap,buildIndex," 23 -24 26 -6 ");
	  makeCell("Wall",System,cellIndex++,mainMat,0.0,Out+side);

	  Out=ModelSupport::getComposite(SMap,buildIndex," 24 -4 26 -6 ");
	  makeCell("Corner",System,cellIndex++,0,0.0,Out+side);
	}

    }

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);

  return;
}


void
LocalShielding::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("LocalShielding","createLinks");

  // ExternalCut::createLink("front",*this,0,Origin,Y);
  // ExternalCut::createLink("back",*this,1,Origin,Y);

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setConnect(3,Origin+X*(width/2.0),X);

  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);

  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  FixedComp::nameSideIndex(0,"front");
  FixedComp::nameSideIndex(1,"back");

  FixedComp::nameSideIndex(2,"left");
  FixedComp::nameSideIndex(3,"right");

  FixedComp::nameSideIndex(4,"bottom");
  FixedComp::nameSideIndex(5,"top");

  return;
}

void
LocalShielding::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("LocalShielding","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  
  return;
}

}  // tdcSystem
