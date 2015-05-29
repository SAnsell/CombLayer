/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/WeightModification.cxx
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
#include <cmath>
#include <complex> 
#include <list>
#include <vector>
#include <map> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
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
#include "Tensor.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Transform.h"
#include "Surface.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "weightManager.h"
#include "WForm.h"
#include "ModeCard.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "PhysImp.h"
#include "Source.h"
#include "KCode.h"
#include "PhysicsCards.h"
#include "Simulation.h"
#include "WeightModification.h"

namespace WeightSystem
{

void 
vecModReduction(Simulation& System,const int RA,const int RB)  
  /*!
    Reduce the top level moderator by a bit.
    \param System :: simulation to modify
    \param RA :: Start of number section [objects]
    \param RB :: End of number section [objects]
  */
{
  ELog::RegMethod RegA("WeightModification","vecModReduction");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();  

  WeightSystem::WForm* WF=WM.getParticle('n');

  ELog::EM<<"REMOVAL OF MODERATOR (Low energy) "<<ELog::endDebug;
  ELog::EM<<"Reduces the weight-windows "<<ELog::endDebug;
  
//  Weights& WR(System.getWeight());
  std::vector<int> CL=System.getCellInputVector();
  std::vector<int>::const_iterator vc;
  StrFunc::sliceVector(CL,RA,RB);
  ELog::EM<<"Top recuction between : "
	  <<CL.front()<<" : "<<CL.back()
	  <<" "<<CL.size()<<ELog::endDebug;

//  PhysImp& ProtonI=Sim.getPC().getPhysImp("h");
//  PhysImp& PhotonI=Sim.getPC().getPhysImp("p");
//  ProtonI.modifyCells(CL,0);
//  PhotonI.modifyCells(CL,0);

  for(vc=CL.begin();vc!=CL.end();vc++)
    {
      for(size_t i=0;i<5;i++)
	WF->setWeights((*vc),i,0.7);
    } 
  return;
}

void
addForcedCollision(Simulation& System,const double FCTemp)
  /*!
    Add a forced collition card [based on temperature]
    \param System :: simulation to modify
    \param FCTemp :: Temp below which fcl is set
  */
{
  std::vector<int> CL=System.getCellInputVector();
  std::vector<int> RL=System.getCellTempRange(0,FCTemp);
  std::vector<int>::const_iterator vc;
  physicsSystem::PhysImp& A=System.getPC().addPhysImp("fcl","n");
  A.setCells(CL,0);
  A.modifyCells(RL,1);
  return;
}

void
setNuclearZero(Simulation& System,const int RA,const int RB)
  /*!
    Sets importance for nuclear stuff for cells in target range
    target
    - Uses the 
    \param System :: Simulation to modify
    \param RA :: First cell to set to zero 
    \param RB :: Last cell to set to zero 
  */
{
  ELog::RegMethod RegA("WeightModification","setNuclearZero");

  std::vector<int> CL=System.getCellInputVector();
  StrFunc::sliceVector(CL,RA,RB);
  ELog::EM<<"Top :SetNulcearZero "<<CL.size()<<ELog::endDebug;
  CL.pop_back(); // removing first object 

  physicsSystem::PhysImp& PI=System.getPC().getPhysImp("imp","s");
  PI.setAllCells(1);
  PI.modifyCells(CL,0);
  return;
}

}  //NAMESPACE WeightSystem
