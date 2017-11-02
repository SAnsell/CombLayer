/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/ContainedGroup.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "AttachSupport.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"

namespace attachSystem
{

ContainedGroup::ContainedGroup()
  /*!
    Constructor 
  */
{}

ContainedGroup::ContainedGroup(const std::string& A)
  /*!
    Constructor 
    \param A :: Key one
  */
{
  CMap.insert(CTYPE::value_type(A,ContainedComp()));  
}

ContainedGroup::ContainedGroup(const std::string& A,
			       const std::string& B)
  /*!
    Constructor 
    \param A :: Key one
    \param B :: Key two
  */
{
  CMap.insert(CTYPE::value_type(A,ContainedComp()));  
  CMap.insert(CTYPE::value_type(B,ContainedComp()));
}

ContainedGroup::ContainedGroup(const std::string& A,const std::string& B,
			       const std::string& C)
  /*!
    Constructor 
    \param A :: Key one
    \param B :: Key two
    \param C :: Key three
  */
{
  CMap.insert(CTYPE::value_type(A,ContainedComp()));  
  CMap.insert(CTYPE::value_type(B,ContainedComp()));
  CMap.insert(CTYPE::value_type(C,ContainedComp()));
}

ContainedGroup::ContainedGroup(const std::string& A,const std::string& B,
			       const std::string& C,const std::string& D)
  /*!
    Constructor 
    \param A :: Key one
    \param B :: Key two
    \param C :: Key three
    \param C :: Key four
  */
{
  CMap.insert(CTYPE::value_type(A,ContainedComp()));  
  CMap.insert(CTYPE::value_type(B,ContainedComp()));
  CMap.insert(CTYPE::value_type(C,ContainedComp()));
  CMap.insert(CTYPE::value_type(D,ContainedComp()));
}

ContainedGroup::ContainedGroup(const ContainedGroup& A) : 
  CMap(A.CMap)
  /*!
    Copy constructor
    \param A :: ContainedGroup to copy
  */
{}

ContainedGroup&
ContainedGroup::operator=(const ContainedGroup& A)
  /*!
    Assignment operator
    \param A :: ContainedGroup to copy
    \return *this
  */
{
  if (this!=&A)
    {
      CMap=A.CMap;
    }
  return *this;
}

ContainedGroup::~ContainedGroup()
  /*!
    Deletion operator
  */
{}

void
ContainedGroup::clearRules()
  /*!
    Zero all the rules
  */
{
  ELog::RegMethod RegA("ContainedGroup","clearRules");

  CTYPE::iterator mc;
  for(mc=CMap.begin();mc!=CMap.end();mc++)
    mc->second.clearRules();
  
  return;
}

bool
ContainedGroup::hasKey(const std::string& Key) const
  /*!
    Check a component to the group
    \param Key :: Key to check
    \return true if key exists
  */
{
  CTYPE::const_iterator mc=CMap.find(Key);
  return (mc!=CMap.end()) ? 1 : 0;
}

ContainedComp&
ContainedGroup::addCC(const std::string& Key)
  /*!
    Add a component to the group
    \param Key :: Key to add
    \return ContainedComp 
  */
{
  ELog::RegMethod RegA("ContainedGroup","addCC");
  
  CTYPE::iterator mc=CMap.find(Key);
  if (mc!=CMap.end())
    throw ColErr::InContainerError<std::string>(Key,"Key in CMap");

  CMap.insert(CTYPE::value_type(Key,ContainedComp()));  
  mc=CMap.find(Key);
  return mc->second;
}

ContainedComp&
ContainedGroup::getCC(const std::string& Key)
  /*!
    Determine the component from the key
    \param Key :: Key to look up
    \return ContainedComp 
  */
{
  ELog::RegMethod RegA("ContainedGroup","getCC");
  
  CTYPE::iterator mc=CMap.find(Key);
  if (mc==CMap.end())
    throw ColErr::InContainerError<std::string>(Key,"Key in CMap");
  return mc->second;
}

const ContainedComp&
ContainedGroup::getCC(const std::string& Key) const
/*!
  Determine the key 
  \param Key :: Key to look up
  \return ContainedComp 
*/
{
  ELog::RegMethod RegA("ContainedGroup","getCC");
  
  CTYPE::const_iterator mc=CMap.find(Key);
  if (mc==CMap.end())
    throw ColErr::InContainerError<std::string>(Key,"Key in CMap");
  return mc->second;
}

void
ContainedGroup::addOuterSurf(const std::string& Key,const int SN) 
  /*!
    Add a surface to the output
    \param Key :: Key name for rule
    \param SN :: Surface number [inward looking]
  */
{
  ELog::RegMethod RegA("ContainedGroup","addInterSurf");
  getCC(Key).addOuterSurf(SN);
  return;
}
  
void
ContainedGroup::addOuterSurf(const std::string& Key,
			     const std::string& SList) 
/*!
  Add a set of surfaces to the output
  \param Key :: Group name for rule
  \param SList ::  Surface string [fully decomposed]
*/
{
  ELog::RegMethod RegA("ContainedGroup","addInterSurf(std::string)");
  getCC(Key).addOuterSurf(SList);
  return;
}
  
void
ContainedGroup::addOuterUnionSurf(const std::string& Key,
				  const std::string& SList) 
/*!
  Add a set of surfaces to the output
  \param Key :: Key name for rule
  \param SList ::  Surface string [fully decomposed]
*/
{
  ELog::RegMethod RegA("ContainedGroup","addInterSurf(std::string)");
  getCC(Key).addOuterUnionSurf(SList);
  return;
}

void
ContainedGroup::addBoundarySurf(const std::string& Key,
				const int SN)
  
  /*!
    Add a set of surfaces to the boundary object
    \param Key :: Group name for rule
    \param SN ::  Signed surface number
  */
{
  ELog::RegMethod RegA("ContainedGroup","addBoundarySurf(std::string)");
  getCC(Key).addBoundarySurf(SN);
  return;
}
  
void
ContainedGroup::addBoundarySurf(const std::string& Key,
				const std::string& SList) 
  /*!
    Add a set of surfaces to the boundary object
    \param Key :: Key name for rule
    \param SList ::  Surface string [fully decomposed]
  */
{
  ELog::RegMethod RegA("ContainedGroup","addBoundarySurf(std::string)");
  getCC(Key).addBoundarySurf(SList);
  return;
}

void
ContainedGroup::addBoundaryUnionSurf(const std::string& Key,
				     const int SN)
  
  /*!
    Add a set of surfaces to the boundary object in union
    \param Key :: Group name for rule
    \param SN ::  Signed surface number
  */
{
  ELog::RegMethod RegA("ContainedGroup","addBoundaryUnionSurf(std::string)");
  getCC(Key).addBoundaryUnionSurf(SN);
  return;
}

void
ContainedGroup::addBoundaryUnionSurf(const std::string& Key,
				     const std::string& SList) 
  /*!
    Add a set of surfaces to the boundary object in union
    \param Key :: Key name for rule
    \param SList ::  Surface string [fully decomposed]
  */
{
  ELog::RegMethod RegA("ContainedGroup","addBoundaryUnionSurf(std::string)");
  getCC(Key).addBoundaryUnionSurf(SList);
  return;
}

std::string
ContainedGroup::getExclude(const std::string& Key) const
  /*!
    Calculate the write out the excluded surface.
    This allows the object to be inserted in a larger
    object.
    \param Key :: Key name for rule
    \return Exclude string [union]
  */
{
  ELog::RegMethod RegA("ContainedGroup","getExclude");
  return getCC(Key).getExclude();
}

std::string
ContainedGroup::getCompExclude(const std::string& Key) const
  /*!
    Calculate the write out the excluded surface.
    This allows the object to be inserted in a larger
    object.
    \param Key :: Key name for rule
    \return Exclude string [union]
  */
{
  ELog::RegMethod RegA("ContainedGroup","getExclude");
  return getCC(Key).getCompExclude();
}


std::string
ContainedGroup::getContainer(const std::string& Key) const
  /*!
    Calculate the write out the containing surface,
    allows an object to be inserted into this object without
    exceeding the boundary.
    \param Key :: Key name for rule
    \return Exclude string [union]
  */
{
  ELog::RegMethod RegA("ContainedGroup","getContainer");
  return getCC(Key).getContainer();
}

std::string
ContainedGroup::getCompContainer(const std::string& Key) const
  /*!
    Calculate the write out the containing surface,
    allows an object to be inserted into this object without
    exceeding the boundary.
    \param Key :: Key name for rule
    \return Exclude string [union]
  */
{
  ELog::RegMethod RegA("ContainedGroup","getContainer");
  return getCC(Key).getCompContainer();
}
  
int
ContainedGroup::surfOuterIntersect(const std::string& Key,
				   const Geometry::Line& LA) const
  /*!
    Get the signed surface that intersects the outer object layer
    \param Key :: Key name for rule
    \param LA :: Line to calculate from
    \return signed surface
  */
{
  ELog::RegMethod RegA("ContainedGroup","surfOuterIntersect");
  return getCC(Key).surfOuterIntersect(LA);
}

void 
ContainedGroup::addInsertCell(const std::string& Key,
			      const int CN)
  /*!
    Adds a cell to the insert list
    \param Key :: Key name for rule
    \param CN :: Cell number
  */
{
  ELog::RegMethod RegA("ContainedGroup","addInsertCell");
  getCC(Key).addInsertCell(CN);
  return;
}

void 
ContainedGroup::addInsertCell(const std::string& Key,
			      const ContainedComp& CC)
  /*!
    Adds a cell to the insert list
    \param Key :: Key name for rule
    \param CC :: Contained component to copy
  */
{
  ELog::RegMethod RegA("ContainedGroup","addInsertCell(CC)");
  getCC(Key).addInsertCell(CC);
  return;
}

void 
ContainedGroup::addAllInsertCell(const int CN)
  /*!
    Adds a cell to the insert list [for all]
    \param CN :: Cell number
  */
{
  ELog::RegMethod RegA("ContainedGroup","addAllInsertCell(cell)");
  CTYPE::iterator mc;
  for(mc=CMap.begin();mc!=CMap.end();mc++)
    mc->second.addInsertCell(CN);
  
  return;
}

void 
ContainedGroup::addAllInsertCell(const ContainedComp& CC)
  /*!
    Adds a cell to the insert list [for all]
    \param CC :: Contained comp to use
  */
{
  ELog::RegMethod RegA("ContainedGroup","addAllInsertCell(CC)");
  CTYPE::iterator mc;
  for(mc=CMap.begin();mc!=CMap.end();mc++)
    mc->second.addInsertCell(CC);
  
  return;
}

void 
ContainedGroup::setInsertCell(const std::string& Key,
			      const int CN)
  /*!
    Sets a cell to the insert list
    \param Key :: Group name
    \param CN :: Cell number
  */
{
  ELog::RegMethod RegA("ContainedGroup","setInsertCell");
  getCC(Key).setInsertCell(CN);
  return;
}
  
void 
ContainedGroup::setAllInsertCell(const int CN)
  /*!
    Sets a cell to the insert list
    \param CN :: Cell number
  */
{
  ELog::RegMethod RegA("ContainedGroup","setAllInsertCell");
  CTYPE::iterator mc;
  for(mc=CMap.begin();mc!=CMap.end();mc++)
    mc->second.setInsertCell(CN);
  return;
}

void 
ContainedGroup::addInsertCell(const std::string& Key,
			      const std::vector<int>& CN)
  /*!
    Adds a cell to the insert list
    \param Key :: Key name for rule
    \param CN :: Cell numbers
  */
{
  ELog::RegMethod RegA("ContainedGroup","addInsertCell");
  getCC(Key).addInsertCell(CN);
  return;
}


void 
ContainedGroup::insertInCell(const std::string& Key,
			     Simulation& System,
			     const int CN)
  /*!
    Inserts a contained component into the cell 
    \param Key :: Group name
    \param System :: Simulation to get cells
    \param CN :: Cell number
  */
{
  ELog::RegMethod RegA("ContainedGroup","insertInCell");
  getCC(Key).insertInCell(System,CN);
  return;
}

void 
ContainedGroup::insertInCell(const std::string& Key,
			     Simulation& System,
			     const std::vector<int>& CVec)
  /*!
    Inserts a contained component into the cell 
    \param Key :: Group name
    \param System :: Simulation to get cells
    \param CVec :: Cell numbers
  */
{
  ELog::RegMethod RegA("ContainedGroup","insertInCell");
  getCC(Key).insertInCell(System,CVec);
  return;
}
  
void
ContainedGroup::insertObjects(Simulation& System)
  /*!
    Create outer virtual space that includes the beamstop etc
    \param System :: Simulation to add to 
  */
{
  ELog::RegMethod RegA("ContainedGroup","insertObjects");
  
  CTYPE::iterator mc;
  for(mc=CMap.begin();mc!=CMap.end();mc++)
    mc->second.insertObjects(System);
  return;
}

}  // NAMESPACE attachSystem
