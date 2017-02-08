/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/NullSurface.cxx
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
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Transform.h"
#include "Surface.h"
#include "NullSurface.h"

namespace Geometry
{

NullSurface::NullSurface() : Surface()
 /*!
   Constructor sets void object
 */	     
{}

NullSurface::NullSurface(const int ID,const int TN) : 
  Surface(ID,TN)
 /*!
   Constructor sets void object
   \param ID :: Id number
   \param TN :: Transform number
 */	     
{}

NullSurface::NullSurface(const NullSurface& A) : 
  Surface(A)
  /*!
    Copy Constructor
    \param A :: NullSurface to copy
  */
{}

NullSurface*
NullSurface::clone() const
  /*!
    Makes a clone (implicit virtual copy constructor) 
    \return new(this)
  */
{
  return new NullSurface(*this);
}

NullSurface&
NullSurface::operator=(const NullSurface& A) 
  /*!
    Assignment operator
    \param A :: NullSurface to copy
    \return *this
  */
{
  if (&A!=this)
    {
      this->Surface::operator=(A);
    }
  return *this;
}

NullSurface::~NullSurface()
  /*!
    Destructor
  */
{}

int
NullSurface::operator==(const NullSurface&) const
  ///< Effective equals operator (always true)
{
  return 1;
}

int
NullSurface::setSurface(const std::string& Pstr)
  /*!
    Sets a surface and all surfaces are valid
    \param Pstr :: Initial string
    \return 0 on success/ -ve on failure
  */
{
  this->stripID(Pstr);
  return (getName()<=0) ? 0 : -1;
}

void
NullSurface::rotate(const Geometry::Matrix<double>&) 
  /*!
    Rotate the plane about the origin by MA 
  */
{
  return;
}

void
NullSurface::displace(const Geometry::Vec3D&) 
  /*!
    Displace the plane by Point Sp.  
    i.e. r+sp now on the rectable
  */
{
  return;
}

double
NullSurface::distance(const Geometry::Vec3D&) const
  /*!
    Determine the distance of point A to the surface
    returns a value relative to the normal
    \returns singled distance from point
  */
{
  throw ColErr::AbsObjMethod("NullSurface::distance");  
}

int
NullSurface::side(const Geometry::Vec3D&) const
  /*!
    Calculates if the point is inside the object :
    on the side or outside.
    \return NONE exit via throw.
  */
{
  throw ColErr::AbsObjMethod("NullSurface::side");  
}

Geometry::Vec3D 
NullSurface::surfaceNormal(const Geometry::Vec3D&) const
  ///<  Calculate the normal at the point
{
  throw ColErr::AbsObjMethod("NullSurface::surfaceNormal");  
}

int
NullSurface::onSurface(const Geometry::Vec3D&) const
///<   Calcuate the side that the point is on
{
  throw ColErr::AbsObjMethod("NullSurface::onSurface");  
}

void 
NullSurface::print() const
  /*!
    Prints out the surface info and
    the NullSurface info.
  */
{
  std::cout<<"----- Null Surface -----"<<std::endl;
  Surface::print();
  std::cout<<"----- Null Surface -----"<<std::endl;
  return;
}

void 
NullSurface::writeFLUKA(std::ostream&) const
  /*! 
    Object of write is to output a FLUKA surface info
    This should not write but puts a warning to EMessage
  */
{
  ELog::EM<<"Writing Null Surface: "<<ELog::endWarn;
  return;
}

void 
NullSurface::writePOVRay(std::ostream&) const
  /*! 
    Object of write is to output a POV-Ray
    This should not write but puts a warning to EMessage
  */
{
  ELog::EM<<"Writing Null Surface: "<<ELog::endWarn;
  return;
}
  
void 
NullSurface::write(std::ostream&) const
  /*! 
    Object of write is to output a MCNPX plane info 
    This should not write but puts a warning to EMessage
  */
{
  ELog::EM<<"Writing Null Surface: "<<ELog::endWarn;
  return;
}

}  // NAMESPACE Geometry
