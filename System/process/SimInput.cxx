/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/SimInput.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <string>
#include <algorithm>
#include <iterator>
#include <array>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Rules.h"
#include "Code.h"
#include "FItem.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "ModeCard.h"
#include "PhysImp.h"
#include "PhysCard.h"
#include "PStandard.h"
#include "LSwitchCard.h"
#include "PhysicsCards.h"
#include "LineTrack.h"
#include "ImportControl.h"
#include "SimValid.h"
#include "MainProcess.h"
#include "WeightControl.h"
#include "WCellControl.h"
#include "WWGControl.h"
#include "SimInput.h"


namespace SimProcess
{

void
importanceSim(Simulation& System,const mainSystem::inputParam& IParam)
  /*!
    Apply importances/renumber and weights
     \param System :: Simuation object 
     \param IParam :: Input parameters
   */
{
  ELog::RegMethod RegA("SimInput","importanceSim");

  System.populateCells();
  System.createObjSurfMap();

  WeightSystem::simulationImp(System,IParam);

  WeightSystem::ExtField(System,IParam);
  WeightSystem::FCL(System,IParam);
  WeightSystem::IMP(System,IParam);
  WeightSystem::DXT(System,IParam);
  WeightSystem::PWT(System,IParam);
  WeightSystem::EnergyCellCut(System,IParam);
  mainSystem::renumberCells(System,IParam);
  WeightSystem::WCellControl WCell;
  WeightSystem::WWGControl WWGC;
  WCell.processWeights(System,IParam);
  WWGC.processWeights(System,IParam);
  
  return;
}

int
processExitChecks(Simulation& System,const mainSystem::inputParam& IParam)
  /*!
    Check the validity of the simulation
    \param System :: Simuation object 
    \param IParam :: Inpute parameters
    \return error code for exit
  */
{
  ELog::RegMethod RegA("SimInput[F]","processExitChecks");

  int errFlag(0);
  if (IParam.flag("validCheck"))
    {
      ELog::EM<<"SIMVALID TRACK "<<ELog::endDiag;
      ELog::EM<<"-------------- "<<ELog::endDiag;
      ModelSupport::SimValid SValidCheck;

      if (IParam.flag("validPoint"))
	{
	  const Geometry::Vec3D CPoint=
	    IParam.getValue<Geometry::Vec3D>("validPoint");
	  if (!SValidCheck.runPoint(System,CPoint,
				    IParam.getValue<size_t>("validCheck")))
	    errFlag += -1;
	}
    }

  
  const size_t NLine = IParam.setCnt("validLine");
  for(size_t i=0;i<NLine;i++)
    {
      ELog::EM<<"Processing "<<i<<" / "<<NLine<<ELog::endDiag;
      size_t cnt(0);
      const Geometry::Vec3D C=
	IParam.getCntVec3D("validLine",i,cnt,"Start point");
      Geometry::Vec3D D=
	IParam.getCntVec3D("validLine",i,cnt,"End point/Direction");
      if (D.abs()>1.5 || D.abs()<0.5)
	D-=C;
      D.makeUnit();
      ELog::EM<<std::setprecision(12)<<"C == "<<C<<":"<<D<<ELog::endDiag;
      ModelSupport::LineTrack LT(C,D,1000.0);
      ModelSupport::LineTrack LTR(C,-D,1000.0);
      LT.calculate(System);
      LTR.calculate(System);
    }
  
	
  if (IParam.flag("cinder"))
    System.writeCinder();

  return errFlag;
}


void
inputProcessForSim(Simulation& System,
                const mainSystem::inputParam& IParam)
  /*!
    Check the validity of the simulation
    \param System :: Simuation object 
    \param IParam :: Inpute parameters
  */
{
  ELog::RegMethod RegA("SimInput[F]","inputPatterSim");
  
  if (IParam.flag("cinder"))
    System.setForCinder();
  
  // Cut energy tallies:
  if (IParam.flag("ECut"))
    System.setEnergy(IParam.getValue<double>("ECut"));

  if (IParam.flag("endf"))
    System.setENDF7();

  if (IParam.flag("ptrac"))
    processPTrack(IParam,System.getPC());

  if (IParam.flag("event"))
    processEvent("event",IParam,System.getPC());

  if (IParam.flag("dbcn"))
    processEvent("dbcn",IParam,System.getPC());
  
  return;
}

void
processPTrack(const mainSystem::inputParam& IParam,
              physicsSystem::PhysicsCards& PCard)
  /*!
    Process the input
    \param IParam :: Input deck
    \param PCard :: Physics card
   */
{
  ELog::RegMethod RegA("SimInput[F]","processPTrack");

  const size_t NItems=IParam.itemCnt("ptrac",0);
  if (NItems && IParam.getValue<std::string>("ptrac",0,0)=="help")
    {
      PCard.writeHelp("ptrac");
      return;
    }
  
  for(size_t index=1;index<NItems;index+=2)
    {
      const std::string key=IParam.getValue<std::string>("ptrac",0,index-1);
      const std::string Val=IParam.getValue<std::string>("ptrac",0,index);
      PCard.setPTRAC(key,Val);
    }
  PCard.setPTRACactive(1);
  return;
}

void
processEvent(const std::string& typeName,
             const mainSystem::inputParam& IParam,
             physicsSystem::PhysicsCards& PCard)
  /*!
    Process the input
    \param typeName :: name [event/dbcn]
    \param IParam :: Input deck
    \param PCard :: Physics card
  */
{
  ELog::RegMethod RegA("SimInput[F]","processEvent");

  const size_t NItems=IParam.itemCnt(typeName,0);
  if (NItems && IParam.getValue<std::string>(typeName,0,0)=="help")
    {
      PCard.writeHelp(typeName);
      return;
    }
  
  for(size_t index=1;index<NItems;index+=2)
    {
      const std::string key=IParam.getValue<std::string>(typeName,0,index-1);
      const std::string Val=IParam.getValue<std::string>(typeName,0,index);
      PCard.setDBCN(key,Val);
    }
  PCard.setDBCNactive(1);
  return;
}

  
}  // NAMESPACE SimProcess
