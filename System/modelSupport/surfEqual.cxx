/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   modelProcess/surfEqual.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
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
#include "surfEqual.h"

//typedef ModelSupport::surfIndex::STYPE SMAP;
typedef std::map<int,Geometry::Surface*> SMAP;

namespace ModelSupport
{

//----------------------------------------------------------------------
//                    OUTSIDE
//----------------------------------------------------------------------
  
Geometry::Surface*
equalSurface(Geometry::Surface* SPtr)
  /*!
    Process a equal surface request
    \param SPtr :: surface pointer
    \return Surface pointer 
  */
{
  if (!SPtr)
    throw ColErr::EmptyValue<Geometry::Surface*>("equalSurface");
  const SMAP& sMap=surfIndex::Instance().surMap();

  const Geometry::SurfKey cIndex=SPtr->classIndex();
  for(const auto& [N, TPtr] : sMap)
    {
      if (TPtr->classIndex()==cIndex &&
	  cmpSurfaces(SPtr,TPtr))
	return TPtr;
    }
      
  return SPtr;
}

const Geometry::Surface*
equalSurface(const Geometry::Surface* SPtr)
  /*!
    Process a equal surface request
    \param SPtr :: surface pointer
    \return Surface pointer 
  */
{
  if (!SPtr)
    throw ColErr::EmptyValue<Geometry::Surface*>("equalSurface(const)");
  const SMAP& sMap=surfIndex::Instance().surMap();
  

  for(const auto& [N, TPtr] : sMap)
    {
      if (cmpSurfaces(SPtr,TPtr))
	return TPtr;
    }
  
  return SPtr;
}

int
equalSurfNum(const Geometry::Surface* SPtr)
  /*!
    Process a equal surface request to a surface number 
    \param SPtr :: surface pointer
    \return Surface pointer 
   */
{
  if (!SPtr)
    throw ColErr::EmptyValue<Geometry::Surface*>("equalSurfNum");

  const Geometry::Surface* OutPtr=
    equalSurface(SPtr);
  return OutPtr->getName();
}

bool
cmpSurfaces(const Geometry::Surface* SPtr,
	    const Geometry::Surface* TPtr)
  /*!
    Simple comparison of two surfaces
    NOTE: the quadratic nature of quadratic surfaces are so extreme
    that simple comparison is almost impossible
    Therefore need to convert to dynamic type. This was done
    by double-dispatch but was a mess.
    
    \param SPtr :: First surface
    \param TPtr :: Second surface
    \return true if surfaces are equal
  */
{
  ELog::RegMethod RegA("surfEqual","cmpSurfaces");

  if (SPtr==TPtr) return 1;
  if (!SPtr || !TPtr) return 0; 
  // Strip out planes is a quick optimization:
  const Geometry::Plane* SP=dynamic_cast<const Geometry::Plane*>(SPtr);
  const Geometry::Plane* TP=dynamic_cast<const Geometry::Plane*>(TPtr);
  if (SP && TP)
    return (SP->operator==(*TP));
  if (SP || TP) return 0;   // definately failed

  const Geometry::SurfKey sKey=SPtr->classIndex();
  if (sKey!=TPtr->classIndex())
    return 0;
  
  if (sKey == Geometry::SurfKey::Cone)
    {
      const Geometry::Cone* SQ=
	dynamic_cast<const Geometry::Cone*>(SPtr);
      const Geometry::Cone* TQ=
	dynamic_cast<const Geometry::Cone*>(TPtr);
      return SQ->operator==(*TQ);
    }
  else if (sKey == Geometry::SurfKey::Cylinder)
    {
      const Geometry::Cylinder* SQ=
	dynamic_cast<const Geometry::Cylinder*>(SPtr);
      const Geometry::Cylinder* TQ=
	dynamic_cast<const Geometry::Cylinder*>(TPtr);
      return SQ->operator==(*TQ);
    }
  else if (sKey == Geometry::SurfKey::General)
    {
      const Geometry::General* SQ=
	dynamic_cast<const Geometry::General*>(SPtr);
      const Geometry::General* TQ=
	dynamic_cast<const Geometry::General*>(TPtr);
      return SQ->operator==(*TQ);
    }

  if (sKey == Geometry::SurfKey::Sphere)
    {
      const Geometry::Sphere* SQ=
	dynamic_cast<const Geometry::Sphere*>(SPtr);
      const Geometry::Sphere* TQ=
	dynamic_cast<const Geometry::Sphere*>(TPtr);
      	

      return SQ->operator==(*TQ);
    }
  else if (sKey == Geometry::SurfKey::ArbPoly)
    {
      const Geometry::ArbPoly* SQ=
	dynamic_cast<const Geometry::ArbPoly*>(SPtr);
      const Geometry::ArbPoly* TQ=
	dynamic_cast<const Geometry::ArbPoly*>(TPtr);
      return SQ->operator==(*TQ);
    }
  else if (sKey == Geometry::SurfKey::CylCan)
    {
      const Geometry::CylCan* SQ=
	dynamic_cast<const Geometry::CylCan*>(SPtr);
      const Geometry::CylCan* TQ=
	dynamic_cast<const Geometry::CylCan*>(TPtr);
      return SQ->operator==(*TQ);
    }
  else if (sKey == Geometry::SurfKey::EllipticCyl)
    {
      const Geometry::EllipticCyl* SQ=
	dynamic_cast<const Geometry::EllipticCyl*>(SPtr);
      const Geometry::EllipticCyl* TQ=
	dynamic_cast<const Geometry::EllipticCyl*>(TPtr);
      return SQ->operator==(*TQ);
    }
  else if (sKey == Geometry::SurfKey::MBrect)
    {
      const Geometry::MBrect* SQ=
	dynamic_cast<const Geometry::MBrect*>(SPtr);
      const Geometry::MBrect* TQ=
	dynamic_cast<const Geometry::MBrect*>(TPtr);
      return SQ->operator==(*TQ);
    }
  else if (sKey == Geometry::SurfKey::Quadratic)
    {
      const Geometry::Quadratic* SQ=
	dynamic_cast<const Geometry::Quadratic*>(SPtr);
      const Geometry::Quadratic* TQ=
	dynamic_cast<const Geometry::Quadratic*>(TPtr);
      return SQ->operator==(*TQ);
    }
  return 0;
}

bool
oppositeSurfaces(const Geometry::Surface* SPtr,
		 const Geometry::Surface* TPtr)
  /*!
    Find if two surfaes are opposite
    \param SPtr :: First surface
    \param TPtr :: Second surface
    \return true if surfaces are opposite
  */
{
  ELog::RegMethod RegA("surfEqual","oppositeSurface");
  if (!SPtr || !TPtr) return 0;
  
  const Geometry::Plane* SP=dynamic_cast<const Geometry::Plane*>(SPtr);
  const Geometry::Plane* TP=dynamic_cast<const Geometry::Plane*>(TPtr);

  if (!SP || !TP) return 0;
  return (SP->isEqual(*TP)==-1) ? 1 : 0;
}

} // NAMESPACE ModelSuppot
