/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/surfExpand.cxx
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
#include <complex>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <stack>
#include <string>
#include <algorithm>
#include <memory>
#include <boost/type_traits.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/vector.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Cylinder.h"
#include "Plane.h"

#include "surfaceFactory.h"
#include "surfVector.h"
#include "surfExpand.h"

typedef ModelSupport::surfIndex::STYPE SMAP;

namespace ModelSupport
{

template<typename T>
Geometry::Surface*
createExpand(const Geometry::Surface*,const double)
  /*!						
    This creates a surface which is shifted from the current
    point in by the appropiate a mount.
    \param  :: placeholder for Surface Ptr
    \param  :: placeholder for value
    \return Point to new surface 
  */
{		
  throw ColErr::AbsObjMethod("createExpand<Surface>");
}

template<>
Geometry::Surface*
createExpand<Geometry::Plane>(const Geometry::Surface* SPtr,
			      const double Dist)
  /*!						
    This creates a surface which is shifted from the current
    point in by the appropiate a mount. Planes move -ve normal on
    +ve Dist. 
    \param SPtr :: Point to the plane
    \param Dist :: Distance to shift by
    \return Point to new surface 
  */
{
  ELog::RegMethod RegA("surfExpand","createExpand<Plane>");

  const Geometry::Plane* PPtr=
    dynamic_cast<const Geometry::Plane*>(SPtr);
  if (!PPtr)
    throw ColErr::CastError<Geometry::Surface>(SPtr,"No Plane Conversion");
  
  Geometry::Plane* OutPtr=PPtr->clone();
  OutPtr->setDistance(OutPtr->getDistance()+Dist);
  return OutPtr;
}

template<>
Geometry::Surface*
createExpand<Geometry::Cylinder>(const Geometry::Surface* SPtr,
				 const double Dist)
  /*!						
    This creates a surface which is shifted from the current
    point in by the appropiate a mount. Cylinders expand on +ve Dist
    \param SPtr :: Point to the cylinder
    \param Dist :: Distance to shift by
    \return Point to new surface 
  */
{				
  ELog::RegMethod RegA("surfExpand","createExpand<Cylinder>");		
  const Geometry::Cylinder* PPtr=
    dynamic_cast<const Geometry::Cylinder*>(SPtr);
  if (!PPtr)
    throw ColErr::CastError<Geometry::Surface>(SPtr,"No Cyl Conv");

  Geometry::Cylinder* OutPtr=PPtr->clone();
  OutPtr->setRadius(OutPtr->getRadius()+Dist);
  return OutPtr;
}

/*!
  \struct unknownSurface
  \author S. Ansell
  \date December 2009
  \version 1.0
  \brief mis-match class
*/

struct unknownSurface
{
  /// Handle Index out of range
  static Geometry::Surface*
  dispatch(const int Index,const Geometry::Surface*,const double)
    {
      throw ColErr::IndexError<int>(Index,Geometry::ExportSize,
				    "unknownSurface");
    }
};

/*!
  \struct surfExpand
  \brief calls pointer directed createExpand
  \author S. Ansell
  \version 1.0
  \date December 2009
  \tparam State (list of types - this one) 
  \tparam Index :: Type id number to execute on (runtime)
*/

template<class State,class Index>
struct surfExpand
{
  static Geometry::Surface*
  dispatch(const int I,const Geometry::Surface* SPtr,const double Dist)
    /*!
      Creates the pointer to the surface
      Index refers to the positon on the ExportClass list
      \param I :: Index of runtime value (check with Index)
      \param SPtr :: Surface pointer      
      \param Dist :: Distance to move surface by
      \retval Ptr to new surface  /  next dispatch
    */
    {
      if (I==Index())
        {
	  typedef typename 
	    boost::mpl::at<Geometry::ExportClass,Index>::type ObjectType;
	  return createExpand<ObjectType>(SPtr,Dist);
	}
      else
        {
	  // Dispatches next in list
	  return State::dispatch(I,SPtr,Dist);
	}
    }
};

// ------------------------------------------
//               OUTSIDE
// ------------------------------------------

  
Geometry::Surface*
surfaceCreateExpand(const Geometry::Surface* SPtr,const double Distance)
  /*!
    Expand/collapse a surface.  Contained surfaces contract on 
    negative distance. Planes are scaled by their sign so +ve distance 
    is -ve to the normal.
    \param SPtr :: surface pointer
    \param Distance :: Distance to expand surface by [-ve == inward]
    \return Surface pointer 
   */
{
  ELog::RegMethod RegA("surfExpand","surfaceCreateExpand");
  if (!SPtr)
    throw ColErr::EmptyValue<Geometry::Surface*>("SPtr");

  // Surface type:
  const int Index=
    Geometry::surfaceFactory::Instance().getIndex(SPtr->className());
  
  typedef boost::mpl::fold<boost::mpl::range_c<int,0,Geometry::ExportSize>,
    unknownSurface,
    surfExpand<boost::mpl::_1 , boost::mpl::_2> >::type FTYPE;
  
  return FTYPE::dispatch(Index,SPtr,Distance);
}


} // NAMESPACE ModelSuppot
