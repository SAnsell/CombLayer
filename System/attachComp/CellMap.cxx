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
  Cells(A.Cells),SplitUnits(A.SplitUnits)
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
      SplitUnits=A.SplitUnits;
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
  setCell(Key,0,CN);
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

  LCTYPE::iterator mc=Cells.find(Key);
  if (mc==Cells.end())
    {
      if (Index==0)
	Cells.insert(LCTYPE::value_type(Key,CN));
      else
	throw ColErr::InContainerError<std::string>
	  (Key,"Key not defined for index > 0");

      return;
    }
  
  if (mc->second>=0)           // +1 case
    {
      if (Index==0)
	mc->second=CN;
      else if (Index==1)
	{
	  SplitUnits.push_back(std::vector<int>({mc->second,CN}));
	  mc->second = -static_cast<int>(SplitUnits.size());
	}
      else
	throw ColErr::IndexError<size_t>
	  (Index,0,"Key["+Key+"] index error");
      return;
    }

  // mc->second :: -ve
  const size_t SI(static_cast<size_t>(-mc->second)-1);
  const size_t SU(SplitUnits[SI].size());

  if (Index<SU)
    SplitUnits[SI][Index]=CN;
  else if (Index==SU)
    SplitUnits[SI].push_back(CN);
  else
    throw ColErr::IndexError<size_t>
      (Index,SU,"Key["+Key+"] index error");

  return; 
}

void
CellMap::setCells(const std::string& Key,const int CNA,const int CNB)
  /*!
    Insert a set of cells
    \param Key :: Keyname
    \param CNA :: Cell number [Start]
    \param CNB :: Cell number [End]
  */
{
  ELog::RegMethod RegA("CellMap","setCells");
  
  int mA(std::min(CNA,CNB));
  int mB(std::max(CNA,CNB));
  std::vector<int> Out;
  for(;mA<=mB;mA++)
    Out.push_back(mA);

  LCTYPE::iterator mc=Cells.find(Key);
  if (mc==Cells.end())
    {
      if (mA==mB)
	Cells.insert(LCTYPE::value_type(Key,mA));
      else
	{
	  SplitUnits.push_back(Out);
	  Cells.insert(LCTYPE::value_type
		       (Key,-static_cast<int>(SplitUnits.size())));
	}      
      return;
    }
  
  if (mc->second>=0)           // +1 case
    {
      Out.push_back(mc->second);
      SplitUnits.push_back(Out);
      mc->second = -static_cast<int>(SplitUnits.size());
      return;
    }

  // mc->second :: -ve
  const size_t SI(static_cast<size_t>(-mc->second)-1);
  std::move(Out.begin(),Out.end(),
	    std::back_inserter(SplitUnits[SI]));


  return; 
}

void
CellMap::addCell(const std::string& Key,const int CN)
  /*!
    Insert a cell 
    \param Key :: Keyname
    \param CN :: Cell number
  */
{
  ELog::RegMethod RegA("CellMap","addCell");
  
  LCTYPE::iterator mc=Cells.find(Key);
  if (mc==Cells.end())
    setCell(Key,0,CN);
  else if (mc->second>=0)  // +1 case
    setCell(Key,1,CN);
  else
    {
      const size_t index=static_cast<size_t>((-mc->second-1));
      setCell(Key,SplitUnits[index].size(),CN);
    }
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
  return getCell(Key,0);
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

  LCTYPE::const_iterator mc=Cells.find(Key);
  if (mc==Cells.end())
    throw ColErr::InContainerError<std::string>(Key,"Key not present");

  if (mc->second>=0)
    {
      if (Index!=0)
	throw ColErr::IndexError<size_t>(Index,0,"Key["+Key+"] index error");
      return mc->second;
    }
  // This can't fail:
  const size_t SI(static_cast<size_t>(-mc->second-1));
  const size_t SU(SplitUnits[SI].size());
  
  if (Index>=SU)
    throw ColErr::IndexError<size_t>(Index,SU,"Key["+Key+"] index error");
  return SplitUnits[SI][Index];
}

std::vector<int>
CellMap::getCells(const std::string& Key) const
  /*!
    Returns a vector of cells: 
    Note a bit of care is needed over the case on a single value
    \param Key :: keyname to search
    \return vector
   */
{
  ELog::RegMethod RegA("CellMap","getCells");

  std::vector<int> Out;
  
  LCTYPE::const_iterator mc=Cells.find(Key);
  if (mc==Cells.end())
    return Out;

  if (mc->second>=0)
    return std::vector<int>({mc->second});
  
  const size_t SU(static_cast<size_t>(-mc->second-1));


  for(const int& CN : SplitUnits[SU])
    if (CN)
      Out.push_back(CN);
  return Out;
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

  const int CN=removeCell(Key,Index);

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

  const int CN=removeCell(Key,Index);

  if (!CN)
    throw ColErr::InContainerError<int>(CN,"Key["+Key+"] zero cell");
  const MonteCarlo::Object* ObjPtr=System.findQhull(CN);
  if (!ObjPtr)
    throw ColErr::InContainerError<int>(CN,"Cell Ptr zero");

  std::pair<int,double> Out(ObjPtr->getMat(),ObjPtr->getTemp());
  System.removeCell(CN);  // too complex to handle from ObjPtr
  return Out;
}

int
CellMap::removeCell(const std::string& Key,
		    const size_t Index) 
 /*!
    Remove a cell number a cell
    \param Key :: KeyName for cell
    \param Index :: Cell index
    \return cell number removed
  */
{
  ELog::RegMethod RegA("CellMap","removeCell");

  LCTYPE::iterator mc=Cells.find(Key);
  if (mc==Cells.end())
    throw ColErr::InContainerError<std::string>(Key,"Key not present");
  if (mc->second>=0)
    {
      if (Index!=0)
	throw ColErr::IndexError<size_t>
	  (Index,0,"Key["+Key+"] index error");

      const int outCN=mc->second;
      Cells.erase(mc);
      return outCN;
    }
  // NOTE HERE WE ZERO and not delete because otherwise a seti
  // of linear calls to this function are junk.
  const size_t SI(static_cast<size_t>(-mc->second-1));
  const size_t SU(SplitUnits[SI].size());
  if (Index>=SU)
    throw ColErr::IndexError<size_t>
      (Index,SU,"Key["+Key+"] index error");

  const int outCN=SplitUnits[SI][SU];
  SplitUnits[SI][SU]=0;
  return outCN;
}

 
}  // NAMESPACE attachSystem
