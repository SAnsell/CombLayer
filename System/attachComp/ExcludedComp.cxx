/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/ExcludedComp.cxx
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
#include <deque>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "SurInter.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Line.h" 
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "AttachSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedGroup.h"
#include "ExcludedComp.h"

namespace attachSystem
{

ExcludedComp::ExcludedComp() 
  /*!
    Constructor 
  */
{}

ExcludedComp::ExcludedComp(const ExcludedComp& A) : 
  excludeCells(A.excludeCells),ExcludeObj(A.ExcludeObj),
  boundary(A.boundary)
  /*!
    Copy constructor
    \param A :: ExcludedComp to copy
  */
{}

ExcludedComp&
ExcludedComp::operator=(const ExcludedComp& A)
  /*!
    Assignment operator
    \param A :: ExcludedComp to copy
    \return *this
  */
{
  if (this!=&A)
    {
      excludeCells=A.excludeCells;
      ExcludeObj=A.ExcludeObj;
      boundary=A.boundary;
    }
  return *this;
}

ExcludedComp::~ExcludedComp()
  /*!
    Deletion operator
  */
{}

void
ExcludedComp::clearRules()
  /*!
    Zero all the rules
  */
{
  ELog::RegMethod RegA("ExludedComp","clearRules");

  boundary.reset();
  return;
}

std::string
ExcludedComp::getExcludeUnit() const
  /*!
    Calculate the write out the excluded surface.
    This allows the object to be inserted in a larger
    object.
    \return Exclude string [union]
  */
{
  ELog::RegMethod RegA("ExcludedComp","getExcludeUnit");

  return boundary.display();
}

std::string
ExcludedComp::getNotExcludeUnit() const
  /*!
    Calculate the write out the excluded surface.
    This allows the object to be inserted in a larger
    object.
    \return Exclude string [union]
  */
{
  ELog::RegMethod RegA("ExcludedComp","getNotExcludeUnit");

  return boundary.complement().display();
}



int
ExcludedComp::isExcludeUnitValid(const Geometry::Vec3D& V) const
  /*!
    Determine if the boundary is valid
    \param V :: Vector to test
    \return true/false
   */
{
  ELog::RegMethod RegA("ExcludedComp","isExcludeUnitValid"); 

  return boundary.isValid(V);
}


void 
ExcludedComp::addExcludeCell(const int CN)
  /*!
    Adds a cell to the insert list
    \param CN :: Cell number
  */
{
  ELog::RegMethod RegA("ExcludedComp","addInsertCell");
  excludeCells.push_back(CN);
  return;
}

void 
ExcludedComp::setExcludeCell(const int CN)
  /*!
    Sets a cell to the insert list
    \param CN :: Cell number
  */
{
  ELog::RegMethod RegA("ExcludedComp","setInsertCell");

  excludeCells.clear();
  if (CN)
    excludeCells.push_back(CN);
  return;
}

void
ExcludedComp::addExcludeSurf(const int SN) 
  /*!
    Add a surface to the output [intersection]
    \param SN :: Surface number [inward looking]
  */
{
  ELog::RegMethod RegA("ExcludedComp","addExcludeSurf(int)");

  boundary.addIntersection(SN);

  return;
}

void
ExcludedComp::addExcludeSurf(const std::string& SList) 
  /*!
    Add a set of surfaces to the output
    \param SList ::  Surface string [fully decomposed]
  */
{
  ELog::RegMethod RegA("ExcludedComp","addExcludeSurf(std::string)");

  boundary.addUnion(SList);
  return;
}


void
ExcludedComp::addExcludeSurf(const attachSystem::FixedComp& FC,
			    const long int LIndex)
/*!
  Add a boundary surface
  \param FC :: Fixed object to use
  \param LIndex :: Link surface index 
*/

{
  ELog::RegMethod RegA("RefBox","addExcludeSurf(FC,Index)");

  // Surfaces on links point outwards (hence swap of sign)
  boundary.addUnion(FC.getLinkSurf(LIndex));
  
  return;
}



void
ExcludedComp::addExcludeObj(const ContainedComp& CC) 
  /*!
    Add a set of surfaces to the output
    \param CC ::  Contained component to exclude
  */
{
  ELog::RegMethod RegA("ExcludedComp","addExcludeObj(CC)");
  
  const std::string OutStr=CC.getCompExclude();
  if (!OutStr.empty())
    boundary.addUnion(OutStr);
  return;
}

void
ExcludedComp::addExcludeObj(const objectGroups& OGrp,
			    const std::string& ObjName) 
  /*!
    Add a set of surfaces to the output
    \param OGrp :: Object group
    \param ObjName ::  Object Name [to use outer cell
  */
{
  ELog::RegMethod RegA("ExcludedComp","addExcludeObj(str)");

  
  const ContainedComp* CCPtr=
    OGrp.getObjectThrow<ContainedComp>(ObjName,"CC-Object Not found");

  
  const std::string OutStr=CCPtr->getCompExclude();
  if (!OutStr.empty())
    boundary.addUnion(OutStr);

  return;
}

void
ExcludedComp::addExcludeObj(const objectGroups& OGrp,
			    const std::string& objName,
			    const std::string& grpName) 
  /*!
    Add a set of surfaces to the output
    \param ObjName ::  ContainedGroup Object Name [to use outer cell]
    \param grpName ::  Group in CG
  */
{
  ELog::RegMethod RegA("ExcludedComp","addExcludeObj(str,str)");

  
  const ContainedGroup* CCPtr=
    OGrp.getObject<ContainedGroup>(objName);
  
  if (CCPtr)
    {
      const std::string OutStr=CCPtr->getCompExclude(grpName);
      boundary.addUnion(OutStr);
    }
  return;
}

void
ExcludedComp::addExcludeObj(const std::string& grpName,
			    const ContainedGroup& CG) 
  /*!
    Add a set of surfaces to the output
    \param grpName ::  Component name
    \param CG :: ContainGroup 
  */
{
  ELog::RegMethod RegA("ExcludedComp","addExcludeObj(str,CG)");
  
  const std::string OutStr=CG.getCompExclude(grpName);
  boundary.addUnion(OutStr);

  return;
}

void
ExcludedComp::applyBoundary(Simulation& System)
  /*!
    Applies the boundary to the cells that are required.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("ExcludedComp","applyBoundary");

  if (!boundary.hasRule()) return;

  for(const int CN : excludeCells)
    {
      MonteCarlo::Object* outerObj=System.findObject(CN);
      if (outerObj)
	outerObj->addSurfString(getNotExcludeUnit());
      else
	ELog::EM<<"Failed to find outerObject: "<<CN<<ELog::endErr;
    }
  return;
}


}  // NAMESPACE attachSystem
