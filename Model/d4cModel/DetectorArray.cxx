/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   d4cModel/DetectorArray.cxx
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "DetectorArray.h"

#include "TallySelector.h" 
#include "SpecialSurf.h"
#include "pointConstruct.h" 

namespace d4cSystem
{

DetectorArray::DetectorArray(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,3),
  nDet(0)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}


DetectorArray::DetectorArray(const DetectorArray& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  centRadius(A.centRadius),tubeRadius(A.tubeRadius),
  wallThick(A.wallThick),height(A.height),wallMat(A.wallMat),
  detMat(A.detMat),nDet(A.nDet),initAngle(A.initAngle),
  finalAngle(A.finalAngle),DPoints(A.DPoints)
  /*!
    Copy constructor
    \param A :: DetectorArray to copy
  */
{}

DetectorArray&
DetectorArray::operator=(const DetectorArray& A)
  /*!
    Assignment operator
    \param A :: DetectorArray to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      centRadius=A.centRadius;
      tubeRadius=A.tubeRadius;
      wallThick=A.wallThick;
      height=A.height;
      wallMat=A.wallMat;
      detMat=A.detMat;
      nDet=A.nDet;
      initAngle=A.initAngle;
      finalAngle=A.finalAngle;
      DPoints=A.DPoints;
    }
  return *this;
}

DetectorArray::~DetectorArray()
  /*!
    Destructor
   */
{}

void
DetectorArray::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("DetectorArray","populate");

  FixedOffset::populate(Control);

  centRadius=Control.EvalVar<double>(keyName+"CentRadius");
  tubeRadius=Control.EvalVar<double>(keyName+"TubeRadius");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  height=Control.EvalVar<double>(keyName+"Height");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  detMat=ModelSupport::EvalMat<int>(Control,keyName+"DetMat");

  nDet=Control.EvalVar<size_t>(keyName+"NDetector");
  initAngle=M_PI*Control.EvalVar<double>(keyName+"InitAngle")/180.0;
  finalAngle=M_PI*Control.EvalVar<double>(keyName+"FinalAngle")/180.0;
  

  return;
}

void
DetectorArray::createUnitVector(const attachSystem::FixedComp& FC,
				const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: FixedComp for origin
  */
{
  ELog::RegMethod RegA("DetectorArray","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);

  applyOffset();

  return;
}

void
DetectorArray::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("DetectorArray","createSurfaces");


  if (!nDet) return;

  const double angleStep= (nDet==1) ? 0.0 : 
    (finalAngle-initAngle)/static_cast<double>(nDet-1);

  ModelSupport::buildPlane(SMap,buildIndex+5,
			   Origin-Z*(height/2.0),Z);    
  ModelSupport::buildPlane(SMap,buildIndex+6,
			   Origin+Z*(height/2.0),Z);    
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(height/2.0+wallThick),Z);    
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(height/2.0+wallThick),Z);    

  // MASTER BORDER
  if (nDet>1)
    { 
      const double A=initAngle;
      const Geometry::Vec3D CPt=Origin+(X*sin(A)+Y*cos(A))*centRadius;
      const Geometry::Vec3D IDir=X*cos(A)+Y*sin(-A);
      ModelSupport::buildPlane(SMap,buildIndex+3,
		  CPt-IDir*(tubeRadius+wallThick+Geometry::zeroTol),IDir);

      const double B=finalAngle;
      const Geometry::Vec3D DPt=Origin+(X*sin(B)+Y*cos(B))*centRadius;
      const Geometry::Vec3D FDir=X*cos(B)+Y*sin(-B);
      ModelSupport::buildPlane(SMap,buildIndex+4,
		  DPt+FDir*(tubeRadius+wallThick+Geometry::zeroTol),FDir);

      ModelSupport::buildCylinder(SMap,buildIndex+7,
	 Origin,Z,centRadius-(tubeRadius+wallThick+Geometry::zeroTol));
      ModelSupport::buildCylinder(SMap,buildIndex+17,
	 Origin,Z,centRadius+(tubeRadius+wallThick+Geometry::zeroTol));
    }

  int SI(buildIndex+20);
  for(size_t i=0;i<nDet;i++)
    {
      const double A=initAngle+static_cast<double>(i)*angleStep;
      const Geometry::Vec3D CPt=Origin+(X*sin(A)+Y*cos(A))*centRadius;
      DPoints.push_back(CPt);
      ModelSupport::buildCylinder(SMap,SI+7,CPt,Z,tubeRadius);  
      ModelSupport::buildCylinder(SMap,SI+17,CPt,Z,tubeRadius+wallThick);  
      SI+=20;
    }

  return; 
}

void
DetectorArray::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("DetectorArray","createObjects");

  std::string Out;
  // First make inner/outer void/wall and top/base

  std::string Bound=
    ModelSupport::getComposite(SMap,buildIndex,"3 -4 15 -16 7 -17 ");
  addOuterSurf(Bound);  
  int SI(buildIndex+20);
  for(size_t i=0;i<nDet;i++)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,SI,"-7M 5 -6");
      System.addCell(MonteCarlo::Qhull(cellIndex++,detMat,0.0,Out));
      Out=ModelSupport::getComposite(SMap,buildIndex,SI,"-17M 15 -16 (7M:-5:6)");
      Bound+=ModelSupport::getComposite(SMap,SI," 17 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
      SI+=20;
    }
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Bound));

  return; 
}

void
DetectorArray::createLinks()
  /*!
    Creates a full attachment set
  */
{
  
  return;
}

void
DetectorArray::createTally(Simulation& System) const
  /*!
    Extrenal build everything
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("DetectorArray","createTally");

  SimMCNP* SimPTR=dynamic_cast<SimMCNP*>(&System);
  if (SimPTR)
    {
      int tNum=5;
      for(size_t i=0;i<nDet;i++)
	{
	  tallySystem::addF5Tally(*SimPTR,tNum);      
	  tallySystem::setF5Position(*SimPTR,tNum,DPoints[i]);
	  tallySystem::setTallyTime(*SimPTR,tNum,"1.0 8log 1e8");
	  tNum+=10;
	}
    }
      
  return;
}



void
DetectorArray::createAll(Simulation& System,
			 const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to add
    \param sideIndex :: link point						
   */
{
  ELog::RegMethod RegA("DetectorArray","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE instrumentSystem
