/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chip/CylinderColl.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <numeric>
#include <iterator>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "Tensor.h"
#include "Vec3D.h"
#include "PointOperation.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "Convex.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "shutterBlock.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "cylTrack.h"
#include "CylinderColl.h"


namespace shutterSystem
{

CylinderColl::CylinderColl(const std::string& Key)  : 
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key,3),keyName(Key),
  trackIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(trackIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Insert Key to use
  */
{}

CylinderColl::~CylinderColl() 
  /*!
    Destructor
  */
{}

void
CylinderColl::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("CylinderColl","populate");

  const FuncDataBase& Control=System.getDataBase();

  nCyl=Control.EvalVar<size_t>(keyName+"NCylinder");
  for(size_t i=0;i<nCyl;i++)
    {
      const double iR=
	Control.EvalVar<double>(keyName+StrFunc::makeString("Radius",i+1));
      const double thick=
	Control.EvalVar<double>(keyName+StrFunc::makeString("Thick",i+1));
      const int mat=
	ModelSupport::EvalMat<int>(Control,keyName+
				   StrFunc::makeString("Mat",i+1));
      const Geometry::Vec3D AOffVec=
	Control.EvalVar<Geometry::Vec3D>
	(keyName+StrFunc::makeString("AOffVec",i+1));
      const Geometry::Vec3D BOffVec=
	Control.EvalVar<Geometry::Vec3D>
	(keyName+StrFunc::makeString("BOffVec",i+1));
      CVec.push_back(ModelSupport::cylTrack(iR,iR+thick,mat,0.0));
    }
  return;
}

void
CylinderColl::createUnitVector(const attachSystem::TwinComp& TC)
  /*!
    Create the unit vectors
  */
{
  ELog::RegMethod RegA("CylinderColl","createUnitVector");
  return;
}

void
CylinderColl::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("CylinderColl","createSurface");
  // Front plane
  return;
}

void
CylinderColl::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("CylinderColl","createObjects");
  return;
}


void
CylinderColl::createAll(Simulation& System,
			const attachSystem::TwinComp& TC)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param TC :: Twin system
   */
{
  ELog::RegMethod RegA("CylinderColl","createAll");

  populate(System);
  createUnitVector(TC);
  createSurfaces();
  createObjects(System);
  return;
}
  
}  // NAMESPACE shutterSystem
