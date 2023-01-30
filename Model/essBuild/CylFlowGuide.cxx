/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/CylFlowGuide.cxx
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
#include "BaseModVisit.h"
#include "surfRegister.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Cylinder.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
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
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"


#include "CylFlowGuide.h"

namespace essSystem
{

CylFlowGuide::CylFlowGuide(const std::string& Key) :
  attachSystem::FixedRotate(Key,0),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

CylFlowGuide::CylFlowGuide(const CylFlowGuide& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  wallThick(A.wallThick),
  wallMat(A.wallMat),
  gapWidth(A.gapWidth),
  nBaffles(A.nBaffles)
  /*!
      Copy constructor
      \param A :: CylFlowGuide to copy
    */
  {}

CylFlowGuide&
CylFlowGuide::operator=(const CylFlowGuide& A)
  /*!
    Assignment operator
    \param A :: CylFlowGuide to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      wallThick=A.wallThick;
      wallMat=A.wallMat;
      gapWidth=A.gapWidth;
      nBaffles=A.nBaffles;
    }
  return *this;
}
  
CylFlowGuide*
CylFlowGuide::clone() const
 /*!
   Clone self 
   \return new (this)
 */
{
  return new CylFlowGuide(*this);
}

CylFlowGuide::~CylFlowGuide()
  /*!
    Destructor
  */
{}
  
void
CylFlowGuide::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("CylFlowGuide","populate");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  gapWidth=Control.EvalVar<double>(keyName+"GapWidth");

  nBaffles=Control.EvalVar<size_t>(keyName+"NBaffles");

  const Geometry::Cylinder* CPtr=
    SurfMap::realPtrThrow<Geometry::Cylinder>
    ("InnerCyl","Unable to convert to cylinder");
  radius=CPtr->getRadius();
  
  return;
}

void
CylFlowGuide::createSurfaces()
  /*!
    Create planes for the inner structure iside DiskPreMod
  */
{
  ELog::RegMethod RegA("CylFlowGuide","createSurfaces");

  // y-distance between plates
  const double dy((2.0*radius)/static_cast<double>(nBaffles+1)); 

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(wallThick/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(wallThick/2.0),X);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,radius-gapWidth);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin-X*(gapWidth+wallThick/2.0),X);
  ModelSupport::buildPlane(SMap, buildIndex+24,
			   Origin+X*(gapWidth+wallThick/2.0),X);

  double yStep(-radius);  // going from -ve to +ve
  int SI(buildIndex);
  for (size_t i=0;i<nBaffles;i++)
    {
      yStep+=dy;
      ModelSupport::buildPlane(SMap,SI+1,Origin+Y*(yStep-wallThick/2.0),Y);
      ModelSupport::buildPlane(SMap,SI+2,Origin+Y*(yStep+wallThick/2.0),Y);
      SI += 10;
    }
  
  return; 
}

void
CylFlowGuide::createObjects(Simulation& System)
/*!
    Create the objects
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("CylFlowGuide","createObjects");
  
  HeadRule HR;
  const std::pair<int,double> MatInfo=
    CellMap::deleteCellWithData(System,"Inner");
  const int innerMat=MatInfo.first;
  const double innerTemp=MatInfo.second;

  const HeadRule vertHR=getRule("TopBase");
  const HeadRule cylHR=getRule("InnerCyl");

  const int initCellIndex(cellIndex);
  // central plate
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4");
  System.addCell(cellIndex++,wallMat,innerTemp,HR*vertHR*cylHR);

  // side plates
  int SI(buildIndex);

  for (size_t i=0;i<nBaffles;i++)
    {
      // Baffles
      if (i%2)
	{
	  HR=ModelSupport::getHeadRule(SMap,SI,buildIndex,"1 -2 -14M");
	  System.addCell(cellIndex++,wallMat,innerTemp,
			 HR*vertHR*cylHR);
          
	  HR=ModelSupport::getHeadRule(SMap,SI,buildIndex,"1 -2 14M -3M");
	  System.addCell(cellIndex++,innerMat,innerTemp,HR*vertHR);
          
	  HR=ModelSupport::getHeadRule(SMap,SI,buildIndex,"1 -2 24M");
	  System.addCell(cellIndex++,wallMat,innerTemp,
			 HR*vertHR*cylHR);
                    
	  HR=ModelSupport::getHeadRule(SMap,SI,buildIndex,"1 -2 -24M 4M");
	  System.addCell(cellIndex++,innerMat,innerTemp,
			 HR*vertHR*cylHR);

	}
      else 
	{
	  HR=ModelSupport::getHeadRule(SMap,SI,buildIndex,"1 -2 -3M -7M");
	  System.addCell(cellIndex++,wallMat,innerMat,HR*vertHR);
	  
	  // x<0
	  HR=ModelSupport::getHeadRule(SMap,SI,buildIndex,"1 -2 7M -3M");
	  System.addCell(cellIndex++,innerMat,innerTemp,
			 HR*vertHR*cylHR);
	  // same but x>0 - divided by surf 3M to gain speed
	  HR=ModelSupport::getHeadRule(SMap,SI,buildIndex,"1 -2 7M 3M");
	  System.addCell(cellIndex++,innerMat,innerTemp,
			 HR*vertHR*cylHR);
	  
	  HR=ModelSupport::getHeadRule(SMap,SI,buildIndex,"1 -2 4M -7M");
	  System.addCell(cellIndex++,wallMat,innerTemp,HR*vertHR);
	}
      
      // Splitting of innerCell (to gain speed)
      if (i==0)
	{
	  HR=ModelSupport::getHeadRule(SMap,SI,buildIndex,"-1 -3M");
	  System.addCell(cellIndex++,innerMat,innerTemp,
			 HR*vertHR*cylHR);

	  HR=ModelSupport::getHeadRule(SMap,SI,buildIndex,"-1 4M");
	  System.addCell(cellIndex++,innerMat,
			 innerTemp,HR*vertHR*cylHR);
	}
      else
	{
	  HR=ModelSupport::getSetHeadRule(SMap,SI-10,buildIndex,"-11 2 -3M");
	  System.addCell(cellIndex++,innerMat,innerTemp,HR*vertHR*cylHR);
	  
	  HR=ModelSupport::getSetHeadRule(SMap,SI-10,buildIndex,"-11 2 4M");
	  System.addCell(cellIndex++,innerMat,innerTemp,HR*vertHR*cylHR);

	}
      SI += 10;
    }
  // Tail end:
  HR=ModelSupport::getHeadRule(SMap,SI-10,buildIndex,"2 -3M");
  System.addCell(cellIndex++,innerMat,innerTemp,HR*vertHR*cylHR);
  HR=ModelSupport::getHeadRule(SMap,SI-10,buildIndex,"2 4M");
  System.addCell(cellIndex++,innerMat,innerTemp,HR*vertHR*cylHR);

  // Add cell map info:
  CellMap::setCells("InnerGuide",initCellIndex,cellIndex-1);
  
  return; 
}
  
void
CylFlowGuide::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("CylFlowGuide","createLinks");
  return;
}

void
CylFlowGuide::createAll(Simulation& System,
                        attachSystem::FixedComp& FC,
                        const long int sideIndex)
  /*!
    External build everything
    \param System :: Simulation
    \param FC :: Attachment point (and cellMap)
    \param sideIndex :: inner cylinder index [signed for consistancy]
  */
{
  ELog::RegMethod RegA("CylFlowGuide","createAll");
    
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);  
  createSurfaces();
  createObjects(System);
  createLinks();
  
  insertObjects(System);       
  return;
}
  
}  // NAMESPACE essSystem
