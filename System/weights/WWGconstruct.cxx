/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WWGconstruct.cxx
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
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
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
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "weightManager.h"
#include "Simulation.h"
#include "objectRegister.h"
#include "inputParam.h"
#include "WeightMesh.h"
#include "LineTrack.h"
#include "ObjectTrackAct.h"
#include "WWGWeight.h"
#include "WWG.h"
#include "WWGconstruct.h"

namespace WeightSystem
{

WWGconstruct::WWGconstruct()
  /*!
    Constructor
  */
{}





  
void
WWGconstruct::createWWG(Simulation& System,
			const mainSystem::inputParam& IParam)
  /*!
    Set WWG weights based 
    \param System :: Simulation
    \param IParam :: input stream
   */
{
  ELog::RegMethod RegA("WWGconstruct","createWWG");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();
 


  WWG& wwg=WM.getWWG();
  //  const size_t NItems=IParam.itemCnt("wwgMesh",0);

  std::vector<std::vector<double>> boundaryVal(3);
  std::vector<std::vector<size_t>> bCnt(3);
      
  for(size_t index=0;index<3;index++)
    {
      const std::string itemName("wwg"+XYZ[index]+"Mesh");
      const size_t NXM=IParam.itemCnt(itemName,0);

      if (NXM<3 || !(NXM % 2) )
	throw ColErr::IndexError<size_t>
	  (NXM,3,"Insufficient items for "+itemName+
	   ": X_0 : N_0 : X1 : N1 ...");

      for(size_t i=0;i<NXM;i++)
	{
	  if (i % 2)   // Odd : Integer
	    bCnt[index].push_back
	      (IParam.getValue<size_t>(itemName,i));
	  else
	    boundaryVal[index].push_back
	      (IParam.getValue<double>(itemName,i));
	}
    }

  wwg.getGrid().setMesh(boundaryVal[0],bCnt[0],
			boundaryVal[1],bCnt[1],
			boundaryVal[2],bCnt[2]);

  // ENERGY BOUNDARY
  if (IParam.flag("wwgE"))
    {
      std::vector<double> EBin;
      const size_t ECnt=IParam.itemCnt("wwgE",0);

      for(size_t i=0;i<ECnt;i++)
	EBin.push_back
	  (IParam.getValue<double>("wwgE",i));
      if (EBin.back()<1e5)
	EBin.push_back(1e5);
      wwg.setEnergyBin(EBin);
    }
  
  
  // if (NItems<3)
  //   throw ColErr::IndexError<size_t>
  //     (NItems,3,"Insufficient items for wwgMesh");

 

  return;
}

}  // NAMESPACE WeightSystem
