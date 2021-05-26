/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/SimImportance.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "SimPHITS.h"
#include "SimFLUKA.h"
#include "MainProcess.h"
#include "flukaDefPhysics.h"
#include "phitsDefPhysics.h"
#include "SetMagnets.h"
#include "BasicMesh3D.h"
#include "ImportControl.h"
#include "WeightControl.h"
#include "WCellControl.h"
#include "WWGControl.h"


namespace SimProcess
{

void
importanceSim(SimFLUKA& System,
	      const mainSystem::inputParam& IParam)
/*!
    Apply importances/renumber and weights
    \param System :: Simuation object 
    \param IParam :: Input parameters
   */
{
  ELog::RegMethod RegA("SimImportance[F]","importanceSim(FLUKA)");

  System.populateCells();
  System.createObjSurfMap();
  
  WeightSystem::WCellControl WCell;
  WeightSystem::WWGControl WWGC;
  WCell.processWeights(System,IParam);
  WWGC.processWeights(System,IParam);
  
  mainSystem::renumberCells(System,IParam);
  flukaSystem::setModelPhysics(System,IParam);
  magnetSystem::setMagneticPhysics(System,IParam);
  return;
}

void
importanceSim(SimPHITS& System,
	      const mainSystem::inputParam& IParam)
/*!
    Apply importances/renumber and weights
    \param System :: Simuation object 
    \param IParam :: Input parameters
   */
{
  ELog::RegMethod RegA("SimImportance[F]","importanceSim(PHITS)");

  System.populateCells();
  System.createObjSurfMap();

  WeightSystem::WCellControl WCell;
  WeightSystem::WWGControl WWGC;
  WCell.processWeights(System,IParam);
  WWGC.processWeights(System,IParam);
  ELog::EM<<"Pass Process"<<ELog::endDiag;
  phitsSystem::setModelPhysics(System,IParam);
  magnetSystem::setMagneticPhysics(System,IParam);

  mainSystem::renumberCells(System,IParam);
  return;
}  

void
importanceSim(SimMCNP& System,
	      const mainSystem::inputParam& IParam)
  /*!
    Apply importances/renumber and weights
    \param System :: Simuation object 
    \param IParam :: Input parameters
   */
{
  ELog::RegMethod RegA("SimImportance[F]","importanceSim(MCNP)");

  System.populateCells();
  System.createObjSurfMap();

  WeightSystem::simulationImp(System,IParam);

  WeightSystem::ExtField(System,IParam);
  WeightSystem::FCL(System,IParam);
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



}  // NAMESPACE SimProcess
