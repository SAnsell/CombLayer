/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTally/phitsTally.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <sstream>
#include <fstream>
#include <complex>
#include <cmath>
#include <string>
#include <list>
#include <set>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>

#include "FileReport.h"
#include "OutputLog.h"
#include "support.h"
#include "phitsTally.h"

namespace phitsSystem
{

phitsTally::phitsTally(const int ID)  :
  keyName(std::to_string(ID)),
  idNumber(ID),particle("all"),
  epsFlag(0),vtkout(0),vtkBinary(0)
  /*!
    Constructor 
    \param ID :: phitsTally ID number
  */
{}

phitsTally::phitsTally(const std::string& KName,const int ID)  :
  keyName(KName+std::to_string(ID)),idNumber(ID),
  particle("all"),
  epsFlag(0),vtkout(0),vtkBinary(0)
  /*!
    Constructor 
    \param KName :: keyname
    \param ID :: phitsTally ID number
  */
{}

phitsTally::phitsTally(const phitsTally& A) : 
  keyName(A.keyName),idNumber(A.idNumber),
  comments(A.comments),particle(A.particle),
  title(A.title),xTxt(A.xTxt),
  yTxt(A.yTxt),epsFlag(A.epsFlag),
  vtkout(A.vtkout),vtkBinary(A.vtkBinary)
  /*!
    Copy constructor
    \param A :: phitsTally to copy
  */
{}

phitsTally&
phitsTally::operator=(const phitsTally& A)
  /*!
    Assignment operator
    \param A :: phitsTally to copy
    \return *this
  */
{
  if (this!=&A)
    {
      comments=A.comments;
      particle=A.particle;
      
      title=A.title;
      xTxt=A.xTxt;
      yTxt=A.yTxt;

      epsFlag=A.epsFlag;
      vtkout=A.vtkout;
      vtkBinary=A.vtkBinary;
    }
  return *this;
}


phitsTally*
phitsTally::clone() const
  /*!
    Basic clone class
    \return new this
  */
{
  return new phitsTally(*this);
}

phitsTally::~phitsTally()
 /*!
   Destructor
 */
{}

void
phitsTally::setComment(const std::string& C)
  /*!
    Set the comment line
    \param C :: New comment
  */
{
  comments=C;
  return;
}


void
phitsTally::setParticle(const std::string& P)
  /*!
     Call for particle detectors
     \param P :: valid particles (outside of phits) ( all )
  */
{
  particle= (StrFunc::isEmpty(P)) ? "all" : P;
  return;
}

void
phitsTally::setEnergy(const eType&)
  /*!
    Null op call for non-energy detectors
  */
{}

void
phitsTally::setAngle(const aType&)
 /*!
   Null op call for non-angle detectors
 */
{}

void
phitsTally::setVTKout()
 /*!
   Set the output for vtk format
 */
{
  vtkout=1;
}

void
phitsTally::setBinary()
 /*!
   Set the output for vtk into binary form
 */
{
  vtkBinary=1;
}
  

void
phitsTally::write(std::ostream&,const std::string&) const
  /*!
    Writes out the phitsTally depending on the 
    fields that have been set.
    \param OX :: Output Stream
  */
{
  return;
}

}  // NAMESPACE phitsSystem
