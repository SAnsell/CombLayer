/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/CylFlowGuide.cxx
 *
 * Copyright (c) 2004-2017 by Konstantin Batkov
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Cylinder.h"
#include "surfIndex.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"

#include "CylFlowGuide.h"

namespace essSystem
{

CylFlowGuide::CylFlowGuide(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key,0),
  attachSystem::CellMap(),
  insIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(insIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

CylFlowGuide::CylFlowGuide(const CylFlowGuide& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedComp(A),
  attachSystem::CellMap(A),
  insIndex(A.insIndex),
  cellIndex(A.cellIndex),
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
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
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
  
  return;
}

void
CylFlowGuide::createUnitVector(const attachSystem::FixedComp& FC,
			       const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Centre for object
    \param sideIndex :: Inner link point
  */
{
  ELog::RegMethod RegA("CylFlowGuide","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

  // Take data from containing object
  const int CN=std::abs(FC.getLinkSurf(sideIndex));

  const Geometry::Cylinder* CPtr=SMap.realPtr<Geometry::Cylinder>(CN);
  if (!CPtr)
    throw ColErr::InContainerError<int>(CN,"Unable to convert to cylinder");

  radius=CPtr->getRadius();
  Origin=CPtr->getCentre(); 

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

  ModelSupport::buildPlane(SMap,insIndex+3,Origin-X*(wallThick/2.0),X);
  ModelSupport::buildPlane(SMap,insIndex+4,Origin+X*(wallThick/2.0),X);

  ModelSupport::buildCylinder(SMap,insIndex+7,Origin,Z,radius-gapWidth);
  ModelSupport::buildPlane(SMap,insIndex+14,
			   Origin-X*(gapWidth+wallThick/2.0),X);
  ModelSupport::buildPlane(SMap, insIndex+24,
			   Origin+X*(gapWidth+wallThick/2.0),X);

  double yStep(-radius);  // going from -ve to +ve
  int SI(insIndex);
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
CylFlowGuide::createObjects(Simulation& System,
                            attachSystem::FixedComp& FC,
                            const long int sideIndex)
/*!
    Create the objects
    \param System :: Simulation to add results
    \param FC :: FC object where the inner structure is to be added
    \param sideIndex :: link point for inner volume 
  */
{
  ELog::RegMethod RegA("CylFlowGuide","createObjects");
  
  attachSystem::CellMap* CM = dynamic_cast<attachSystem::CellMap*>(&FC);
  if (!CM)
    throw ColErr::DynamicConv("FixedComp","CellMap",FC.getKeyName());
  
  const std::pair<int,double> MatInfo=CM->deleteCellWithData(System,"Inner");
  const int innerMat=MatInfo.first;
  const double innerTemp=MatInfo.second;
  std::string Out;

  // This is AWFUL:
  const std::string vertStr =
    FC.getLinkString(sideIndex+3)+
    FC.getLinkString(sideIndex+2);
  const std::string sideStr =
    FC.getLinkString(sideIndex);

  const int initCellIndex(cellIndex);
  // central plate
  Out=ModelSupport::getComposite(SMap,insIndex," 3 -4 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,innerTemp,
				   Out+vertStr+sideStr));

  // side plates
  int SI(insIndex);

  for (size_t i=0;i<nBaffles;i++)
    {
      // Baffles
      if (i%2)
	{
	  Out = ModelSupport::getComposite(SMap,SI,insIndex," 1 -2 -14M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,innerTemp,
					   Out+vertStr+sideStr));
          
	  Out = ModelSupport::getComposite(SMap,SI,insIndex," 1 -2 14M -3M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,
                                           Out+vertStr));
          
	  Out = ModelSupport::getComposite(SMap,SI,insIndex," 1 -2 24M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,innerTemp,
					   Out+vertStr+sideStr));

                    
	  Out = ModelSupport::getComposite(SMap,SI,insIndex," 1 -2 -24M 4M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,
					   Out+vertStr+sideStr));

	}
      else 
	{
	  Out = ModelSupport::getComposite(SMap,SI,insIndex, " 1 -2 -3M -7M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,innerMat,Out+vertStr));
	  
	  // x<0
	  Out = ModelSupport::getComposite(SMap,SI,insIndex, " 1 -2 7M -3M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,
					   Out+vertStr+sideStr));
	  // same but x>0 - divided by surf 3M to gain speed
	  Out = ModelSupport::getComposite(SMap,SI,insIndex, " 1 -2 7M 3M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,
					   Out+vertStr+sideStr));
	  
	  Out = ModelSupport::getComposite(SMap,SI,insIndex, " 1 -2 4M -7M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,innerTemp,
					   Out+vertStr));
	  
	}
      
      // Splitting of innerCell (to gain speed)
      if (i==0)
	{
	  Out = ModelSupport::getComposite(SMap,SI,insIndex," -1 -3M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,
					   Out+vertStr+sideStr));
	  Out = ModelSupport::getComposite(SMap,SI,insIndex," -1 4M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,
					   innerTemp,Out+vertStr+sideStr));
	}
      else
	{
	  Out = ModelSupport::getSetComposite(SMap,SI-10,insIndex," -11 2 -3M");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,Out+vertStr+sideStr));
	  
	  Out = ModelSupport::getSetComposite(SMap,SI-10,insIndex," -11 2 4M");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,Out+vertStr+sideStr));

	}
      SI += 10;
    }
  // Tail end:
  Out = ModelSupport::getComposite(SMap,SI-10,insIndex," 2 -3M ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,Out+vertStr+sideStr));
  Out = ModelSupport::getComposite(SMap,SI-10,insIndex," 2 4M ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,Out+vertStr+sideStr));

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
  createObjects(System,FC,sideIndex);
  createLinks();
  
  insertObjects(System);       
  return;
}
  
}  // NAMESPACE essSystem
