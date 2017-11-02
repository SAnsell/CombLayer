/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Surface.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <complex>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "GTKreport.h"
#include "FileReport.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Transform.h"
#include "Line.h"
#include "Surface.h"

namespace Geometry
{

std::ostream&
operator<<(std::ostream& OX,const Surface& A)
  /*!
    Write out the stream. Uses virtual system
    \param OX :: Output stream
    \param A :: Surface to write 
    \return Stream 
   */
{
  A.write(OX);
  return OX;
}

Surface::Surface() : 
  Name(-1),TransN(0)
  /*!
    Constructor
  */
{}

Surface::Surface(const int N,const int T) : 
  Name(N),TransN(T)
  /*!
    Constructor
    \param N :: Name 
    \param T :: Transform
  */
{}

Surface::Surface(const Surface& A) : 
  Name(A.Name),TransN(A.TransN)
  /*!
    Copy constructor
    \param A :: Surface to copy
  */
{}


Surface&
Surface::operator=(const Surface& A)
  /*!
    Assignment operator
    \param A :: Surface to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Name=A.Name;
      TransN=A.TransN;
    }
  return *this;
}

Surface::~Surface()
  /*!
    Destructor
  */
{}

void
Surface::processSetHead(std::string& Line)
/*!
  Process the incoming header line to strip name { transN}
  from the start of the object description IF it exists.
  \param Line :: Line to process
 */
{
  int cellN;
  if (StrFunc::section(Line,cellN))
    {
      if (cellN>0)
        {
	  setName(cellN);
	  if (StrFunc::section(Line,cellN) && cellN>=0)
	    setTrans(cellN);
	}
    }
  return;
}

void 
Surface::print() const
  /*!
    Simple print out function for surface header 
  */
{
  std::cout<<"Surf (name,trans) == "<<Name<<" : "<<TransN<<" ";
  return;
}

void
Surface::writeHeader(std::ostream& OX) const
  /*!
    Writes out the start of an MCNPX surface description .
    Does not check the length etc
    \param OX :: Output stream
  */
{
  OX<<Name<<" ";
  if (TransN>0)
    OX<<TransN<<" ";
  return;
}

std::string
Surface::stripID(const std::string& Line)
 /*!
   Strip the name + transform number off 
   of the line and return the remainder
   \param Line :: full line to strip
   \return String starting with the 
  */
{
  std::string LOut=Line;
  int nx;
  if (StrFunc::section(LOut,nx))
    {
      Name=nx;
      if (StrFunc::section(LOut,nx))
	TransN=nx;
    }
  return LOut;
}

int
Surface::applyTransform(const std::map<int,Transform>& TMap)
  /*! 
     Applies the appropiate transform from the system to the surface.
     \param TMap - list of known transforms
     \retval 0 :: No transform needs appling
     \retval 1 :: Tranform successfully applied
     \retval -1 :: Transform could not be found in TMap
  */
{
  ELog::RegMethod RegA("Surface","applyTransform");
  if (TransN<=0) return 0;

  std::map<int,Transform>::const_iterator sm=TMap.find(TransN);
  if (sm==TMap.end())
    {
      ELog::EM<<"Out of range (cell="<<Name<<
	", Tr="<<TransN<<ELog::endErr;
      return -1;
    }
  this->rotate(sm->second.rotMat());
  this->displace(sm->second.shift());
  TransN*=-1;
  return 1;
}

int
Surface::sideDirection(const Geometry::Vec3D& Pt,
		       const Geometry::Vec3D& uVec) const
  /*!
    Determine the direction of a beam crossing 
    a surface
    \param Pt :: Point on surface
    \param uVec :: Direction line
    \return sign of the dot product
  */
{
  const Geometry::Vec3D N=surfaceNormal(Pt);
  return (N.dotProd(uVec)>0.0) ? 1 : -1;
}


void
Surface::rotate(const Geometry::Quaternion& QM)
  /*!
    Rotate the surface by a quaternion
    \param QM :: Quaternion to use 
  */
{
  this->rotate(QM.rMatrix());
  return;
}
  

void
Surface::writeXML(const std::string&) const
  /*!
    The generic XML writing system.
    This should call the virtual function procXML
    to correctly built the XMLcollection.
    \param :: Filename 
  */
{
//  XML::XMLwriteVisitor XVisit;
//  this->acceptVisitor(XVisit);
//  XVisit.writeFile(Fname);
  return;
}

  
}  // NAMESPACE Geometry
