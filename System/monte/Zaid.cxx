/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/Zaid.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <sstream>
#include <cmath>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Triple.h"
#include "support.h"
#include "IsoTable.h"
#include "Element.h"
#include "Zaid.h"

std::ostream&
operator<<(std::ostream& OX,const Zaid& Z)
  /*!
    Output the zaid to a stream
    \param OX :: output stream
    \param Z :: Zaid to write
    \return Stream
  */
{
  Z.write(OX);
  return OX;
}

Zaid::Zaid() : 
  index(0),tag(0),type('c'),
  density(0.0)
  /*!
    Constructor
  */
{}

Zaid::Zaid(const Zaid& A) :
  index(A.index),tag(A.tag),type(A.type),
  density(A.density)
  /*!
    Copy Constructor
    \param A :: Zaid to copy
  */
{}

Zaid&
Zaid::operator=(const Zaid& A)
  /*!
    Assignment operator
    \param A :: Zaid to copy
    \return *this
  */
{
  if (this!=&A)
    {
      index=A.index;
      tag=A.tag;
      type=A.type;
      density=A.density;
    }
  return *this;
}

bool
Zaid::operator==(const Zaid& A) const
  /*!
    Determine if the zaids [excluding density] are equal
    \param A :: Object to check
    \return true if matched
   */
{
  return (A.index==index && 
	  A.tag==tag &&
	  A.type==type) ? 1  : 0;
}

bool
Zaid::isEquavilent(const int Z,const int T,const char C) const
  /*!
    Determine if the numbers are equivilent
    \param Z :: Zaid number [0 to ignore]
    \param T :: Tag number  [0 to ignore]
    \param C :: Type        [0 to ignore] 
    \return true if equal
  */
{
  return ((!Z || Z==index)  &&
	  (!T || T==tag) &&
	  (!C || C==type));
}


int
Zaid::setZaid(const std::string& A)
  /*!
    Given a zaid string determine if 
    it is a proper zaid and set
    \param A :: String to test
    \return 1 on success / 0 on failure
   */
{
  ELog::RegMethod RegA("Zaid","setZaid");

  int Z;  
  std::string::size_type pos=A.find('.');
  if ( pos==std::string::npos ||
       pos+4>A.size() ||
       !StrFunc::convert(A.substr(0,pos),Z)  ||
       !isdigit(A[pos+1]) || !isdigit(A[pos+2]) ||
       isdigit(A[pos+3]) ) 
    return 0;

  index=Z;
  const std::string Num=A.substr(pos+1,2);
  StrFunc::convert(Num,tag);
  type=A[pos+3];
  return 1;
}
  
    
void
Zaid::setDensity(const double D) 
  /*!
    Sets the density
    \param D :: Density in 
   */
{
  density=D;
  return;
}

double
Zaid::getAtomicMass() const
  /*!
    Get the precise atomic mass for 
    the isotope. If the zaid is a common zaid (X000) 
    then return the stable mean atomic mass, otherwize
    return the isotope mass
    \return mean atomic mass
  */
{
  const int INum=getIso();
  if (INum) 
    {
      const IsoTable& IT=IsoTable::Instance();
      return IT.getMass(getZ(),INum);
    }
  const Element& ET=Element::Instance();
  return ET.mass(getZ());
}    

void
Zaid::write(std::ostream& OX) const
  /*!
    Write out to an MCNPX zaid with density if (c)
    \param OX :: Output stream
   */
{
  boost::format FMTstr("%1$d.%2$02d%3$c");
  boost::format FMTnum("%1$.6g");
  OX<<FMTstr % index % tag % type;
  if (type=='c')
    OX<<" "<<FMTnum % density;
  return;
}

