/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/GeneralPipe.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
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
#include "GeneralPipe.h"

namespace constructSystem
{

GeneralPipe::GeneralPipe(const std::string& Key) :
  attachSystem::FixedRotate(Key,12),
  attachSystem::ContainedGroup("Main","FlangeA","FlangeB"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut(),
  activeFlag(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  FixedComp::nameSideIndex(0,"front");
  FixedComp::nameSideIndex(1,"back");
  FixedComp::nameSideIndex(6,"midPoint");
}

GeneralPipe::GeneralPipe(const std::string& Key,
			   const size_t nLink) :
  attachSystem::FixedRotate(Key,nLink),
  attachSystem::ContainedGroup("Main","FlangeA","FlangeB"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut(),
  activeFlag(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  FixedComp::nameSideIndex(0,"front");
  FixedComp::nameSideIndex(1,"back");
  FixedComp::nameSideIndex(6,"midPoint");
}

GeneralPipe::GeneralPipe(const GeneralPipe& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedGroup(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  activeFlag(A.activeFlag)
  /*!
    Copy constructor
    \param A :: GeneralPipe to copy
  */
{}

GeneralPipe&
GeneralPipe::operator=(const GeneralPipe& A)
  /*!
    Assignment operator
    \param A :: GeneralPipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      activeFlag=A.activeFlag;
    }
  return *this;
}
  
void
GeneralPipe::applyActiveFrontBack(const double length)
  /*!
    Apply the active front/back point to re-calcuate Origin
    It applies the rotation of Y to Y' to both X/Z to preserve
    orthogonality.
   */
{
  ELog::RegMethod RegA("GeneralPipe","applyActiveFrontBack");
  
  const Geometry::Vec3D curFP=((activeFlag & 1) && frontPointActive()) ?
    getFrontPoint() : Origin;
  const Geometry::Vec3D curBP=((activeFlag & 2) && backPointActive()) ?
    getBackPoint() : Origin+Y*length;
  
  Origin=(curFP+curBP)/2.0;

  if (activeFlag)
    {
      const Geometry::Vec3D YAxis=(curBP-curFP).unit();
      // need unit for numerical acc.
      Geometry::Vec3D RotAxis=(YAxis*Y).unit();
      
      if (!RotAxis.nullVector())
	{
	  const Geometry::Quaternion QR=
	    Geometry::Quaternion::calcQVRot(Y,YAxis,RotAxis);
	  Y=YAxis;
	  QR.rotate(X);
	  QR.rotate(Z);
	}
      else if (Y.dotProd(YAxis) < -0.5) // (reversed
	{
	  Y=YAxis;
	  X*=-1.0;
	  Z*=-1.0;
	}
    }
  return;
}

}  // NAMESPACE constructSystem
