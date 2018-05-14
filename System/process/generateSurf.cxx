/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/generateSurf.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <cmath>
#include <complex> 
#include <vector>
#include <map> 
#include <string>
#include <algorithm>

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
#include "Quaternion.h"
#include "stringCombine.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Cone.h"
#include "EllipticCyl.h"
#include "Sphere.h"
#include "Line.h"
#include "generateSurf.h"

namespace ModelSupport
{

Geometry::Plane*
buildRotatedPlane(surfRegister& SMap,const int N,
		  const Geometry::Plane* PN,const double angle,
		  const Geometry::Vec3D& Axis,
		  const Geometry::Vec3D& Centre)
  /*!
    Builds a plane that is rotated about the centre and the axis
    \param SMap :: Surface register
    \param PN :: Base plane
    \param angle :: angle [degrees]
    \param Axis :: Axis of rotation
    \param Centre :: Centre of rotation
  */
{
  ELog::RegMethod("generateSurf","buildRotatedPlane");
  if (!PN) return 0;

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(angle,Axis.unit());

  Geometry::Plane* PX=SurI.createUniqSurf<Geometry::Plane>(N);  

  PX->setPlane(*PN);
  PX->displace(-Centre);
  PX->rotate(Qxy);
  PX->displace(Centre);
  const int NFound=SMap.registerSurf(N,PX);

  return SMap.realPtr<Geometry::Plane>(NFound);
}

Geometry::Plane*
buildSignedShiftedPlane(surfRegister& SMap,const int signValue,
			const int N,const Geometry::Plane* PN,
			const double Dist)
  /*!
    Divider based on signed value 
    \param SMap :: Surface Map system
    \param signedValue :: Deciding value
    \param N :: Initial Number
    \param PN :: Plane to use as template
    \param Dist :: Distance along normal to move plane
    \return New plane ptr [inserted/tested]
   */
{
  ELog::RegMethod("generateSurf","buildSignedShiftedPlane");
  
  return (signValue>=0) ?
    buildShiftedPlane(SMap,N,PN,Dist) :
    buildShiftedPlaneReversed(SMap,N,PN,Dist);  
}

Geometry::Plane*
buildShiftedPlane(surfRegister& SMap,const int N,
		  const Geometry::Plane* PN,const double Dist)
  /*!
    Builds a plane that is shifted relative to a current plane
    \param SMap :: Surface Map system
    \param N :: Initial Number
    \param PN :: Plane to use as template
    \param Dist :: Distance along normal to move plane
    \return New plane ptr [inserted/tested]
  */
{
  ELog::RegMethod("generateSurf","buildShiftedPlane");
  if (!PN) return 0;

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();


  Geometry::Plane* PX=SurI.createUniqSurf<Geometry::Plane>(N);  

  PX->setPlane(*PN);
  PX->displace(PN->getNormal()*Dist);
  const int NFound=SMap.registerSurf(N,PX);

  return SMap.realPtr<Geometry::Plane>(NFound);
}

Geometry::Plane*
buildShiftedPlaneReversed(surfRegister& SMap,const int N,
			  const Geometry::Plane* PN,
			  const double Dist)
  /*!
    Builds a plane that is shifted (and normal reversed)
    \param SMap :: Surface Map system
    \param N :: Initial Number
    \param PN :: Plane to use as template
    \param Dist :: Distance along normal to move plane
    \return New plane ptr [inserted/tested]
  */
{
  ELog::RegMethod("generateSurf","buildShiftedPlaneReversed");
  if (!PN) return 0;

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();


  Geometry::Plane* PX=SurI.createUniqSurf<Geometry::Plane>(N);
  PX->setPlane(-PN->getNormal(),-PN->getDistance());
  PX->displace(-PN->getNormal()*Dist);
  
  const int NFound=SMap.registerSurf(N,PX);
  return SMap.realPtr<Geometry::Plane>(NFound);
}

Geometry::Plane*
buildPlane(surfRegister& SMap,const int N,
	   const Geometry::Vec3D& A,const Geometry::Vec3D& B,
	   const Geometry::Vec3D& C,const Geometry::Vec3D& normDir) 
  /*!
    Simple constructor to build a surface [type plane]
    (based on three points
    \param SMap :: Surface Map
    \param N :: Surface number
    \param A :: First point
    \param B :: Second Point
    \param C :: Third Point
    \param normDir :: Approximate normal direction [allow to force sign]
    \return New Plane Pointer
   */
{
  ELog::RegMethod("generateSurf","buildPlane(point)");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  Geometry::Plane* PX=SurI.createUniqSurf<Geometry::Plane>(N);  
  PX->setPlane(A,B,C);
  const double DP=PX->getNormal().dotProd(normDir);
  if (DP<-Geometry::zeroTol)
    PX->mirrorSelf();
  const int NFound=SMap.registerSurf(N,PX);

  return SMap.realPtr<Geometry::Plane>(NFound);
}

Geometry::Plane*
buildPlane(surfRegister& SMap,const int N,
	   const Geometry::Vec3D& O,const Geometry::Vec3D& D) 
  /*!
    Simple constructor to build a surface [type plane]
    \param SMap :: Surface Map
    \param N :: Surface number
    \param O :: Origin
    \param D :: Direction
    \return New Plane Pointer
   */
{
  ELog::RegMethod("generateSurf","buildPlane");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  Geometry::Plane* PX=SurI.createUniqSurf<Geometry::Plane>(N);  
  PX->setPlane(O,D);
  const int NFound=SMap.registerSurf(N,PX);

  return SMap.realPtr<Geometry::Plane>(NFound);
}

Geometry::Plane*
buildPlane(surfRegister& SMap,const int N,
	   const Geometry::Vec3D& Norm,const double& Dist) 
  /*!
    Simple constructor to build a surface [type plane]
    \param SMap :: Surface Map
    \param N :: Surface number
    \param Norm :: Origin
    \param Dist :: Distance
    \return New Plane Pointer
   */
{
  ELog::RegMethod("generateSurf","buildPlane(N,dist)");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  Geometry::Plane* PX=SurI.createUniqSurf<Geometry::Plane>(N);  
  PX->setPlane(Norm,Dist);
  const int NFound=SMap.registerSurf(N,PX);

  return SMap.realPtr<Geometry::Plane>(NFound);
}

Geometry::Plane*
buildPlaneRotAxis(surfRegister& SMap,const int N,
		  const Geometry::Vec3D& O,const Geometry::Vec3D& D,
		  const Geometry::Vec3D& Axis,const double degAngle) 
  /*!
    Simple constructor to build a surface [type plane]
    The normal of the plane is rotated about the axis
    \param SMap :: Surface Map
    \param N :: Surface number
    \param O :: Origin
    \param D :: Direction before rotation
    \param Axis :: Axis to rotate about 
    \param degAngle :: Angle to rotate about [deg]
    \return New Plane Pointer
   */
{
  ELog::RegMethod("generateSurf","buildPlaneRotAxis");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  Geometry::Plane* PX=SurI.createUniqSurf<Geometry::Plane>(N);  
  Geometry::Vec3D RotNorm(D);
  Geometry::Quaternion::calcQRotDeg(degAngle,Axis).rotate(RotNorm);  
  PX->setPlane(O,RotNorm);
  const int NFound=SMap.registerSurf(N,PX);

  return SMap.realPtr<Geometry::Plane>(NFound);
}

Geometry::Cylinder*
buildCylinder(surfRegister& SMap,const int N,
	      const Geometry::Vec3D& O,const Geometry::Vec3D& D,
	      const double R) 
  /*!
    Simple constructor to build a surface [type Cylinder]
    \param SMap :: Surface Map
    \param N :: Surface number
    \param O :: Origin
    \param D :: Direction
    \param R :: Radius
    \return New cylinder pointer
   */
{
  ELog::RegMethod("generateSurf","buildCylinder");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  Geometry::Cylinder* CX=SurI.createUniqSurf<Geometry::Cylinder>(N);  
  if (CX->setCylinder(O,D,R))
    throw ColErr::ConstructionError("setCylinder","CX",
				 StrFunc::makeString(O),
				 StrFunc::makeString(D),
				 StrFunc::makeString(R));
  const int NFound=SMap.registerSurf(N,CX);

  return SMap.realPtr<Geometry::Cylinder>(NFound);
}


Geometry::Cone*
buildCone(surfRegister& SMap,const int N,
	  const Geometry::Vec3D& O,
	  const Geometry::Vec3D& A,
	  const double angleDeg) 
  /*!
    Simple constructor to build a surface [type Cone]
    \param SMap :: Surface Map
    \param N :: Surface number
    \param O :: Origin
    \param A :: Axis
    \param angleDeg :: Angle in degrees
    \return New cone
   */
{
  ELog::RegMethod("generateSurf","buildCone");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  Geometry::Cone* CX=SurI.createUniqSurf<Geometry::Cone>(N);  
  CX->setCone(O,A,angleDeg);
  const int NFound=SMap.registerSurf(N,CX);

  return SMap.realPtr<Geometry::Cone>(NFound);
}

Geometry::Cone*
buildCone(surfRegister& SMap,const int N,
	  const Geometry::Vec3D& CentPt,
	  const Geometry::Vec3D& Axis,
	  const Geometry::Vec3D& APt,
	  const Geometry::Vec3D& BPt)
  /*!
    Simple constructor to build a surface [type Cone]
    \param SMap :: Surface Map
    \param N :: Surface number
    \param Axis :: Axis of cone
    \param CentPt :: Point on centre line of cone
    \param APt :: Point on cone
    \param BPt :: Point on cone
    \return New cone
   */
{
  ELog::RegMethod("generateSurf","buildCone");

  const Geometry::Line AL(CentPt,Axis.unit());
  const Geometry::Line BL(APt,(BPt-APt).unit());

  // Taking closest on AL to be centre point
  const std::pair<Geometry::Vec3D,Geometry::Vec3D>
    CPoints=AL.closestPoints(BL);

  const double cosAng=std::abs(AL.getDirect().dotProd(BL.getDirect()));
  
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  Geometry::Cone* CX=SurI.createUniqSurf<Geometry::Cone>(N);  
  CX->setCone(CPoints.first,Axis.unit(),180.0*acos(cosAng)/M_PI);
  const int NFound=SMap.registerSurf(N,CX);

  return SMap.realPtr<Geometry::Cone>(NFound);
}

Geometry::Cone*
buildCone(surfRegister& SMap,const int N,
	  const Geometry::Vec3D& O,
	  const Geometry::Vec3D& A,
	  const double angleDeg,const int cutFlag) 
  /*!
    Simple constructor to build a surface [type Cone]
    \param SMap :: Surface Map
    \param N :: Surface number
    \param O :: Origin
    \param A :: Axis
    \param angleDeg :: Angle in degrees
    \param cutFlag :: addtional cut flag
    \return New cone
   */
{
  ELog::RegMethod("generateSurf","buildCone(int)");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  Geometry::Cone* CX=SurI.createUniqSurf<Geometry::Cone>(N);  
  CX->setCone(O,A,angleDeg);
  CX->setCutFlag(cutFlag);
  const int NFound=SMap.registerSurf(N,CX);

  return SMap.realPtr<Geometry::Cone>(NFound);
}

Geometry::Sphere*
buildSphere(surfRegister& SMap,const int N,
	    const Geometry::Vec3D& O,const double R) 
  /*!
    Simple constructor to build a surface [type Cylinder]
    \param SMap :: Surface Map
    \param N :: Surface number
    \param O :: Origin
    \param R :: Radius
    \return New sphere pointer
   */
{
  ELog::RegMethod("generateSurf","buildCylinder");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  Geometry::Sphere* SX=SurI.createUniqSurf<Geometry::Sphere>(N);  
  SX->setSphere(O,R);
  const int NFound=SMap.registerSurf(N,SX);
  return SMap.realPtr<Geometry::Sphere>(NFound);
}

Geometry::EllipticCyl*
buildEllipticCyl(surfRegister& SMap,const int N,
		 const Geometry::Vec3D& O,
		 const Geometry::Vec3D& D,
		 const Geometry::Vec3D& LA,
		 const double RA,const double RB) 
  /*!
    Simple constructor to build a surface [type Cylinder]
    \param SMap :: Surface Map
    \param N :: Surface number
    \param O :: Origin
    \param D :: Direction
    \param LA :: RA-X direction
    \param RA :: Radius
    \param RB :: Radius
    \return New cylinder pointer
   */
{
  ELog::RegMethod("generateSurf","buildEllipticCyl");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  Geometry::EllipticCyl* EX=SurI.createUniqSurf<Geometry::EllipticCyl>(N);  
  EX->setEllipticCyl(O,D,LA,RA,RB);

  const int NFound=SMap.registerSurf(N,EX);
  return SMap.realPtr<Geometry::EllipticCyl>(NFound);
}

} // ModelSupport
