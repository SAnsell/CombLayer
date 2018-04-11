/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/userBin.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <iterator>
#include <array>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "writeSupport.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Mesh3D.h"

#include "flukaTally.h"
#include "userBin.h"

namespace flukaSystem
{

userBin::userBin(const int outID) :
  flukaTally("mesh"+std::to_string(outID),outID),
  meshType(10),particle("208")
  /*!
    Constructor
    \param outID :: Identity number of tally [fortranOut]
  */
{}

userBin::userBin(const userBin& A) : 
  flukaTally(A),
  meshType(A.meshType),particle(A.particle),
  Pts(A.Pts),minCoord(A.minCoord),
  maxCoord(A.maxCoord)
  /*!
    Copy constructor
    \param A :: userBin to copy
  */
{}

userBin&
userBin::operator=(const userBin& A)
  /*!
    Assignment operator
    \param A :: userBin to copy
    \return *this
  */
{
  if (this!=&A)
    {
      flukaTally::operator=(A);
      meshType=A.meshType;
      particle=A.particle;
      Pts=A.Pts;
      minCoord=A.minCoord;
      maxCoord=A.maxCoord;
    }
  return *this;
}

  
userBin*
userBin::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new userBin(*this);
}

userBin::~userBin()
  /*!
    Destructor
  */
{}
  
void
userBin::setParticle(const std::string& P)
  /*!
    Set the mesh particle
    \param P :: Partile
  */
{
  particle=P;
  return;
}

void
userBin::setDoseType(const std::string& P,
		     const std::string& D)
  /*!
    Set the auxParticle [can be a range?]
    \param P :: particle to use
    \param D :: set dose type
  */
{
  ELog::RegMethod RegA("userBin","setDoseType");

  flukaTally::setDoseType(P,D);
  return;
}

  
void
userBin::setIndex(const std::array<size_t,3>& IDX)
  /*!
    Sets the individual index for each x,y,z
    \param IDX :: array of three object
  */
{
  ELog::RegMethod RegA("userBin","setIndex");
  for(size_t i=0;i<3;i++)
    {
      if (!IDX[i])
	throw ColErr::IndexError<size_t>(IDX[i],i,"IDX[index] zero");
      Pts[i]=IDX[i];
    }
  
  return;
}


void
userBin::setCoordinates(const Geometry::Vec3D& A,
			const Geometry::Vec3D& B)
  /*!
    Sets the min/max coordinates
    \param A :: First coordinate
    \param B :: Second coordinate
  */
{
  ELog::RegMethod RegA("userBin","setCoordinates");
  
  minCoord=A;
  maxCoord=B;
  // Add some checking here
  for(size_t i=0;i<3;i++)
    {
      if (std::abs(minCoord[i]-maxCoord[i])<Geometry::zeroTol)
	throw ColErr::NumericalAbort(StrFunc::makeString(minCoord)+" ::: "+
				     StrFunc::makeString(maxCoord)+
				     " Equal components");
      if (minCoord[i]>maxCoord[i])
	std::swap(minCoord[i],maxCoord[i]);
    }


  return;
}

void
userBin::writeAuxScore(std::ostream& OX) const
  /*!
    Write an auxScore card
    \param OX :: Ouput stream
  */
{
  if (!auxParticle.empty() && particle=="DOSE-EQ")
    {
      std::ostringstream cx;
      cx<<"AUXSCORE USRBIN "<<auxParticle<<" - "<<keyName
	<<" "<<keyName<<" - "<<doseType;
      StrFunc::writeFLUKA(cx.str(),OX);  
    }
  return;
}

  
void
userBin::write(std::ostream& OX) const
  /*!
    Write out the mesh tally into the tally region
    \param OX :: Output stream
   */
{
  std::ostringstream cx;
  
  cx<<"USRBIN "<<meshType<<" "<<particle<<" "
    <<outputUnit<<" "<<maxCoord;  
  cx<<" mesh"<<std::to_string(outputUnit);
  
  StrFunc::writeFLUKA(cx.str(),OX);

  cx.str("");
  cx<<"USRBIN "<<minCoord<<" ";
  
  for(size_t i=0;i<3;i++)
    cx<<Pts[i]<<" ";
  cx<<"  & ";
  StrFunc::writeFLUKA(cx.str(),OX);  
  writeAuxScore(OX);
  return;
}

}  // NAMESPACE flukaSystem

