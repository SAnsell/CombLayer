/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
  * File:   essBuild/DiskPreSimple.cxx
  *
  * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "SurInter.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "CellMap.h"
#include "BilbaoWheelInnerStructure.h"

namespace essSystem
{

  BilbaoWheelInnerStructure::BilbaoWheelInnerStructure(const std::string& Key) :
    attachSystem::ContainedComp(),
    attachSystem::FixedComp(Key,6),
    insIndex(ModelSupport::objectRegister::Instance().cell(Key)),
    cellIndex(insIndex+1)
    /*!
      Constructor
      \param Key :: Name of construction key
    */
  {}

  BilbaoWheelInnerStructure::BilbaoWheelInnerStructure(const BilbaoWheelInnerStructure& A) : 
    attachSystem::ContainedComp(A),
    attachSystem::FixedComp(A),
    insIndex(A.insIndex),
    cellIndex(A.cellIndex),
    brickLen(A.brickLen),
    brickWidth(A.brickWidth),
    brickMat(A.brickMat),
    brickGapLen(A.brickGapLen),
    brickGapWidth(A.brickGapWidth),
    brickGapMat(A.brickGapMat)
    //    BeMat(A.BeMat),
    //    BeWallMat(A.BeWallMat)
    /*!
      Copy constructor
      \param A :: BilbaoWheelInnerStructure to copy
    */
  {}

  BilbaoWheelInnerStructure&
  BilbaoWheelInnerStructure::operator=(const BilbaoWheelInnerStructure& A)
  /*!
    Assignment operator
    \param A :: BilbaoWheelInnerStructure to copy
    \return *this
  */
  {
    if (this!=&A)
      {
	attachSystem::ContainedComp::operator=(A);
	attachSystem::FixedComp::operator=(A);
	cellIndex=A.cellIndex;
	brickLen=A.brickLen;
	brickWidth=A.brickWidth;
	brickMat=A.brickMat;
	brickGapLen=A.brickGapLen;
	brickGapWidth=A.brickGapWidth;
	brickGapMat=A.brickGapMat;
	//	BeMat=A.BeMat;
	//	BeWallMat=A.BeWallMat;
      }
    return *this;
  }

  BilbaoWheelInnerStructure*
  BilbaoWheelInnerStructure::clone() const
  /*!
    Clone self 
    \return new (this)
  */
  {
    return new BilbaoWheelInnerStructure(*this);
  }

  BilbaoWheelInnerStructure::~BilbaoWheelInnerStructure()
  /*!
    Destructor
  */
  {}
  

  void
  BilbaoWheelInnerStructure::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
  {
    ELog::RegMethod RegA("BilbaoWheelInnerStructure","populate");

    brickLen=Control.EvalVar<double>(keyName+"BrickLength");
    brickWidth=Control.EvalVar<double>(keyName+"BrickWidth");
    brickMat=ModelSupport::EvalMat<int>(Control,keyName+"BrickMat");

    brickGapLen=Control.EvalVar<double>(keyName+"BrickGapLength");
    brickGapWidth=Control.EvalVar<double>(keyName+"BrickGapWidth");
    brickGapMat=ModelSupport::EvalMat<int>(Control,keyName+"BrickGapMat");

    //    BeMat=ModelSupport::EvalMat<int>(Control,keyName+"BeMat");
    //    BeWallMat=ModelSupport::EvalMat<int>(Control,keyName+"BeWallMat");

    return;
  }

  void
  BilbaoWheelInnerStructure::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Centre for object
  */
  {
    ELog::RegMethod RegA("BilbaoWheelInnerStructure","createUnitVector");
    attachSystem::FixedComp::createUnitVector(FC);

    return;
  }


  void
  BilbaoWheelInnerStructure::createSurfaces(const attachSystem::FixedComp& Wheel)
  /*!
    Create planes for the inner structure iside BilbaoWheel
  */
  {
    ELog::RegMethod RegA("BilbaoWheelInnerStructure","createSurfaces");

    const double BilbaoWheelZBottom = Wheel.getLinkPt(6)[2];
    const double BilbaoWheelZTop = Wheel.getLinkPt(7)[2];

    ModelSupport::buildPlane(SMap, insIndex+5, Origin+Z*(BilbaoWheelZBottom+brickLen), Z);
    ModelSupport::buildPlane(SMap, insIndex+6, Origin+Z*(BilbaoWheelZTop-brickLen), Z);

    ModelSupport::buildPlane(SMap, insIndex+15, Origin+Z*(BilbaoWheelZBottom+brickLen+brickWidth), Z);
    ModelSupport::buildPlane(SMap, insIndex+16, Origin+Z*(BilbaoWheelZTop-brickLen-brickWidth), Z);

    ModelSupport::buildCylinder(SMap, insIndex+7, Origin, Z, brickGapWidth);
    ModelSupport::buildCylinder(SMap, insIndex+17, Origin, Z, brickGapWidth+brickGapLen);

    return; 
  }

  void
  BilbaoWheelInnerStructure::createObjects(Simulation& System, const attachSystem::FixedComp& Wheel)
  /*!
    Create the objects
    \param System :: Simulation to add results
    \param Wheel :: Wheel object where the inner structure is to be added
  */
  {
    ELog::RegMethod RegA("BilbaoWheelInnerStructure","createObjects");
    
    const attachSystem::CellMap* CM = dynamic_cast<const attachSystem::CellMap*>(&Wheel);
    MonteCarlo::Object* LowBeObj(0);
    MonteCarlo::Object* TopBeObj(0);
    int lowBeCell(0);
    int topBeCell(0);

    if (CM)
      {
	lowBeCell=CM->getCell("lowBe");
	LowBeObj=System.findQhull(lowBeCell);

	topBeCell=CM->getCell("topBe");
	TopBeObj=System.findQhull(topBeCell);
      }
    if (!LowBeObj)
      throw ColErr::InContainerError<int>(topBeCell,"Wheel lowBe cell not found");
    if (!TopBeObj)
      throw ColErr::InContainerError<int>(topBeCell,"Wheel topBe cell not found");
    

    return; 
  }

  void
  BilbaoWheelInnerStructure::createLinks()
  /*!
    Creates a full attachment set
  */
  {  
    ELog::RegMethod RegA("BilbaoWheelInnerStructure","createLinks");


    return;
  }

  void
  BilbaoWheelInnerStructure::createAll(Simulation& System,
				 const attachSystem::FixedComp& FC)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point	       
  */
  {
    ELog::RegMethod RegA("BilbaoWheelInnerStructure","createAll");

    populate(System.getDataBase());
    createUnitVector(FC);

    createSurfaces(FC);
    createObjects(System, FC);
    createLinks();

    insertObjects(System);       
    return;
  }

}  // NAMESPACE essSystem
