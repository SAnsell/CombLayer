/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/generateSurf.cxx
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
		  const int refPlaneN,const double Dist)
  /*!
    Builds a plane that is shifted relative to a current plane
    \param SMap :: Surface Map system
    \param N :: Initial Number
    \param refPlaneN :: Plane indexto use as template

    \param Dist :: Distance along normal to move plane
    \return New plane ptr [inserted/tested]
  */
{
  ELog::RegMethod("generateSurf","buildShiftedPlane(int)");
  if (!refPlaneN) return 0;

  const int PN=SMap.realSurf(refPlaneN);
  const Geometry::Plane* PRef=SMap.realPtr<Geometry::Plane>(refPlaneN);
  if (!PRef)
    throw ColErr::InContainerError<int>(refPlaneN,"Reference Plane");
  return (PN<0) ?
    buildShiftedPlaneReversed(SMap,N,PRef,Dist) :
    buildShiftedPlane(SMap,N,PRef,Dist);
}

Geometry::Plane*
buildShiftedPlane(surfRegister& SMap,const int N,
		  const Geometry::Plane* PN,
		  const double Dist)
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
buildShiftedPlaneReversed(surfRegister& SMap,const int N,
			  const int refPlaneN,const double Dist)
  /*!
    Builds a plane that is shifted relative to a current plane
    \param SMap :: Surface Map system
    \param N :: Initial Number
    \param refPlaneN :: Plane indexto use as template
    \param Dist :: Distance along normal to move plane
    \return New plane ptr [inserted/tested]
  */
{
  ELog::RegMethod("generateSurf","buildShiftedPlaneReversed(int)");
  if (!refPlaneN) return 0;

  const int PN=SMap.realSurf(refPlaneN);
  const Geometry::Plane* PRef=SMap.realPtr<Geometry::Plane>(refPlaneN);

  if (!PRef)
    throw ColErr::InContainerError<int>(refPlaneN,"Reference Plane");
  return (PN>0) ?
    buildShiftedPlaneReversed(SMap,N,PRef,Dist) :
    buildShiftedPlane(SMap,N,PRef,Dist);
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
				 "Origin = "+StrFunc::makeString(O),
				 "Dir    = "+StrFunc::makeString(D),
				 "Radius = "+StrFunc::makeString(R));
  const int NFound=SMap.registerSurf(N,CX);

  return SMap.realPtr<Geometry::Cylinder>(NFound);
}


Geometry::Cone*
buildCone(surfRegister& SMap,const int N,
	  const Geometry::Vec3D& circleCent,
	  const double radius,
	  const Geometry::Vec3D& Axis,
	  const double angleDeg) 
  /*!
    Simple constructor to build a surface [type Cone]
    \param SMap :: Surface Map
    \param N :: Surface number
    \param circleCent :: Centre of circle
    \param radius :: Radius of circle
    \param A :: Axis [FORWARD going]
    \param angleDeg :: angle of cone [deg]
    \return New cone
   */
{
  ELog::RegMethod("generateSurf","buildCone(cent,radius,Axis,angleDeg)");

  
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  // distance to move centre [half angle]
  const double D=radius/tan(M_PI*angleDeg/(180.0));

  ELog::EM<<"Cone == "<<circleCent-Axis.unit()*D<<ELog::endDiag;
  Geometry::Cone* CX=SurI.createUniqSurf<Geometry::Cone>(N);  
  CX->setCone(circleCent-Axis.unit()*D,Axis,angleDeg);
  const int NFound=SMap.registerSurf(N,CX);


  return SMap.realPtr<Geometry::Cone>(NFound);
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
  ELog::RegMethod("generateSurf","buildCone(O,Axis,angle)");

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
    Assumption is that the Points APt and BPt are at
    different radii from the center line.
    \param SMap :: Surface Map
    \param N :: Surface number
    \param CentPt :: Point on centre line of cone
    \param Axis :: Forward Axis of cone
    \param APt :: Point on cone
    \param BPt :: Point on cone
    \return New cone
   */
{
  ELog::RegMethod("generateSurf","buildCone(Org,Axis,Pt1,pt2)");

  const Geometry::Vec3D AUnit(Axis.unit());
  const Geometry::Line AL(CentPt,AUnit);

  // Taking closest on AL to be centre point
  const Geometry::Vec3D ACent=AL.closestPoint(APt);
  const Geometry::Vec3D BCent=AL.closestPoint(BPt);
  const double ADist=APt.Distance(ACent);
  const double BDist=BPt.Distance(BCent);
  
  Geometry::Vec3D coneCent;
  double cosAng;
  
  const double S=BCent.Distance(ACent);
  const double ABdiff(BDist-ADist);

  if (ABdiff>Geometry::zeroTol)
    {
      const double L=S+(ADist*S)/ABdiff;
      coneCent=BCent-AUnit*L;
      cosAng=std::abs(L/sqrt(BDist*BDist+L*L));
    }
  else  // assume centre Cpt is centre [as degenerate]
    {
      const double L=CentPt.Distance(BCent);
      coneCent=CentPt;
      cosAng=std::abs(L/sqrt(L*L+BDist*BDist));
      ELog::EM<<"Cone Centre == "<<cosAng<<" "
	      <<acos(cosAng)*180/M_PI<<ELog::endDiag;
    }
  
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  Geometry::Cone* CX=SurI.createUniqSurf<Geometry::Cone>(N);  
  CX->setCone(coneCent,AUnit,180.0*acos(cosAng)/M_PI);
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

Geometry::Surface*
buildShiftedSurf(surfRegister& SMap,
		const int SN,
		const int index,
		const int dFlag,
		const Geometry::Vec3D& YAxis,
		const double length)
  /*!
    Support function to calculate the shifted surface based
    on surface type and form
    \param SMap :: local surface register
    \param SN :: HeadRule to extract plane surf
    \param index :: offset index
    \param dFlag :: direction of surface axis (relative to HR.Plane)
    \param YAxis :: Direction of cylindical shift [NOT PLANE]
    \param length :: length to shift by
  */
{
  ELog::RegMethod RegA("ExternalCut","makeShiftedSurf");
  
  const Geometry::Surface* SPtr=SMap.realSurfPtr(SN);
  
  const Geometry::Plane* PPtr=
    dynamic_cast<const Geometry::Plane*>(SPtr);
  if (PPtr)
    {
      if (SN*dFlag>0)
	buildShiftedPlane(SMap,index,PPtr,dFlag*length);
      else
	buildShiftedPlaneReversed(SMap,index,PPtr,dFlag*length);
      return SMap.realSurfPtr(index);
    }
  
  const Geometry::Cylinder* CPtr=
    dynamic_cast<const Geometry::Cylinder*>(SPtr);
  // Cylinder case:
  if (CPtr)
    {
      if (SN>0)
	buildCylinder(SMap,index,CPtr->getCentre()+YAxis*length,
	   CPtr->getNormal(),CPtr->getRadius());
      else
	buildCylinder(SMap,index,CPtr->getCentre()-YAxis*length,
	       CPtr->getNormal(),CPtr->getRadius());
      return SMap.realSurfPtr(index);
    }  
  return 0;
} 


Geometry::Surface*
buildExpandedSurf(ModelSupport::surfRegister& SMap,
		 const int SN,
		 const int index,
		 const Geometry::Vec3D& expandCentre,
		 const double dExtra) 
  /*!
    Support function to calculate the shifted surface based
    on surface type and form. Moves away from the center if dExtra
    positive.
    \param SMap :: local surface register
    \param SN :: surface to expand
    \param index :: offset index
    \param expandCentre :: Centre for expansion
    \param dExtra :: displacement extra [cm]
    \return Surface ptr [nullPtr on failure]
  */
{
  ELog::RegMethod RegA("generateSurf","buildExpandedSurf");
  
  const Geometry::Surface* SPtr=SMap.realSurfPtr(SN);
  // plane case does not use distance
  const Geometry::Plane* PPtr=
    dynamic_cast<const Geometry::Plane*>(SPtr);
  if (PPtr)
    {
      int sideFlag=PPtr->side(expandCentre);
      if (sideFlag==0) sideFlag=1;
      buildShiftedPlane(SMap,index,PPtr, -sideFlag*dExtra);
      return SMap.realSurfPtr(index);
    }
  
  const Geometry::Cylinder* CPtr=
    dynamic_cast<const Geometry::Cylinder*>(SPtr);
  // Cylinder case:
  if (CPtr)
    {
      // // this may not alway be what we want:
      Geometry::Vec3D NVec=(CPtr->getCentre()-expandCentre);
      const Geometry::Vec3D CAxis=CPtr->getNormal();
      // now must remove component in axis direction
      NVec-= CAxis * NVec.dotProd(CAxis);
      NVec.makeUnit();
      //	  const Geometry::Vec3D NC=CPtr->getCentre()+NVec*dExtra;
      const Geometry::Vec3D NC=CPtr->getCentre();
      
      ModelSupport::buildCylinder
	(SMap,index,NC,CPtr->getNormal(),CPtr->getRadius()+dExtra);
      return SMap.realSurfPtr(index);
    }
  return 0;
} 



} // ModelSupport
