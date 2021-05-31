/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/FCLControl.cxx
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
#include "writeSupport.h"
#include "PhysicsCardFunctions.h"
#include "particleConv.h"

#include "Triple.h"
#include "NRange.h"

#include "FCLControl.h"

namespace physicsSystem
{

FCLControl::FCLControl() :
  wwgFlag(1)
  /*!
    Constructor
  */
{}

FCLControl::FCLControl(const FCLControl& A) :
  wwgFlag(A.wwgFlag),particles(A.particles),
  cells(A.cells)
  /*!
    Copy Constructor
    \param A :: FCLControl to copy
  */
{}

FCLControl&
FCLControl::operator=(const FCLControl& A) 
  /*!
    Assignment operator
    \param A :: FCLControl to copy
    \return *this
  */
{
  if (this!=&A)
    {
      wwgFlag=A.wwgFlag;
      particles=A.particles;
      cells=A.cells;
    }
  return *this;
}

FCLControl::~FCLControl()
  /*!
    Destructor
  */
{}


void
FCLControl::clear()
  /*!
    Clear everything
   */
{
  particles.clear();
  cells.clear();
  return;
}

void
FCLControl::addCell(const int ID)
  /*!
    Set a special interest in a cell ID
    The existance of the cell is only 
    checked at the write out stage.
    \param ID :: Cell number
  */
{
  cells.emplace(ID);
  return;
}

void
FCLControl::setParticle(const std::string& P)
 /*!
   Set the particles to be just P
   \param P :: PArticle 
 */
{
  ELog::RegMethod RegA("FCLControl","setParticle");
  const int index=particleConv::Instance().mcplITYP(P);
  particles.clear();
  particles.emplace(index);
  return;
}


void
FCLControl::addParticle(const std::string& pName) 
  /*!
    Adds an element to the particles list
    (note it also checks the elment)
    \param particelName :: element identifier
  */
{
  ELog::RegMethod RegA("FCLControl","addParticle");
  
  const int index=particleConv::Instance().mcplITYP(pName);
  particles.emplace(index);
  return;
}

bool
FCLControl::hasParticle(const std::string& E) const
  /*!
    Finds the item on the list if it exists
    Note this throws is E is not a valid particle
    \param E :: Particle string to find
    \return 0 :: failure , 1 :: success
  */
{
  ELog::RegMethod RegA("FCLControl","hasParticle");

  const int index=particleConv::Instance().mcplITYP(E);
  
  return (particles.find(index)!=particles.end());
}

bool
FCLControl::removeParticle(const std::string& PT)
  /*!
    Removes a particle type
    Note: throws if PT is not a real particle
    \param PT :: Particle type
    \return 1 :: success (or zero for failure)
   */
{
  ELog::RegMethod RegA("FCLControl","removeParticle");

  const int index=particleConv::Instance().mcplITYP(PT);
  
  std::set<int>::iterator vc=particles.find(index);
  if (vc==particles.end()) return 0;
  particles.erase(vc);
  return 1;
}
  
void
FCLControl::setCells(const std::set<int>& cellSet,
		     const double defValue)
  /*!
    Process the ordered list of the cells.
    \param cellSet :: list of cells
    \param defValue :: default value to use (set to 1.0)
  */
{
  wwgFlag= (defValue>=0.0) ? 1 : 0;

  cells=cellSet;
  return;
}

void
FCLControl::renumberCell(const int oldCellN,const int newCellN)
  /*!
    Renumbers a cell 
    \param oldCellN :: Old cell number
    \param newCellN :: New cell nubmer
  */
{
  ELog::RegMethod RegA("FCLControl","renumberCell");

  if (cells.empty() || oldCellN==newCellN) return;
  
  std::set<int>::iterator mc;
  if (cells.find(newCellN)!=cells.end())
    throw ColErr::InContainerError<int>(newCellN,"New cellN exists");
  mc=cells.find(oldCellN);
  if (mc==cells.end())
    throw ColErr::InContainerError<int>(oldCellN,"Old cellN not found ");

  // Doesn't invaidate 
  cells.erase(mc);
  cells.emplace(newCellN);

  return; 
}


void
FCLControl::write(std::ostream& OX,
		  const std::vector<int>& cellOutOrder) const
 /*!
    Writes out the imp list including
    those files that are required.
    \param OX :: output stream
    \param cellOutOrder :: List of cell in order to output
  */
{
  ELog::RegMethod RegA("FCLControl","write");
  
  
  if (!particles.empty() && !cells.empty())
    {
      std::ostringstream cx;
      cx<<"fcl";
      cx<<getMCNPparticleList(particles);

      NRange A;
      const double fcValue(wwgFlag ? -1.0 : 1.0);
      std::vector<double> Index;
      for(const int& CN : cellOutOrder)
	{
	  if (cells.find(CN)!=cells.end())
	    Index.push_back(fcValue);
	  else
	    Index.push_back(0.0);
	}

      A.setVector(Index);
      A.condense();
      A.write(cx);
      StrFunc::writeMCNPX(cx.str(),OX);
    }

  return;
}

} // NAMESPACE physicsSystem
