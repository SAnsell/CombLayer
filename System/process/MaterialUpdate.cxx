/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/MaterialUpdate.cxx
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
#include "support.h"
#include "surfRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MainProcess.h"
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "objectRegister.h"
#include "Object.h"

#include "MaterialUpdate.h"

namespace ModelSupport
{

void
materialUpdateHelp()
  /*!
    Write out the material update help
   */
{
  ELog::EM<<"voidObject Help "<<ELog::endBasic;
  ELog::EM<<
    " -- FixedComp :: voids all cells in FixedComp \n "
    " -- FixedName::CellMap :: voids all CellMap Name \n"
    " -- FixedName::CellMap::<Index> ::  "
    "  voids all CellMap Name at index \n"
	  <<ELog::endBasic;
  ELog::EM<<ELog::endErr;
  return;
}

void
materialUpdate(Simulation& System,
	       const mainSystem::inputParam& IParam)
  /*
    Sets a component to all void if set
    \param System :: Simulation to get objects from
    \param IParam :: Parameters
  */
{
  ELog::RegMethod RegA("MaterialUpdate[F]","materialUpdate");

  const size_t nP=IParam.setCnt("voidObject");
  for(size_t index=0;index<nP;index++)
    {

      const size_t nItem=IParam.itemCnt("voidObject",index);

      for(size_t iName=0;iName<nItem;iName++)
	{
	  const std::string objName=
	    IParam.getValue<std::string>("voidObject",index,iName);
      
	  if (!iName && objName=="help")
	    {
	      materialUpdateHelp();
	      return;
	    }
	  const std::vector<int> cellN=System.getObjectRange(objName);
		
	  for(const int CN : cellN)
	    {
	      MonteCarlo::Object* OPtr=System.findObject(CN);
	      if (OPtr)
		OPtr->setMaterial(0);
	    }
	}
    }  
  return;
}

}  // NAMESPACE ModelSupport
