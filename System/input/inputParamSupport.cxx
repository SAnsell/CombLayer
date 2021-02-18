/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   input/inputParamSupport.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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

#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "MapSupport.h"
#include "InputControl.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "IItemBase.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"


namespace mainSystem
{

std::vector<int>
getNamedCells(const Simulation& System,
	      const inputParam& IParam,
	      const std::string& keyItem,
	      const long int setIndex,
	      const long int index,
	      const std::string& errStr)
  /*!
    Calculate the objects based on a name e.g. a FixedComp.
    \param System :: Main simulation
    \param IParam :: Input parameters
    \param keyItem :: key Item to search fore
    \param setIndex :: input set index
    \param index :: item index
    \param errStr :: base of error string
  */  
{
  ELog::RegMethod RegA("inputParamSupport[F]","getNamedCells");

  const std::string objName=
    IParam.getValueError<std::string>
    (keyItem,setIndex,index,errStr+"[Object Name]");
  
  const std::vector<int> Cells=System.getObjectRange(objName);
  if (Cells.empty())
    throw ColErr::InContainerError<std::string>
      (objName,errStr+" [Empty cell]");

  return Cells;
}

std::set<MonteCarlo::Object*>
getNamedObjects(const Simulation& System,
		const inputParam& IParam,
		const std::string& keyItem,
		const long int setIndex,
		const long int index,
		const std::string& errStr)
  /*!
    Calculate the objects based on a name e.g. a FixedComp.
    \param System :: Main simulation
    \param IParam :: Input parameters
    \param keyItem :: key Item to search for
    \param setIndex :: input set index
    \param index :: item index
    \param errStr :: base of error string
  */  
{
  ELog::RegMethod RegA("inputParamSupport[F]","getNamedObjects");

  const std::vector<int> Cells=
    getNamedCells(System,IParam,keyItem,setIndex,index,errStr);

  std::set<MonteCarlo::Object*> outObjects;
    

  const Simulation::OTYPE& CellObjects=System.getCells();
  // Special to set cells in OBJECT  [REMOVE]
  for(const int CN : Cells)
    {
      Simulation::OTYPE::const_iterator mc=
	CellObjects.find(CN);
      if (mc!=CellObjects.end())
	outObjects.emplace(mc->second);
    }
  
  
  return outObjects;
}

} // NAMESPACE mainSystem
