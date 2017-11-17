/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/ELPTConstructor.cxx
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
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "MapRange.h"
#include "Triple.h"
#include "support.h"
#include "stringCombine.h"
#include "NList.h"
#include "NRange.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "Transform.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MainProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinkSupport.h"
#include "Simulation.h"
#include "inputParam.h"
#include "ModeCard.h"
#include "PhysImp.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "NList.h"
#include "NRange.h"
#include "PhysicsCards.h"
#include "ZoneUnit.h"
#include "ELPTConstructor.h" 


namespace physicsSystem
{

ELPTConstructor::ELPTConstructor()
  /// Constructor
{}


void
ELPTConstructor::processUnit(Simulation& System,
			    const mainSystem::inputParam& IParam,
			    const size_t Index) 
  /*!
    Add ext component 
    \param System :: Simulation to get physics/fixed points
    \param IParam :: Main input parameters
    \param Index :: index of the -wECut card
  */
{
  ELog::RegMethod RegA("ELPTConstructor","processUnit");
  double ECutValue(0.0);  /// ENERGY cut value;
  
  const size_t NParam=IParam.itemCnt("wECut",Index);
  if (NParam<1)
    throw ColErr::IndexError<size_t>(NParam,2,"Insufficient items wECut");

  std::string FStr=IParam.getValue<std::string>("wECut",Index,0);  
  if (FStr=="help" || FStr=="Help")
    {
      writeHelp(ELog::EM.Estream());
      ELog::EM<<ELog::endBasic;
      return;
    }
  const std::string particleStr=FStr;
  if (NParam<2)
    throw ColErr::IndexError<size_t>(NParam,3,"particle not give");
  FStr=IParam.getValue<std::string>("wECut",Index,1);  

  // Get all other values:
  std::vector<std::string> StrItem;
  for(size_t j=2;j<NParam;j++)
    StrItem.push_back
      (IParam.getValue<std::string>("wECut",Index,j));

  if (!StrFunc::section(FStr,ECutValue) ||
      !ZUnits.procZone(StrItem))
    throw ColErr::InvalidLine
      ("procZone ==> StrItems","-wECut "+IParam.getFull("wECut",Index),0);	
  ZUnits.addData(ECutValue);
  ZUnits.sortZone();

  physicsSystem::PhysicsCards& PC=System.getPC();
  physicsSystem::PhysImp& ECImp=PC.addPhysImp("elpt",particleStr);
  
  // care here : ECImp coule be a new particle value
  if (ECImp.isEmpty()) 
    {
      const std::vector<int> cellOrder=
	System.getCellVector();
      // Default is no cutting:
      ECImp.setCells(cellOrder,0.0);
    }      
  
  ECImp.updateCells(ZUnits);
  return;
}
  
void
ELPTConstructor::writeHelp(std::ostream& OX) const
  /*!
    Write out help
    \param OX :: Output stream
  */
{

    OX<<"-wECut particle ECut  \n"
      "ZONE :: \n"
      "   All / object [objectName] / Cells {numbers}(s) / [cellNumber] \n"
      " min-particle energy in cell\n";
  return;
}


}  // NAMESPACE physicsSystem
