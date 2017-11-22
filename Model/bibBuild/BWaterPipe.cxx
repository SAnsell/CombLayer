/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuild/BWaterPipe.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "BWaterPipe.h"

#include "Debug.h"

namespace bibSystem
{

BWaterPipe::BWaterPipe(const std::string& Key)  :
  attachSystem::FixedComp(Key,0),
  pipeIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(pipeIndex+1),Central("BWatPipe")
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BWaterPipe::BWaterPipe(const BWaterPipe& A) : 
  attachSystem::FixedComp(A),
  pipeIndex(A.pipeIndex),cellIndex(A.cellIndex),
  Central(A.Central),Xoffset(A.Xoffset),Yoffset(A.Yoffset),
  waterMat(A.waterMat),alMat(A.alMat),watRadius(A.watRadius),
  wallRadius(A.wallRadius),clearRadius(A.clearRadius),
  fullLen(A.fullLen)
  /*!
    Copy constructor
    \param A :: BWaterPipe to copy
  */
{}

BWaterPipe&
BWaterPipe::operator=(const BWaterPipe& A)
  /*!
    Assignment operator
    \param A :: BWaterPipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      Central=A.Central;
      Xoffset=A.Xoffset;
      Yoffset=A.Yoffset;
      waterMat=A.waterMat;
      alMat=A.alMat;
      watRadius=A.watRadius;
      wallRadius=A.wallRadius;
      clearRadius=A.clearRadius;
      fullLen=A.fullLen;
    }
  return *this;
}
 
BWaterPipe::~BWaterPipe() 
  /*!
    Destructor
  */
{}

void
BWaterPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("BWaterPipe","populate");
  

  Xoffset=Control.EvalVar<double>(keyName+"XOffset"); 
  Yoffset=Control.EvalVar<double>(keyName+"YOffset"); 
  
  fullLen=Control.EvalVar<double>(keyName+"FullLen");
 
  watRadius=Control.EvalVar<double>(keyName+"WatRadius");
  wallRadius=Control.EvalVar<double>(keyName+"WallRadius");
  clearRadius=Control.EvalVar<double>(keyName+"ClearRadius");

  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");    

  return;
}
  

void
BWaterPipe::createUnitVector(const attachSystem::FixedComp& CUnit,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    - X Across the moderator
    - Z up 
    \param CUnit :: Fixed unit that it is connected to 
    \param sideIndex :: Connection point to use as origin
  */
{
  ELog::RegMethod RegA("BWaterPipe","createUnitVector");

  FixedComp::createUnitVector(CUnit);
  Origin=CUnit.getLinkPt(sideIndex);

  return;
}

void 
BWaterPipe::insertPipe(Simulation& System,const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Add a pipe to the hydrogen system:
    \param System :: Simulation to add pipe to
    \param FC :: Fixed componet
    \param sideIndex :: index fo side
  */
{
  ELog::RegMethod RegA("BWaterPipe","insertPipe");

  // Base Points
  Central.addPoint(FC.getLinkPt(sideIndex)+X*Xoffset+Y*Yoffset);
  Central.addPoint(FC.getLinkPt(sideIndex)+X*Xoffset+Y*Yoffset
		   +Z*fullLen);

  Geometry::Vec3D Pt(FC.getLinkPt(sideIndex)+X*Xoffset+Y*Yoffset
		     +Z*fullLen);

  Pt+=X*20.0+Y*20.0;
  Central.addPoint(Pt);

  // Smallest to largest radius.
  Central.addRadius(watRadius,waterMat,0.0);
  Central.addRadius(wallRadius,alMat,0.0);
  Central.addRadius(clearRadius,0,0.0);
 
  Central.setActive(0,1);
  Central.setActive(1,1);

  Central.createAll(System);
  return;
}

  
void
BWaterPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FUnit,
		      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param sideIndex :: FixUnit
  */
{
  ELog::RegMethod RegA("BWaterPipe","createAll");

  populate(System.getDataBase());
  createUnitVector(FUnit,sideIndex);
  insertPipe(System,FUnit,sideIndex);
  
  return;
}
  
}  // NAMESPACE moderatorSystem
