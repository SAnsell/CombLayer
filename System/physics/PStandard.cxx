/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/PStandard.cxx
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
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <complex>
#include <list>
#include <map>
#include <algorithm>
#include <iterator>
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "PhysCard.h"
#include "PStandard.h"

namespace physicsSystem
{

std::ostream&
operator<<(std::ostream& OX,const PStandard& A)
  /*!
    Write out the cut to the stream
    \param OX :: Output Stream
    \param A :: PStandard object
    \return Output stream
   */
{
  A.write(OX);
  return OX;
}

PStandard::PStandard(const std::string& Key) :
  PhysCard(),KeyName(Key)
  /*!
    Constructor
    \param Key :: KeyName
  */
{
  std::fill(defFlag.begin(),defFlag.end(),1);
} 

PStandard::PStandard(const PStandard& A) :
  PhysCard(),KeyName(A.KeyName),particles(A.particles),
  defFlag(A.defFlag),vals(A.vals)
  /*!
    Copy Constructor
    \param A :: PStandard to copy
  */
{}

PStandard&
PStandard::operator=(const PStandard& A) 
  /*!
    Assignment operator
    \param A :: PStandard to copy
    \return *this
  */
{
  if (this!=&A)
    {
      KeyName=A.KeyName;
      particles=A.particles;
      defFlag=A.defFlag;
      vals=A.vals;
    }
  return *this;
}

PStandard::~PStandard()
  /*!
    Destructor
  */
{}

PStandard*
PStandard::clone() const
  /*!
    Clone constructor 
    \return new(this)
  */
{
  return new PStandard(*this);
}

void
PStandard::clear()
  /*!
    Clear everything
   */
{
  particles.clear();
  std::fill(defFlag.begin(),defFlag.end(),1);
  return;
}

void
PStandard::setValues(const std::string& Line) 
  /*!
    Set the values from a card
    \param Line :: Line to process	       
  */
{
  ELog::RegMethod RegA("PStandard","setValues(string)");
  std::string procLine=Line;
  std::string Unit;
  double V;
  for(size_t i=0;i<5;i++)
    {
      if (StrFunc::section(procLine,V))
	{
	  defFlag[i]=0;
	  vals[i]=V;
	}
      else if (StrFunc::section(procLine,Unit))

	{
	  if (tolower(Unit[0])=='j')
	    defFlag[i]=1;
	  else
	    ELog::EM<<"Mis-understood line:"<<Line<<" at "<<i+1<<ELog::endErr;
	}
      else 
	{
	  defFlag[i]=1;
	}
    }
  return;
}

void
PStandard::setValue(const size_t ID,const double value)
  /*!
    Set a special interest in a cell ID
    The existance of the cell is only 
    checked at the write out stage.
    \param ID :: Cell number
    \param value :: new value
  */
{
  if (ID>=5)
    throw ColErr::IndexError<size_t>(ID,5,"PStandard::setValue");
  defFlag[ID]=0;
  vals[ID]=value;
  return;
}

void
PStandard::setValues(const size_t NVal,const double A,const double B,
		   const double C,const double D,const double E)
  /*!
    Set the values
    \param NVal :: Number of values
    \param A :: First Item
    \param B :: Second Item
    \param C :: Third Item
    \param D :: Fourth Item
    \param E :: Fifth Item
  */
{
  vals[0]=A;
  vals[1]=B;
  vals[2]=C;
  vals[3]=D;
  vals[4]=E;
  for(size_t i=0;i<defFlag.size();i++)
     defFlag[i]=(i>=NVal) ? 1 : 0;
 
  return;
}

void
PStandard::setDef(const size_t ID)
  /*!
    Set an index to default.
    \param ID :: Index value
  */
{
  if (ID>=5)
    throw ColErr::IndexError<size_t>(ID,5,"PStandard::setDef");
  defFlag[ID]=1;
  return;
}

void
PStandard::setEnergyCut(const double value)
  /*!
    Sets the minimum energy: Does not lower the 
    energy if already set
    \param value :: new value
  */
{
  ELog::RegMethod RegA("PStandard","setEenergyCut");
  if (KeyName=="cut")
    {
      if (value>0.0 && (defFlag[1] || vals[1]<value))
	{
	  vals[1]=value;
	  defFlag[1]=0;
	}
    }
  return;
}

void
PStandard::addElm(const std::string& EN) 
  /*!
    Adds an element to the particles list
    (note it also checks the element ??)
    \param EN :: element identifier
  */
{
  particles.push_back(EN);
  return;
}

void
PStandard::addElmList(const std::string& EM) 
  /*!
    Adds an element to the particles list
    \param EM :: Add element list
  */
{
  std::string ELine=EM;
  std::string Item;
  while(StrFunc::section(ELine,Item))
    {
      particles.push_back(Item);
    }
  return;
}

bool
PStandard::hasElm(const std::string& E) const
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

double
PStandard::getValue(const size_t ID) const
  /*!
    Get the Value  for a given cell.
    \param ID :: Physics value to index
    \throw InContainerError when ID not found in Phys card
    \return Phys card value
  */
{
  if (ID>=5)
    throw ColErr::IndexError<size_t>(ID,5,"PStandard::getValue");
  return vals[ID];
}

int
PStandard::removeParticle(const std::string& PT)
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
PStandard::write(std::ostream& OX) const
  /*!
    Writes out the imp list including
    those files that are required.
    \param OX :: output stream
  */
{
  std::ostringstream cx;
  cx<<KeyName;
  if (!particles.empty())
    {
      // Write out cut:n,x list
      cx<<":";
      std::list<std::string>::const_iterator pc=particles.begin();
      cx<<(*pc++);
      for(;pc!=particles.end();pc++)
	cx<<","<<(*pc);
      // get last non-zero item
      size_t noJ=defFlag.size();
      for(;noJ>0 && defFlag[noJ-1];noJ--) ;
      for(size_t i=0;i<noJ;i++)
        { 
	  if (defFlag[i])
	    cx<<" j";
	  else
	    cx<<" "<<vals[i];
	}
      StrFunc::writeMCNPX(cx.str(),OX);
    }

  return;
}



}  // NAMESPACE physicsSystem
