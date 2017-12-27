/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weight/FCLConstructor.cxx
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
#include <list>
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "support.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Object.h"
#include "Qhull.h"
#include "weightManager.h"
#include "ModeCard.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "PhysImp.h"
#include "PhysicsCards.h"
#include "Simulation.h"
#include "objectRegister.h"
#include "inputParam.h"
#include "FCLConstructor.h"

namespace physicsSystem
{


void
FCLConstructor::init(Simulation& System)
  /*!
    Initialize the FCL card
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("FCLConstructor","init");
  
  physicsSystem::PhysicsCards& PC=System.getPC();
  physicsSystem::PhysImp& FLCImp=PC.addPhysImp("fcl","n");
  if (FLCImp.isEmpty()) 
    {
      const std::vector<int> cellOrder=
	System.getCellVector();
      // Default is no cutting:
      FLCImp.setCells(cellOrder,0.0);
    }
  return;
}

  
void
FCLConstructor::processUnit(Simulation& System,
                            const mainSystem::inputParam& IParam,
                            const size_t index)
  /*!
    Set individual FCL based on Iparam
    \param System :: Simulation
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("FCLConstructor","processUnit");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
    
  physicsSystem::PhysicsCards& PC=System.getPC();
  
  const double value=IParam.getValueError<double>
    ("wFCL",index,0,"No fractional for wFCL");
  const std::string key=IParam.getValueError<std::string>
    ("wFCL",index,1,"No keyname for wFCL");
  if (std::abs<double>(value)>1.0+Geometry::zeroTol)
    throw ColErr::RangeError<double>(value,-1.0,1.0,"Value for FCL");
  if (key=="Object" || key=="object")
    {
      const std::string objName=
        IParam.getValueError<std::string>
        ("wFCL",index,2,"No objName for wFCL");
      const std::vector<int> Cells=
        OR.getObjectRange(objName);
      if (Cells.empty())
        throw ColErr::InContainerError<std::string>
          (objName,"Empty cell");
      
      for(const int CN : Cells)
        PC.setCells("fcl","n",CN,value);
    }
  return;
}


  
} // Namespace physicsSystem
