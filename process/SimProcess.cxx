/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/SimProcess.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <string>
#include <algorithm>
#include <iterator>
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
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "Rules.h"
#include "Code.h"
#include "FItem.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "ModeCard.h"
#include "PhysCard.h"
#include "PhysImp.h"
#include "LSwitchCard.h"
#include "KGroup.h"
#include "Source.h"
#include "KCode.h"
#include "PhysicsCards.h"
#include "Simulation.h"
#include "SimProcess.h"

namespace SimProcess
{

void
writeMany(Simulation& System,const std::string& FName,const int Number)
   /*!
     Writes out many different files, each with a new random
     number
     \param System :: Simuation object 
     \param FName :: basic filename
     \param Number :: number to write
   */
{
  physicsSystem::PhysicsCards& PC=System.getPC();
  for(int i=0;i<Number;i++)
    {
      std::ostringstream cx;
      cx<<FName<<i+1<<".x";
      System.write(cx.str());
      // increase the RND seed by 10
      PC.setRND(PC.getRND()+10);
    }
  return;
}


void
writeIndexSim(Simulation& System,const std::string& FName,const int Number)
   /*!
     Writes out many different files, each with a new random
     number
     \param System :: Simuation object 
     \param FName :: basic filename
     \param Number :: number to write
   */
{
  physicsSystem::PhysicsCards& PC=System.getPC();
  PC.setRND(PC.getRND()+Number*10);
  std::ostringstream cx;
  cx<<FName<<Number+1<<".x";
  System.prepareWrite();
  System.write(cx.str());
  // increase the RND seed by 10
  
  return;
}
  
void
writeIndexSimPHITS(Simulation& System,const std::string& FName,const int Number)
  /*!
    Writes out many different files, each with a new random
    number
    \param System :: Simuation object 
    \param FName :: basic filename
    \param Number :: number to write
  */
{
  physicsSystem::PhysicsCards& PC=System.getPC();
  PC.setRND(PC.getRND()+Number*10);
  std::ostringstream cx;
  cx<<FName<<Number+1<<".x";
  System.write(cx.str());
  // increase the RND seed by 10
  
  return;
}

template<typename T,typename U>
T
getIndexVar(const FuncDataBase& Control,
	    const std::string& FName,
	    const std::string& BName,
	    const U& index)
/*!
  Get an value item based on the FName+BName 
  with interal index
  \param Control :: Control name
  \param FName :: Forward name
  \param BName :: Back name
  \param index :: index value
  \return Value
*/
 {
   ELog::RegMethod RegA("SimProcess","getIndexVar");
   std::ostringstream cx;

   cx<<FName<<index<<BName;
   if (Control.hasVariable(cx.str()))
    return Control.EvalVar<T>(cx.str());
  // rely on this to throw
  return Control.EvalVar<T>(FName+BName);
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

template<typename T>
int
getIndexVec(const FuncDataBase& Control,
	    const std::string& FName,
	    const int index,
	    const std::string& BName,
	    const std::vector<T>& outVec)
  /*!
    Get an item based on the FName+BName 
    with interal index. It adds a default value to obtain
    \tparam T :: Type of variable (int/double etc)
    \param Control :: Control name
    \param FName :: Forward name
    \param index :: index value
    \param BName :: Base name
    \param outVec :: Output vector
    \return Number of objects found
  */
{
  ELog::RegMethod RegA("SimProcess","getIndexVec");
  std::ostringstream cx;
  std::ostringstream dx;
  int sndIndex(0);
  int valid(0);
  do
    {
      valid=0;
      cx.str("");
      dx.str("");
      cx<<FName<<index<<BName<<sndIndex;
      dx<<FName<<BName<<sndIndex;
      if (Control.hasVariable(cx.str()))
	{
	  outVec.push_back(Control.EvalVar<T>(cx.str()));
	  valid=1;
	}
      else if (Control.hasVariable(dx.str()))
	{
	  outVec.push_back(Control.EvalVar<T>(dx.str()));
	  valid=1;
	}
      sndIndex++;
    } while(valid);

  return sndIndex-1;
}


template<typename T>
T
getDefVar(const FuncDataBase& Control,
	  const std::string& VName,
	  const T& defVal)
  /*!
    Get a general variable
    \tparam T :: Type of variable (int/double etc)
    \param Control :: Control name
    \param VName :: Variable name
    \param defVal :: default value to return if no variable.
    \return Value found/ defVal
  */
{
  ELog::RegMethod RegA("SimProcess","getDefVar");
  return (Control.hasVariable(VName)) ?
    Control.EvalVar<T>(VName) : defVal;
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

  MonteCarlo::Qhull* Cptr=System.findQhull(cellNum);
  if (!Cptr)
    throw ColErr::InContainerError<int>(cellNum,RegA.getBase());

  std::ostringstream cx;
  cx<<" #"<<vNum;
  Cptr->addSurfString(cx.str());
  return;
}

template<typename T>
std::vector<T>
getVarVec(const FuncDataBase& Control,
	  const std::string& VName)
  /*!
    Get a general variable based on VName[index]
    \tparam T :: Type of variable (int/double/vec3d etc)
    \param Control :: Control name
    \param VName :: Variable name
    \return vector of components
  */
{
  ELog::RegMethod RegA("SimProcess","getVarVec");

  std::ostringstream cx;
  std::vector<T> Out;

  int index(0);
  cx<<VName<<index;
  while(Control.hasVariable(cx.str()))
    {
      Out.push_back(Control.EvalVar<T>(cx.str()));
      index++;
      cx.str("");
      cx<<VName<<index;
    }
  return Out;
}


///\cond TEMPLATE

template 
double
getIndexVar(const FuncDataBase&,const std::string&,
	    const std::string&,const int&);
template 
int
getIndexVar(const FuncDataBase&,const std::string&,
	    const std::string&,const int&);
template 
size_t
getIndexVar(const FuncDataBase&,const std::string&,
	    const std::string&,const int&);
template 
double
getIndexVar(const FuncDataBase&,const std::string&,
	    const std::string&,const size_t&);
template 
int
getIndexVar(const FuncDataBase&,const std::string&,
	    const std::string&,const size_t&);
template 
size_t
getIndexVar(const FuncDataBase&,const std::string&,
	    const std::string&,const size_t&);

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


template 
int
getDefVar(const FuncDataBase&,const std::string&,const int&);

template 
double
getDefVar(const FuncDataBase&,const std::string&,const double&);

template
std::vector<Geometry::Vec3D>
getVarVec(const FuncDataBase&,const std::string&);

template
std::vector<double>
getVarVec(const FuncDataBase&,const std::string&);

///\endcond TEMPLATE  


}  // NAMESPACE SimProcess
