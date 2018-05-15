/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/CH4Pipe.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
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
#include "LayerComp.h"
#include "ContainedComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "CH4Pipe.h"

#include "Debug.h"

namespace ts1System
{

CH4Pipe::CH4Pipe(const std::string& Key)  :
  attachSystem::FixedComp(Key,0),
  pipeIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(pipeIndex+1),Central("CH4Centre")
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

CH4Pipe::CH4Pipe(const CH4Pipe& A) : 
  attachSystem::FixedComp(A),
  pipeIndex(A.pipeIndex),cellIndex(A.cellIndex),
  Central(A.Central),Xoffset(A.Xoffset),Yoffset(A.Yoffset),
  ch4Mat(A.ch4Mat),alMat(A.alMat),ch4Temp(A.ch4Temp),
  ch4InnerRadius(A.ch4InnerRadius),alInnerRadius(A.alInnerRadius),
  ch4OuterRadius(A.ch4OuterRadius),alOuterRadius(A.alOuterRadius),
  vacRadius(A.vacRadius),alTerRadius(A.alTerRadius),
  clearRadius(A.clearRadius),fullLen(A.fullLen),
  depthLen(A.depthLen)
  /*!
    Copy constructor
    \param A :: CH4Pipe to copy
  */
{}

CH4Pipe&
CH4Pipe::operator=(const CH4Pipe& A)
  /*!
    Assignment operator
    \param A :: CH4Pipe to copy
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
      ch4Mat=A.ch4Mat;
      alMat=A.alMat;
      ch4Temp=A.ch4Temp;
      ch4InnerRadius=A.ch4InnerRadius;
      alInnerRadius=A.alInnerRadius;
      ch4OuterRadius=A.ch4OuterRadius;
      alOuterRadius=A.alOuterRadius;
      vacRadius=A.vacRadius;
      alTerRadius=A.alTerRadius;
      clearRadius=A.clearRadius;
      fullLen=A.fullLen;
      depthLen=A.depthLen;
    }
  return *this;
}


CH4Pipe::~CH4Pipe() 
  /*!
    Destructor
  */
{}

void
CH4Pipe::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("CH4Pipe","populate");
  
  const FuncDataBase& Control=System.getDataBase();
  

  Xoffset=Control.EvalVar<double>(keyName+"XOffset"); 
  Yoffset=Control.EvalVar<double>(keyName+"YOffset"); 
  
  fullLen=Control.EvalVar<double>(keyName+"FullLen");
  depthLen=Control.EvalVar<double>(keyName+"DepthLen");
 
  ch4InnerRadius=Control.EvalVar<double>(keyName+"CH4InnerRadius");
  alInnerRadius=Control.EvalVar<double>(keyName+"AlInnerRadius");
  ch4OuterRadius=Control.EvalVar<double>(keyName+"CH4OuterRadius");
  alOuterRadius=Control.EvalVar<double>(keyName+"AlOuterRadius");
  vacRadius=Control.EvalVar<double>(keyName+"VacRadius");
  alTerRadius=Control.EvalVar<double>(keyName+"AlTerRadius");
  clearRadius=Control.EvalVar<double>(keyName+"ClearRadius");

  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat"); 
  ch4Mat=ModelSupport::EvalMat<int>(Control,keyName+"CH4Mat"); 
  ch4Temp=Control.EvalVar<double>(keyName+"CH4Temp"); 

  return;
}
  

void
CH4Pipe::createUnitVector(const attachSystem::FixedComp& CUnit,
                          const long int sideIndex)
  /*!
    Create the unit vectors
    - X Across the moderator
    - Z up 
    \param CUnit :: Fixed unit that it is connected to 
    \param sideIndex :: Connection point to use as origin
  */
{
  ELog::RegMethod RegA("CH4Pipe","createUnitVector");

  FixedComp::createUnitVector(CUnit);
  Origin=CUnit.getLinkPt(sideIndex);

  return;
}

void 
CH4Pipe::insertOuter(Simulation& System,
                     const attachSystem::FixedComp& FC,
                     const long int sideIndex)
  /*!
    Add a pipe to the hydrogen system:
    \param System :: Simulation to add pipe to
    \param FC :: Fixed componet
    \param sideIndex :: index fo side
  */
{
  ELog::RegMethod RegA("CH4Pipe","addPipe");

  const attachSystem::LayerComp* LC=
    dynamic_cast<const attachSystem::LayerComp*>(&FC);
  if (!LC)
    throw ColErr::CastError<void>(0,"FixedComp to LayerComp");

  // Base Points
  Central.addPoint(LC->getSurfacePoint(0,sideIndex)+X*Xoffset+Y*Yoffset
		   +Z*depthLen);
  Central.addPoint(LC->getSurfacePoint(0,sideIndex)+X*Xoffset+Y*Yoffset);
  Central.addPoint(LC->getSurfacePoint(1,sideIndex)+X*Xoffset+Y*Yoffset);
  Central.addPoint(LC->getSurfacePoint(2,sideIndex)+X*Xoffset+Y*Yoffset);
  Central.addPoint(LC->getSurfacePoint(3,sideIndex)+X*Xoffset+Y*Yoffset);
  Central.addPoint(LC->getSurfacePoint(4,sideIndex)+X*Xoffset+Y*Yoffset);
  Central.addPoint(LC->getSurfacePoint(4,sideIndex)+
		   X*Xoffset+Y*Yoffset-Z*fullLen);

  Central.setActive(0,3);
  Central.setActive(1,7);
  Central.setActive(2,15);
  Central.setActive(3,63);
  Central.setActive(4,63);


  // Smallest to largest radius.
  Central.addRadius(ch4InnerRadius,ch4Mat,ch4Temp);
  Central.addRadius(alInnerRadius,alMat,ch4Temp);
  Central.addRadius(ch4OuterRadius,ch4Mat,ch4Temp);
  Central.addRadius(alOuterRadius,alMat,ch4Temp);
  Central.addRadius(vacRadius,0,0.0);
  Central.addRadius(alTerRadius,alMat,0.0);
  Central.addRadius(clearRadius,0,0.0);
 
  Central.setNAngle(18);
  Central.createAll(System);
  return;
}

  
void
CH4Pipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FUnit,
		      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param sideIndex :: side for pipe creation
  */
{
  ELog::RegMethod RegA("CH4Pipe","createAll");
  // First job is to re-create the OSM and populate cells
  System.populateCells();
  System.validateObjSurfMap();

  populate(System);
  createUnitVector(FUnit,sideIndex);
  insertOuter(System,FUnit,sideIndex);
  
  return;
}
  
}  // NAMESPACE moderatorSystem
