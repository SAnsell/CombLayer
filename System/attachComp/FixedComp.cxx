/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/FixedComp.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <string>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "stringCombine.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "surfEqual.h"
#include "Rules.h"
#include "HeadRule.h"
#include "SurInter.h"
#include "LinkUnit.h"
#include "FixedComp.h"

namespace attachSystem
{

FixedComp::FixedComp(const std::string& KN,const size_t NL) :
  keyName(KN),X(Geometry::Vec3D(1,0,0)),Y(Geometry::Vec3D(0,1,0)),
  Z(Geometry::Vec3D(0,0,1)),primeAxis(0),LU(NL)
 /*!
    Constructor 
    \param KN :: KeyName
    \param NL :: Number of links
  */
{}

FixedComp::FixedComp(const std::string& KN,const size_t NL,
		     const Geometry::Vec3D& O) :
  keyName(KN),X(Geometry::Vec3D(1,0,0)),Y(Geometry::Vec3D(0,1,0)),
  Z(Geometry::Vec3D(0,0,1)),Origin(O),primeAxis(0),LU(NL)
  /*!
    Constructor 
    \param KN :: KeyName
    \param NL :: Number of links
    \param O :: Origin Point
  */
{}

FixedComp::FixedComp(const std::string& KN,const size_t NL,
		     const Geometry::Vec3D& O,
		     const Geometry::Vec3D& xV,
		     const Geometry::Vec3D& yV,
		     const Geometry::Vec3D& zV) :
  keyName(KN),X(xV.unit()),Y(yV.unit()),Z(zV.unit()),
  Origin(O),primeAxis(0),LU(NL)
  /*!
    Constructor with defined axis / origin
    \param KN :: KeyName
    \param NL :: Number of links
    \param O :: Origin Point
    \param xV :: X direction
    \param yV :: Y direction
    \param zV :: Z direction
  */
{}

FixedComp::FixedComp(const FixedComp& A) : 
  keyName(A.keyName),SMap(A.SMap),keyMap(A.keyMap),
  X(A.X),Y(A.Y),Z(A.Z),
  Origin(A.Origin),beamAxis(A.beamAxis),
  orientateAxis(A.orientateAxis),primeAxis(A.primeAxis),
  LU(A.LU)
  /*!
    Copy constructor
    \param A :: FixedComp to copy
  */
{}

FixedComp&
FixedComp::operator=(const FixedComp& A)
  /*!
    Assignment operator
    \param A :: FixedComp to copy
    \return *this
  */
{
  if (this!=&A)
    {
      SMap=A.SMap;
      keyMap=A.keyMap;
      X=A.X;
      Y=A.Y;
      Z=A.Z;
      Origin=A.Origin;
      beamAxis=A.beamAxis;
      orientateAxis=A.orientateAxis;
      primeAxis=A.primeAxis;
      LU=A.LU;
    }
  return *this;
}


void
FixedComp::setAxisControl(const long int axisIndex,
			  const Geometry::Vec3D& NAxis)
  /*!
    Set the new reorientation axis
    \param axisIndex :: X/Y/Z for reorientation [1-3]
    \param NAxis :: New Axis 
  */
{
  ELog::RegMethod Rega("FixedComp","setAxisControl");
  if (NAxis.abs()<Geometry::zeroTol)
    throw ColErr::NumericalAbort("NAxis is zero");
  if (axisIndex>3 || axisIndex<-3)
    throw ColErr::IndexError<long int>(axisIndex,2,"axisIndex");
    
  orientateAxis=NAxis.unit(); ///< Axis for reorientation
  primeAxis=axisIndex;
  return;
}
  

void
FixedComp::createUnitVector(const FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Relative to another FC
  */
{
  ELog::RegMethod RegA("FixedComp","createUnitVector(FixedComp)");

  Z=FC.Z;
  Y=FC.Y;
  X=FC.X;
  Origin=FC.Origin;
  beamOrigin=FC.beamOrigin;
  beamAxis=FC.beamAxis;

  if (primeAxis>0)
    reOrientate();

  return;
}

void
FixedComp::createUnitVector(const FixedComp& FC,
			    const Geometry::Vec3D& POrigin)
  /*!
    Create the unit vectors
    \param FC :: Relative to another FC
    \param POrigin :: New Origin
  */
{
  ELog::RegMethod RegA("FixedComp","createUnitVector(FixedComp,Vec3D)");

  Z=FC.Z;
  Y=FC.Y;
  X=FC.X;
  Origin=POrigin;
  beamOrigin=FC.beamOrigin;
  beamAxis=FC.beamAxis;

  if (primeAxis>0) reOrientate();
  
  return;
}

void
FixedComp::createUnitVector(const FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed unit for link points
    \param sideIndex :: SIGNED +1 side index
  */
{
  ELog::RegMethod RegA("FixedComp","createUnitVector(FixedComp,side)");

  createUnitVector(FC,sideIndex,sideIndex);
  return;
}

void
FixedComp::createUnitVector(const FixedComp& FC,
			    const long int orgIndex,
			    const long int basisIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed unit for link points
    \param orgIndex :: SIGNED +1 side index for origin
    \param basisIndex :: SIGNED +1 side index for directoin
  */
{
  ELog::RegMethod RegA("FixedComp","createUnitVector(FixedComp,org,basis)");

  if (basisIndex==0)
    {
      createUnitVector(FC);
      Origin=FC.getLinkPt(orgIndex);
      return;
    }
	  
  const size_t linkIndex=
    (basisIndex>0) ? static_cast<size_t>(basisIndex-1) :
    static_cast<size_t>(-basisIndex-1) ;
  if (linkIndex>=FC.LU.size())
    throw ColErr::IndexError<size_t>
      (linkIndex,FC.LU.size(),
       "LU.size()/linkIndex in object:"+FC.getKeyName()+" to object "+
       keyName);
     
  const LinkUnit& LU=FC.getLU(linkIndex);
  const double signV((basisIndex>0) ? 1.0 : -1.0);

  const Geometry::Vec3D yTest=LU.getAxis()*signV;
      
  Geometry::Vec3D zTest=FC.getZ();
  Geometry::Vec3D xTest=FC.getX();
  if (std::abs(zTest.dotProd(yTest))>1.0-Geometry::zeroTol)
    zTest=FC.getY();
  else if (std::abs(xTest.dotProd(yTest))>1.0-Geometry::zeroTol)
    xTest=FC.getY();

  computeZOffPlane(xTest,yTest,zTest);

  createUnitVector(FC.getLinkPt(orgIndex),
		   yTest*zTest,yTest,zTest);
  
  return;
}

void
FixedComp::createUnitVector(const Geometry::Vec3D& OG,
			    const Geometry::Vec3D& XAxis,
                            const Geometry::Vec3D& YAxis,
			    const Geometry::Vec3D& ZAxis)
  /*!
    Create the unit vectors [using beam directions]
    \param OG :: Origin
    \param XAxis :: Direction for X
    \param YAxis :: Direction for Y
    \param ZAxis :: Direction for Z
  */
{
  ELog::RegMethod RegA("FixedComp","createUnitVector(Vec3D,Vec3D,Vec3D))");

  //Geometry::Vec3D(-1,0,0);          // Gravity axis [up]
  X=XAxis.unit();
  Y=YAxis.unit();
  Z=ZAxis.unit();
  
  makeOrthogonal();
  Origin=OG;
  beamOrigin=OG;
  beamAxis=Y;
  if (primeAxis>0) reOrientate();
  return;
}
  
void
FixedComp::makeOrthogonal()
  /*!
    Reconstructs the basis set to be orthogonal
    Y is primary and Z is secondary
    Assumes X,Y,Z are unitary
  */
{
  ELog::RegMethod RegA("FixedComp","makeOrthogonal");

  // Use this later
  const double XYcos=X.dotProd(Y); 
  if (std::abs<double>(XYcos)<Geometry::zeroTol &&
      std::abs(X.dotProd(Z))<Geometry::zeroTol &&
      std::abs(Y.dotProd(Z))<Geometry::zeroTol)
    return;
  // Now have possiblity that we are not othogonal
  // Y is PRIMARY
  // Z is SECONDARY

  if (std::abs(XYcos)>1.0-Geometry::zeroTol)
    throw ColErr::NumericalAbort("Vectors X/Y parallel:"+
                                 StrFunc::makeString(X));

  // Calc X:
  const Geometry::Vec3D XDir(X);
  X=(Y*Z).unit();
  if (XDir.dotProd(X)<0.0)
    X*=-1;

  // Calc Z' to be close to Z
  const Geometry::Vec3D ZDir(Z);
  Z=(Y*X).unit();
  if (ZDir.dotProd(Z)<0.0)
    Z*=-1;
  return;
}

void
FixedComp::reOrientate()
  /*!
    Calling function to reOrientate
    This allows a primary axis to be-reorientated to 
    a previously given direction. Typically used to 
    set an object to the vertical etc, after moving the object
  */
{
  ELog::RegMethod RegA("FixedComp","reOrientate");
  
  if (primeAxis>0)
    reOrientate(static_cast<size_t>(primeAxis)-1,orientateAxis);
  else if (primeAxis<0)
    reOrientate(static_cast<size_t>(-primeAxis)-1,orientateAxis);
  primeAxis=0;
  return;
}

void
FixedComp::reOrientate(const size_t index,
                       const Geometry::Vec3D& ADir)
  /*!
    Reorientate the system to have the index axis orientated
    along the axisDir.
    Compute the normal vector to the plane axisDir/Axis[index]
    and the rotation required to move axis[index] -> axisDir.
    then applied to the other two axis vectors.
    \param index :: index to orientate round [0-2]
    \param ADir :: axis direction
   */
{
  ELog::RegMethod RegA("FixedComp","reorientate");
  
  if (index>=3)
    throw ColErr::IndexError<size_t>(index,3,"index -- 3D vectors required");
  
  Geometry::Vec3D axisDir(ADir.unit());  

  std::vector<Geometry::Vec3D*> AVec({&X,&Y,&Z});

  const double cosTheta=axisDir.dotProd( *AVec[index] );
  // Both checks with tolerance:
  if (cosTheta-Geometry::zeroTol>1.0) return;
  if (cosTheta+1.0<Geometry::zeroTol)
    {
      for(Geometry::Vec3D* A : AVec)
        *A *=-1;
      return;
    }
  
  // Calc normal
  const Geometry::Vec3D n=axisDir * *AVec[index];
  
  const double rotAngle=acos(cosTheta);
  const Geometry::Quaternion QR
    (Geometry::Quaternion::calcQRot(-rotAngle,n));

  QR.rotate(*AVec[(index+1) % 3]);
  QR.rotate(*AVec[(index+2) % 3]);
  *AVec[index]=axisDir;
  return;
}
  
void
FixedComp::computeZOffPlane(const Geometry::Vec3D& XAxis,
                            const Geometry::Vec3D& YAxis,
                            Geometry::Vec3D& ZAxis)
  /*!
    Rotate the ZAxis to the component of the main rotation
    \param XAxis :: X direction [orthogonal to Z and Y]
    \param YAxis :: Y direction [non-orthogonal]
    \param ZAxis :: Z direction [non-orthogonal]
   */
{
  ELog::RegMethod RegA("FixedComp","computeZOffPlane");

  const double XPart=XAxis.dotProd(YAxis);
  
  Geometry::Vec3D YPrime=YAxis-XAxis*XPart;

  if (YPrime.abs()<Geometry::zeroTol)
    ELog::EM<<"Error with XRemoval from plane"<<ELog::endErr;
  YPrime.makeUnit();

  const double cosValue=std::abs(YPrime.dotProd(ZAxis));
  if (1.0-cosValue<Geometry::zeroTol) return;
  const double yAngle= -asin(cosValue);  // remove 90deg.

  const Geometry::Quaternion QR=
    Geometry::Quaternion::calcQRot(yAngle,XAxis);

  QR.rotate(ZAxis);
  return;
}


void
FixedComp::applyShift(const double xStep,
		      const double yStep,
		      const double zStep)
  /*!
    Create the unit vectors
    \param xStep :: XStep [of X vector]
    \param yStep :: YStep [of Y vector]
    \param zStep :: ZStep [of Z vector]
  */
{
  Origin+=X*xStep+Y*yStep+Z*zStep;
  return;
}


void
FixedComp::applyAngleRotate(const double xAngle,
			    const double yAngle,
			    const double zAngle)
 /*!
   Applies a triple angle rotation.
   Rotates about:
     Z : Y : X
     \param xAngle :: X-rotation angle
     \param yAngle :: Y-rotation angle
     \param zAngle :: Z-rotation angle [first]
  */
{
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,Z);
  const Geometry::Quaternion Qy=
    Geometry::Quaternion::calcQRotDeg(yAngle,Y);
  const Geometry::Quaternion Qx=
    Geometry::Quaternion::calcQRotDeg(xAngle,X);
  Qz.rotate(Y);
  Qz.rotate(X);
  
  Qy.rotate(X);
  Qy.rotate(Y);
  Qy.rotate(Z);
  
  Qx.rotate(X);
  Qx.rotate(Y);
  Qx.rotate(Z);
  return;
}

void
FixedComp::applyAngleRotate(const double xyAngle,
			    const double zAngle)
 /*!
    Create the unit vectors
    \param xyAngle :: XY Rotation [second]
    \param zAngle :: Z Rotation [first]
  */
{
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);
  Qz.rotate(Y);
  Qz.rotate(Z);

  Qxy.rotate(Y);
  Qxy.rotate(X);
  Qxy.rotate(Z);
  return;
}

void
FixedComp::linkShift(const long int sideIndex,
			   const double xStep,
			   const double yStep,
			   const double zStep)
 /*!
   Shift a link point by displacement given
   \param sideIndex :: signed ink point [sign for direction]
   \param xStep :: X-step
   \param yStep :: Y-step
   \param zStep :: Z-step
 */
{
  ELog::RegMethod RegA("FixedComp","linkAngleRotate");

  LinkUnit& LItem=getSignedLU(sideIndex);
  const double signV=(sideIndex>0) ? 1.0 : -1.0;

  Geometry::Vec3D Pt(LItem.getConnectPt());
  LItem.setConnectPt(Pt+(X*xStep+Y*yStep+Z*zStep)*signV);
  return;
}

void
FixedComp::linkAngleRotate(const long int sideIndex,
			   const double xyAngle,
			   const double zAngle)
 /*!
   Rotate a link point axis [not connection point]
   \param sideIndex :: signed ink point [sign for direction]
   \param xyAngle :: XY Rotation [second]
   \param zAngle :: Z Rotation [first]
 */
{
  ELog::RegMethod RegA("FixedComp","linkAngleRotate");

  LinkUnit& LItem=getSignedLU(sideIndex);
  const double signV=(sideIndex>0) ? 1.0 : -1.0;
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle*signV,X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle*signV,Z);

  Geometry::Vec3D Axis=LItem.getAxis();
  Qz.rotate(Axis);
  Qxy.rotate(Axis);

  LItem.setAxis(Axis);
  return;
}

void
FixedComp::applyFullRotate(const double xAngle,
                           const double yAngle,
			   const double zAngle,
			   const Geometry::Vec3D& RotCent)
  /*!
    Create the unit vectors
    \param xAngle :: X Rotation [third]
    \param yAngle :: Y Rotation [second]
    \param zAngle :: Z Rotation [first]
    \param RotCent :: Displacement centre
  */
{
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,Z);
  const Geometry::Quaternion Qy=
    Geometry::Quaternion::calcQRotDeg(yAngle,Y);
  const Geometry::Quaternion Qx=
    Geometry::Quaternion::calcQRotDeg(xAngle,X);

  Qz.rotate(Y);
  Qz.rotate(X);
  
  Qy.rotate(X);
  Qy.rotate(Y);
  Qy.rotate(Z);
  
  Qx.rotate(X);
  Qx.rotate(Y);
  Qx.rotate(Z);

  Origin-=RotCent;
  Qz.rotate(Origin);
  Qy.rotate(Origin);
  Qx.rotate(Origin);
  Origin+=RotCent;

  return;
}
void
FixedComp::applyFullRotate(const double xyAngle,
			   const double zAngle,
			   const Geometry::Vec3D& RotCent)
  /*!
    Create the unit vectors
    \param xyAngle :: XY Rotation [second]
    \param zAngle :: Z Rotation [first]
    \param RotCent :: Displacement centre
  */
{
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);
  Qz.rotate(Y);
  Qz.rotate(Z);
  Qxy.rotate(Y);
  Qxy.rotate(X);
  Qxy.rotate(Z);

  Origin-=RotCent;
  Qz.rotate(Origin);
  Qxy.rotate(Origin);
  Origin+=RotCent;

  return;
}

void
FixedComp::reverseZ()
  /*!
    Flip the Z axis keeping Y Fixed
    (could generalize but ...)
  */
{
  Z*= -1.0;
  X*= -1.0;
  return;
}

void
FixedComp::setNConnect(const size_t N) 
  /*!
    Create/Remove new links point
    \param N :: New size of link points
  */
{
  LU.resize(N);
  return;
}

void
FixedComp::copyLinkObjects(const FixedComp& A)
  /*!
    Copy all the link object from A to this FixedComp.
    Overwrites the existing link units.
    Sizes must match or be smaller
    \param A :: FixedComp to copy
  */
{
  ELog::RegMethod RegA("FixedComp","copyLinkObjects");
  if (this!=&A)
    {
      if(LU.size()!=A.LU.size())
	ELog::EM<<"Changing link size from "<<LU.size()
		<<" to "<<A.LU.size()<<ELog::endCrit;
      LU=A.LU;
    }
  return;
}

void
FixedComp::addLinkSurf(const size_t Index,const int SN) 
  /*!
    Add a surface to output
    \param Index :: Link number
    \param SN :: Surface number [inward looking]
  */
{
  ELog::RegMethod RegA("FixedComp","addLinkSurf(int)");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU size/Index");
  
  LU[Index].addLinkSurf(SN);
  return;
}

void
FixedComp::addLinkSurf(const size_t Index,
		       const std::string& SList) 
  /*!
    Add a surface to output
    \param Index :: Link number
    \param SList :: String to process
  */
{
  ELog::RegMethod RegA("FixedComp","addLinkSurf(std::string)");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU size/Index");

  LU[Index].addLinkSurf(SList);
  return;
}

void
FixedComp::addLinkSurf(const size_t Index,const HeadRule& HR) 
  /*!
    Add a surface to output
    \param Index :: Link number
    \param HR :: Surface rule to add
  */
{
  ELog::RegMethod RegA("FixedComp","addLinkSurf(HeadRule)");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU size/Index");
  
  LU[Index].addLinkSurf(HR);
  return;
}

void
FixedComp::addLinkComp(const size_t Index,const int SN) 
  /*!
    Add a surface to output
    \param Index :: Link number
    \param SN :: Surface number [inward looking]
  */
{
  ELog::RegMethod RegA("FixedComp","addLinkComp(int)");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU size/Index");
  
  LU[Index].addLinkComp(SN);
  return;
}

void
FixedComp::addLinkComp(const size_t Index,
		       const std::string& SList) 
  /*!
    Add a surface to output
    \param Index :: Link number
    \param SList :: String to process
  */
{
  ELog::RegMethod RegA("FixedComp","addLinkComp(std::string)");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU size/Index");

  LU[Index].addLinkComp(SList);
  return;
}

void
FixedComp::addLinkComp(const size_t Index,const HeadRule& HR) 
  /*!
    Add a surface to output
    \param Index :: Link number
    \param HR :: Surface rule to add
  */
{
  ELog::RegMethod RegA("FixedComp","addLinkComp(HeadRule)");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU size/Index");
  
  LU[Index].addLinkComp(HR);
  return;
}
  
void
FixedComp::setLinkSurf(const size_t Index,
		       const std::string& SList) 
  /*!
    Set a surface to output
    \param Index :: Link number
    \param SList :: String to process
  */
{
  ELog::RegMethod RegA("FixedComp","setLinkSurf(string)");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU size/Index");

  LU[Index].setLinkSurf(SList);
  return;
}

void
FixedComp::setLinkSurf(const size_t Index,
		       const HeadRule& HR) 
  /*!
    Set a surface to output
    \param Index :: Link number
    \param HR :: HeadRule to add
  */
{
  ELog::RegMethod RegA("FixedComp","setLinkSurf(HR)");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU size/Index");

  LU[Index].setLinkSurf(HR);
  return;
}

void
FixedComp::setLinkSurf(const size_t Index,const HeadRule& HR,
		       const bool compFlag,const HeadRule& BR,
		       const bool bridgeCompFlag) 
  /*!
    Set a link surface based on both a rule and
    a bridgin rule
    \param Index :: Link number
    \param HR :: HeadRule to add
    \param compFlag :: make primary rule complementary
    \param BR :: Bridge rule to add
    \param bridgeCompFlag :: make bridge surface complementary
  */
{
  ELog::RegMethod RegA("FixedComp","setLinkSurf(HR,BR)");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU size/Index");

  if (!compFlag)
    LU[Index].setLinkSurf(HR);
  else
    LU[Index].setLinkSurf(HR.complement());

  // bridge rule
  if (BR.hasRule())
    {
      if (!bridgeCompFlag)
	LU[Index].setBridgeSurf(BR);
      else
	LU[Index].setBridgeSurf(BR.complement());
    }
  return;
}

void
FixedComp::setLinkSurf(const size_t Index,const int SN) 
  /*!
    Set  a surface to output
    \param Index :: Link number
    \param SN :: Surface number [inward looking]
  */
{
  ELog::RegMethod RegA("FixedComp","setLinkSurf");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU size/index");

  LU[Index].setLinkSurf(SN);
  return;
}


void
FixedComp::setBridgeSurf(const size_t Index,const int SN) 
  /*!
    Set a surface to bridge output
    \param Index :: Link number
    \param SN :: Surface number [inward looking]
  */
{
  ELog::RegMethod RegA("FixedComp","setBridgeSurf");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU size/index");

  LU[Index].setBridgeSurf(SN);
  return;
}

void
FixedComp::setBridgeSurf(const size_t Index,const HeadRule& HR) 
  /*!
    Set a surface to bridge output
    \param Index :: Link number
    \param HR :: HeadRule for bridge
  */
{
  ELog::RegMethod RegA("FixedComp","setBridgeSurf(HR)");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU size/index");

  LU[Index].setBridgeSurf(HR);
  return;
}

void
FixedComp::addBridgeSurf(const size_t Index,const int SN) 
  /*!
    Add a surface to output
    \param Index :: Link number
    \param SN :: Surface number [inward looking]
  */
{
  ELog::RegMethod RegA("FixedComp","addBridgeSurf");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU size/Index");
  
  LU[Index].addBridgeSurf(SN);
  return;
}



void
FixedComp::addBridgeSurf(const size_t Index,
			 const std::string& SList) 
  /*!
    Add a surface to output
    \param Index :: Link number
    \param SList :: String to process
  */
{
  ELog::RegMethod RegA("FixedComp","addBridgeSurf(string)");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU size/Index");

  LU[Index].addBridgeSurf(SList);
  return;
}

void
FixedComp::setConnect(const size_t Index,const Geometry::Vec3D& C,
		      const Geometry::Vec3D& A)
 /*!
   Set the axis of the linked component
   \param Index :: Link number
   \param C :: Centre coordinate
   \param A :: Axis direciton
 */
{
  ELog::RegMethod RegA("FixedComp","setConnect");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU.size/index");

  LU[Index].setConnectPt(C);
  LU[Index].setAxis(A);
  return;
}

void
FixedComp::setLineConnect(const size_t Index,const Geometry::Vec3D& C,
			  const Geometry::Vec3D& A)
 /*!
   Set the axis of the linked component
   \param Index :: Link number
   \param C :: Centre coordinate
   \param A :: Axis direciton
 */
{
  ELog::RegMethod RegA("FixedComp","setLineConnect");

  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU.size/index");

  LinkUnit& LObj=LU[Index];
  LObj.populateSurf();
  const Geometry::Vec3D Pt=
    SurInter::getLinePoint(C,A,LObj.getMainRule(),LObj.getCommonRule()); 
  LObj.setConnectPt(Pt);
  LObj.setAxis(A);

  return;
}

void
FixedComp::setUSLinkComplement(const size_t Index,
			       const FixedComp& FC,
			       const size_t sideIndex)
  /*!
    Copy the opposite (as if joined) link surface 
    Note that the surfaces are complemented
    \param Index :: Link number
    \param FC :: Other Fixed component to copy object from
    \param sideIndex :: link unit of other object
  */
{
  ELog::RegMethod RegA("FixedComp","setUSLinkComplement");

  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU size/index");
  if (sideIndex>=FC.LU.size())
    throw ColErr::IndexError<size_t>(sideIndex,FC.LU.size(),"FC/index");
  
  LU[Index]=FC.LU[sideIndex];
  LU[Index].complement();
  return;
}

void
FixedComp::setUSLinkCopy(const size_t Index,
		       const FixedComp& FC,
		       const size_t sideIndex)
  /*!
    Copy the opposite (as if joined) link surface 
    Note that the surfaces are complemented
    \param Index :: Link number
    \param FC :: Other Fixed component to copy object from
    \param sideIndex :: link unit of other object
  */
{
  ELog::RegMethod RegA("FixedComp","setUSLinkCopy");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"LU size/index");
  if (sideIndex>=FC.LU.size())
    throw ColErr::IndexError<size_t>(sideIndex,FC.LU.size(),"FC/index");
  
  LU[Index]=FC.LU[sideIndex];
  return;
}

void
FixedComp::setLinkSignedCopy(const size_t Index,
			     const FixedComp& FC,
			     const long int sideIndex)
  /*!
    Copy the opposite (as if joined) link surface 
    Note that the surfaces are complemented
    \param Index :: Link number
    \param FC :: Other Fixed component to copy object from
    \param sideIndex :: signed link unit of other object
  */
{
  ELog::RegMethod RegA("FixedComp","setLinkSurf");
  if (sideIndex>0)
    setUSLinkCopy(Index,FC,static_cast<size_t>(sideIndex-1));
  else if (sideIndex<0)   // complement form
    setUSLinkComplement(Index,FC,static_cast<size_t>(-1-sideIndex));
  else
    throw ColErr::IndexError<long int>
      (sideIndex,static_cast<long int>(FC.LU.size()),"FC/index");

  return;
}

void
FixedComp::setBasicExtent(const double XWidth,const double YWidth,
			  const double ZWidth)
 /*!
   Set the axis of the linked component
   \param XWidth :: Axis direciton Width (half)
   \param YWidth :: Axis direciton width (half)
   \param ZWidth :: Axis direciton width (half)
 */
{
  ELog::RegMethod RegA("FixedComp","setBasicExtent");
  if (LU.size()!=6)
    throw ColErr::MisMatch<int>(6,static_cast<int>(LU.size()),
				"6/LU.size");
  

  LU[0].setConnectPt(Origin-Y*XWidth);
  LU[1].setConnectPt(Origin+Y*XWidth);
  LU[2].setConnectPt(Origin-X*YWidth);
  LU[3].setConnectPt(Origin+X*YWidth);
  LU[4].setConnectPt(Origin-Z*ZWidth);
  LU[5].setConnectPt(Origin+Z*ZWidth);

  LU[0].setAxis(-Y);
  LU[1].setAxis(Y);
  LU[2].setAxis(-X);
  LU[3].setAxis(X);
  LU[4].setAxis(-Z);
  LU[5].setAxis(Z);

  return;
}

const LinkUnit&
FixedComp::operator[](const size_t Index) const
 /*!
   Get the axis of the linked component
   \param Index :: Link number
   \return LinkUnit reference
 */
{
  ELog::RegMethod RegA("FixedComp","operator[]");
  if (Index>LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"Index/LU.size");

  return LU[Index];
}

const LinkUnit&
FixedComp::getLU(const size_t Index) const
 /*!
   Get the axis of the linked component
   \param Index :: Link number
   \return LinkUnit reference
 */
{
  ELog::RegMethod RegA("FixedComp","getLinkUnit");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"Index/LU.size");

  return LU[Index];
}

const LinkUnit&
FixedComp::getSignedLU(const long int sideIndex) const
  /*!
    Accessor to the link unit
    \param sideIndex :: SIGNED +1 side index
    \return Link Unit 
  */
{
  ELog::RegMethod RegA("FixedComp","getSignedLU:"+keyName);

  if (sideIndex)
    {
      const size_t linkIndex=
	(sideIndex>0) ? static_cast<size_t>(sideIndex-1) :
	static_cast<size_t>(-sideIndex-1) ;
      if (linkIndex<LU.size())
	return LU[linkIndex];
    }
  throw ColErr::IndexError<long int>
    (sideIndex,static_cast<long int>(LU.size()),"Index/LU.size");
}

LinkUnit&
FixedComp::getSignedLU(const long int sideIndex) 
  /*!
    Accessor to the link unit
    \param sideIndex :: SIGNED +1 side index
    \return Link Unit 
  */
{
  ELog::RegMethod RegA("FixedComp","getSignedLU:"+keyName);

  if (sideIndex)
    {
      const size_t linkIndex=
	(sideIndex>0) ? static_cast<size_t>(sideIndex-1) :
	static_cast<size_t>(-sideIndex-1) ;
      if (linkIndex<LU.size())
	return LU[linkIndex];
    }
  throw ColErr::IndexError<long int>
    (sideIndex,static_cast<long int>(LU.size()),"Index/LU.size");
}

  
int
FixedComp::getUSLinkSurf(const size_t Index) const
  /*!
    Accessor to the link surface string
    \param Index :: Link number
    \return Surface Key number
  */
{
  ELog::RegMethod RegA("FixedComp","getUSLinkSurf");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"Index to big");
  
  return LU[Index].getLinkSurf();
}

void
FixedComp::nameSideIndex(const size_t lP,
                         const std::string& linkName)
  /*!
    Set the named sideIndex
    \param lP :: Link point
    \param linkName :: keyname for link point
   */
{
  ELog::RegMethod RegA("FixedComp","nameSideIndex"+keyName);

  if (keyMap.find(linkName)!=keyMap.end())
    ColErr::InContainerError<std::string>(linkName,"linkName exists");

  keyMap.emplace(linkName,lP);
  return;
}
  
long int
FixedComp::getSideIndex(const std::string& sideName) const
  /*!
    Find the sideIndex from the name
    \param sideName :: Name with +/- at front if require to change 
    \return sideIndex which is signed
  */
{
  ELog::RegMethod RegA("FixedComp","getSideIndex");
  if (!sideName.empty())
    {
      const std::string partName=
        (sideName[0]=='+' || sideName[0]=='-' || sideName[0]=='#') ?
           sideName.substr(1) : sideName;
      std::map<std::string,size_t>::const_iterator mc=
        keyMap.find(partName);

      if (mc!=keyMap.end())
        return (sideName[0]!='+') ?
          -static_cast<long int>(mc->second+1) :
          static_cast<long int>(mc->second+1);

      if (partName=="Origin" || partName=="origin")
        return 0;
    }
  throw ColErr::InContainerError<std::string>(sideName,"sideName");
}
  
std::vector<Geometry::Vec3D>
FixedComp::getAllLinkPts() const
  /*!
    Accessor to all the link point
    \return Link points
  */
{
  ELog::RegMethod RegA("FixedComp","getAllLinkPts:"+keyName);

  std::vector<Geometry::Vec3D> LPout;
  
  for(const LinkUnit& lunit : LU)
    LPout.push_back(lunit.getConnectPt());

  return LPout;
}

double
FixedComp::getLinkDistance(const long int AIndex,
                           const long int BIndex) const
  /*!
    Accessor to the distance between link points
    \param AIndex :: SIGNED +1 side index
    \param BIndex :: SIGNED +1 side index 
    \return Distance between points
  */
{
  ELog::RegMethod RegA("FixedComp","getLinkDistance:"+keyName);

  if (AIndex==BIndex) return 0.0;
  return getLinkPt(AIndex).Distance(getLinkPt(BIndex));
}

Geometry::Vec3D
FixedComp::getLinkPt(const std::string& sideName) const
  /*!
    Accessor to the link point
    \param sideName :: named link point
    \return Link point
  */
{
  ELog::RegMethod RegA("FixedComp","getLinkPt[str]:"+keyName);

  const long sideIndex =getSideIndex(sideName);
  return getLinkPt(sideName);
}


Geometry::Vec3D
FixedComp::getLinkAxis(const std::string& sideName) const
  /*!
    Accessor to the link axis
    \param sideName :: named link point
    \return Link point
  */
{
  ELog::RegMethod RegA("FixedComp","getLinkAxis[str]:"+keyName);

  const long sideIndex =getSideIndex(sideName);
  return getLinkAxis(sideName);
}
  
Geometry::Vec3D
FixedComp::getLinkPt(const long int sideIndex) const
  /*!
    Accessor to the link point
    \param sideIndex :: SIGNED +1 side index
    \return Link point
  */
{
  ELog::RegMethod RegA("FixedComp","getLinkPt:"+keyName);

  if (!sideIndex) return Origin;
  const LinkUnit& LItem=getSignedLU(sideIndex);
  return LItem.getConnectPt();
}

int
FixedComp::getLinkSurf(const long int sideIndex) const
  /*!
    Accessor to the link surface string
    \param sideIndex :: Link number
    \return Surface Key number
  */
{
  ELog::RegMethod RegA("FixedComp","getLinkSurf");
  if (!sideIndex) return 0;
  
  const LinkUnit& LItem=getSignedLU(sideIndex);
  const int sign((sideIndex>0) ? 1 : -1);
  return sign*LItem.getLinkSurf();
}


Geometry::Vec3D
FixedComp::getLinkAxis(const long int sideIndex) const
  /*!
    Accessor to the link axis
    \param sideIndex :: SIGNED +1 side index
    \return signed Link Axis [Y is sideIndex == 0]
  */
{
  ELog::RegMethod RegA("FixedComp","getLinkAxis:"+keyName);

  if (sideIndex==0)
    return Y;
  
  const LinkUnit& LItem=getSignedLU(sideIndex);
  return (sideIndex>0)  ? LItem.getAxis() : -LItem.getAxis();
}

std::string
FixedComp::getLinkString(const long int sideIndex) const
  /*!
    Accessor to the link string
    \param sideIndex :: SIGNED +1 side index
    \return Link string 
  */
{
  ELog::RegMethod RegA("FixedComp","getLinkString:"+keyName);

  if (!sideIndex) return "";
  
  const size_t linkIndex=
    (sideIndex>0) ? static_cast<size_t>(sideIndex-1) :
    static_cast<size_t>(-sideIndex-1) ;

  return (sideIndex>0) ?
    getUSLinkString(linkIndex) : getUSLinkComplement(linkIndex);
}
  
std::string
FixedComp::getUSLinkString(const size_t Index) const
  /*!
    Accessor to the link surface string
    \param Index :: Link number
    \return String of link
  */
{
  ELog::RegMethod RegA("FixedComp","getUSLinkString");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"Index/LU.size");
  
  return LU[Index].getLinkString();
}

std::string
FixedComp::getUSLinkComplement(const size_t Index) const
  /*!
    Accessor to the link surface string [negative]
    \param Index :: Link number
    \return String of link
  */
{
  ELog::RegMethod RegA("FixedComp","getUSLinkComplement");
  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"Index/LU.size");

  HeadRule RP;
  RP.procString(LU[Index].getMain());
  RP.makeComplement();
  
  RP.addIntersection(LU[Index].getCommon());
  return RP.display();
}

void 
FixedComp::setCentre(const Geometry::Vec3D& C)
  /*!
    User Interface to LU[1] to set Point + Axis
    \param C :: Centre point
  */
{
  ELog::RegMethod RegA("FixedComp","setCentre");
  Origin=C;
  return;
}

void 
FixedComp::setExit(const Geometry::Vec3D& C,
		   const Geometry::Vec3D& A) 
  /*!
    User Interface to LU[1] to set Point + Axis
    \param C :: Connect point
    \param A :: Axis
  */
{
  ELog::RegMethod RegA("FixedComp","setExit");
  if (LU.size()<2)
    throw ColErr::IndexError<size_t>(2,LU.size(),"2/LU.size");

  LU[1].setAxis(A);
  LU[1].setConnectPt(C);

  return;
}

const Geometry::Vec3D&
FixedComp::getExit() const
  /*!
    Get Exit if set / Default to Origin
    \return Exit point
  */
{
  return (LU.size()>1 && LU[1].hasConnectPt())  ? 
    LU[1].getConnectPt() : Origin;
}

const Geometry::Vec3D&
FixedComp::getExitNorm() const
  /*!
    Get exit normal if set / Default to Beam axis
    \return Exit direction
  */
{
  return (LU.size()>1 && LU[1].hasAxis())  ? 
    LU[1].getAxis() : beamAxis;
}

size_t
FixedComp::findLinkAxis(const Geometry::Vec3D& AX) const
  /*!
    Determine the closest direciton to the given axis
    \param AX :: Axis point to test
    \return index value
  */
{
  ELog::RegMethod RegA("FixedComp","findLinkAxis");

  size_t outIndex(0);
  double maxValue(-1.0);  
  for(size_t i=0;i<LU.size();i++)
    {
      const double MX=AX.dotProd(LU[i].getAxis());
      if (MX>maxValue)
	{
	  outIndex=i;
	  maxValue=MX;
	}
    }
  return outIndex;
}
  
void
FixedComp::selectAltAxis(const long int sideIndex,
			 Geometry::Vec3D& XOut,Geometry::Vec3D& YOut,
			 Geometry::Vec3D& ZOut) const
  /*!
    From a given index find the optimal X,Y,Z axis to use for the
    output: YOut is compared with beam to find closes axis.
    \param sideIndex :: Link surface to use
    \param XOut :: X axis
    \param YOut :: Y axis [ beam ]
    \param ZOut :: Z axis 
  */
{
  ELog::RegMethod RegA("FixedComp","selectAltAxis");
  
  YOut=getLinkAxis(sideIndex);

  double dp[3];
  dp[0]=std::abs(X.dotProd(YOut)); 
  dp[1]=std::abs(Y.dotProd(YOut)); 
  dp[2]=std::abs(Z.dotProd(YOut)); 
  const double* dptr=std::max_element(dp,dp+3);

  XOut=(dptr==dp) ? Y : X;
  ZOut=(dptr==dp+2) ? Y : Z;
    
  return;
}

void
FixedComp::applyRotation(const Geometry::Vec3D& Axis,
			 const double Angle)
  /*!
    Apply a rotation to the basis set
    \param Axis :: rotation axis 
    \param Angle :: rotation angle
  */
{
  ELog::RegMethod RegA("FixedComp","applyRotation");

  const Geometry::Quaternion Qrot=
    Geometry::Quaternion::calcQRotDeg(Angle,Axis.unit());
  
  Qrot.rotate(X);
  Qrot.rotate(Y);
  Qrot.rotate(beamAxis);
  Qrot.rotate(Z);
  return;
}

HeadRule
FixedComp::getFullRule(const long int sideIndex) const
  /*!
    Get the main full rule.
    \param sideIndex :: Index for LinkUnit
    \return Main HeadRule
   */
{
  ELog::RegMethod RegA("FixedComp","getMainRule"); 

  const LinkUnit& LObj=getSignedLU(sideIndex);
  HeadRule Out=(sideIndex>0) ? 
    LObj.getMainRule() :
    LObj.getMainRule().complement();
  Out.addIntersection(LObj.getCommonRule());
  return Out;
}

HeadRule
FixedComp::getMainRule(const long int sideIndex) const
  /*!
    Get the main rule.
    \param sideIndex :: Index for LinkUnit
    \return Main HeadRule
   */
{
  ELog::RegMethod RegA("FixedComp","getMainRule"); 

  const LinkUnit& LObj=getSignedLU(sideIndex);
  return (sideIndex>0) ? 
    LObj.getMainRule() :
    LObj.getMainRule().complement();
}


const HeadRule&
FixedComp::getUSMainRule(const size_t Index) const
  /*!
    Get the main rule.
    \param Index :: Index for LinkUnit
    \return Main HeadRule
   */
{
  ELog::RegMethod RegA("FixedComp","getMainRule"); 

  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"Index/LU.size");
  
  return LU[Index].getMainRule();
}

  
HeadRule
FixedComp::getCommonRule(const long int sideIndex) const
  /*!
    Get the main rule.
    \param sideIndex :: Index for LinkUnit [signed]
    \return Main HeadRule
  */
{
  ELog::RegMethod RegA("FixedComp","getSignedCommonRule"); 

  const LinkUnit& LObj=getSignedLU(sideIndex);
  return LObj.getCommonRule();
}

const HeadRule&
FixedComp::getUSCommonRule(const size_t Index) const
  /*!
    Get the common rule.
    \param Index :: Index for link unit
    \return Common Headrule 
   */
{
  ELog::RegMethod RegA("FixedComp","getMainRule"); 

  if (Index>=LU.size())
    throw ColErr::IndexError<size_t>(Index,LU.size(),"Index/LU.size");
  
  return LU[Index].getCommonRule();
}

void
FixedComp::calcLinkAxis(const long int sideIndex,
			Geometry::Vec3D& XVec,
			Geometry::Vec3D& YVec,
			Geometry::Vec3D& ZVec) const
  /*!
    Given a sideIndex calculate the axises at that point.
    biasing the prefrence to select Z relative to 
    \param sideIndex :: Signed side+1 (zero for origin of FC)
    \param XVec :: Output X Vec
    \param YVec :: Output Y Vec
    \param ZVec :: Output Z Vec
  */
{
  ELog::RegMethod RegA("FixedComp","calcLinkAxis");

  if (sideIndex==0)
    {
      XVec=X;
      YVec=Y;
      ZVec=Z;
      return;
    }
  YVec=getLinkAxis(sideIndex);
  // Y not parallel to Z case
  const double ZdotYVec=Z.dotProd(YVec);

  
  const Geometry::Vec3D& ZPrime=
    (fabs(ZdotYVec) < 1.0-Geometry::zeroTol) ? Z : X;

  XVec=YVec*ZPrime;
  ZVec=YVec*XVec;
  // note that ZdotYVec could have been invalidated by swapping
  // x for z so have to recalc Y.Z'.
  if ((ZVec.dotProd(ZPrime) * ZPrime.dotProd(YVec))< -Geometry::zeroTol)
    {
      ZVec*=-1.0;
      XVec*=-1.0;
    }
  return;
}

void
FixedComp::applyRotation(const localRotate& LR)
  /*!
    Apply rotation
    \param LR :: Local rotation system
  */
{
  ELog::RegMethod RegA("FixedComp","applyRotation(localRotate)");

  LR.applyFullAxis(X);
  LR.applyFullAxis(Y);
  LR.applyFullAxis(Z);
  LR.applyFullAxis(beamAxis);
  
  LR.applyFull(Origin);
  LR.applyFull(beamOrigin);

  for(LinkUnit& linkItem : LU)
    linkItem.applyRotation(LR);
  
  return;
}
  

int
FixedComp::getExitWindow(const long int sideIndex,
			 std::vector<int>& window) const
  /*!
    Generic exit window system : 
    -- Requires at least 6 surfaces
    -- Requires 3-6 to be sign surf
    \param sideIndex :: Link point
    \param window :: window vector of paired planes
    \return Viewed surface
  */
{
  ELog::RegMethod RegA("FixedComp","getExitWindow");
  if (LU.size()<5)
    throw ColErr::IndexError<size_t>(LU.size(),6,"Link size too small");

  const size_t outIndex((sideIndex>0) ?
			static_cast<size_t>(sideIndex)-1 :
			static_cast<size_t>(-sideIndex)-1);
			
  if (outIndex>LU.size())
    throw ColErr::IndexError<size_t>(outIndex,6,"outIndex too big");

  // Get four surfaces for size:
  size_t oA[4]={2,3,4,5};
  if (outIndex>=2 && outIndex<=5)
    {
      if (outIndex>4)
	{
	  oA[2]=0;
	  oA[3]=1;
	}
      else
	{
	  oA[0]=0;
	  oA[1]=1;
	}
    }

  window.clear();
  for(size_t i=0;i<4;i++)
    window.push_back(std::abs(getUSLinkSurf(oA[i])));
  // check two pairs of order:

  const Geometry::Vec3D& aX=LU[0].getAxis();
  const Geometry::Vec3D& bX=LU[1].getAxis();
  const Geometry::Vec3D& cX=LU[2].getAxis();

  
  if (std::abs<double>(aX.dotProd(bX))>0.99)
    std::swap(window[1],window[2]);
  else if (std::abs<double>(bX.dotProd(cX))>0.99)
    std::swap(window[2],window[3]);

  //
  // Extremely ugly code to extract the dividing surface [if it exists]
  //   -- determine if there is a secondary surface to the linkstring
  //      if so add as the divide plane
  //      ELSE add zero
  //
  window.push_back(0);
  std::string OutSurf=getUSLinkString(outIndex);
  int dSurf(0);
  const int primOutSurf(getUSLinkSurf(outIndex));

  for(size_t i=0;i<2 && StrFunc::section(OutSurf,dSurf) 
	&& std::abs(dSurf)==std::abs(primOutSurf);i++) ;

  if (dSurf && std::abs(dSurf)!=std::abs(primOutSurf))
    window.back()=dSurf;
    
  return std::abs(SMap.realSurf(getUSLinkSurf(outIndex)));
}

}  // NAMESPACE attachSystem
