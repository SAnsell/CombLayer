/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/PSimple.cxx
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
#include "PSimple.h"

namespace physicsSystem
{

std::ostream&
operator<<(std::ostream& OX,const PSimple& A)
  /*!
    Write out the cut to the stream
    \param OX :: Output Stream
    \param A :: PSimple object
    \return Output stream
   */
{
  A.write(OX);
  return OX;
}

PSimple::PSimple(const std::string& Key) :
  PhysCard(),KeyName(Key)
  /*!
    Constructor
    \param Key :: KeyName
  */
{
  std::fill(defFlag.begin(),defFlag.end(),1);
} 

PSimple::PSimple(const PSimple& A) :
  PhysCard(),KeyName(A.KeyName),
  defFlag(A.defFlag),vals(A.vals)
  /*!
    Copy Constructor
    \param A :: PSimple to copy
  */
{}

PSimple&
PSimple::operator=(const PSimple& A) 
  /*!
    Assignment operator
    \param A :: PSimple to copy
    \return *this
  */
{
  if (this!=&A)
    {
      KeyName=A.KeyName;
      defFlag=A.defFlag;
      vals=A.vals;
    }
  return *this;
}

PSimple::~PSimple()
  /*!
    Destructor
  */
{}

PSimple*
PSimple::clone() const
  /*!
    Clone constructor 
    \return new(this)
  */
{
  return new PSimple(*this);
}

void
PSimple::clear()
  /*!
    Clear everything
   */
{
  std::fill(defFlag.begin(),defFlag.end(),1);
  return;
}

void
PSimple::setValues(const std::string& Line) 
  /*!
    Set the values from a card
    \param Line :: Line to process	       
  */
{
  ELog::RegMethod RegA("PSimple","setValues(string)");
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
PSimple::setValue(const size_t ID,const double value)
  /*!
    Set a special interest in a cell ID
    The existance of the cell is only 
    checked at the write out stage.
    \param ID :: Cell number
    \param value :: new value
  */
{
  if (ID>=5)
    throw ColErr::IndexError<size_t>(ID,5,"PSimple::setValue");
  defFlag[ID]=0;
  vals[ID]=value;
  return;
}

void
PSimple::setValues(const size_t NVal,const double A,const double B,
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
PSimple::setDef(const size_t ID)
  /*!
    Set an index to default.
    \param ID :: Index value
  */
{
  if (ID>=5)
    throw ColErr::IndexError<size_t>(ID,5,"PSimple::setDef");
  defFlag[ID]=1;
  return;
}

void
PSimple::setEnergyCut(const double value)
  /*!
    Sets the minimum energy: Does not lower the 
    energy if already set
    \param value :: new value
  */
{
  ELog::RegMethod RegA("PSimple","setEenergyCut");
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


double
PSimple::getValue(const size_t ID) const
  /*!
    Get the Value  for a given cell.
    \param ID :: Physics value to index
    \throw InContainerError when ID not found in Phys card
    \return Phys card value
  */
{
  if (ID>=5)
    throw ColErr::IndexError<size_t>(ID,5,"PSimple::getValue");
  return vals[ID];
}

void
PSimple::write(std::ostream& OX) const
  /*!
    Writes out the imp list including
    those files that are required.
    \param OX :: output stream
  */
{
  std::ostringstream cx;
  cx<<KeyName;
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

  return;
}



}  // NAMESPACE physicsSystem
