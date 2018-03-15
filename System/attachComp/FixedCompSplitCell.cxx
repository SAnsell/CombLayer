/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/FixedComp.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "stringCombine.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "surfEqual.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "SurInter.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FixedComp.h"

namespace attachSystem
{

std::vector<int>
FixedComp::splitObject(Simulation& System,
		       const int SNoffset,const int cellN,
		       const Geometry::Vec3D& Org,
		       const Geometry::Vec3D& XYZ)
  /*!
    Carries out a splitObject function -- not 100% sure
    is goes here but...
    Note that the NEGATIVE surface constructed is the original cell.
    \param Org :: Origin offset relative to FC
    \param XYZ :: XYZ plane
    \return cellList 
  */
{
  ELog::RegMethod RegA("FixedComp","splitObject(org,axis)");

  std::vector<int> OutCell;
  
  const Geometry::Vec3D O=Origin+X*Org[0]+Y*Org[1]+Z*Org[2];
  Geometry::Vec3D Axis=X*XYZ[0]+Y*XYZ[1]+Z*XYZ[2];
  Axis.makeUnit();
  
  ModelSupport::buildPlane(SMap,buildIndex+SNoffset,O,Axis);
  const int cellExtra=
    System.splitObject(cellN,SMap.realSurf(buildIndex+SNoffset));

  CellMap* CMapPtr=dynamic_cast<attachSystem::CellMap*>(this);
  if (CMapPtr)
    CMapPtr->registerExtra(cellN,cellExtra);
  
  return OutCell;  
}

std::vector<int>
FixedComp::splitObject(Simulation& System,
		       const int SNoffset,const int cellN,
		       const std::vector<Geometry::Vec3D>& OrgVec,
		       const std::vector<Geometry::Vec3D>& XYZVec)
  /*!
    Carries out a splitObject function -- not 100% sure
    is goes here but...
    Note that the NEGATIVE surface constructed is the original cell.
    and first out cell
    \param Org :: Origin offset relative to FC
    \param XYZ :: XYZ plane

  */
{
  ELog::RegMethod RegA("FixedComp","splitObject(vec)");

  int SN(SNoffset);
  int CN(cellN);
  std::vector<int> OutCell({cellN});
  for(size_t i=0;i<OrgVec.size();i++)
    {
      const Geometry::Vec3D& Org(OrgVec[i]);
      const Geometry::Vec3D& XYZ(XYZVec[i]);
      
      const Geometry::Vec3D O=Origin+X*Org[0]+Y*Org[1]+Z*Org[2];
      Geometry::Vec3D Axis=X*XYZ[0]+Y*XYZ[1]+Z*XYZ[2];
      Axis.makeUnit();

      ModelSupport::buildPlane(SMap,buildIndex+SN,O,Axis);
      CN=System.splitObject(CN,SMap.realSurf(buildIndex+SN));
      OutCell.push_back(CN);
      
      CellMap* CMapPtr=dynamic_cast<attachSystem::CellMap*>(this);
      if (CMapPtr)
	CMapPtr->registerExtra(cellN,CN);
      SN++;
    }
  
  return OutCell;  
}

std::vector<int>
FixedComp::splitObject(Simulation& System,const int SN,
		       const int cellN)
  /*!
    Carries out a splitObject function -- not 100% sure
    is goes here but...
    Note that the NEGATIVE surface constructed is the original cell.
    \param SN :: Surface number to use [already in cell]
    \param cellN :: Cell number
    \return cellList 
  */
{
  ELog::RegMethod RegA("FixedComp","splitObject(cell)");

  std::vector<int> OutCell;

  const int ASN=std::abs(SN);
  const int signSN=(SN>0) ? 1 : -1;
  
  const int cellExtra=
    System.splitObject(cellN,signSN*SMap.realSurf(buildIndex+ASN));
  
  CellMap* CMapPtr=dynamic_cast<attachSystem::CellMap*>(this);
  if (CMapPtr)
    CMapPtr->registerExtra(cellN,cellExtra);
  
  return OutCell;  
}

  
}  // NAMESPACE attachSystem
