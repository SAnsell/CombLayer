/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/ContainedComp.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "writeSupport.h"
#include "Vec3D.h"
#include "interPoint.h"
#include "Surface.h"
#include "SurInter.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ContainedComp.h"
#include "surfRegister.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedGroup.h"

namespace attachSystem
{

std::ostream&
operator<<(std::ostream& OX,const ContainedComp& A)
  /*!
    Standard write
    \param OX :: Output stream
    \param A :: ContainedComp to write
   */
{
  A.write(OX);
  return OX;
}

ContainedComp::ContainedComp() 
  /*!
    Constructor 
  */
{}

ContainedComp::ContainedComp(const ContainedComp& A) : 
  outerSurf(A.outerSurf),
  insertCells(A.insertCells)
  /*!
    Copy constructor
    \param A :: ContainedComp to copy
  */
{}

ContainedComp&
ContainedComp::operator=(const ContainedComp& A)
  /*!
    Assignment operator
    \param A :: ContainedComp to copy
    \return *this
  */
{
  if (this!=&A)
    {
      outerSurf=A.outerSurf;
      insertCells=A.insertCells;
    }
  return *this;
}


ContainedComp::~ContainedComp()
  /*!
    Deletion operator
  */
{}

void
ContainedComp::clearRules()
  /*!
    Zero all the rules
   */
{
  outerSurf.reset();
  return;
}

void
ContainedComp::copyRules(const ContainedComp& A) 
  /*!
    Copy all the rules from another object to this object
    \param A :: Object to copy
  */
{
  ELog::RegMethod RegA("ContainedComp","copyRules");
  if (&A != this)
    {
      outerSurf=A.outerSurf;
    }
  return;
}

void
ContainedComp::copyInterObj(const ContainedComp& A) 
  /*!
    Copy all intersect objects from one object to another
    \param A :: Object to copy
  */
{
  ELog::RegMethod RegA("ContainedComp","copyInterObj");
  if (this!=&A)
    insertCells=A.insertCells;
  return;
}
  
void
ContainedComp::addOuterSurf(const int SN) 
  /*!
    Add a surface to the output
    \param SN :: Surface number [inward looking]
  */
{
  ELog::RegMethod RegA("ContainedComp","addOuterSurf");
  
  outerSurf.addIntersection(HeadRule(SN));
  outerSurf.populateSurf();
  return;
}

void
ContainedComp::addOuterSurf(const HeadRule& HR) 
  /*!
    Add a set of surfaces to the output
    \param HR ::  Surface string [fully decomposed]
  */
{
  ELog::RegMethod RegA("ContainedComp","addOuterSurf(HeadRule)");

  if (HR.hasRule())
    {
      outerSurf.addIntersection(HR);
      outerSurf.populateSurf();
    }
  return;
}

void
ContainedComp::addOuterSurf(const std::string& SList) 
  /*!
    Add a set of surfaces to the output
    \param SList ::  Surface string [fully decomposed]
  */
{
  ELog::RegMethod RegA("ContainedComp","addOuterSurf(std::string)");
  if (!SList.empty())
    {
      outerSurf.addIntersection(SList);
      outerSurf.populateSurf();
    }
  return;
}

void
ContainedComp::addOuterSurf(const ContainedComp& CC) 
  /*!
    Add a surface to the output
    \param CC :: Contaned comp to intersect
  */
{
  ELog::RegMethod RegA("ContainedComp","addOuterSurf(CC)");

  outerSurf.addIntersection(CC.outerSurf);
  outerSurf.populateSurf();
  return;
}
  
void
ContainedComp::addOuterUnionSurf(const std::string& SList) 
  /*!
    Add a set of surfaces to the output
    \param SList ::  Surface string [fully decomposed]
  */
{
  ELog::RegMethod RegA("ContainedComp","addOuterUnionSurf(std::string)");

  outerSurf.addUnion(SList);
  outerSurf.populateSurf();

  return;
}

void
ContainedComp::addOuterUnionSurf(const HeadRule& HR) 
  /*!
    Add a set of surfaces to the output
    \param HR ::  Surface string [fully decomposed]
  */
{
  ELog::RegMethod RegA("ContainedComp","addOuterUnionSurf(HeadRule)");

  if (HR.hasRule())
    {
      outerSurf.addUnion(HR);
      outerSurf.populateSurf();
    }
  return;
}

void
ContainedComp::addOuterUnionSurf(const ContainedComp& CC) 
  /*!
    Add a surface to the output as a union
    \param CC :: Contaned comp to intersect
  */
{
  ELog::RegMethod RegA("ContainedComp","addOuterUnionSurf(CC)");

  outerSurf.addUnion(CC.outerSurf);
  outerSurf.populateSurf();
  return;
}

const HeadRule&
ContainedComp::getOuterSurf() const
  /*!
    Care here because this can return a referenece
    due to ContainedGroup not having a complete outer surf
    \return Outer headRule
  */
{
  return outerSurf;
}

HeadRule
ContainedComp::getExcludeSurf() const
  /*!
    Care here because this can return a referenece
    due to ContainedGroup not having a complete outer surf
    \return Outer headRule
  */
{
  return outerSurf.complement();
}
  
std::string
ContainedComp::getCompExclude() const
  /*!
    Calculate the write out the excluded surface.
    This allows the object to be inserted in a larger
    object.
    \return Exclude string [union]
  */
{
  ELog::RegMethod RegA("ContainedComp","getCompExclude");

  return outerSurf.display();
}

std::string
ContainedComp::getExclude() const
  /*!
    Calculate the write out the excluded surface.
    This allows the object to be inserted in a larger
    object.
    \return Exclude string [union]
  */
{
  ELog::RegMethod RegA("ContainedComp","getExclude");
  
  if (outerSurf.hasRule())
    return outerSurf.complement().display();
  return "";
}
  
int
ContainedComp::surfOuterIntersect(const Geometry::Line& LA) const
  /*!
    Get the signed surface that intersects the outer object layer
    \param LA :: Line to calculate from
    \return signed surface
  */
{
  ELog::RegMethod RegA("ContainedComp","surfOuterIntersect");

  if (outerSurf.hasRule())
    {
      const Geometry::interPoint
	result=outerSurf.trackSurfIntersect(LA.getOrigin(),LA.getDirect());

      return result.SNum;
    }
  return 0;
}


void
ContainedComp::addInsertCell(const ContainedComp& CC)
  /*!
    Adds CC-cells to the insert list
    \param CC :: ContainedComp to copy
  */
{
  ELog::RegMethod RegA("ContainedComp","addInsertCell<CC>");

  addInsertCell(CC.insertCells);
  return;
}

void
ContainedComp::addInsertCell(const std::vector<int>& CVec)
  /*!
    Adds a cell to the insert list
    \param CVec :: Set of cell numbers
  */
{
  ELog::RegMethod RegA("ContainedComp","addInsertCell<vector>");

  for(const int CN : CVec)
    {
      if (CN==0)
	throw ColErr::EmptyValue<int>("CN index");
      if (std::find(insertCells.begin(),insertCells.end(),CN)
	  ==insertCells.end())
	insertCells.push_back(CN);
    }
  return;
}

void 
ContainedComp::addInsertCell(const int CN)
  /*!
    Adds a cell to the insert list
    \param CN :: Cell number
  */
{
  ELog::RegMethod RegA("ContainedComp","addInsertCell(in)");
  if (CN==0)
    throw ColErr::EmptyValue<int>("CN index");
  if (std::find(insertCells.begin(),insertCells.end(),CN)
      ==insertCells.end())
    insertCells.push_back(CN);

  return;
}

void 
ContainedComp::setInsertCell(const int CN)
  /*!
    Sets a cell to the insert list
    \param CN :: Cell number
  */
{
  ELog::RegMethod RegA("ContainedComp","setInsertCell");
  insertCells.clear();
  if (CN)
    insertCells.push_back(CN);
  return;
}

void 
ContainedComp::setInsertCell(const std::vector<int>& CN)
  /*!
    Sets a cell list to the insert list
    \param CN :: Cell numbers
  */
{
  ELog::RegMethod RegA("ContainedComp","setInsertCell(vec)");
  if (find_if(CN.begin(),CN.end(),
	      std::bind(std::equal_to<int>(),
			std::placeholders::_1,0))!=CN.end())
    throw ColErr::EmptyValue<int>("CN index");

  insertCells=CN;
  return;
}

void
ContainedComp::insertExternalObject(Simulation& System,
				    const MonteCarlo::Object& excludeObj) const
  /*!
    Insert the ContainedComp into the cell list
    \param System :: Simulation to get objects from
    \param excludeObj :: object that is added to insert cells
  */
{
  ELog::RegMethod RegA("ContainedComp","insertExternalObject");
  
  const HeadRule excludeHR=
    excludeObj.getHeadRule().complement();

  for(const int CN : insertCells)
    {
      MonteCarlo::Object* outerObj=System.findObjectThrow(CN,"OuterObject");
      outerObj->addIntersection(excludeHR);
    }
  return;
}

void
ContainedComp::insertObjects(Simulation& System)
  /*!
    Insert the ContainedComp into the cell list
    \param System :: Simulation to get objects from
  */
{
  ELog::RegMethod RegA("ContainedComp","insertObjects");
  if (!hasOuterSurf()) return;

  for(const int CN : insertCells)
    {
      MonteCarlo::Object* outerObj=
	System.findObjectThrow(CN,"Cell in Simulation");
      outerObj->addIntersection(outerSurf.complement());
    }
  insertCells.clear();
  return;
}

void
ContainedComp::insertObjects(Simulation& System,
			     const std::vector<Geometry::Vec3D>& testPts)
  /*!
    Insert the ContainedComp into the cell list
    \param System :: Simulation to get objects from
    \param testPts  :: unless TestPts are in the cell no insert.
    insertion takes place
  */
{
  ELog::RegMethod RegA("ContainedComp","insertObjects(testPts)");
  if (!hasOuterSurf()) return;

  for(const int CN : insertCells)
    {
      MonteCarlo::Object* outerObj=System.findObject(CN);
      if (outerObj)
	{
	  const HeadRule& HR=outerObj->getHeadRule();
	  for(const Geometry::Vec3D& Pts : testPts)
	    {
	      if (HR.isValid(Pts))
		{
		  outerObj->addIntersection(outerSurf.complement());
		  break;
		}
	    }
	}	  
      else
	ELog::EM<<"Failed to find outerObject: "<<CN<<ELog::endErr;
    }
  insertCells.clear();
  return;
}

void
ContainedComp::insertInCell(MonteCarlo::Object& outerObj) const
  /*!
    Insert the ContainedComp in a single cell.
    \param outerObj :: Outer Object
  */
{
  ELog::RegMethod RegA("ContainedComp","insertInCell(Obj)");
  
  if (!hasOuterSurf()) return;
  outerObj.addIntersection(outerSurf.complement());
  return;
}

void
ContainedComp::insertInCell(Simulation& System,
			    const int cellN) const
  /*!
    Insert the ContainedComp in a single cell.
    \param System :: Simulation to get objects 
    \param cellN :: Cell number
  */
{
  ELog::RegMethod RegA("ContainedComp","insertInCell");
  
  MonteCarlo::Object* outerObj=System.findObjectThrow(cellN,"cellN");
  this->insertInCell(*outerObj);
  return;
}

void
ContainedComp::insertInCell(Simulation& System,
			    const std::vector<int>& cellVec) const
  /*!
    Insert the ContainedComp in a single cell.
    \param System :: Simulation to get objects 
    \param cellVec :: Cell numbers
  */
{
  ELog::RegMethod RegA("ContainedComp","insertInCell(Vec)");
  
  if (!hasOuterSurf()) return;
  for(const int cellN : cellVec)
    {
      MonteCarlo::Object* outerObj=System.findObjectThrow(cellN,"cellN");
      outerObj->addIntersection(outerSurf.complement());
    }
  return;
}

void
ContainedComp::write(std::ostream& OX) const
  /*!
    Write out the boundar/outer rules
    \param OX :: Output stream
  */
{
  OX<<"outerSurf:";
  StrFunc::writeMCNPX(outerSurf.display(),OX);
  OX<<std::endl;
  return;
}

}  // NAMESPACE attachSystem
 
