/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/pairFactory.cxx
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
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>
#include <boost/multi_array.hpp>
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
#include "ArbPoly.h"
#include "Cylinder.h"
#include "Cone.h"
#include "CylCan.h"
#include "MBrect.h"
#include "NullSurface.h"
#include "Sphere.h"
#include "Torus.h"
#include "General.h"
#include "Plane.h"
#include "pairBase.h"
#include "pairItem.h"
#include "pairFactory.h"

typedef ModelSupport::surfIndex::STYPE SMAP;

namespace ModelSupport
{

template<typename T,typename U>
pairBase*
pairFactory::getPair(const T* APtr,const Geometry::Surface* BPtr)
   /*!
     Construct a pair given that T is known and U is the test type
     \param APtr :: Known surface type
    \param BPtr :: Second pointer
    \return pairBase Item [new pairItem<AType,BType,RType>()]
   */
{
  const U* PPtr=
    dynamic_cast<const U*>(BPtr);

  return (PPtr) ? new pairItem<T,U>(APtr,PPtr) : 0;
}

template<typename T>
pairBase*
pairFactory::getFirstItem(const Geometry::Surface* APtr,
			  const Geometry::Surface* BPtr)
  /*!
    Given the first surface get a template call the appropiate
    second object
    \param APtr :: First point
    \param BPtr :: Second pointer
    \return pairBase Item [new pairItem<AType,BType,RType>()]
  */
{
  ELog::RegMethod RegA("pairFactory","getFirstItem<T>");

  pairBase* outPtr(0);
  const T* dtypePtr= dynamic_cast<const T*>(APtr);
  if (dtypePtr)
    {
      outPtr=getPair<T,Geometry::Plane>(dtypePtr,BPtr);
      if (outPtr) return outPtr;
      outPtr=getPair<T,Geometry::Cylinder>(dtypePtr,BPtr);
      if (outPtr) return outPtr;
      outPtr=getPair<T,Geometry::Sphere>(dtypePtr,BPtr);
    }
  return outPtr;
}


// ------------------------------------------
//               OUTSIDE
// ------------------------------------------

  
pairBase*
pairFactory::createPair(const Geometry::Surface* APtr,
			const Geometry::Surface* BPtr)
  /*!
    Expand/collapse a surface.  Contained surfaces contract on 
    negative distance. Planes are scaled by their sign so +ve distance 
    is -ve to the normal.
    \param APtr :: surface pointer
    \param BPtr :: surface pointer
    \return pairBase object
   */
{
  ELog::RegMethod RegA("surfExpand","surfaceCreateExpand");
  if (!APtr || !BPtr)
    throw ColErr::EmptyValue<Geometry::Surface*>("Surface Pointer");

  // Surface type:
  pairBase* outPtr=pairFactory::getFirstItem<Geometry::Plane>(APtr,BPtr);
  if (outPtr) return outPtr;
  outPtr=getFirstItem<Geometry::Cylinder>(APtr,BPtr);
  if (outPtr) return outPtr;
  outPtr=getFirstItem<Geometry::Sphere>(APtr,BPtr);

  return outPtr;
}

} // NAMESPACE ModelSuppot
