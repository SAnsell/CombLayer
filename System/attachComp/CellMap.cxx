/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/CellMap.cxx
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
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "SurInter.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "Line.h"
#include "Qhull.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "surfRegister.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "AttachSupport.h"
#include "ContainedComp.h"
#include "CellMap.h"

namespace attachSystem
{

CellMap::CellMap()
 /*!
    Constructor 
  */
{}

CellMap::CellMap(const CellMap& A) : 
  Cells(A.Cells)
  /*!
    Copy constructor
    \param A :: CellMap to copy
  */
{}

CellMap&
CellMap::operator=(const CellMap& A)
  /*!
    Assignment operator
    \param A :: CellMap to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Cells=A.Cells;
    }
  return *this;
}

void
CellMap::setCell(const std::string& Key,const int CN)
  /*!
    Insert a cell 
    \param Key :: Keyname
    \param CN :: Cell number
  */
{
  ELog::RegMethod RegA("CellMap","setCell");
  
  if (Key.empty() || Cells.find(Key)!=Cells.end())
    throw ColErr::InContainerError<std::string>(Key,"Key already present");

  Cells.insert(LCTYPE::value_type(Key,CN));
  return; 
}

void
CellMap::setCell(const std::string& Key,const size_t Index,
		 const int CN)
  /*!
    Insert a cell 
    \param Key :: Keyname
    \param Index :: Index number
    \param CN :: Cell number
  */
{
  ELog::RegMethod RegA("CellMap","setCell(s,i,i)");

  const std::string keyVal=Key+StrFunc::makeString(Index);
  if (Cells.find(keyVal)!=Cells.end())
    throw ColErr::InContainerError<std::string>(keyVal,"Key already present");

  Cells.insert(LCTYPE::value_type(keyVal,CN));
  return; 
}

  
int
CellMap::getCell(const std::string& Key) const
  /*!
    Get a cell number
    \param Key :: Keyname
    \return cell number
  */
{
  ELog::RegMethod RegA("CellMap","getCell");

  LCTYPE::const_iterator mc=Cells.find(Key);
  if (mc==Cells.end())
    throw ColErr::InContainerError<std::string>(Key,"Key not present");

  return mc->second;
}

int
CellMap::getCell(const std::string& Key,const size_t Index) const
  /*!
    Get a cell number
    \param Key :: Keyname
    \param Index :: Index number
    \return cell number
  */
{
  ELog::RegMethod RegA("CellMap","getCell(s,index)");

  const std::string keyVal=Key+StrFunc::makeString(Index);
  LCTYPE::const_iterator mc=Cells.find(keyVal);
  if (mc==Cells.end())
    throw ColErr::InContainerError<std::string>(Key,"Key not present");

  return mc->second;
}

void
CellMap::insertComponent(Simulation& System,
			  const std::string& Key,
			  const ContainedComp& CC) const
  /*!
    Insert a component into a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param CC :: Contained Componenet
   */
{
  ELog::RegMethod RegA("CellMap","insertComponent(CC)");
  if (CC.hasOuterSurf())
    insertComponent(System,Key,CC.getExclude());
  return;
}

void
CellMap::insertComponent(Simulation& System,
			  const std::string& Key,
			  const HeadRule& HR) const
  /*!
    Insert a component into a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param HR :: Contained Componenet
   */
{
  ELog::RegMethod RegA("CellMap","insertComponent(HR)");
  if (HR.hasRule())
    insertComponent(System,Key,HR.display());
  return;
}

void
CellMap::insertComponent(Simulation& System,
			 const std::string& Key,
			 const FixedComp& FC,
			 const long int sideIndex) const
/*!
    Insert a component into a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param FC :: FixedComp for link surface
   */
{
  ELog::RegMethod RegA("CellMap","insertComponent(FC)");

  if (sideIndex>0)
    {
      insertComponent
	(System,Key,FC.getLinkString(static_cast<size_t>(sideIndex-1)));
    }
  else if (sideIndex<0)
    {
      insertComponent
	(System,Key,FC.getLinkComplement(static_cast<size_t>(-sideIndex-1)));
    }
  else
    throw ColErr::InContainerError<long int>
      (0,"Zero line surface not define");
  return;
}

  
void
CellMap::insertComponent(Simulation& System,
			  const std::string& Key,
			  const std::string& exclude) const
  /*!
    Insert a component into a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param CC :: Contained Componenet
   */
{
  ELog::RegMethod RegA("CellMap","insertComponent");
    
  const int cellNum=getCell(Key);
  MonteCarlo::Qhull* outerObj=System.findQhull(cellNum);
  if (!outerObj)
    throw ColErr::InContainerError<int>(cellNum,"Cell["+Key+"] not present");
  
  outerObj->addSurfString(exclude);  
  return;
}

void
CellMap::deleteCell(Simulation& System,
		    const std::string& Key) 
  /*!
    Delete a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
  */
{
  ELog::RegMethod RegA("CellMap","deleteCell");

  LCTYPE::iterator mc=Cells.find(Key);
  if (mc==Cells.end())
    throw ColErr::InContainerError<std::string>(Key,"Key not present");

  System.removeCell(mc->second);
  Cells.erase(mc);
  return;
}
  

}  // NAMESPACE attachSystem
