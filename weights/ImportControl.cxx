/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   weights/ImportControl.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

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
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "KCode.h"
#include "LSwitchCard.h"
#include "ModeCard.h"
#include "PhysImp.h"
#include "PhysCard.h"
#include "PhysicsCards.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "objectRegister.h"
#include "inputParam.h"
#include "ImportControl.h"

namespace WeightSystem
{

void
zeroImp(Simulation& System,const int initCell,
	const int cellRange)
  /*!
    Zero cell importances in a range
    \param System :: Simulation 
    \param initCell :: inital cell to searcha
   */
{
  ELog::RegMethod RegA("ImportControl[F]","zeroImp");

  physicsSystem::PhysicsCards& PC=System.getPC();  
  for(int cellNum=initCell;cellNum<initCell+cellRange;cellNum++)
    {
      MonteCarlo::Qhull* Obj=
	System.findQhull(cellNum);
      if (Obj)
	{
	  PC.setCells("imp",cellNum,0); 
	  Obj->setImp(0);
	  Obj->setMaterial(0);
	}
    }
  return;
}


void
simulationImp(Simulation& System,
	      const mainSystem::inputParam& IParam)
  /*!
    Control importances in individual cells
    \param System :: Simulation
    \param IParam :: input stream
   */
{
  ELog::RegMethod RegA("ImportControl","simulationImport");

  
  // WEIGHTS:
  if (IParam.flag("imp") )
    {
      const std::string type=IParam.getValue<std::string>("imp",0);
      const std::string bStr=IParam.getValue<std::string>("imp",1);

      const ModelSupport::objectRegister& OR=
	ModelSupport::objectRegister::Instance();

      if (type=="void")
	{
	  int cellNum=OR.getCell("void")+1;	  
	  const int cellRange=OR.getRange("void");	      
	  zeroImp(System,cellNum,cellRange);
	}

      int bNum(0);
      if (type=="beamline" && StrFunc::convert(bStr,bNum))
	{
	  bNum--;
	  if (bNum<0 || bNum>17)
	    throw ColErr::RangeError<int>(bNum+1,1,18,"bNumber");

	  std::list<std::string> BlockName;
	  BlockName.push_back("torpedo");
	  BlockName.push_back("shutter");
	  BlockName.push_back("bulkInsert");

	  for(int i=0;i<18;i++)
	    {
	      if (i!=bNum)
		{
		  std::list<std::string>::const_iterator vc;
		  for(vc=BlockName.begin();vc!=BlockName.end();vc++)
		    {
		      // Zero everything in catorgoies:
		      int cellNum=OR.getCell(*vc,i)+1;
		      const int cellRange=OR.getRange(*vc,i);
		      zeroImp(System,cellNum,cellRange);
		    }
		}
	    }
	}
    }
  return;
}

}  // NAMESPACE weightSytem
