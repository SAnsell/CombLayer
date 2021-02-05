/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/PhysImp.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <complex>
#include <set>
#include <list>
#include <map>
#include <iterator>
#include <algorithm>


#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "writeSupport.h"
#include "particleConv.h"

#include "Triple.h"
#include "NRange.h"
#include "MapRange.h"
#include "ZoneUnit.h"
#include "PhysicsCardFunctions.h"

#include "PhysImp.h"

namespace physicsSystem
{


PhysImp::PhysImp(const std::string& TP) :
  fullName(TP),type(TP),defFlag(0),defValue(0.0)
  /*!
    Constructor with Type
    \param TP :: Type identifier
  */
{}

PhysImp::PhysImp(const std::string& TP,
		 const std::string& particleList) :
  type(TP),defFlag(0),defValue(0.0)
  /*!
    Constructor with Type
    \param TP :: Type identifier
  */
{
  setParticles(particleList);
  fullName=type+getParticles();
}

PhysImp::PhysImp(const std::string& TP,
		 const std::string& particleList,
		 const double V) :
  type(TP),defFlag(1),defValue(V)
  /*!
    Constructor with Type
    \param TP :: Type identifier
  */
{
  setParticles(particleList);
  fullName=type+getParticles();
}

PhysImp::PhysImp(const PhysImp& A) :
  fullName(A.fullName),type(A.type),
  defFlag(A.defFlag),
  defValue(A.defValue),
  particles(A.particles),
  impNum(A.impNum)
  /*!
    Copy Constructor
    \param A :: PhysImp to copy
  */
{}

PhysImp&
PhysImp::operator=(const PhysImp& A) 
  /*!
    Assignment operator
    \param A :: PhysImp to copy
    \return *this
  */
{
  if (this!=&A)
    {
      fullName=A.fullName;
      type=A.type;
      defFlag=A.defFlag;
      defValue=A.defValue;
      particles=A.particles;
      impNum=A.impNum;
    }
  return *this;
}

PhysImp::~PhysImp()
  /*!
    Destructor
  */
{}

size_t
PhysImp::commonParticles(const PhysImp& A) const
  /*!
    Given a particle list between two PhysImp. Count
    the number of common particles
    \return number of common particles						
   */
{
  ELog::RegMethod RegA("PhysImp","commonParticles");
  
  size_t cnt(0);
  for(const int PN : particles)
    {
      if (A.particles.find(PN)!=A.particles.end())
	cnt++;
    }
  return cnt;
}

void
PhysImp::copyValues(const PhysImp& A)
  /*!
    Copy over the values
    \param A :: PhysImp to copy values from
  */
{
  if (&A!=this)
    {
      impNum=A.impNum;
    }
  return;
}
  
void
PhysImp::clear()
  /*!
    Clear everything
   */
{
  particles.clear();
  impNum.erase(impNum.begin(),impNum.end());
  return;
}

void
PhysImp::setCell(const int ID,const double value)
  /*!
    Set a special interest in a cell ID
    The existance of the cell is only 
    checked at the write out stage.
    \param ID :: Cell number
    \param value :: new value
  */
{
  impNum[ID]=value;
  return;
}


std::string
PhysImp::getParticles() const
 /*!
   Get the particle list as an mcnp name
   \return particle ist
 */
{
  ELog::RegMethod RegA("PhysImp","getParticle");

  const particleConv& pConv=particleConv::Instance();

  return pConv.mcnpParticleList(particles);
}

void
PhysImp::setParticles(const std::string& P)
 /*!
   Set the particles to be just P
   \param P :: Particle 
 */
{
  ELog::RegMethod RegA("PhysImp","setParticle");

  particles.clear();

  const particleConv& pConv=particleConv::Instance();
  std::string Line(P);
  std::string item;
  while(StrFunc::section(Line,item))
    {
      const int index=pConv.mcplITYP(item);
      particles.emplace(index);
    }
  return;
}


void
PhysImp::addParticle(const std::string& pName) 
  /*!
    Adds an element to the particles list
    (note it also checks the elment)
    \param particelName :: element identifier
  */
{
  ELog::RegMethod RegA("PhysImp","addParticle");
  
  const int index=particleConv::Instance().mcplITYP(pName);
  particles.emplace(index);
  return;
}

bool
PhysImp::hasParticle(const std::string& E) const
  /*!
    Finds the item on the list if it exists
    Note this throws is E is not a valid particle
    \param E :: Particle string to find
    \return 0 :: failure , 1 :: success
  */
{
  ELog::RegMethod RegA("PhysImp","hasParticle");

  const int index=particleConv::Instance().mcplITYP(E);
  

  return (particles.find(index)!=particles.end());
}

bool
PhysImp::removeParticle(const PhysImp& A)
  /*!
    The particles using the particles from A
    \param PT :: Particle type
    \return 1 :: success (or zero for failure)    
   */
{
  ELog::RegMethod RegA("PhysImp","removeParticle(PhysImp)");

  size_t outFlag(0);
  for(const int pIndex : A.particles)
    outFlag+=particles.erase(pIndex);

  if (outFlag)
    {
      fullName=type+getParticles();
      return 1;
    }
  
  return 0;
}

bool
PhysImp::removeParticle(const std::string& PT)
  /*!
    Removes a particle type
    Note: throws if PT is not a real particle
    \param PT :: Particle type
    \return 1 :: success (or zero for failure)
   */
{
  ELog::RegMethod RegA("PhysImp","removeParticle");

  const int index=particleConv::Instance().mcplITYP(PT);
  
  std::set<int>::iterator vc= particles.find(index);
  if (vc==particles.end()) return 0;
  particles.erase(vc);
  return 1;
}

  
void
PhysImp::removeCell(const int ID)
  /*!
    Remove cell form list
    \param ID :: Cell number
  */
{
  std::map<int,double>::iterator ac;
  ac=impNum.find(ID);
  if (ac!=impNum.end())
    {
      impNum.erase(ac);
    }
  return;
}

void
PhysImp::modifyCells(const std::vector<int>& cellOrder,
		     const double defValue)
  /*!
    Process the ordered list of the cells.
    If the cell number does not exists a default value
    of 1 is added.
    \param cellOrder :: list of cells
    \param defValue :: default value to use (set to 1.0)
  */
{
  std::vector<int>::const_iterator vc;
  std::map<int,double>::iterator ac;
  std::map<int,double> nMap;
  for(vc=cellOrder.begin();vc!=cellOrder.end();vc++)
    {
      ac=impNum.find(*vc);
      if (ac!=impNum.end())
	ac->second=defValue;
    }
  return;
}

void
PhysImp::updateCells(const ZoneUnit<double>& ZU)
  /*!
    Given a zone unit update cells within the zone unit
    \param ZU :: ZoneUnit to update
  */
{
  ELog::RegMethod RegA("PhysImp","updateCells");

  double V;
  for(std::map<int,double>::value_type& MC : impNum)
    {
      if (ZU.inRange(MC.first,V))
        MC.second=V;
    }
  return;
}

double
PhysImp::getValue(const int cellN) const
  /*!
    Get the Value  for a given cell.
    \param cellN :: cell number to find
    \throw InContainerError when cellN not found in impNum
    \return Importance Value
   */
{
  std::map<int,double>::const_iterator mc;
  mc=impNum.find(cellN);
  if (mc==impNum.end())
    throw ColErr::InContainerError<int>(cellN,"PhysImp::getValue");
  return mc->second;
}

void
PhysImp::setValue(const int cellN,const double V) 
  /*!
    Get the Value  for a given cell.
    \param cellN :: cell number to find
   */
{
  impNum[cellN]=V;
  return;
}

void
PhysImp::renumberCell(const int oldCellN,const int newCellN)
  /*!
    Renumbers a cell 
    \param oldCellN :: Old cell number
    \param newCellN :: New cell nubmer
  */
{
  ELog::RegMethod RegA("PhysImp","renumberCell");
  if (impNum.empty() || oldCellN==newCellN) return;
  
  typedef std::map<int,double> ITYPE;
  ITYPE::iterator mc;
  if (impNum.find(newCellN)!=impNum.end())
    throw ColErr::InContainerError<int>(newCellN,"New cell exists");
  mc=impNum.find(oldCellN);
  if (mc==impNum.end())
    throw ColErr::InContainerError<int>(oldCellN,"Old cell not found "+
					RegA.getFull());    

  // Doesn't invaidate 
  impNum.insert(ITYPE::value_type(newCellN,mc->second));
  impNum.erase(mc);

  return; 
}


void
PhysImp::write(std::ostream& OX,
	       const std::set<std::string>& excludeParticles,
	       const std::vector<int>& cellOutOrder) const
  /*!
    Writes out the imp list including
    those files that are required.
    \param OX :: output stream
    \param cellOutOrder :: List of cell in order to output
  */
{
  ELog::RegMethod RegA("PhysImp","write");
  
  std::map<int,double>::const_iterator vc;

  if (!impNum.empty())
    {
      const std::string pList=getMCNPparticleList
	(particles,excludeParticles);
      
      std::ostringstream cx;
      cx<<type<<pList;

      NRange A;
      std::vector<double> Index;
      for(const int& CN : cellOutOrder)
	{
	  vc=impNum.find(CN);
	  if (vc==impNum.end())
	    {
	      if (!defFlag)
		throw ColErr::InContainerError<int>(CN,"Cellnumber in i,pNum");
	      Index.push_back(defValue);
	    }
	  else
	    {
	      Index.push_back(vc->second);
	    }
	}

      A.setVector(Index);
      A.condense();
      A.write(cx);
      StrFunc::writeMCNPX(cx.str(),OX);
    }

  return;
}

} // NAMESPACE physicsSystem
