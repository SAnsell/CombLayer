/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/sswTally.cxx
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
#include <iomanip>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <complex>
#include <string>
#include <sstream>
#include <list>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Transform.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "sswTally.h"

namespace tallySystem
{

sswTally::sswTally(const int ID) :
  Tally(ID)
  /*!
    Constructor
    \param ID :: Identity number of tally
  */
{}

sswTally::sswTally(const sswTally& A) :
  Tally(A),surfList(A.surfList)
  /*!
    Copy Constructor
    \param A :: sswTally to copy
  */
{}

  
sswTally&
sswTally::operator=(const sswTally& A)
  /*!
    Assignment operator
    \param A :: sswTally to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Tally::operator=(A);
      surfList=A.surfList;
    }
  return *this;
}

sswTally*
sswTally::clone() const
  /*!
    Clone object
    \return new (*this)
  */
{
  return new sswTally(*this);
}

sswTally::~sswTally()
  /*!
    Destructor
  */
{}

void
sswTally::addSurfaces(const std::vector<int>& surfVec)
  /*!
    Add surfaces to the tally into the NList form
    \param surfVec :: surface vector
   */
{
  ELog::RegMethod RegA("sswTally","addSurfaces");
  
  surfList.insert(surfList.end(),surfVec.begin(),surfVec.end());
  return;
}

void
sswTally::renumberSurf(const int oldN,const int newN)
  /*!
    Renumber the cell based on the old/New numbers
    \param oldN :: Old number to find
    \param newN :: new number to replace the surface with
  */
{
  ELog::RegMethod RegA("ssWTally","renumberCell");

  // handle the sign change in +/- numbers
  std::replace_if(surfList.begin(),surfList.end(),
		  [&oldN](const int& x) { return (x==oldN); },
		  newN);
  std::replace_if(surfList.begin(),surfList.end(),
		  [&oldN](const int& x) { return (x== -oldN); },
		  -newN);
  return;
}

void
sswTally::write(std::ostream& OX) const
  /*!
    Writes out the point tally depending on the 
    fields that have been set.
    \param OX :: Output Stream
  */
{
  ELog::RegMethod RegA("sswTally","write");

  if (!isActive())
    return;
  
  std::stringstream cx;
  cx<<"ssw";
  for(const int CN: surfList)
    cx<<" "<<CN;

  if (!Particles.empty())
    {
      cx<<" pty=";
      std::string extra;
      for(const std::string& PN : Particles)
        cx<<PN+" ";
    }
  
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}


}   // NAMESPACE tallySystem
