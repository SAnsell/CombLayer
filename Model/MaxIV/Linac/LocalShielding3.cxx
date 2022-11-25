/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/LocalShielding3.cxx
 *
 * Copyright (c) 2004-2022 by Konstantin Batkov
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

#include "LocalShielding.h"
#include "LocalShielding3.h"

namespace tdcSystem
{

LocalShielding3::LocalShielding3(const std::string& Key)  :
  tdcSystem::LocalShielding(Key)
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

LocalShielding3::LocalShielding3(const LocalShielding3& A) :
  tdcSystem::LocalShielding(A),
  midHoleShieldHeight(A.midHoleShieldHeight)
  /*!
    Copy constructor
    \param A :: LocalShielding3 to copy
  */
{}

LocalShielding3&
LocalShielding3::operator=(const LocalShielding3& A)
  /*!
    Assignment operator
    \param A :: LocalShielding3 to copy
    \return *this
  */
{
  if (this!=&A)
    {
      tdcSystem::LocalShielding::operator=(A);
      midHoleShieldHeight=A.midHoleShieldHeight;
    }
  return *this;
}

LocalShielding3*
LocalShielding3::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new LocalShielding3(*this);
}

LocalShielding3::~LocalShielding3()
  /*!
    Destructor
  */
{}

void
LocalShielding3::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("LocalShielding3","populate");

  FixedRotate::populate(Control);
  tdcSystem::LocalShielding::populate(Control);

  for (size_t i=0;i<6;++i) { // always exactly 6 variables for 3 pipes
    //    try {
      const double H = Control.EvalVar<double>(keyName+"MidHoleShieldHeight"+std::to_string(i+1));
      midHoleShieldHeight.push_back(H);
    // } catch(ColErr::InContainerError<std::string>) {
    //   break;
    // }
  }

  if (midHoleShieldHeight.size() != 6) {
    throw ColErr::SizeError<size_t>(6, midHoleShieldHeight.size(),
				    keyName+"MidHoleShieldHeight vector length");
  }

  return;
}

void
LocalShielding3::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("LocalShielding3","createSurfaces");

  tdcSystem::LocalShielding::createSurfaces();

  double h = 0.0;
  int SI(buildIndex+100);
  // bottom hole surface again in order to simplify loops in createObjects
  SMap.addMatch(SI+5,SMap.realSurf(buildIndex+15));
  SI+=10;
  const size_t N = midHoleShieldHeight.size();
  for (size_t i=0;i<N;++i) {
    h += midHoleShieldHeight[i];
    ModelSupport::buildShiftedPlane(SMap,SI+5,buildIndex+105,Z,h);
    SI += 10;
  }
  // top hole surface again in order to simplify loops in createObjects
  SMap.addMatch(SI+5,SMap.realSurf(buildIndex+16));

  return;
}

void
LocalShielding3::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("LocalShielding3","createObjects");

  //  tdcSystem::LocalShielding::createObjects(System)

  HeadRule HR;
  std::vector<HeadRule> ICellHR;
  for (size_t i=0; i<3; i++)
    {
      const std::string name = "Inner"+std::to_string(i+1);
      ICellHR.push_back(getRule(name));
    }

  const HeadRule sideHR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2");

  const bool isMidHole = (midHoleWidth>Geometry::zeroTol) &&
    (midHoleHeight>Geometry::zeroTol);
  const bool isCorners = (cornerWidth>Geometry::zeroTol) &&
    (cornerHeight>Geometry::zeroTol);

  const HeadRule topHR(SMap,buildIndex,(isCorners) ? -26:-6);
  
  if ((!isMidHole) && (!isCorners))
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6");
      makeCell("Wall",System,cellIndex++,mainMat,0.0,HR*sideHR);
    }
  else if (isMidHole)
    {
      if (opt == "SideOnly")
	{
	  throw ColErr::AbsObjMethod
	    ("Not implemented (and does not make sence with LocalShielding3)."
	     "Use the LocalShielding class instead.");
	}
      else
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex," 3 -4 5 -15");
	  makeCell("Wall",System,cellIndex++,mainMat,0.0,HR*sideHR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex," 3 -13 15 -16");
	  makeCell("Wall",System,cellIndex++,mainMat,0.0,HR*sideHR);

	  int mat;
	  int SI(buildIndex+100);
	  for (size_t i=0;i<7;++i)
	    {
	      HR=ModelSupport::getHeadRule(SMap,buildIndex,SI,
					   "13 -14 5M -15M");
	      if (i%2)
		{
		  HR*=ICellHR[i/2];
		  mat=mainMat;
		}
	      makeCell("HoleWall",System,cellIndex++,mat,0.0,HR*sideHR);
	      SI += 10;
	    }

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"14 -4 15 -16");
	  makeCell("Wall",System,cellIndex++,mainMat,0.0,HR*sideHR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 16");
	  makeCell("Wall",System,cellIndex++,mainMat,0.0,HR*sideHR*topHR);
	}
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5");
      makeCell("Wall",System,cellIndex++,mainMat,0.0,HR*sideHR*topHR);
    }

  if (isCorners)
    {
      if (cType == "right") // left corner is not built
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex," 3 -23 26 -6");
	  makeCell("Corner",System,cellIndex++,0,0.0,HR*sideHR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex," 23 -4 26 -6");
	  makeCell("Wall",System,cellIndex++,mainMat,0.0,HR*sideHR);
	}
      else if (cType == "left") // right corner is not built
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -24 26 -6");
	  makeCell("Wall",System,cellIndex++,mainMat,0.0,HR*sideHR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"24 -4 26 -6");
	  makeCell("Corner",System,cellIndex++,0,0.0,HR*sideHR);
	}
      else // both corners are built
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -23 26 -6");
	  makeCell("Corner",System,cellIndex++,0,0.0,HR*sideHR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"23 -24 26 -6");
	  makeCell("Wall",System,cellIndex++,mainMat,0.0,HR*sideHR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"24 -4 26 -6");
	  makeCell("Corner",System,cellIndex++,0,0.0,HR*sideHR);
	}

    }

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6");
  addOuterSurf(HR);

  return;
}

void
LocalShielding3::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("LocalShielding3","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  tdcSystem::LocalShielding::createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
