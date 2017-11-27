/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/WaterPipe.cxx
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
#include "ContainedComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "WaterPipe.h"

namespace ts1System
{

WaterPipe::WaterPipe(const std::string& Key)  :
  attachSystem::FixedComp(Key,0),
  pipeIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(pipeIndex+1),
  Inlet(Key+"Inlet"),Outlet(Key+"Outlet")
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}
  
WaterPipe::WaterPipe(const WaterPipe& A) : 
  attachSystem::FixedComp(A),
  pipeIndex(A.pipeIndex),cellIndex(A.cellIndex),
  Inlet(A.Inlet),Outlet(A.Outlet),iXStep(A.iXStep),
  iYStep(A.iYStep),oXStep(A.oXStep),oYStep(A.oYStep),
  watRadius(A.watRadius),outRadius(A.outRadius),
  voidRadius(A.voidRadius),steelRadius(A.steelRadius),
  outMat(A.outMat),watMat(A.watMat),steelMat(A.steelMat),
  ifullLen(A.ifullLen),ofullLen(A.ofullLen)
  /*!
    Copy constructor
    \param A :: WaterPipe to copy
  */
{}

WaterPipe&
WaterPipe::operator=(const WaterPipe& A)
  /*!
    Assignment operator
    \param A :: WaterPipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      Inlet=A.Inlet;
      Outlet=A.Outlet;
      iXStep=A.iXStep;
      iYStep=A.iYStep;
      oXStep=A.oXStep;
      oYStep=A.oYStep;
      watRadius=A.watRadius;
      outRadius=A.outRadius;
      voidRadius=A.voidRadius;
      steelRadius=A.steelRadius;
      outMat=A.outMat;
      watMat=A.watMat;
      steelMat=A.steelMat;
      ifullLen=A.ifullLen;
      ofullLen=A.ofullLen;
    }
  return *this;
}


WaterPipe::~WaterPipe() 
  /*!
    Destructor
  */
{}

void
WaterPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase to use
  */
{
  ELog::RegMethod RegA("WaterPipe","populate");
  
  const std::string iKey(keyName+"In");
  const std::string oKey(keyName+"Out");

  iXStep=Control.EvalVar<double>(iKey+"XStep"); 
  iYStep=Control.EvalVar<double>(iKey+"YStep"); 
  ifullLen=Control.EvalVar<double>(iKey+"FullLen");

  oXStep=Control.EvalVar<double>(oKey+"XStep"); 
  oYStep=Control.EvalVar<double>(oKey+"YStep"); 
  ofullLen=Control.EvalVar<double>(oKey+"FullLen");
  

  outRadius=Control.EvalVar<double>(keyName+"OuterRadius") ;
  watRadius=Control.EvalVar<double>(keyName+"WaterRadius"); 
  voidRadius=Control.EvalVar<double>(keyName+"VoidRadius"); 
  steelRadius=Control.EvalVar<double>(keyName+"SteelRadius"); 

  outMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat"); 
  watMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat"); 
  steelMat=ModelSupport::EvalMat<int>(Control,keyName+"SteelMat"); 

  return;
}
  

void
WaterPipe::createUnitVector(const attachSystem::FixedComp& CUnit,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    - X Across the moderator
    - Z up 
    \param CUnit :: Fixed unit that it is connected to 
    \param sideIndex :: Connection point to use as origin
  */
{
  ELog::RegMethod RegA("WaterPipe","createUnitVector");

  FixedComp::createUnitVector(CUnit);
  Origin=CUnit.getLinkPt(sideIndex);

  return;
}

void 
WaterPipe::insertPipes(Simulation& System)		       
  /*!
    Add a pipe to the water systems
    \param System :: Simulation to add pipe to
  */
{
  ELog::RegMethod RegA("WaterPipe","insertPipes");

  // Inner Points
  Inlet.addPoint(Origin+X*iXStep+Y*iYStep);
  Inlet.addPoint(Origin+X*iXStep+Y*iYStep+Z*ifullLen);
  // Outer Points
  Outlet.addPoint(Origin+X*oXStep+Y*oYStep);
  Outlet.addPoint(Origin+X*oXStep+Y*oYStep+Z*ofullLen);

  // Smallest to largest radius.
  Inlet.addRadius(watRadius,watMat,0.0);
  Inlet.addRadius(outRadius,outMat,0.0);
  Inlet.addRadius(voidRadius,0,0.0);
  Inlet.addRadius(steelRadius,steelMat,0.0);

  Outlet.addRadius(watRadius,watMat,0.0);
  Outlet.addRadius(outRadius,outMat,0.0);
  Outlet.addRadius(voidRadius,0,0.0);
  Outlet.addRadius(steelRadius,steelMat,0.0);
 
  Inlet.createAll(System);
  Outlet.createAll(System);
  return;
}

void
WaterPipe::createAll(Simulation& System,
		     const attachSystem::FixedComp& FUnit,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param sideIndex :: Side index [signed]
  */
{
  ELog::RegMethod RegA("WaterPipe","createAll");
  // First job is to re-create the OSM and populate cells
  System.populateCells();
  System.validateObjSurfMap();

  populate(System.getDataBase());
  createUnitVector(FUnit,sideIndex);
  insertPipes(System);
  
  return;
}
  
}  // NAMESPACE ts1System
