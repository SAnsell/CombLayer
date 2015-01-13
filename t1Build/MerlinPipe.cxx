/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Build/MerlinPipe.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "MerlinPipe.h"

#include "Debug.h"

namespace ts1System
{

MerlinPipe::MerlinPipe(const std::string& Key)  :
  attachSystem::FixedComp(Key,0),
  pipeIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(pipeIndex+1),
  Central("central"),WatIn("WaterIn"),WatOut("WaterOut")
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

MerlinPipe::MerlinPipe(const MerlinPipe& A) : 
  attachSystem::FixedComp(A),
  pipeIndex(A.pipeIndex),cellIndex(A.cellIndex),
  Central(A.Central),WatIn(A.WatIn),WatOut(A.WatOut),
  Xoffset(A.Xoffset),Yoffset(A.Yoffset),outMat(A.outMat),
  outVacMat(A.outVacMat),midMat(A.midMat),midVacMat(A.midVacMat),
  watSkinMat(A.watSkinMat),watMat(A.watMat),outRadius(A.outRadius),
  outVacRadius(A.outVacRadius),midRadius(A.midRadius),
  midVacRadius(A.midVacRadius),watSkinRadius(A.watSkinRadius),
  watRadius(A.watRadius),fullLen(A.fullLen),wInXoffset(A.wInXoffset),
  wInYoffset(A.wInYoffset),wOutXoffset(A.wOutXoffset),
  wOutYoffset(A.wOutYoffset)
  /*!
    Copy constructor
    \param A :: MerlinPipe to copy
  */
{}

MerlinPipe&
MerlinPipe::operator=(const MerlinPipe& A)
  /*!
    Assignment operator
    \param A :: MerlinPipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      Central=A.Central;
      WatIn=A.WatIn;
      WatOut=A.WatOut;
      Xoffset=A.Xoffset;
      Yoffset=A.Yoffset;
      outMat=A.outMat;
      outVacMat=A.outVacMat;
      midMat=A.midMat;
      midVacMat=A.midVacMat;
      watSkinMat=A.watSkinMat;
      watMat=A.watMat;
      outRadius=A.outRadius;
      outVacRadius=A.outVacRadius;
      midRadius=A.midRadius;
      midVacRadius=A.midVacRadius;
      watSkinRadius=A.watSkinRadius;
      watRadius=A.watRadius;
      fullLen=A.fullLen;
      wInXoffset=A.wInXoffset;
      wInYoffset=A.wInYoffset;
      wOutXoffset=A.wOutXoffset;
      wOutYoffset=A.wOutYoffset;
    }
  return *this;
}


MerlinPipe::~MerlinPipe() 
  /*!
    Destructor
  */
{}

void
MerlinPipe::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("MerlinPipe","populate");
  
  const FuncDataBase& Control=System.getDataBase();
  

  Xoffset=Control.EvalVar<double>(keyName+"XOffset"); 
  Yoffset=Control.EvalVar<double>(keyName+"YOffset"); 
  
  fullLen=Control.EvalVar<double>(keyName+"FullLen");
 
  outRadius=Control.EvalVar<double>(keyName+"OutRadius") ;
  outVacRadius=Control.EvalVar<double>(keyName+"OutVacRadius"); 
  midRadius=Control.EvalVar<double>(keyName+"MidRadius"); 
  midVacRadius=Control.EvalVar<double>(keyName+"MidVacRadius"); 
  watSkinRadius=Control.EvalVar<double>(keyName+"WaterSkinRadius"); 
  watRadius=Control.EvalVar<double>(keyName+"WaterRadius"); 

  outMat=ModelSupport::EvalMat<int>(Control,keyName+"OutMat"); 
  outVacMat=ModelSupport::EvalMat<int>(Control,keyName+"OutVacMat"); 
  midMat=ModelSupport::EvalMat<int>(Control,keyName+"MidMat"); 
  midVacMat=ModelSupport::EvalMat<int>(Control,keyName+"MidVacMat"); 
  watSkinMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterSkinMat"); 
  watMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat"); 

  // wInXoffset=Control.EvalVar<double>(keyName+"WInXOff"); 
  // wInYoffset=Control.EvalVar<double>(keyName+"WInYOff"); 
  // wOutXoffset=Control.EvalVar<double>(keyName+"WOutXOff"); 
  // wOutYoffset=Control.EvalVar<double>(keyName+"WOutYOff"); 

  return;
}
  

void
MerlinPipe::createUnitVector(const attachSystem::FixedComp& CUnit,
			     const size_t sideIndex)
  /*!
    Create the unit vectors
    - X Across the moderator
    - Z up 
    \param CUnit :: Fixed unit that it is connected to 
    \param sideIndex :: Connection point to use as origin
  */
{
  ELog::RegMethod RegA("MerlinPipe","createUnitVector");

  FixedComp::createUnitVector(CUnit);
  Origin=CUnit.getLinkPt(sideIndex);
  //  Z=CUnit.getLinkAxis(sideIndex);

  return;
}

void 
MerlinPipe::insertOuter(Simulation& System,
			const attachSystem::FixedComp& FC,
			const size_t side)
  /*!
    Add a pipe to the hydrogen system:
    \param System :: Simulation to add pipe to
    \param FC :: 
    \param side :: index fo sid
  */
{
  ELog::RegMethod RegA("MerlinPipe","insertOuter");

  // Inner Points
  Central.addPoint(FC.getLinkPt(side)+X*Xoffset+Y*Yoffset);
  Central.addPoint(FC.getLinkPt(side)+X*Xoffset+Y*Yoffset+Z*fullLen);

  // Smallest to largest radius.
  Central.addRadius(midVacRadius,midVacMat,0.0);
  Central.addRadius(midRadius,midMat,0.0);
  Central.addRadius(outVacRadius,outVacMat,0.0);
  Central.addRadius(outRadius,outMat,0.0);
 
  Central.createAll(System);
  return;
}

void
MerlinPipe::insertPipes(Simulation& System)
  /*!
    Add a pipe to the hydrogen system:
    \remark This should be called after the void vessel has
    been constructed and all objects inserted.
    \param System :: Simulation to add pipe to
  */
{
  ELog::RegMethod RegA("MerlinPipe","insertPipes");

  // // First job is to re-create the OSM
  // System.validateObjSurfMap();

  // // Hydrogen inner
  // HInner.addPoint(Origin+X*hXoffset+Y*hYoffset);
  // HInner.addPoint(Origin+X*hXoffset+Y*hYoffset+Z*hLen);
  
  // HInner.addRadius(hRadius-hThick,hydMat,hydTemp);
  // HInner.addRadius(hRadius,innerAlMat,hydTemp);
 
  // HInner.createAll(System);
  return;
}

  
void
MerlinPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FUnit,
		      const size_t sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param sideIndex :: FixUnit
  */
{
  ELog::RegMethod RegA("MerlinPipe","createAll");
  // First job is to re-create the OSM and populate cells
  System.populateCells();
  System.validateObjSurfMap();

  populate(System);
  createUnitVector(FUnit,sideIndex);
  insertOuter(System,FUnit,sideIndex);
  insertPipes(System);       
  
  return;
}
  
}  // NAMESPACE ts1System
