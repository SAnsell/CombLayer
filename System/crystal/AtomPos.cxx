/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   crystal/AtomPos.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <list>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "AtomPos.h"

namespace Crystal
{

std::ostream& 
operator<<(std::ostream& OX,const AtomPos& A)
  /*!
    Writes out the AtomPos
    \param OX :: Output stream
    \param A :: AtomPos to write
    \return Current state of stream
  */
{
  A.write(OX);
  return OX;
}

AtomPos::AtomPos() : 
  Name("Unset"),Z(0),Ion(0),occ(1.0)
  /*!
    Default constructor
  */
{}

AtomPos::AtomPos(const std::string& N,const int Znum,const int I,
		 const Geometry::Vec3D& Pt,const double OVal) :
  Name(N),Z(Znum),Ion(I),Cent(Pt),occ(OVal)
  /*!
    Assignment constructor
    \param N :: Name
    \param Znum :: Z number
    \param I :: Ion value
    \param Pt :: Centre Point 
    \param OVal :: occupation value
  */
{}

AtomPos::AtomPos(const AtomPos& A) :
  Name(A.Name),Z(A.Z),Ion(A.Ion),Cent(A.Cent),
  occ(A.occ)
  /*!
    Copy constructor
    \param A :: AtomPos to copy
  */
{}

AtomPos&
AtomPos::operator=(const AtomPos& A) 
  /*!
    Assignment operator
    \param A :: AtomPos to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Name=A.Name;
      Z=A.Z;
      Ion=A.Ion;
      Cent=A.Cent;
      occ=A.occ;
    }
  return *this;
}

AtomPos::~AtomPos()
  /// Destructor
{}


bool
AtomPos::operator<(const AtomPos& A) const
  /*!
    Assignment operator
    \param A :: AtomPos to copy
    \return *this
  */
{
  if (this==&A)
    return 0;
  for(int i=0;i<3;i++)
    {
      if (fabs(Cent[i]-A.Cent[i])>1e-3)
	return Cent[i]<A.Cent[i];
    }
  // within +/- 1e-3
  return 0;
}

void
AtomPos::setAtom(const int Znum,const int Ival)
  /*!
    Set the atom value
    \param Znum :: Atom number
    \param Ival :: Ion value
   */
{
  if (Znum<0 || Z>110)
    throw ColErr::RangeError<int>(Znum,0,110,"AtomPos::setAtoms");
  
  Z=Znum;
  Ion=Ival;
  return;
}

void
AtomPos::setPos(const Geometry::Vec3D& Pt)
  /*!
    Set the atom position
    \param Pt :: Atom point
    \todo Do we check it is in the 0 1 cube.
   */
{
  Cent=Pt;
  return;
}

void 
AtomPos::setOcc(const double D)
  /*!
    Set atom occupancy
    \param D :: Value to use
    \throw RangeError ::  D not between 0-1.
   */
{
  if (D<0.0 || D>1.0)
    throw ColErr::RangeError<double>(D,0,1,"AtomPos::setOcc");
  occ=D;
  return;
}

int
AtomPos::equal(const AtomPos& A, const double DTol) const
  /*!
    Calculate if two atoms are coincident
    \param A :: Other AtomPos to compare
    \param DTol :: Tolerance of the centre distances
    
    \return -1 :: Mismatch in atom type and equal
    \retval 0 :: Not coincident
    \retval 1 :: Conincident
   */
{
  const double D(Cent.Distance(A.Cent));
  if (D>DTol)
    return 0;
  if (Z==A.Z && Ion==A.Ion)
    return 1;
  return -1;
}

double
AtomPos::getEdgeFactor() const
  /*!
    Determine if the particle is on the edge and
    return a corresponding fraction 
  */
{
  int edgeCount(0);
  for(int i=0;i<3;i++)
    {
      if (Cent[i]<1e-2 || Cent[i]>1.0-1e-2)
	edgeCount++;
    }
  return 2.0/(2 << edgeCount);
}

void
AtomPos::write(std::ostream& OX) const
  /*!
    Write the output to a steam following the 
    cif formalism.
    \param OX :: Output stream
   */
{
  OX<<Name<<" "<<Z<<" "<<Ion<<" "
    <<Cent<<" "<<occ;
  return;
}

}  // NAMESPACE Crystal

