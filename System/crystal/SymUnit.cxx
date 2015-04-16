/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   crystal/SymUnit.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "SymUnit.h"

namespace Crystal
{

std::ostream&
operator<<(std::ostream& OX,const SymUnit& A)
  /*!
    Write to a standard stream
    \param OX :: Stream
    \param A :: SymUnit to write
    \return Stream
  */
{
  A.write(OX);
  return OX;
}

 SymUnit::SymUnit()  :  
   ShiftVec(0,0,0)
  /*!
    Default constructor
    Set to basic translation
  */
{
  for(int i=0;i<3;i++)
    {
      Geometry::Vec3D unit;
      unit[i]=1.0;
      OpSet[i]=unit;
    }
}

SymUnit::SymUnit(const SymUnit& A) :
  ShiftVec(A.ShiftVec)    
  /*!
    Copy constructor
    \param A :: SymUnit to copy
  */
{
  for(int i=0;i<3;i++)
    OpSet[i]=A.OpSet[i];
}

SymUnit&
SymUnit::operator=(const SymUnit& A) 
  /*!
    Assignment operator
    \param A :: SymUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      for(size_t i=0;i<3;i++)
	OpSet[i]=A.OpSet[i];
      ShiftVec=A.ShiftVec;
    }
  return *this;
}

SymUnit::~SymUnit()
  /// Destructor
{}

void
SymUnit::setComponent(const size_t index,
		      const Geometry::Vec3D& Unit)
  /*!
    Unchecked assignment to atom type
    \param index :: index number to change
    \param Unit :: Vector to translater
  */
{
  if (index>3)
    throw ColErr::IndexError<int>(index,3,"SymUnit::setComponent");
  if (index==3)
    ShiftVec=Unit;
  else
    OpSet[index]=Unit;
  return;
}

int
SymUnit::setLine(const std::string& Line)
  /*!
    Process a line
    \param Line :: input line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("SymUnit","setLine");
  
  std::string LX;
  std::string::size_type posA(0);
  std::string::size_type posB(0);
  ShiftVec=Geometry::Vec3D(0,0,0);
  int i;
  double digit;
  for(i=0;i<3 && posB!=std::string::npos;i++)
    {
      OpSet[i]=Geometry::Vec3D(0,0,0);
      posB=Line.find(',',posA);
      std::string Part=Line.substr(posA,posB-posA);
      posA=posB+1;
      int sign(1);
      const long int PSize(Part.length());
      for(int j=0;j<PSize;j++)
        {
	  char PV=static_cast<char>(tolower(Part[j]));
	  
	  if (PV=='+') 
	    sign=1;
	  else if (PV=='-') 
	    sign=-1;
	  else if (PV=='x')
	    OpSet[i][0]=sign;
	  else if (PV=='y')
	    OpSet[i][1]=sign;
	  else if (PV=='z')
	    OpSet[i][2]=sign;
	  else if (Part.substr(j,2)=="1/" &&
		   StrFunc::convert(Part.substr(j+2,1),digit))
	    ShiftVec[i]=sign/digit;
	  else if (Part.substr(j,2)=="3/" &&
		   StrFunc::convert(Part.substr(j+2,1),digit))
	    ShiftVec[i]=(3*sign)/digit;
	}
    }
  // No enough points :
  if (i!=3)
    return -1;
  return 0;
}

Geometry::Vec3D
SymUnit::unitPoint(const Geometry::Vec3D& Pt) const
  /*!
    Finds all the points based on a point that
    are in a unit cell: 
    \param Pt :: Point to [for 0->1 , 0-1 ,0-1]
    \return Master atoms
   */
{
  // No check to see if Pt is 0->1:
  Geometry::Vec3D UPt(0,0,0);

  for(int index=0;index<3;index++)
    {
      for(int i=0;i<3;i++)
	UPt[index] += Pt[i]*OpSet[index][i];
      UPt[index] +=ShiftVec[index];
    }
  UPt.boundaryCube(Geometry::Vec3D(0,0,0),Geometry::Vec3D(1.000,1.000,1.000));
  return UPt;
}

int
SymUnit::symPoint(const Geometry::Vec3D& Pt,Geometry::Vec3D& UPt) const
  /*!
    Moves a point based on fractions of the unit cell 
    into cartesian coordinates.

    \param Pt :: Vector of unit cell dimensions
    \param UPt :: Real point in cartesian form
    \retval 0 :: Failed to put point in box [ Not checked ]
    \retval 1 :: Success  point in box
   */
{
  UPt=Geometry::Vec3D(0,0,0);
  for(int index=0;index<3;index++)
    {
      for(int i=0;i<3;i++)
	UPt[index] += Pt[i]*OpSet[index][i];
      UPt[index] +=ShiftVec[index];
    }

  return 1;
}

std::string
SymUnit::str() const
  /*!
    Get a string 
    \return this line string
  */
{
  std::ostringstream cx;
  write(cx);
  return cx.str();
}

void
SymUnit::write(std::ostream& OX) const
  /*!
    Write to standard output. Note care is taken
    to put it back in cif form.
    \param OX :: Output stream
   */
{
  const char xyz[]="xyz";
  for(int i=0;i<3;i++)
    {
      int first(1);
      for(int j=0;j<3;j++)
        {
	  if (fabs(OpSet[i][j])>0.5)
	    {
	      if (OpSet[i][j]>0.0 && !first)
		OX<<"+";
	      else if (OpSet[i][j]<0.0)
		OX<<"-";
	      OX<<xyz[j];
	      first=0;
	    }
	}
      if (fabs(ShiftVec[i])>1e-3)
        {
	  if (ShiftVec[i]>0.0 && !first)
	    OX<<"+";
	  OX<<ShiftVec[i];
	}
      if (i!=2)
	OX<<",";
    }
  return;
}




}  // NAMESPACE Crystal

