/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/fmeshTally.cxx
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
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <iterator>
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
  Tally(A)
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
fmeshTally::setIndexLine(std::string)
{
  return;
}
  
void
fmeshTally::setIndex(const size_t* IDX)
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
    }
  return;
}

int
fmeshTally::addLine(const std::string& LX)
{
  return Tally::addLine(LX);
}

void
fmeshTally::setCoordinates(const Geometry::Vec3D&,
                           const Geometry::Vec3D&)
  /*!
    Sets the min/max coordinates
    \param A :: First coordinate
    \param B :: Second coordinate
  */
{
  // Add some checking here
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
      const masterRotate& MR=masterRotate::Instance(); 
      //      MR.applyFull(minCoord);
      //      MR.applyFull(maxCoord);
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
  static char abc[]="abc";
  
  //  for(size_t i=0;i<3;i++)
    //    OX<<"cor"<<abc[i]<<IDnum<<" "<<minCoord[i]<<" "
    //      <<(Pts[i]-1)<<"i "<<maxCoord[i]<<std::endl;
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
      cx<<"GEOM="<<geomType<<" ";
      cx<<"ORIGIN="<<MW.Num(Origin)<<" ";
      
      //      std::vector<double>::const_iterator vc;
      //      for(vc=kIndex.begin();vc!=kIndex.end();vc++)
      //	cx<<MW.Num(*vc)<<" ";
      
      StrFunc::writeMCNPX(cx.str(),OX);
      if (!getEnergy().empty())
	{
	  cx.str("");
	  cx<<"ergsh"<<IDnum<<" "<<getEnergy();
	  StrFunc::writeMCNPX(cx.str(),OX);
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


