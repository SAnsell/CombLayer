/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/SurfMap.cxx
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
#include "BaseMap.h"
#include "SurfMap.h"

namespace attachSystem
{

SurfMap::SurfMap() : BaseMap()
 /*!
    Constructor 
  */
{}

SurfMap::SurfMap(const SurfMap& A) : 
  BaseMap(A)
  /*!
    Copy constructor
    \param A :: SurfMap to copy
  */
{}

SurfMap&
SurfMap::operator=(const SurfMap& A)
  /*!
    Assignment operator
    \param A :: SurfMap to copy
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
SurfMap::insertComponent(Simulation& System,
			  const std::string& Key,
			  const ContainedComp& CC) const
  /*!
    Insert a component into a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param CC :: Contained Component ot insert 
   */
{
  ELog::RegMethod RegA("SurfMap","insertComponent(CC)");
  if (CC.hasOuterSurf())
    insertComponent(System,Key,CC.getExclude());
  return;
}

void
SurfMap::insertComponent(Simulation& System,
			  const std::string& Key,
			  const HeadRule& HR) const
  /*!
    Insert a component into a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param HR :: Contained Componenet
   */
{
  ELog::RegMethod RegA("SurfMap","insertComponent(HR)");
  if (HR.hasRule())
    insertComponent(System,Key,HR.display());
  return;
}

void
SurfMap::insertComponent(Simulation& System,
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
  ELog::RegMethod RegA("SurfMap","insertComponent(FC)");

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
SurfMap::insertComponent(Simulation& System,
			  const std::string& Key,
			  const std::string& exclude) const
  /*!
    Insert a component into a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param CC :: Contained Componenet
   */
{
  ELog::RegMethod RegA("SurfMap","insertComponent");

  const std::vector<int> CVec=getSurfs(Key);
  
  for(const int cellNum : CVec)
    {
      MonteCarlo::Qhull* outerObj=System.findQhull(cellNum);
      if (!outerObj)
	throw ColErr::InContainerError<int>(cellNum,
					    "Surf["+Key+"] not present");
      outerObj->addSurfString(exclude);
    }
  return;
}


void
SurfMap::deleteSurf(Simulation& System,
		    const std::string& Key,
		    const size_t Index) 
  /*!
    Delete a cell
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param Index :: Surf index
  */
{
  ELog::RegMethod RegA("SurfMap","deleteSurf");

  const int CN=BaseItem::removeSurf(Key,Index);

  if (!CN)
    throw ColErr::InContainerError<int>(CN,"Key["+Key+"] zero cell");
  
  System.removeSurf(CN);
  return;
}

std::pair<int,double>
SurfMap::deleteSurfWithData(Simulation& System,
			    const std::string& Key,
			    const size_t Index) 
/*!
    Delete a cell and then return material  
    \param System :: Simulation to obtain cell from
    \param Key :: KeyName for cell
    \param Index :: Surf index
    \return materialNumber : temperature
  */
{
  ELog::RegMethod RegA("SurfMap","deleteSurf");

  const int CN=BaseMap::removeItem(Key,Index);

  if (!CN)
    throw ColErr::InContainerError<int>(CN,"Key["+Key+"] zero cell");
  const MonteCarlo::Object* ObjPtr=System.findQhull(CN);
  if (!ObjPtr)
    throw ColErr::InContainerError<int>(CN,"Surf Ptr zero");

  std::pair<int,double> Out(ObjPtr->getMat(),ObjPtr->getTemp());
  System.removeSurf(CN);  // too complex to handle from ObjPtr
  return Out;
}


 
}  // NAMESPACE attachSystem
