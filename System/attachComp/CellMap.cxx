/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/CellMap.cxx
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
#include "BaseMap.h"
#include "CellMap.h"

namespace attachSystem
{

CellMap::CellMap() : BaseMap()
 /*!
    Constructor 
  */
{}

CellMap::CellMap(const CellMap& A) : 
  BaseMap(A)
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
      BaseMap::operator=(A);
    }
  return *this;
}
  
void
CellMap::insertComponent(Simulation& System,
			 const std::string& cutKey,
			 const CellMap& CM,
			 const std::string& holdKey) const
  /*!
    Insert a component into a cell
    \param System :: Simulation to obtain cell from
    \param cutKey :: Items in the Cell map to slicde
    \param CM :: Items that will cut this
    \param holdKey :: Items in the Cell map to be inserted
   */
{
  ELog::RegMethod RegA("CellMap","insertComponent(CellMap)");

  for(const int cn : CM.getCells(holdKey))
    {
      const MonteCarlo::Object* OPtr=
	System.findQhull(cn);
      if (OPtr)
	{
	  const HeadRule compObj=OPtr->getHeadRule().complement();
	  insertComponent(System,cutKey,compObj);	  
	}
    }
  return;
}

void
CellMap::insertComponent(Simulation& System,
			  const std::string& Key,
			  const ContainedComp& CC) const
  /*!
    Insert a component into a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param CC :: Contained Component ot insert 
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
			 const size_t index,
			 const ContainedComp& CC) const
  /*!
    Insert a component into a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param index :: index value
    \param CC :: Contained Component ot insert 
   */
{
  ELog::RegMethod RegA("CellMap","insertComponent(index,CC)");
  
  if (CC.hasOuterSurf())
    insertComponent(System,Key,index,CC.getExclude());

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
			 const std::string& exclude) const
  /*!
    Insert a component into a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param exclude :: Excluded key
   */
{
  ELog::RegMethod RegA("CellMap","insertComponent(string)");

  const std::vector<int> CVec=getCells(Key);
  if (CVec.empty())
    throw ColErr::InContainerError<std::string>
      (Key,"Cell["+Key+"] not present");

  for(const int cellNum : CVec)
    {
      MonteCarlo::Qhull* outerObj=System.findQhull(cellNum);
      if (!outerObj)
	throw ColErr::InContainerError<int>(cellNum,
					    "Cell["+Key+"] not in simlutation");
      outerObj->addSurfString(exclude);
    }
  return;
}

void
CellMap::insertComponent(Simulation& System,
			 const std::string& Key,
			 const size_t index,
			 const std::string& exclude) const
  /*!
    Insert a component into a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param index :: Index to use
    \param exclude :: Excluded key
   */
{
  ELog::RegMethod RegA("CellMap","insertComponent(index,string)");

  const int cellNum=getCell(Key,index);

  MonteCarlo::Qhull* outerObj=System.findQhull(cellNum);
  if (!outerObj)
    throw ColErr::InContainerError<int>(cellNum,
					"Cell["+Key+"] not present");
  outerObj->addSurfString(exclude);
  return;
}

void
CellMap::insertComponent(Simulation& System,
			 const std::string& Key,
			 const FixedComp& FC,
			 const long int sideIndex) const
  /*!
    Insert an exclude component into a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param FC :: FixedComp for link surface
    \param sideIndex :: signed direction side
   */
{
  ELog::RegMethod RegA("CellMap","insertComponent(FC,index)");

  if (!sideIndex)
    throw ColErr::InContainerError<long int>
      (0,"Zero line surface not defined for : "+FC.getKeyName());

  insertComponent(System,Key,FC.getLinkString(sideIndex));
  return;
}

void
CellMap::makeCell(const std::string& Key,Simulation& System,
		  const int cellIndex,const int matNumber,
		  const double matTemp,const std::string& Out)

  /*!
    Builds a new cell in Simulation and registers it with the CellMap
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param cellIndex :: Cell index
    \param matNumber :: Material number
    \param matTemp :: Temperature
    \param Out :: Boolean surface string
  */
{
  ELog::RegMethod RegA("CellMap","makeCell");
  System.addCell(cellIndex,matNumber,matTemp,Out);
  addCell(Key,cellIndex);
  return;
}
void
CellMap::deleteCell(Simulation& System,
		    const std::string& Key,
		    const size_t Index) 
  /*!
    Delete a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param Index :: Cell index
  */
{
  ELog::RegMethod RegA("CellMap","deleteCell");

  const int CN=BaseMap::removeItem(Key,Index);

  if (!CN)
    throw ColErr::InContainerError<int>(CN,"Key["+Key+"] zero cell");
  
  System.removeCell(CN);
  return;
}

std::pair<int,double>
CellMap::deleteCellWithData(Simulation& System,
			    const std::string& Key,
			    const size_t Index) 
/*!
    Delete a cell and then return material  
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param Index :: Cell index
    \return materialNumber : temperature
  */
{
  ELog::RegMethod RegA("CellMap","deleteCell");

  const int CN=BaseMap::removeItem(Key,Index);

  if (!CN)
    throw ColErr::InContainerError<int>(CN,"Key["+Key+"] zero cell");
  const MonteCarlo::Object* ObjPtr=System.findQhull(CN);
  if (!ObjPtr)
    throw ColErr::InContainerError<int>(CN,"Cell Ptr zero");

  std::pair<int,double> Out(ObjPtr->getMat(),ObjPtr->getTemp());
  System.removeCell(CN);  // too complex to handle from ObjPtr
  return Out;
}
 
}  // NAMESPACE attachSystem
