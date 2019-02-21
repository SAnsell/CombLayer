/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/FixedCompSplitCell.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "groupRange.h"
#include "objectGroups.h"
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
		       const int SNoffset,
		       const int cellN,
		       const Geometry::Vec3D& Org,
		       const Geometry::Vec3D& XYZ)
  /*!
    Carries out a splitObject function -- not 100% sure
    is goes here but...
    Note that the NEGATIVE surface constructed is the original cell.
    \param System :: Simuation for the model
    \param SNoffset :: Number for new surface [relative to build index]
    \param cellN :: Cell number to split
    \param Org :: Origin offset relative to FC basis set + Origin
    \param XYZ :: XYZ plane  [relative to XYZ]
    \return cellList 
  */
{
  ELog::RegMethod RegA("FixedComp","splitObject(org,axis)");

  std::vector<int> OutCell;
  
  const Geometry::Vec3D O=Origin+X*Org[0]+Y*Org[1]+Z*Org[2];
  Geometry::Vec3D Axis=X*XYZ[0]+Y*XYZ[1]+Z*XYZ[2];
  Axis.makeUnit();
    
  ModelSupport::buildPlane(SMap,buildIndex+SNoffset,O,Axis);
  ELog::EM<<"OF == "<<SNoffset<<" "<<buildIndex<<ELog::endDiag;
  ELog::EM<<"Plane == "<<*SMap.realSurfPtr(buildIndex+SNoffset)<<ELog::endDiag;
  const int cellExtra=
    System.splitObject(cellN,SMap.realSurf(buildIndex+SNoffset));

  CellMap* CMapPtr=dynamic_cast<attachSystem::CellMap*>(this);
	
  if (CMapPtr)
    CMapPtr->registerExtra(cellN,cellExtra);

  OutCell.push_back(cellN);
  OutCell.push_back(cellExtra);
  return OutCell;  
}

std::vector<int>
FixedComp::splitObjectAbsolute(Simulation& System,
			       const int SNoffset,
			       const int cellN,
			       const Geometry::Vec3D& Org,
			       const Geometry::Vec3D& XYZ)
/*!
    Carries out a splitObject function -- not 100% sure
    is goes here but...
    Note that the NEGATIVE surface constructed is the original cell.
    \param System :: Simuation for the model
    \param SNoffset :: Number for new surface [relative to build index]
    \param cellN :: Cell number to split
    \param Org :: Origin 
    \param XYZ :: XYZ plane 
    \return cellList 
  */
{
  ELog::RegMethod RegA("FixedComp","splitObjectAbsolute(org,axis)");

  std::vector<int> OutCell;
  
  const Geometry::Vec3D Axis=XYZ.unit();
    
  ModelSupport::buildPlane(SMap,buildIndex+SNoffset,Org,Axis);
  const int cellExtra=
    System.splitObject(cellN,SMap.realSurf(buildIndex+SNoffset));

  CellMap* CMapPtr=dynamic_cast<attachSystem::CellMap*>(this);
  if (CMapPtr)
    CMapPtr->registerExtra(cellN,cellExtra);

  OutCell.push_back(cellN);
  OutCell.push_back(cellExtra);
  return OutCell;  
}

std::vector<int>
FixedComp::splitObject(Simulation& System,
		       const int SNoffset,const int cellN,
		       const std::vector<Geometry::Vec3D>& OrgVec,
		       const std::vector<Geometry::Vec3D>& XYZVec)
  /*!
    This split the original Cell on the -/+ side of a plane 
    through Org,XYZ. The next point in the vector are then used
    to split the +ve remainder cell.
    Note that the NEGATIVE surface constructed is the original cell.
    and first out cell
    \param SNoffset :: surface offset
    \param cellN :: Cell to divide
    \param OrgVec :: Origins offset relative to FC
    \param XYZVec :: XYZ plane directions
    \return vector of cells [first is original]
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
FixedComp::splitObjectAbsolute(Simulation& System,
			       const int SNoffset,const int cellN,
			       const std::vector<Geometry::Vec3D>& OrgVec,
			       const std::vector<Geometry::Vec3D>& XYZVec)
  /*!
    This split the original Cell on the -/+ side of a plane 
    through Org,XYZ. The next point in the vector are then used
    to split the +ve remainder cell.
    Note that the NEGATIVE surface constructed is the original cell.
    and first out cell
    \param SNoffset :: surface offset
    \param cellN :: Cell to divide
    \param OrgVec :: Origins offset [ABSOLUTE]
    \param XYZVec :: XYZ plane directions [ABSOLUTE]
    \return vector of cells [first is original]
  */
{
  ELog::RegMethod RegA("FixedComp","splitObjectAbsolute(vec)");

  int SN(SNoffset);
  int CN(cellN);
  std::vector<int> OutCell({cellN});
  for(size_t i=0;i<OrgVec.size();i++)
    {
      
      const Geometry::Vec3D& O=OrgVec[i];
      const Geometry::Vec3D Axis=XYZVec[i].unit();

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
FixedComp::splitObject(Simulation& System,
		       const int SN,
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
  const int trueSN= (ASN<100000) ?
    signSN*SMap.realSurf(buildIndex+ASN) : SN;

  const int cellExtra=System.splitObject(cellN,trueSN);
      
  
  CellMap* CMapPtr=dynamic_cast<attachSystem::CellMap*>(this);
  if (CMapPtr)
    CMapPtr->registerExtra(cellN,cellExtra);
  
  return OutCell;  
}

std::vector<int>
FixedComp::splitObject(Simulation& System,const std::string& SName,
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
  ELog::RegMethod RegA("FixedComp","splitObject(surfmap)");

  const SurfMap* SMapPtr=
    dynamic_cast<const attachSystem::SurfMap*>(this);
  if (!SMapPtr)
    throw ColErr::TypeMatch("SurfMap",this->getKeyName(),"FixedComp convert");

  return splitObject(System,SMapPtr->getSignedSurf(SName),cellN);
}

  
}  // NAMESPACE attachSystem
