/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/H2Pipe.cxx
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
#include <array>

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
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
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
#include "VacVessel.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "H2Pipe.h"

#include "Debug.h"

namespace ts1System
{

H2Pipe::H2Pipe(const std::string& Key)  :
  attachSystem::FixedComp(Key,0),
  pipeIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(pipeIndex+1),Central("H2Centre")
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

H2Pipe::H2Pipe(const H2Pipe& A) : 
  attachSystem::FixedComp(A),
  pipeIndex(A.pipeIndex),cellIndex(A.cellIndex),
  Central(A.Central),Xoffset(A.Xoffset),Yoffset(A.Yoffset),
  h2Mat(A.h2Mat),alMat(A.alMat),steelMat(A.steelMat),h2Temp(A.h2Temp),
  h2InnerRadius(A.h2InnerRadius),alInnerRadius(A.alInnerRadius),
  h2OuterRadius(A.h2OuterRadius),alOuterRadius(A.alOuterRadius),
  vacRadius(A.vacRadius),midRadius(A.midRadius),
  terRadius(A.terRadius),alTerRadius(A.alTerRadius),
  clearRadius(A.clearRadius),steelRadius(A.steelRadius),
  fullLen(A.fullLen),
  depthLen(A.depthLen)
  /*!
    Copy constructor
    \param A :: H2Pipe to copy
  */
{}

H2Pipe&
H2Pipe::operator=(const H2Pipe& A)
  /*!
    Assignment operator
    \param A :: H2Pipe to copy
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
      h2Mat=A.h2Mat;
      alMat=A.alMat;
      steelMat=A.steelMat;      
      h2Temp=A.h2Temp;
      h2InnerRadius=A.h2InnerRadius;
      alInnerRadius=A.alInnerRadius;
      h2OuterRadius=A.h2OuterRadius;
      alOuterRadius=A.alOuterRadius;
      vacRadius=A.vacRadius;
      midRadius=A.midRadius;
      terRadius=A.terRadius;
      alTerRadius=A.alTerRadius;
      clearRadius=A.clearRadius;
      steelRadius=A.steelRadius;
      fullLen=A.fullLen;
      depthLen=A.depthLen;
    }
  return *this;
}


H2Pipe::~H2Pipe() 
  /*!
    Destructor
  */
{}

void
H2Pipe::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("H2Pipe","populate");
  
  const FuncDataBase& Control=System.getDataBase();
  

  Xoffset=Control.EvalVar<double>(keyName+"XOffset"); 
  Yoffset=Control.EvalVar<double>(keyName+"YOffset"); 
  
  fullLen=Control.EvalVar<double>(keyName+"FullLen");
  depthLen=Control.EvalVar<double>(keyName+"DepthLen");
 
  h2InnerRadius=Control.EvalVar<double>(keyName+"H2InnerRadius");
  alInnerRadius=Control.EvalVar<double>(keyName+"AlInnerRadius");
  h2OuterRadius=Control.EvalVar<double>(keyName+"H2OuterRadius");
  alOuterRadius=Control.EvalVar<double>(keyName+"AlOuterRadius");
  vacRadius=Control.EvalVar<double>(keyName+"VacRadius");
  midRadius=Control.EvalVar<double>(keyName+"MidRadius");
  terRadius=Control.EvalVar<double>(keyName+"TerRadius");
  alTerRadius=Control.EvalVar<double>(keyName+"AlTerRadius");
  clearRadius=Control.EvalVar<double>(keyName+"ClearRadius");
  steelRadius=Control.EvalVar<double>(keyName+"SteelRadius");

  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  steelMat=ModelSupport::EvalMat<int>(Control,keyName+"SteelMat");    
  h2Mat=ModelSupport::EvalMat<int>(Control,keyName+"H2Mat"); 
  h2Temp=Control.EvalVar<double>(keyName+"H2Temp"); 

  return;
}
  

void
H2Pipe::createUnitVector(const attachSystem::FixedComp& CUnit,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    - X Across the moderator
    - Z up 
    \param CUnit :: Fixed unit that it is connected to 
    \param sideIndex :: Connection point to use as origin
  */
{
  ELog::RegMethod RegA("H2Pipe","createUnitVector");

  FixedComp::createUnitVector(CUnit);
  Origin=CUnit.getLinkPt(sideIndex);

  return;
}

void 
H2Pipe::insertOuter(Simulation& System,const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Add a pipe to the hydrogen system:
    \param System :: Simulation to add pipe to
    \param FC :: Fixed componet
    \param sideIndex :: index fo side
  */
{
  ELog::RegMethod RegA("H2Pipe","addPipe");

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
  Central.addPoint(LC->getSurfacePoint(5,sideIndex)+X*Xoffset+Y*Yoffset);
  Central.addPoint(LC->getSurfacePoint(5,sideIndex)+
		   X*Xoffset+Y*Yoffset-Z*fullLen);

  Central.setActive(0,3);
  Central.setActive(1,7);
  Central.setActive(2,15);
  Central.setActive(3,63);
  Central.setActive(4,63);
  Central.setActive(5,127);

  // Smallest to largest radius.
  Central.addRadius(h2InnerRadius,h2Mat,h2Temp);
  Central.addRadius(alInnerRadius,steelMat,h2Temp);
  Central.addRadius(h2OuterRadius,h2Mat,h2Temp);
  Central.addRadius(alOuterRadius,alMat,h2Temp);
  Central.addRadius(vacRadius,0,0.0);
  Central.addRadius(midRadius,alMat,0.0);
  Central.addRadius(terRadius,0,0.0);
  Central.addRadius(alTerRadius,alMat,0.0);
  Central.addRadius(clearRadius,0,0.0);
  Central.addRadius(steelRadius,steelMat,0.0);
 
  Central.setNAngle(18);
  Central.createAll(System);
  return;
}

  
void
H2Pipe::createAll(Simulation& System,
		  const attachSystem::FixedComp& FUnit,
		  const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param sideIndex :: FixUnit
  */
{
  ELog::RegMethod RegA("H2Pipe","createAll");

  populate(System);
  createUnitVector(FUnit,sideIndex);
  insertOuter(System,FUnit,sideIndex);
  
  return;
}
  
}  // NAMESPACE moderatorSystem
