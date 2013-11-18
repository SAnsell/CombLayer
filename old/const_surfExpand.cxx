/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/const_surfExpand.cxx
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
#include "mathSupport.h"
#include "support.h"
#include "Element.h"
#include "Material.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "Transform.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Weights.h"
#include "PhysImp.h"
#include "KGroup.h"
#include "Source.h"
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
#include "surfaceFactory.h"
#include "PhysicsCards.h"
#include "surfRegister.h"
#include "surfVector.h"
#include "const_surfExpand.h"

//typedef ModelSupport::surfIndex::STYPE SMAP;
typedef std::map<int,Geometry::Surface*> SMAP;

namespace ModelSupport
{

/*!
  \struct unknownSurface
  \author S. Ansell
  \date December 2009
  \version 1.0
  \brief mis-match class
*/
template<typename RetType>
struct unknownSurface
{
  static RetType
  dispatch(const int Index,const Geometry::Surface*,const double)
    ///< Handle Index out of range
    {
      throw ColErr::IndexError<int>(Index,Geometry::ExportSize,
				    "unknownSurface");
    }


  static RetType 
  dispatch(const int Index,const Geometry::Surface*,
	   const std::map<int,Geometry::Surface*>&)
  ///< Handle Index out of range
    {
      throw ColErr::IndexError<int>(Index,Geometry::ExportSize,"unknownSurface");
    }
};

/*!
  \struct EqualSurface
  \brief calls pointer directed EqualSurface
  \author S. Ansell
  \version 1.0
  \date December 2009
  \tparam State (list of types - this one) 
  \tparam Index :: Type id number to execute on (runtime)
*/

template<class State,class Index,typename RetType>
struct EqualSurface
{
  static RetType
  dispatch(const int I,RetType SPtr,
	   const std::map<int,Geometry::Surface*>& SMap)
    /*!
      Creates the pointer to the surface
      Index refers to the positon on the Geometry::ExportClass list
      \param I :: Index of runtime value (check with Index)
      \param SPtr :: Surface pointer      
      \param SMap :: Surface to use
      \retval Ptr :: success
    */
    {
      if (I==Index())
        {
	  typedef typename boost::mpl::at<Geometry::ExportClass,Index>::type ObjectType;
//	  typedef typename boost::mpl::if_<boost::is_const<RetType>,
//	    const ObjectType*,const ObjectType*>::type TX;
	  typedef typename boost::mpl::if_<boost::is_convertible<RetType,Geometry::Surface*>,
	    ObjectType*,const ObjectType*>::type TX;

	  TX ObjPtr=dynamic_cast<TX>(SPtr);
	  return EqualSurf<TX,RetType>(ObjPtr,SMap);
	}
      else
        {
	  // Dispatches next in list
	  return State::dispatch(I,SPtr,SMap);
	}
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
  \tparam RetType :: return type [const] Geometry::Surface*
*/

template<class State,class Index,typename RetType>
struct surfExpand
{
  RetType
  dispatch(const int I,RetType SPtr,const double Dist)
    /*!
      Creates the pointer to the surface
      Index refers to the positon on the ExportClass list
      \param I :: Index of runtime value (check with Index)
      \param SPtr :: Surface pointer      
      \param Dist :: Distance to move surface by
      \retval Ptr :: success
    */
    {
      if (I==Index())
        {
	  typedef typename boost::mpl::at<Geometry::ExportClass,Index>::type ObjectType;
	  return createExpand<ObjectType>(SPtr,Dist);
	}
      else
        {
	  // Dispatches next in list
	  return State::dispatch(I,SPtr,Dist);
	}
    }
};

template<typename T>
Geometry::Surface*
createExpand(const Geometry::Surface*,const double)
  /*!						
    This creates a surface which is shifted from the current
    point in by the appropiate a mount.
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
    point in by the appropiate a mount.
    \param SPtr :: Point to the plane
    \param Dist :: Distance to shift by
    \return Point to new surface 
  */
{						
  const Geometry::Plane* PPtr=
    dynamic_cast<const Geometry::Plane*>(SPtr);
  if (!PPtr)
    throw ColErr::CastError<Geometry::Surface>(SPtr,"createExpand<Plane>");
  
  Geometry::Plane* OutPtr=PPtr->clone();
  OutPtr->setDistance(OutPtr->getDistance()-Dist);
  return OutPtr;
}

template<>
Geometry::Surface*
createExpand<Geometry::Cylinder>(const Geometry::Surface* SPtr,
				 const double Dist)
  /*!						
    This creates a surface which is shifted from the current
    point in by the appropiate a mount.
    \param SPtr :: Point to the cylinder
    \param Dist :: Distance to shift by
    \return Point to new surface 
  */
{						
  const Geometry::Cylinder* PPtr=
    dynamic_cast<const Geometry::Cylinder*>(SPtr);
  if (!PPtr)
    throw ColErr::CastError<Geometry::Surface>(SPtr,"createExpand<Cylinder>");
  
  Geometry::Cylinder* OutPtr=PPtr->clone();
  OutPtr->setRadius(OutPtr->getRadius()+Dist);
  return OutPtr;
}


//----------------------------------------------------------------------
//                    OUTSIDE
//----------------------------------------------------------------------



const Geometry::Surface*
equalSurface(const Geometry::Surface* SPtr)
  /*!
    Process a equal surface request
    \param SMap :: Surface Map to use
    \param SPtr :: surface pointer
    \return Surface pointer 
   */
{
  if (!SPtr)
    throw ColErr::EmptyValue<Geometry::Surface*>("equalSurface(const)");
  
  const int Index=
    Geometry::surfaceFactory::Instance()->getIndex(SPtr->className());
  
  typedef boost::mpl::fold<boost::mpl::range_c<int,0,Geometry::ExportSize>,
    unknownSurface<Geometry::Surface*>,
    EqualSurface<boost::mpl::_1 , boost::mpl::_2,const Geometry::Surface*> >::type FTYPE;
  
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  return FTYPE::dispatch(Index,SPtr,SurI.surMap());
}

Geometry::Surface*
equalSurfaceXX(Geometry::Surface* SPtr)
  /*!
    Process a equal surface request
    \param SMap :: Surface Map to use
    \param SPtr :: surface pointer
    \return Surface pointer 
  */
{
  if (!SPtr)
    throw ColErr::EmptyValue<Geometry::Surface*>("equalSurface");
  
  const int Index=
    Geometry::surfaceFactory::Instance()->getIndex(SPtr->className());
  
  typedef boost::mpl::fold<boost::mpl::range_c<int,0,Geometry::ExportSize>,
    unknownSurface<Geometry::Surface*>,
    EqualSurface<boost::mpl::_1 , boost::mpl::_2,Geometry::Surface*> >::type FTYPE;
  
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  return FTYPE::dispatch(Index,SPtr,SurI.surMap());
}


template<typename SurfType,typename RetType>
RetType
EqualSurf(SurfType surf,const SMAP& SurMap)
  /*!
    Helper function to determine the surface object
    is similar to any we currently have
    \param surf : Surface to find
    \param SurMap :: surface map to objects
    \returns Surface Ptr (either new/old)
  */
{
  const int index=surf->getName();
  SMAP::const_iterator mc;
  for(mc=SurMap.begin();mc!=SurMap.end();mc++)
    {
      if (mc->first!=index) 
        {
	  SurfType sndObj=dynamic_cast<SurfType>(mc->second);
          if (sndObj && sndObj->operator==(*surf))
	    return sndObj; 
	}
    }
  return static_cast<RetType>(surf);
}


///\cond TEMPLATE


template const Geometry::Surface* 
EqualSurf<const Geometry::ArbPoly*,const Geometry::Surface*>(const Geometry::ArbPoly*,const SMAP&);
template const Geometry::Surface* 
EqualSurf<const Geometry::Cone*,const Geometry::Surface*>(const Geometry::Cone*,const SMAP&);
template const Geometry::Surface* 
EqualSurf<const Geometry::CylCan*,const Geometry::Surface*>(const Geometry::CylCan*,const SMAP&);
template const Geometry::Surface* 
EqualSurf<const Geometry::Cylinder*,const Geometry::Surface*>(const Geometry::Cylinder*,const SMAP&);
template const Geometry::Surface* 
EqualSurf<const Geometry::General*,const Geometry::Surface*>(const Geometry::General*,const SMAP&);
template const Geometry::Surface* 
EqualSurf<const Geometry::MBrect*,const Geometry::Surface*>(const Geometry::MBrect*,const SMAP&);
template const Geometry::Surface* 
EqualSurf<const Geometry::NullSurface*,const Geometry::Surface*>(const Geometry::NullSurface*,const SMAP&);
template const Geometry::Surface* 
EqualSurf<const Geometry::Plane*,const Geometry::Surface*>(const Geometry::Plane*,const SMAP&);
template const Geometry::Surface* 
EqualSurf<const Geometry::Quadratic*,const Geometry::Surface*>(const Geometry::Quadratic*,const SMAP&);
template const Geometry::Surface* 
EqualSurf<const Geometry::Sphere*,const Geometry::Surface*>(const Geometry::Sphere*,const SMAP&);
template const Geometry::Surface* 
EqualSurf<const Geometry::Torus*,const Geometry::Surface*>(const Geometry::Torus*,const SMAP&);

template Geometry::Surface* 
EqualSurf<Geometry::ArbPoly*,Geometry::Surface*>(Geometry::ArbPoly*,const SMAP&);
template Geometry::Surface* 
EqualSurf<Geometry::Cone*,Geometry::Surface*>(Geometry::Cone*,const SMAP&);
template Geometry::Surface* 
EqualSurf<Geometry::CylCan*,Geometry::Surface*>(Geometry::CylCan*,const SMAP&);
template Geometry::Surface* 
EqualSurf<Geometry::Cylinder*,Geometry::Surface*>(Geometry::Cylinder*,const SMAP&);
template Geometry::Surface* 
EqualSurf<Geometry::General*,Geometry::Surface*>(Geometry::General*,const SMAP&);
template Geometry::Surface* 
EqualSurf<Geometry::MBrect*,Geometry::Surface*>(Geometry::MBrect*,const SMAP&);
template Geometry::Surface* 
EqualSurf<Geometry::NullSurface*,Geometry::Surface*>(Geometry::NullSurface*,const SMAP&);
template Geometry::Surface* 
EqualSurf<Geometry::Plane*,Geometry::Surface*>(Geometry::Plane*,const SMAP&);
template Geometry::Surface* 
EqualSurf<Geometry::Quadratic*,Geometry::Surface*>(Geometry::Quadratic*,const SMAP&);
template Geometry::Surface* 
EqualSurf<Geometry::Sphere*,Geometry::Surface*>(Geometry::Sphere*,const SMAP&);
template Geometry::Surface* 
EqualSurf<Geometry::Torus*,Geometry::Surface*>(Geometry::Torus*,const SMAP&);

///\endcond TEMPLATE


} // NAMESPACE ModelSuppot
