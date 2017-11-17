/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/PhysImp.cxx
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
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "Triple.h"
#include "NRange.h"
#include "MapSupport.h"
#include "MapRange.h"
#include "ZoneUnit.h"
#include "PhysImp.h"

namespace physicsSystem
{

PhysImp::PhysImp() :
  type("None"),nCutters(0)
  /*!
    Constructor
  */
{}

PhysImp::PhysImp(const std::string& TP) :
  type(TP),nCutters(0)
  /*!
    Constructor with Type
    \param TP :: Type identifier
  */
{}

PhysImp::PhysImp(const PhysImp& A) :
  type(A.type),particles(A.particles),
  impNum(A.impNum),nCutters(A.nCutters)
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
      type=A.type;
      particles=A.particles;
      impNum=A.impNum;
      nCutters=A.nCutters;
    }
  return *this;
}

PhysImp::~PhysImp()
  /*!
    Destructor
  */
{}

const std::list<std::string>&
PhysImp::getParticleList() const
  /*!
    Accessor to particle list
  */
{
  return particles;
}
  
void
PhysImp::clear()
  /*!
    Clear everything
   */
{
  particles.clear();
  impNum.erase(impNum.begin(),impNum.end());
  nCutters=0;
  return;
}

void
PhysImp::setValue(const int ID,const double value)
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

void
PhysImp::setAllCells(const double value)
  /*!
    Set a special interest in a cell ID
    The existance of the cell is only 
    checked at the write out stage.
    \param value :: new value
  */
{
  std::map<int,double>::iterator mc;
  for(mc=impNum.begin();mc!=impNum.end();mc++)
    mc->second=value;
  return;
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
      nCutters++;
    }
  return;
}

void
PhysImp::modifyCells(const std::vector<int>& cellOrder,const double defValue)
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
  
void
PhysImp::setCells(const std::vector<int>& cellOrder,const double defValue)
  /*!
    Process the ordered list of the cells.
    If the cell number does not exists a default value
    of 1 is added.
    \param cellOrder :: list of cells
    \param defValue :: default value to use (set to 1.0)
  */
{
  std::vector<int>::const_iterator vc;
  std::map<int,double>::const_iterator ac;
  std::map<int,double> nMap;
  for(vc=cellOrder.begin();vc!=cellOrder.end();vc++)
    {
      ac=impNum.find(*vc);
      nMap[*vc]=(ac!=impNum.end()) ? ac->second : defValue;
    }
  impNum=nMap;
  return;
}

void
PhysImp::setAllCells(const std::vector<int>& cellOrder,
		     const std::vector<double>& impValue)
  /*!
    Process the ordered list of the cells.
    If the cell number does not exists a default value
    of 1 is added.
    \param cellOrder :: list of cells
    \param impValue :: list of importance values
  */
{
  std::vector<int>::const_iterator vc;
  std::vector<double>::const_iterator ic;
  std::map<int,double>::const_iterator ac;
  std::map<int,double> nMap;
  ic=impValue.begin();
  for(vc=cellOrder.begin();vc!=cellOrder.end();vc++,ic++)
    {
      nMap[*vc]=*ic;
    }
  impNum=nMap;
  return;
}

void
PhysImp::addElm(const std::string& EN) 
  /*!
    Adds an element to the particles list
    (note it also checks the element ??)
    \param EN :: element identifier
  */
{
  particles.push_back(EN);
  return;
}

int
PhysImp::hasElm(const std::string& E) const
  /*!
    Finds the item on the list if it exists
    \param E :: Particle string to find
    \return 0 :: failure , 1 :: success
  */
{
  std::list<std::string>::const_iterator vc;
  vc=find(particles.begin(),particles.end(),E);
  return (vc==particles.end()) ? 0 : 1;
}

std::string
PhysImp::getParticles() const
  /*!
    Convert the particle list to a string
    \return particle list as a comma separated list
  */
{  
  if (particles.empty()) return "";
  std::ostringstream cx;
  std::copy(particles.begin(),particles.end(),
            std::ostream_iterator<std::string>(cx,","));
  std::string Out(cx.str());
  Out.pop_back();
  return Out;
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
    throw ColErr::InContainerError<int>(oldCellN,"New cell not found");
  mc=impNum.find(oldCellN);
  if (mc==impNum.end())
    throw ColErr::InContainerError<int>(oldCellN,"Old cell not found "+
					RegA.getFull());    

  // Doesn't invaidate 
  impNum.insert(ITYPE::value_type(newCellN,mc->second));
  impNum.erase(mc);

  return; 
}

int
PhysImp::removeParticle(const std::string& PT)
  /*!
    Removes a particle type
    \param PT :: Particle type
    \return 1 :: success (or zero for failure)
   */
{
  std::list<std::string>::iterator vc;
  vc=find(particles.begin(),particles.end(),PT);
  if (vc==particles.end())
    return 0;
  particles.erase(vc);
  return 1;
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
      std::ostringstream cx;
      cx<<type;
      // Write out imp:n,x list
      if (!particles.empty())
        {
	  char separator(':');
	  for(const std::string& pType : particles)
	    {
	      if (excludeParticles.find(pType) == excludeParticles.end())
		{
		  cx<<separator<<pType;
		  separator=',';
		}
	    }
	  // no work to do
	  if (separator==':')
	    return;
	}
      cx<<" ";

      NRange A;
      std::vector<double> Index;
      std::vector<int>::const_iterator cvec;
      for(cvec=cellOutOrder.begin();cvec!=cellOutOrder.end();cvec++)
	{
	  vc=impNum.find(*cvec);
	  if (vc==impNum.end())
	    {
	      ELog::EM<<"Unable to find cell "<<*cvec<<ELog::endCrit;
	      throw ColErr::InContainerError<int>(*cvec,"Cellnumber in i,pNum");
	    }
	  Index.push_back(vc->second);
	}

      A.setVector(Index);
      for(int i=0;i<nCutters;i++)
	A.addComp(0.0);
      A.condense();  
      A.write(cx);
      StrFunc::writeMCNPX(cx.str(),OX);
    }

  return;
}

} // NAMESPACE physicsSystem
