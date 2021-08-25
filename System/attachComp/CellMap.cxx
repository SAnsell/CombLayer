/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/CellMap.cxx
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
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "surfRegister.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
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
CellMap::insertCellMapInCell(Simulation& System,
			     const std::string& cellKey,
			     const int cellN) const
  /*!
    Insert a cellMap object into a cell
    \param System :: Simulation to obtain cell from
    \param cellKey :: Items in the Cell map to slice
    \param cellN :: System cell number to change
   */
{
  ELog::RegMethod RegA("CellMap","insertCellMapInCell(int)");

  MonteCarlo::Object* CPtr=System.findObject(cellN);
  if (!CPtr)
    throw ColErr::InContainerError<int>(cellN,"cellN in System");

  for(const int cn : getCells(cellKey))
    {
      const MonteCarlo::Object* OPtr=System.findObject(cn);
      if (OPtr)
	{
	  const HeadRule compObj=OPtr->getHeadRule().complement();
	  CPtr->addSurfString(compObj.display());
	}
    }  
  return;
}

void
CellMap::insertCellMapInCell(Simulation& System,
			     const std::string& cellKey,
			     const size_t cellIndex,
			     const int cellN) const
  /*!
    Insert a cellMap object into a cell
    \param System :: Simulation to obtain cell from
    \param cutKey :: Items in the Cell map to slice
    \param cellIndex :: Item number from the cell map
    \param cellN :: System cell number to change
   */
{
  ELog::RegMethod RegA("CellMap","insertCellMapInCell(int)");

  const int cn = getCell(cellKey,cellIndex);
  const MonteCarlo::Object* OPtr=System.findObject(cn);
  if (!OPtr)
    throw ColErr::InContainerError<int>(cn,"CellMap(int) in System");
  const HeadRule compObj=OPtr->getHeadRule().complement();
  
  MonteCarlo::Object* CPtr=System.findObject(cellN);
  if (!CPtr)
    throw ColErr::InContainerError<int>(cellN,"cellN in System");

  CPtr->addSurfString(compObj.display());
  return;
}
  
void
CellMap::insertComponent(Simulation& System,
			 const std::string& cutKey,
			 const CellMap& CM,
			 const std::string& holdKey) const
  /*!
    Insert a component into a cell
    \param System :: Simulation to obtain cell from
    \param cutKey :: Items in the Cell map to slice
    \param CM :: Items that will cut this cellMap
    \param holdKey :: Items in the Cell map to used to insert.
   */
{
  ELog::RegMethod RegA("CellMap","insertComponent(CellMap)");

  for(const int cn : CM.getCells(holdKey))
    {
      const MonteCarlo::Object* OPtr=
	System.findObject(cn);
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
    \param CC :: Contained Component to insert 
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
			  const ContainedGroup& CG) const
  /*!
    Insert a component into a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param CG :: Contained Component to insert 
   */
{
  ELog::RegMethod RegA("CellMap","insertComponent(CG)");
  
  insertComponent(System,Key,CG.getAllExclude());

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
			 const size_t index,
			 const HeadRule& HR) const
  /*!
    Insert a component into a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param index :: cell index
    \param HR :: Contained Componenet
   */
{
  ELog::RegMethod RegA("CellMap","insertComponent(index,HR)");
  if (HR.hasRule())
    insertComponent(System,Key,index,HR.display());
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
    \param exclude :: Excluded surface(s)
   */
{
  ELog::RegMethod RegA("CellMap","insertComponent(string)");

  const std::vector<int> CVec=getCells(Key);
  if (CVec.empty())
    throw ColErr::InContainerError<std::string>
      (Key,"Cell["+Key+"] not present");

  for(const int cellNum : CVec)
    {
      MonteCarlo::Object* outerObj=System.findObject(cellNum);
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

  MonteCarlo::Object* outerObj=System.findObject(cellNum);
  if (!outerObj)
    throw ColErr::InContainerError<int>(cellNum,
					"Cell["+Key+"] not present");
  outerObj->addSurfString(exclude);
  return;
}

void
CellMap::insertComponent(Simulation& System,
			 const std::string& Key,
			 const size_t index,
			 const CellMap& CM,
			 const std::string& cmKey,
			 const size_t cmIndex) const
  /*!
    Insert an exclude component into a cell
    \param System :: Simulation to obtain cell from
    \param Key :: keyName for cel
    \param index :: Index on this cell [to be inserted]
    \param CM :: Cell map to extract obbject for insertion
    \param CMKey :: Key of cell map to insert
    \param cmIndex :: index of CellMap CM
   */
{
  ELog::RegMethod RegA("CellMap","insertComponent(key,index,CMap,cmIndex)");

  const int otherCellNum=CM.getCell(cmKey,cmIndex);
  ELog::EM<<"Other Cell == "<<otherCellNum<<ELog::endDiag;
  const MonteCarlo::Object* otherObj=System.findObject(otherCellNum);
  if (!otherObj)
    throw ColErr::InContainerError<int>(otherCellNum,
					"Cell["+cmKey+"] not present");
  HeadRule HR=otherObj->getHeadRule();
  HR.makeComplement();
  insertComponent(System,Key,index,HR);
  return;
}

void
CellMap::insertComponent(Simulation& System,
			 const std::string& Key,
			 const size_t index,
			 const FixedComp& FC,
			 const long int sideIndex) const
  /*!
    Insert an exclude component into a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param index :: Index on this cell [to be inserted]
    \param FC :: FixedComp for link surface
    \param sideIndex :: signed direction side
   */
{
  ELog::RegMethod RegA("CellMap","insertComponent(FC,index)");

  if (!sideIndex)
    throw ColErr::InContainerError<long int>
      (0,"Zero line surface not defined for : "+FC.getKeyName());

  insertComponent(System,Key,index,FC.getLinkString(sideIndex));
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
CellMap::makeCell(const std::string& Key,Simulation& System,
		  const int cellIndex,const int matNumber,
		  const double matTemp,const HeadRule& HR)

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
  System.addCell(cellIndex,matNumber,matTemp,HR);
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

HeadRule
CellMap::getCellsHR(const Simulation& System,
		    const std::string& Key) const
  /*!
    Get the main head rules for all the cells [UNION]
    \param System :: Simulation to get cell from 
    \param Key :: cell key name
   */

{
  ELog::RegMethod RegA("CellMap","getCellsHR");

  HeadRule Out;
  const std::vector<int> cells=getCells(Key);
  for(const int cellN : cells)
    {
      const MonteCarlo::Object* cellObj=System.findObject(cellN);
      if (!cellObj)
	throw ColErr::InContainerError<int>(cellN,"cellN on found");
      Out.addUnion(cellObj->getHeadRule());
    }
  return Out;
}

MonteCarlo::Object*
CellMap::getCellObject(Simulation& System,
		       const std::string& Key,
		       const size_t Index) const
  /*!
    Get the main head rules for all the cells [UNION]
    \param System :: Simulation to get cell from 
    \param Key :: cell key name
    \param index :: Index name
   */

{
  ELog::RegMethod RegA("CellMap","getCellObject");

  const int cellN=getCell(Key,Index);
  MonteCarlo::Object* cellObj=System.findObject(cellN);

  if (!cellObj)
    throw ColErr::InContainerError<int>(cellN,"cellN on found");

  return cellObj;
}

const HeadRule&
CellMap::getCellHR(const Simulation& System,
		   const std::string& Key,
		   const size_t Index) const
  /*!
    Get the main head rule for the cell
    \param System :: Simulation to get cell from 
    \param Key :: cell key name
    \param Index :: index of keyname unit
   */
{
  ELog::RegMethod RegA("CellMap","getCellHR");

  const int cellN=getCell(Key,Index);
  const MonteCarlo::Object* cellObj=System.findObject(cellN);
  if (!cellObj)
    throw ColErr::InContainerError<int>(cellN,"cellN on found");
  return cellObj->getHeadRule();
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
  const MonteCarlo::Object* ObjPtr=System.findObject(CN);
  if (!ObjPtr)
    throw ColErr::InContainerError<int>(CN,"Cell Ptr zero");

  std::pair<int,double> Out(ObjPtr->getMatID(),ObjPtr->getTemp());
  System.removeCell(CN);  // too complex to handle from ObjPtr
  return Out;
}

int
CellMap::getCellMat(const Simulation& System,
		    const std::string& Key,
		    const size_t Index) const
    /*!
    Obtain the material for a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param Index :: Cell index
    \return material number
  */
{
  ELog::RegMethod RegA("CellMap","getCellMat");
  
  const int cellN=getCell(Key,Index);
  const MonteCarlo::Object* cellObj=System.findObject(cellN);
  if (!cellObj)
    throw ColErr::InContainerError<int>(cellN,"cellN on found");

  return cellObj->getMatID();  
}

void
CellMap::renumberCell(const int oldCell,const int newCell)
  /*!
    Renumber cell -- decide not to do anything if not found
    \param oldCell :: old cell number
    \param newCell :: new cell number
  */
{
  ELog::RegMethod RegA("CellMap","renumberCell");
  
  changeCell(oldCell,newCell);
  return;
}

}  // NAMESPACE attachSystem
