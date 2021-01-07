/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/surfCompare.cxx
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
#include <vector>
#include <map>
#include <set>
#include <list>
#include <stack>
#include <string>
#include <algorithm>
#include <memory>
#include <boost/mpl/if.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/is_convertible.hpp>

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
#include "EllipticCyl.h"
#include "MBrect.h"
#include "NullSurface.h"
#include "Sphere.h"
#include "Torus.h"
#include "General.h"
#include "Plane.h"
#include "surfaceFactory.h"
#include "surfVector.h"
#include "surfEqual.h"

namespace ModelSupport
{

/*!
  \struct unknownSurfaces
  \author S. Ansell
  \date June 2011
  \version 1.0
  \brief mis-match class
*/
struct unknownSurfaces
{
  /// Handle Index out of range
  static int dispatch(const int Index,const Geometry::Surface*,
		      const Geometry::Surface*)
    {
      throw ColErr::IndexError<int>(Index,Geometry::ExportSize,
				    "unknownSurface");
    }
};

/*!
  \struct EqualSurfaces
  \brief Surface / Surface == operator
  \author S. Ansell
  \version 1.0
  \date July 2011
  \tparam State (list of types - this one) 
  \tparam Index :: Type id number to execute on (runtime)
  \tparam SurType :: Current surface type
*/

template<class State,class Index,typename SurType>
struct EqualSurfaces
{
  static int
  dispatch(const int I,SurType SPtr,SurType TPtr)
    /*!
      Creates the pointer to the surface
      Index refers to the positon on the Geometry::ExportClass list
      \param I :: Index of runtime value (check with Index)
      \param SPtr :: First Surface pointer      
      \param TPtr :: Second surface pointer
      \return success/failure
    */
    {
      if (I==Index())
        {
	  typedef typename 
	    boost::mpl::at<Geometry::ExportClass,Index>::type ObjectType;
	  typedef typename 
	    boost::mpl::if_<boost::is_convertible<SurType,Geometry::Surface*>,
			    ObjectType*,const ObjectType*>::type TX;

	  TX ObjSPtr=dynamic_cast<TX>(SPtr);
	  TX ObjTPtr=dynamic_cast<TX>(TPtr);
	  return (ObjSPtr && ObjTPtr) ? ObjSPtr->operator==(*ObjTPtr) : 0; 
	}
      else
        {
	  // Dispatches next in list
	  return State::dispatch(I,SPtr,TPtr);
	}
    }
};

//----------------------------------------------------------------------
//                    OUTSIDE
//----------------------------------------------------------------------

int
equalSurface(const Geometry::Surface* SPtr,const Geometry::Surface* TPtr)
  /*!
    Process a equal surface request to check surfaces are equal
    \param SPtr :: surface pointer
    \param TPtr :: surface pointer
    \return true if surfaces are equivalent 
   */
{
  if (!SPtr || !TPtr)
    return 0;
  
  const int Index=
    Geometry::surfaceFactory::Instance().getIndex(SPtr->className());

  const int IndexB=
    Geometry::surfaceFactory::Instance().getIndex(TPtr->className());
  // Different types:
  if (Index!=IndexB) 
    return 0;
  
  typedef boost::mpl::fold<boost::mpl::range_c<int,0,Geometry::ExportSize>,
    unknownSurfaces,
    EqualSurfaces<boost::mpl::_1 , boost::mpl::_2,const Geometry::Surface*> >::type FTYPE;
  
  return FTYPE::dispatch(Index,SPtr,TPtr);
}


///\cond TEMPLATE

///\endcond TEMPLATE


} // NAMESPACE ModelSuppot
