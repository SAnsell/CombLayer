/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weight/IMPConstructor.cxx
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
#include "IMPConstructor.h"

namespace physicsSystem
{

  
void
IMPConstructor::processUnit(Simulation& System,
                            const mainSystem::inputParam& IParam,
                            const size_t setIndex)
  /*!
    Set individual IMP based on Iparam
    \param System :: Simulation
    \param IParam :: input stream
    \param setIndex :: index for the importance set
  */
{
  ELog::RegMethod RegA("IMPConstructor","processUnit");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  physicsSystem::PhysicsCards& PC=System.getPC();

  
  
  int value;
  std::string particle=IParam.getValueError<std::string>
    ("wIMP",setIndex,0,"No particle for wIMP ");

  if (particle=="help" || particle=="Help")
    {
      writeHelp(ELog::EM.Estream());
      ELog::EM<<ELog::endBasic;
      return;
    }
  
  size_t index(1);
  if (StrFunc::convert(particle,value))
    {
      particle="";
    }
  else
    {
      value=IParam.getValueError<int>
	("wIMP",setIndex,index,"No value for wIMP");
      index++;
    }

  const std::string objName=
    IParam.getValueError<std::string>
    ("wIMP",setIndex,index,"No objName for wIMP");

  const std::vector<int> Cells=
    OR.getObjectRange(objName);
  if (Cells.empty())
    throw ColErr::InContainerError<std::string>
      (objName,"Empty cell");

  if (particle.empty())
    {
      Simulation::OTYPE& CellObjects=System.getCells();
      // Special to set cells in OBJECT
      for(const int CN : Cells)
        {
          PC.setCells("imp",CN,value);
          Simulation::OTYPE::iterator mc=
            CellObjects.find(CN);
          if (mc!=CellObjects.end())
            mc->second->setImp(value);
        }
    }
  else
    for(const int CN : Cells)
      PC.setCells("imp",particle,CN,value);
      
  return;
}

void
IMPConstructor::writeHelp(std::ostream& OX) const
  /*!
    Write out the help
  */
{
  OX<<"wIMP help :: \n";

  OX<<"-wIMP particle value[integer] object/range/cell  -- ::\n\n";
  
  OX<<"  particle : optional [default all]\n";
  OX<<"  value : value for importance \n";
  OX<<"  object : object name  \n";
  OX<<"         : object name:cellname\n";
  OX<<"         : cell number range\n";
  OX<<"         : cell number\n";
  return;
}

  
} // Namespace physicsSystem
