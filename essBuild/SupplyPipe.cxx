/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuild/SupplyPipe.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
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
#include "KGroup.h"
#include "Source.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinearComp.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "SupplyPipe.h"

#include "Debug.h"

namespace essSystem
{

SupplyPipe::SupplyPipe(const std::string& Key)  :
  attachSystem::FixedComp(Key,0),
  pipeIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(pipeIndex+1),Coaxial("LowCoAx")
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

SupplyPipe::~SupplyPipe() 
  /*!
    Destructor
  */
{}

void
SupplyPipe::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("SupplyPipe","populate");
  
  const FuncDataBase& Control=System.getDataBase();
  
  
  NSegIn=Control.EvalVar<size_t>(keyName+"NSegIn");
  for(size_t i=0;i<=NSegIn;i++)
    {
      PPts.push_back(Control.EvalVar<Geometry::Vec3D>
		     (StrFunc::makeString(keyName+"PPt",i)));
    }
  inRadius=Control.EvalVar<double>(keyName+"InRadius"); 
  inAlRadius=Control.EvalVar<double>(keyName+"InAlRadius"); 
  midRadius=Control.EvalVar<double>(keyName+"MidRadius"); 
  midAlRadius=Control.EvalVar<double>(keyName+"MidAlRadius"); 

  inMat=Control.EvalVar<int>(keyName+"InMat"); 
  inAlMat=Control.EvalVar<int>(keyName+"InAlMat"); 
  midMat=Control.EvalVar<int>(keyName+"MidMat"); 
  midAlMat=Control.EvalVar<int>(keyName+"MidAlMat"); 
  
  return;
}
  

void
SupplyPipe::createUnitVector(const attachSystem::FixedComp& CUnit,
			  const size_t sideIndex)
  /*!
    Create the unit vectors
    - Y Points towards NIMROD
    - X Across the moderator
    - Z up (towards the target)
    \param CUnit :: Fixed unit that it is connected to 
    \param sideIndex :: Connection point to use as origin
  */
{
  ELog::RegMethod RegA("SupplyPipe","createUnitVector");

  FixedComp::createUnitVector(CUnit);
  const attachSystem::LinkUnit& LU=CUnit.getLU(sideIndex);
  Origin=LU.getConnectPt();

  return;
}

void 
SupplyPipe::insertInlet(Simulation& System,
			const attachSystem::FixedComp& FC,
			const size_t lSideIndex)
  /*!
    Add a pipe to the hydrogen system:
    \remark This should be called after the void vessel has
    been constructed and all objects inserted.
    \param System :: Simulation to add pipe to
    \param FC :: layer values
    \param lSideIndex :: point for side of moderator
  */
{
  ELog::RegMethod RegA("SupplyPipe","insertOuter");

  const attachSystem::LayerComp* LC=
    dynamic_cast<const attachSystem::LayerComp*>(&FC);
  if (!LC)
    throw ColErr::CastError<void>(0,"FixedComp to LayerComp");

  Geometry::Vec3D Pt=
    X*PPts[0].X()+Y*PPts[0].Y()+
    Z*PPts[0].Z();
  Coaxial.addPoint(Origin+Pt);
  
  // GET Z Point from layer
  const Geometry::Vec3D PtZ=LC->getSurfacePoint(2,lSideIndex);
  Pt=X*Origin.X()+Y*Origin.Y()+Z*PtZ.Z();
  Coaxial.addPoint(Pt);

  Coaxial.setActive(0,3);  
  // Inner Points
  for(size_t i=1;i<=NSegIn;i++)
    {
       Pt=X*PPts[i].X()+Y*PPts[i].Y()+Z*PPts[i].Z();
       Coaxial.addPoint(Origin+Pt);
    }  

  Coaxial.addRadius(inRadius,inMat,0.0);
  Coaxial.addRadius(inAlRadius,inAlMat,0.0);
  Coaxial.addRadius(midRadius,midMat,0.0);
  Coaxial.addRadius(midAlRadius,midAlMat,0.0);
  Coaxial.createAll(System);
  return;
}
  
void
SupplyPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FUnit,
		      const size_t sideIndex,
		      const size_t laySideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param sideIndex :: FixUnit
  */
{
  ELog::RegMethod RegA("SupplyPipe","createAll");

  populate(System);

  createUnitVector(FUnit,sideIndex);
  insertInlet(System,FUnit,laySideIndex);
    
  return;
}
  
}  // NAMESPACE moderatorSystem
