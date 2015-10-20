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

  const std::string XYZ[3]={"X","Y","Z"};
  size_t NXM[3];
  
  const size_t NItems=IParam.itemCnt("wwgMesh",Index);
  for(size_t index=0;index<3;index++)
    {
      NXM[index]=IParam.itemCnt("wwg"+XYZ[index]+"Mesh",Index);
      if (NXM<3)
	throw ColErr::IndexError<size_t>
	  (NItems,3,"Insufficient items for wwg"+XYZ[index]+"Mesh "
	   ": X_0 : N_0 : X1 : N1 ...");

      WWG WM
  


  WM.createWWG();
  
  if (NItems<3)
    throw ColErr::IndexError<size_t>
      (NItems,3,"Insufficient items for wwgMesh");

  
  const std::string PType(IParam.getValue<std::string>("tally",Index,1)); 
  
  const masterRotate& MR=masterRotate::Instance();
  std::string revStr;


  return;
}

}  // NAMESPACE shutterSystem
