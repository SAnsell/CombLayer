/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/SimProcess.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "ModeCard.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "PhysicsCards.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "SimPHITS.h"
#include "SimFLUKA.h"

namespace SimProcess
{
  
void
writeMany(SimMCNP& System,const std::string& OName,const int Number)
   /*!
     Writes out many different files, each with a new random
     number
     \param System :: Simuation object 
     \param OName :: basic filename
     \param Number :: number to write
   */
{
  physicsSystem::PhysicsCards& PC=System.getPC();
  for(int i=0;i<Number;i++)
    {
      System.write(OName+std::to_string(i+1)+".x");
      // increase the RND seed by 10
      PC.setRND(PC.getRNDseed()+10);
    }
  return;
}


void
writeIndexSim(SimMCNP& System,
	      const std::string& OName,
	      const int Number)
  /*!
    Writes out many different files, each with a new random
    number
    \param System :: Simuation object 
    \param FName :: basic filename
    \param Number :: number to write
  */
{
  ELog::RegMethod RegA("SimProcess[F]","writeIndexSim");
  
  physicsSystem::PhysicsCards& PC=System.getPC();
  // increase the RND seed by N*10 [10,20,40,etc]
  PC.setRND(PC.getRNDseed()+Number*10);
  System.prepareWrite();
  System.makeObjectsDNForCNF();
  System.write(OName+std::to_string(Number+1)+".x");
  
  return;
}

void
writeIndexSimFLUKA(SimFLUKA& System,
		   const std::string& OName,
		   const int Number)
   /*!
     Writes out many different files, each with a new random
     number
     \param System :: Simuation object 
     \param OName :: basic filename
     \param Number :: number to write
   */
{
  ELog::RegMethod RegA("SimProcess[F]","writeIndexSimFLUKA");

  System.prepareWrite();
  System.setRND(System.getRNDseed()+Number*11);
  System.write(OName+std::to_string(Number+1)+".inp");
  
  return;
}
  
void
writeIndexSimPHITS(SimPHITS& System,const std::string& FName,
		   const int Number)
  /*!
    Writes out many different files, each with a new random
    number
    \param System :: Simuation object 
    \param FName :: basic filename
    \param Number :: number to write
  */
{
  //  physicsSystem::PhysicsCards& PC=System.getPC();
  //  PC.setRND(PC.getRNDseed()+Number*10);
  std::string fileName=FName+std::to_string(Number+1);
  System.setFileName(fileName);
  System.write(fileName+".phs");
  // increase the RND seed by 10
  
  return;
}


template<typename T,typename U>
T
getDefIndexVar(const FuncDataBase& Control,
	       const std::string& FName,
	       const std::string& BName,
	       const U& index,
	       const T& defVal)
  /*!
    Get an item based on the FName+[index]+BName 
    with interal index. It adds a default value to obtain
    \tparam T :: Type of variable (int/double etc)
    \param Control :: Control name
    \param FName :: Forward name
    \param BName :: Back name
    \param index :: index value
    \param defVal :: default value to return if no variable.
    \return Value found/ defVal
  */
{
  ELog::RegMethod RegA("SimProcess","getDefIndexVar");
  std::ostringstream cx;
  cx<<FName<<index<<BName;
  if (Control.hasVariable(cx.str()))
    return Control.EvalVar<T>(cx.str());
  if (Control.hasVariable(FName+BName))
    return Control.EvalVar<T>(FName+BName);
  return defVal;
}




void
registerOuter(Simulation& System,const int cellNum,const int vNum)
  /*!
    Intermediate funnction (to be moved/removed)
    to add a virtual cell to the cell object
    \param System :: Simulation to use
    \param cellNum :: cell number
    \param vNum :: virtual number
  */
{  
  ELog::RegMethod RegA("SimProcess","registerOuter");

  MonteCarlo::Object* Cptr=System.findObjectThrow(cellNum);

  std::ostringstream cx;
  cx<<" #"<<vNum;
  Cptr->addSurfString(cx.str());
  return;
}


///\cond TEMPLATE


template 
int
getDefIndexVar(const FuncDataBase&,const std::string&,
	       const std::string&,const int&,const int&);
template 
double
getDefIndexVar(const FuncDataBase&,const std::string&,
	       const std::string&,const int&,const double&);
template 
int
getDefIndexVar(const FuncDataBase&,const std::string&,
	       const std::string&,const size_t&,const int&);
template 
double
getDefIndexVar(const FuncDataBase&,const std::string&,
	       const std::string&,const size_t&,const double&);


// template 
// int
// getDefVar(const FuncDataBase&,const std::string&,const int&);

// template 
// double
// getDefVar(const FuncDataBase&,const std::string&,const double&);


}  // NAMESPACE SimProcess
