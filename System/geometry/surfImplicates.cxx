/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/surfImplicates.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <string>
#include <algorithm>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Cylinder.h"
#include "Plane.h"
#include "surfImplicates.h" 


namespace Geometry
{

surfImplicates::surfImplicates() 
  /*!
    Constructor
  */
{
  functionMap.emplace("CylinderCylinder",&surfImplicates::cylinderCylinder);
  functionMap.emplace("CylinderPlane",&surfImplicates::cylinderPlane);
  functionMap.emplace("PlaneCylinder",&surfImplicates::planeCylinder);
  functionMap.emplace("PlanePlane",&surfImplicates::planePlane);
}

surfImplicates& 
surfImplicates::Instance() 
  /*!
    Effective this object			
    \return surfImplicates object
  */
{
  static surfImplicates A;
  return A;
}

std::pair<int,int>
surfImplicates::isImplicate(const Surface* ASPtr,
			    const Surface* BSPtr) const
  /*!
    Determine if the pair is identical
    \param ASPtr :: first surface
    \param BSPtr :: second surface
    \return -1 / 1 if implicate 0 if not
  */
{
  ELog::RegMethod RegA("surfImplicates","isImplicate");

  const std::string AType=ASPtr->className();
  const std::string BType=BSPtr->className();

  const std::string combineA=AType+BType;
  STYPE::const_iterator mc=functionMap.find(combineA);
  if (mc!=functionMap.end())
    {
      testImp IPtr=mc->second;
      return (this->*IPtr)(ASPtr,BSPtr);
    }

  // all else failed
  return std::pair<int,int>(0,0);
}

std::pair<int,int>
surfImplicates::planePlane(const Geometry::Surface* APtr,
			   const Geometry::Surface* BPtr) const
  /*!
    Determine if two planes are implicates
    \param APtr :: First plane pointer
    \param BPtr :: second plane pointer
    \retval value is a->b   1 & 1  
    \retval value is a->b'  1 & -1 
    \retval value is a'->b  -1 & 1 
    \retval value is a'->b'  -1 & -1 
   */
{
  // we already know these are planes
  const Plane* APlane=dynamic_cast<const Plane*>(APtr);
  const Plane* BPlane=dynamic_cast<const Plane*>(BPtr);

  const Geometry::Vec3D& ANorm=APlane->getNormal();
  const Geometry::Vec3D& BNorm=BPlane->getNormal();
  const double AD=APlane->getDistance();
  const double BD=BPlane->getDistance();
  
  if (ANorm==BNorm)
    return (AD>BD) ? std::pair<int,int>(1,1) :
      std::pair<int,int>(-1,-1);
  
  if (ANorm==-BNorm)
    {
      return (AD > -BD) ? std::pair<int,int>(1,-1) :
	std::pair<int,int>(-1,1);
    }


  return std::pair<int,int>(0,0);

}

std::pair<int,int>
surfImplicates::cylinderPlane(const Geometry::Surface* APtr,
			      const Geometry::Surface* BPtr) const
  /*!
    Determine if a cyliner and a planes are implicates
    Condition is that the plane normal -- CylAxis are orthoganal
    We know that +Cyl can imply nothing BUT
     -Cyl can imply either +/- Plane.
    \param APtr :: First cylinder pointer
    \param BPtr :: second plane pointer
    \return first*APtr -> second*BPtr
   */
{
  // possible results: -1,1 : 0,0
  const std::pair<int,int> Out=planeCylinder(BPtr,APtr);
  if (Out.first) 
    return std::pair<int,int>(-1,-Out.first);

  return Out;
}
  
std::pair<int,int>
surfImplicates::planeCylinder(const Geometry::Surface* APtr,
			      const Geometry::Surface* BPtr) const
  /*!
    Determine if plane / cylinder are implicates
    Condition is that the plane normal -- CylAxis are orthoganal

    \param APtr :: First plane pointer
    \param BPtr :: second cylinder pointer
    \return first*APtr -> second*BPtr
   */
{
  // we already know these are planes/cylinders
  const Plane* APlane=dynamic_cast<const Plane*>(APtr);
  const Cylinder* BCyl=dynamic_cast<const Cylinder*>(BPtr);

  const Geometry::Vec3D& ANorm=APlane->getNormal();

  const Geometry::Vec3D& BAxis=BCyl->getNormal();
  const Geometry::Vec3D& BCent=BCyl->getCentre();
  const double R=BCyl->getRadius();

  // test if Norm - Axis are orthogonal
  if (std::abs(ANorm.dotProd(BAxis))<Geometry::zeroTol)
    {
      const double D=APlane->distance(BCent);
      if (std::abs(D)>(R+Geometry::zeroTol))
	{
	  return (D>0) ? std::pair<int,int>(-1,1) :
	    std::pair<int,int>(1,1);
	}
    }
  return std::pair<int,int>(0,0);
}

std::pair<int,int>
surfImplicates::cylinderCylinder(const Geometry::Surface* APtr,
				 const Geometry::Surface* BPtr) const
  /*!
    Determine if plane / cylinder are implicates
    Condition is that the axis are normal and
    radius A is within B.

    \param APtr :: First cylinder pointer
    \param BPtr :: second cylinder pointer

    \return first*APtr -> second*BPtr
   */
{
  // we already know these are planes/cylinders
  const Cylinder* ACyl=dynamic_cast<const Cylinder*>(APtr);
  const Cylinder* BCyl=dynamic_cast<const Cylinder*>(BPtr);

  const Geometry::Vec3D& AAxis=ACyl->getNormal();
  const Geometry::Vec3D& ACent=ACyl->getCentre();

  const Geometry::Vec3D& BAxis=BCyl->getNormal();
  const Geometry::Vec3D& BCent=BCyl->getCentre();
  
  const double RA=ACyl->getRadius();
  const double RB=BCyl->getRadius();
  // test if Axis - Axis are parallel
  if ((std::abs(AAxis.dotProd(BAxis))-1.0) > -Geometry::zeroTol)
    {
      // remember to project centre down length
      const Geometry::Vec3D cutACentre=ACent.cutComponent(AAxis);
      const Geometry::Vec3D cutBCentre=BCent.cutComponent(AAxis);
      
      const double D=cutACentre.Distance(cutBCentre);

      if (RA>RB && (RA-D-RB>Geometry::zeroTol))
	return std::pair<int,int>(1,1);
      if (RA<RB && (RB-D-RA>Geometry::zeroTol))
	return std::pair<int,int>(-1,-1);
    }
  return std::pair<int,int>(0,0);
}
  
} // NAMESPACE Geometry
