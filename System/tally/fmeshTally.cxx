/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/fmeshTally.cxx
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
#include <boost/format.hpp>

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
#include "Mesh3D.h"
#include "Triple.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "masterWrite.h"
#include "NList.h"
#include "NRange.h"
#include "pairRange.h"

#include "Tally.h"
#include "fmeshTally.h"

namespace tallySystem
{

fmeshTally::fmeshTally(const int ID) :
  Tally(ID),
  requireRotation(0)
  /*!
    Constructor
    \param ID :: Identity number of tally
  */
{}

fmeshTally::fmeshTally(const fmeshTally& A) : 
  Tally(A),
  typeID(A.typeID),keyWords(A.keyWords),
  requireRotation(A.requireRotation),Pts(A.Pts),
  minCoord(A.minCoord),maxCoord(A.maxCoord)
  /*!
    Copy constructor
    \param A :: fmeshTally to copy
  */
{}

fmeshTally&
fmeshTally::operator=(const fmeshTally& A)
  /*!
    Assignment operator
    \param A :: fmeshTally to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Tally::operator=(A);
      typeID=A.typeID;
      keyWords=A.keyWords;
      requireRotation=A.requireRotation;
      Pts=A.Pts;
      minCoord=A.minCoord;
      maxCoord=A.maxCoord;
    }
  return *this;
}

fmeshTally*
fmeshTally::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new fmeshTally(*this);
}

fmeshTally::~fmeshTally()
  /*!
    Destructor
  */
{}

void
fmeshTally::setType(const int T)
  /*!
    Set the mesh typeID values
    \param T :: Type to set [1,2,3]
  */
{
  if (T<1 || T>3)
    throw ColErr::RangeError<int>(T,1,4,"fmeshTally::setType");
  return;
}

void
fmeshTally::setKeyWords(const std::string&)
  /*!
    Set the mesh keywords
    \param K :: Keyword to add
  */
{
  return;
}
  
void
fmeshTally::setIndex(const std::array<size_t,3>& IDX)
  /*!
    Sets the individual index for each x,y,z
    \param IDX :: array of three object
  */
{
  ELog::RegMethod RegA("fmeshTally","setIndex");
  for(size_t i=0;i<3;i++)
    {
      if (!IDX[i])
	throw ColErr::IndexError<size_t>(IDX[i],i,"IDX[index] zero");
      Pts[i]=IDX[i];
    }
  
  return;
}

int
fmeshTally::addLine(const std::string& LX)
  /*!
    Add a line
   */
{
  return Tally::addLine(LX);
}

void
fmeshTally::setCoordinates(const Geometry::Vec3D& A,
                           const Geometry::Vec3D& B)
  /*!
    Sets the min/max coordinates
    \param A :: First coordinate
    \param B :: Second coordinate
  */
{
  ELog::RegMethod RegA("fmeshTally","setCoordinates");
  
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
fmeshTally::setResponse(const std::string&)
  /*!
    Set the mesh response function based on the input line
    \param Line :: Line to process
  */
{
  ELog::RegMethod RegA("fmeshTally","setResponse");
  //  if (mshmf.processString(Line))
    // {
    //   ELog::EM<<"Failed to set response line :"<<ELog::endCrit;
    //   ELog::EM<<Line<<ELog::endErr;
    // }
  return;
}

void
fmeshTally::rotateMaster()
  /*!
    Rotate the points [if required]
  */
{
  ELog::RegMethod RegA("fmeshTally","rotateMaster");
  
  if (requireRotation)
    {
      ELog::EM<<"Rotation required"<<ELog::endDiag;
      const masterRotate& MR=masterRotate::Instance(); 
      MR.applyFull(minCoord);
      MR.applyFull(maxCoord);
      requireRotation=0;
    }
  return;
}

void
fmeshTally::writeCoordinates(std::ostream& OX) const
  /*!
    Function to write out the coordinates for the user
    \param OX :: Oupt stream
  */
{
  const static char abc[]="ijk";
  
  std::ostringstream cx;
  for(size_t i=0;i<3;i++)
    {
      cx.str("");
      cx<<abc[i]<<"mesh "<<maxCoord[i]<<" "
	<<abc[i]<<"ints "<<Pts[i];
      StrFunc::writeMCNPXcont(cx.str(),OX);
    }

  
  return;
}
  
void
fmeshTally::write(std::ostream& OX) const
  /*!
    Write out the mesh tally into the tally region
    \param OX :: Output stream
   */
{
  masterWrite& MW=masterWrite::Instance();
  if (isActive())
    {
      std::ostringstream cx;
      cx<<"fmesh"<<IDnum;
      writeParticles(cx);
      //GEOMETRY:
      cx<<"GEOM="<<"xyz"<<" ";
      cx<<"ORIGIN="<<MW.Num(minCoord)<<" ";
      StrFunc::writeMCNPX(cx.str(),OX);
      cx.str("");

      std::vector<double> EPts;
      const size_t EN=getEnergy().writeVector(EPts);
      if (EN)
	{
	  StrFunc::writeMCNPX(cx.str(),OX);
	  cx.str("");
	}					 
      // if (!mshmf.empty())
      //   {
      //     cx.str("");
      //     cx<<"mshmf"<<IDnum<<" "<<mshmf;
      //     StrFunc::writeMCNPX(cx.str(),OX);
      //   }
      writeCoordinates(OX);
      OX<<"endmd"<<std::endl;
    }

  return;
}

}  // NAMESPACE tallySystem

