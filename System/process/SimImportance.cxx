/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/SimImportance.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>
#include <array>

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
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "Rules.h"
#include "Code.h"
#include "FItem.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "ModeCard.h"
#include "PhysCard.h"
#include "PhysImp.h"
#include "LSwitchCard.h"
#include "PhysicsCards.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "SimPHITS.h"
#include "SimFLUKA.h"
#include "MainProcess.h"
#include "flukaProcess.h"
#include "flukaDefPhysics.h"
#include "phitsDefPhysics.h"
#include "ImportControl.h"
#include "WeightControl.h"
#include "WCellControl.h"
#include "WWGControl.h"
#include "SimImportance.h"

namespace SimProcess
{

void
importanceSim(Simulation& System,
	      const mainSystem::inputParam& IParam)
  /*!
    Apply importances/renumber and weights
    \param System :: Simuation object 
    \param IParam :: Input parameters
   */
{
  ELog::RegMethod RegA("SimImportance[F]","importanceSim");

  System.populateCells();
  System.createObjSurfMap();

  SimMCNP* mcnpPtr=dynamic_cast<SimMCNP*>(&System);
  if (mcnpPtr)
    {
      importanceSim(*mcnpPtr,IParam);
      return;
    }
  
  SimFLUKA* flukaPtr=dynamic_cast<SimFLUKA*>(&System);
  if (flukaPtr)
    {
      WeightSystem::WCellControl WCell;
      WeightSystem::WWGControl WWGC;
      WCell.processWeights(System,IParam);
      WWGC.processWeights(System,IParam);
      
      mainSystem::renumberCells(*flukaPtr,IParam);
      flukaSystem::setModelPhysics(*flukaPtr,IParam);
      return;
    }
  SimPHITS* phitsPtr=dynamic_cast<SimPHITS*>(&System);
  if (phitsPtr)
    {
      WeightSystem::WCellControl WCell;
      WCell.processWeights(System,IParam);
      
      mainSystem::renumberCells(*phitsPtr,IParam);
      phitsSystem::setModelPhysics(*phitsPtr,IParam);
      return;
    }
  
  
  ELog::EM<<"Unknown Sim for importance sampling"<<ELog::endDiag;
  return;
  
}

void
importanceSim(SimMCNP& System,const mainSystem::inputParam& IParam)
  /*!
    Apply importances/renumber and weights
    \param System :: Simuation object 
    \param IParam :: Input parameters
   */
{
  ELog::RegMethod RegA("SimImportance[F]","importanceSim(MCNP)");

  physicsSystem::PhysicsCards& PC=System.getPC();      
  WeightSystem::simulationImp(PC,System,IParam);

  WeightSystem::ExtField(PC,IParam);
  WeightSystem::FCL(PC,System,IParam);
  WeightSystem::IMP(PC,System,IParam);
  WeightSystem::DXT(PC,IParam);
  WeightSystem::PWT(PC,IParam);
  WeightSystem::EnergyCellCut(PC,System,IParam);

  mainSystem::renumberCells(System,IParam);

  WeightSystem::WCellControl WCell;
  WeightSystem::WWGControl WWGC;
  WCell.processWeights(System,IParam);
  WWGC.processWeights(System,IParam);
  
  return;
}



}  // NAMESPACE SimImportance
