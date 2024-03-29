/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t2Build/VacVessel.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "ContainedComp.h"
#include "Groove.h"
#include "Hydrogen.h"
#include "VacVessel.h"

namespace moderatorSystem
{

VacVessel::VacVessel(const std::string& Key)  :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

VacVessel::VacVessel(const VacVessel& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  BVec(A.BVec),AltVec(A.AltVec),
  vacPosGap(A.vacPosGap),vacNegGap(A.vacNegGap),
  vacPosRadius(A.vacPosRadius),vacNegRadius(A.vacNegRadius),
  vacLSide(A.vacLSide),vacRSide(A.vacRSide),vacTop(A.vacTop),vacBase(A.vacBase),
  alPos(A.alPos),alNeg(A.alNeg),alSide(A.alSide),alTop(A.alTop),
  alBase(A.alBase),terPos(A.terPos),terNeg(A.terNeg),terSide(A.terSide),
  terTop(A.terTop),terBase(A.terBase),outPos(A.outPos),outNeg(A.outNeg),
  outSide(A.outSide),outTop(A.outTop),outBase(A.outBase),
  alMat(A.alMat),outMat(A.outMat)
  /*!
    Copy constructor
    \param A :: VacVessel to copy
  */
{}

VacVessel&
VacVessel::operator=(const VacVessel& A)
  /*!
    Assignment operator
    \param A :: VacVessel to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      BVec=A.BVec;
      AltVec=A.AltVec;
      vacPosGap=A.vacPosGap;
      vacNegGap=A.vacNegGap;
      vacPosRadius=A.vacPosRadius;
      vacNegRadius=A.vacNegRadius;
      vacLSide=A.vacLSide;
      vacRSide=A.vacRSide;
      vacTop=A.vacTop;
      vacBase=A.vacBase;
      alPos=A.alPos;
      alNeg=A.alNeg;
      alSide=A.alSide;
      alTop=A.alTop;
      alBase=A.alBase;
      terPos=A.terPos;
      terNeg=A.terNeg;
      terSide=A.terSide;
      terTop=A.terTop;
      terBase=A.terBase;
      outPos=A.outPos;
      outNeg=A.outNeg;
      outSide=A.outSide;
      outTop=A.outTop;
      outBase=A.outBase;
      alMat=A.alMat;
      outMat=A.outMat;
    }
  return *this;
}

VacVessel::~VacVessel() 
 /*!
   Destructor
 */
{}

void
VacVessel::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Simulation to use
 */
{
  ELog::RegMethod RegA("VacVessel","populate");
  
  // First get inner widths:
  vacPosGap=Control.EvalVar<double>(keyName+"VacPosGap");
  vacNegGap=Control.EvalVar<double>(keyName+"VacNegGap");
  vacPosRadius=Control.EvalVar<double>(keyName+"VacPosRadius");
  vacNegRadius=Control.EvalVar<double>(keyName+"VacNegRadius");

  // Vac values 
  vacLSide=Control.EvalPair<double>(keyName+"VacLSide",
					keyName+"VacSide");
  vacRSide=Control.EvalPair<double>(keyName+"VacRSide",
			       keyName+"VacSide");

  vacTop=Control.EvalVar<double>(keyName+"VacTop");
  vacBase=Control.EvalVar<double>(keyName+"VacBase");

  // Al values
  alPos=Control.EvalVar<double>(keyName+"AlPos");
  alNeg=Control.EvalVar<double>(keyName+"AlNeg");
  alSide=Control.EvalVar<double>(keyName+"AlSide");
  alTop=Control.EvalVar<double>(keyName+"AlTop");
  alBase=Control.EvalVar<double>(keyName+"AlBase");

  // Ter values
  terPos=Control.EvalVar<double>(keyName+"TerPos");
  terNeg=Control.EvalVar<double>(keyName+"TerNeg");
  terSide=Control.EvalVar<double>(keyName+"TerSide");
  terTop=Control.EvalVar<double>(keyName+"TerTop");
  terBase=Control.EvalVar<double>(keyName+"TerBase");

  // Out values
  outPos=Control.EvalVar<double>(keyName+"OutPos"); 
  outNeg=Control.EvalVar<double>(keyName+"OutNeg");
  outSide=Control.EvalVar<double>(keyName+"OutSide");
  outTop=Control.EvalVar<double>(keyName+"OutTop");
  outBase=Control.EvalVar<double>(keyName+"OutBase");
  // Out values
  clearPos=Control.EvalVar<double>(keyName+"ClearPos");
  clearNeg=Control.EvalVar<double>(keyName+"ClearNeg");
  clearSide=Control.EvalVar<double>(keyName+"ClearSide");
  clearTop=Control.EvalVar<double>(keyName+"ClearTop");
  clearBase=Control.EvalVar<double>(keyName+"ClearBase");
  
  // Material
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  outMat=ModelSupport::EvalMat<int>(Control,keyName+"OutMat");

  return;
}
  

void
VacVessel::createBoundary(const attachSystem::FixedComp& FUnit)
  /*!
    Given one object : get the extrema points base on the boundary
    \param FUnit ::  FixedComp unit
  */
{
  ELog::RegMethod RegA("VacVessel","createBoundary(F)");
  // Sides/Top/Base:
  for(size_t i=0;i<6;i++)
    BVec[i]=FUnit[i].getConnectPt();
  
  divideSurf= FUnit.getLinkSurf(-1);

  return;
}

void
VacVessel::doubleBoundaryCheck()
  /*!
    Boundary check -- this uses the alternat numbers (as needed)
    if we need to orientate relative to X/Z
  */
{
  ELog::RegMethod RegA("VacVessel","doubleBoundaryCheck");

  if (AltVec.size()>=4)
    {
      Geometry::Vec3D A,B;
      
      A=BVec[2]-Origin;
      B=AltVec[0]-Origin;
      BVec[2]= (A.dotProd(X) < B.dotProd(X)) ? A : B;

      A=BVec[3]-Origin;
      B=AltVec[1]-Origin;
      BVec[3]= (A.dotProd(X) > B.dotProd(X)) ? A : B;

      A=BVec[4]-Origin;
      B=AltVec[2]-Origin;
      BVec[4]= (A.dotProd(Z) < B.dotProd(Z)) ? A : B;

      A=BVec[5]-Origin;
      B=AltVec[3]-Origin;
      BVec[5]= (A.dotProd(Z) > B.dotProd(Z)) ? A : B;

      for(size_t i=2;i<6;i++)
	BVec[i]+=Origin;
    }
  return;
}
  
void
VacVessel::createBoundary(const attachSystem::FixedComp& FUnit,
			  const attachSystem::FixedComp& GUnit)
  /*!
    Given two objects : get the extrema points base on the boundary
    \param FUnit :: First FixedComp
    \param GUnit :: Second FixedComp
  */
{
  ELog::RegMethod RegA("VacVessel","createBoundary");
  // Sides/Top/Base:

  AltVec.resize(4);
  
  BVec[2]=FUnit[2].getConnectPt();
  AltVec[0]=GUnit[2].getConnectPt();

  BVec[3]=FUnit[3].getConnectPt();
  AltVec[1]=GUnit[3].getConnectPt();

  BVec[4]=FUnit[4].getConnectPt();
  AltVec[2]=GUnit[4].getConnectPt();

  BVec[5]=FUnit[5].getConnectPt();
  AltVec[3]=GUnit[5].getConnectPt();

  // Centres: 
  BVec[0]=FUnit.getLinkPt(2);
  BVec[1]=GUnit.getLinkPt(2);
  
  divideSurf= FUnit.getLinkSurf(-1);
  return;
}

void
VacVessel::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("VacVessel","createSurface");

  // Inner Layers:
  ModelSupport::buildCylinder(SMap,buildIndex+1,
			      getSurfacePoint(0,1)-Y*vacPosRadius,
			      Z,vacPosRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+2,
			      getSurfacePoint(0,2)+Y*vacNegRadius,
			      Z,vacNegRadius);
  ModelSupport::buildPlane(SMap,buildIndex+3,getSurfacePoint(0,3),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,getSurfacePoint(0,4),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,getSurfacePoint(0,5),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,getSurfacePoint(0,6),Z);


  // SECOND LAYER:
  ModelSupport::buildCylinder(SMap,buildIndex+11,
			      getSurfacePoint(1,1)-Y*vacPosRadius,
			      Z,vacPosRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+12,
			      getSurfacePoint(1,2)+Y*vacNegRadius,
			      Z,vacNegRadius);
  ModelSupport::buildPlane(SMap,buildIndex+13,getSurfacePoint(1,3),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,getSurfacePoint(1,4),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,getSurfacePoint(1,5),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,getSurfacePoint(1,6),Z);

  // TERTIARY LAYER:
  ModelSupport::buildCylinder(SMap,buildIndex+21,
			      getSurfacePoint(2,1)-Y*vacPosRadius,
			      Z,vacPosRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+22,
			      getSurfacePoint(2,2)+Y*vacNegRadius,
			      Z,vacNegRadius);
  ModelSupport::buildPlane(SMap,buildIndex+23,getSurfacePoint(2,3),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,getSurfacePoint(2,4),X);
  ModelSupport::buildPlane(SMap,buildIndex+25,getSurfacePoint(2,5),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,getSurfacePoint(2,6),Z);

  // Outer AL LAYER:
  
  ModelSupport::buildCylinder(SMap,buildIndex+31,
			      getSurfacePoint(3,1)-Y*vacPosRadius,
			      Z,vacPosRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+32,
			      getSurfacePoint(3,2)+Y*vacNegRadius,
			      Z,vacNegRadius);
  ModelSupport::buildPlane(SMap,buildIndex+33,getSurfacePoint(3,3),X);
  ModelSupport::buildPlane(SMap,buildIndex+34,getSurfacePoint(3,4),X);
  ModelSupport::buildPlane(SMap,buildIndex+35,getSurfacePoint(3,5),Z);
  ModelSupport::buildPlane(SMap,buildIndex+36,getSurfacePoint(3,6),Z);

  // Outer Clearance
  ModelSupport::buildCylinder(SMap,buildIndex+41,
			      getSurfacePoint(4,1)-Y*vacPosRadius,
			      Z,vacPosRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+42,
			      getSurfacePoint(4,2)+Y*vacNegRadius,
			      Z,vacNegRadius);

  ModelSupport::buildPlane(SMap,buildIndex+43,getSurfacePoint(4,3),X);
  ModelSupport::buildPlane(SMap,buildIndex+44,getSurfacePoint(4,4),X);
  ModelSupport::buildPlane(SMap,buildIndex+45,getSurfacePoint(4,5),Z);
  ModelSupport::buildPlane(SMap,buildIndex+46,getSurfacePoint(4,6),Z);

  return;
}

void
VacVessel::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param Exclude :: ContainedObject exclude string
  */
{
  ELog::RegMethod RegA("VacVessel","createObjects");

  HeadRule HR;

  const HeadRule excludeHR=getRule("Internal");
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-41 -42 43 -44 45 -46");
  addOuterSurf(HR);

  // Inner 
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 -2 3 -4 5 -6");
  System.addCell(cellIndex++,0,0.0,HR*excludeHR);

  // First Al layer
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 -12 13 -14 15 -16"
				 "(1:2:-3:4:-5:6)");
  System.addCell(cellIndex++,alMat,0.0,HR);

  // Tertiay layer
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-21 -22 23 -24 25 -26"
				 "(11:12:-13:14:-15:16)");
  System.addCell(cellIndex++,0,0.0,HR);

  // Tertiay layer
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-31 -32 33 -34 35 -36"
				 "(21:22:-23:24:-25:26)");
  System.addCell(cellIndex++,outMat,0.0,HR);

  // Outer clearance
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-41 -42 43 -44 45 -46 "
				 " (31:32:-33:34:-35:36) ");
  System.addCell(cellIndex++,0,0.0,HR);

  return;
}

void
VacVessel::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("VacVessel","createLinks");

  nameSideIndex(0,"VacFront");
  nameSideIndex(1,"VacBack");
  nameSideIndex(2,"VacNeg");
  nameSideIndex(3,"VacPlus");
  nameSideIndex(4,"VacDown");
  nameSideIndex(5,"VacUp");
  nameSideIndex(6,"VacDivider");

  // For Cylindrical surface must also have a divider:
  // -- Groove:
  FixedComp::setConnect(0,BVec[0]+Y*(vacPosGap+alPos+terPos+
                                    outPos+clearNeg),Y);
  FixedComp::setConnect(1,BVec[1]-Y*(vacNegGap+alPos+terNeg+
                                    outNeg+clearNeg),-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+41));
  FixedComp::setBridgeSurf(0,SMap.realSurf(divideSurf));

  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+42));
  FixedComp::setBridgeSurf(1,-SMap.realSurf(divideSurf));

  // set Links:
  for(size_t i=2;i<6;i++)
    FixedComp::setConnect(i,getSurfacePoint(4,static_cast<long int>(i+1)),
			  getDirection(i));

  // Set Connect surfaces:
  for(int i=2;i<6;i++)
    FixedComp::setLinkSurf(static_cast<size_t>(i),
			   SMap.realSurf(buildIndex+41+i));

  FixedComp::setConnect(6,Origin,Y);
  FixedComp::setLinkSurf(6,SMap.realSurf(divideSurf));




  /* OLD buildPair
  FixedComp::setLinkSurf(6,-SMap.realSurf(buildIndex+41));
  FixedComp::setBridgeSurf(6,SMap.realSurf(divideSurf));

  Geometry::Vec3D AimPt=GMod.getLinkPt(7);
  Geometry::Vec3D LP=this->getLinkPt(1);
  LP-= Z * Z.dotProd(LP);
  LP+= Z * Z.dotProd(AimPt);
  FixedComp::setConnect(6,LP,Y);

  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+42));
  FixedComp::setBridgeSurf(7,SMap.realSurf(divideSurf));
  FixedComp::setConnect(7,HMod.getLinkPt(2),Y);  
  */
  return;
}


Geometry::Vec3D
VacVessel::getDirection(const size_t side) const
  /*!
    Determine the exit direction
    \param side :: Direction number [0-5] (internally checked)
    \return dirction vector
  */
{
  switch (side) 
    {
    case 0:
      return Y;
    case 1:
      return -Y;
    case 2:
      return -X;
    case 3:
      return X;
    case 4:
      return -Z;
    }
  return Z;
}


Geometry::Vec3D
VacVessel::getSurfacePoint(const size_t layer,
			   const long int sideIndex) const
  /*!
    Get the center point for the surfaces in each layer
    \param layer :: Layer number : 0 is inner 4 is outer
    \param sideIndex :: Index to side (front/back/left/right/up/down)
    \return point on surface
  */
{
  ELog::RegMethod RegA("VacVessel","getSurfacePoint");

  if (std::abs(sideIndex)>6) 
    throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex ");
  if (layer>4) 
    throw ColErr::IndexError<size_t>(layer,5,"layer");

  if (sideIndex==0) return Origin;

  const size_t SI(static_cast<size_t>(std::abs(sideIndex))-1);
  const double frontDist[]={vacPosGap,alPos,terPos,outPos,clearPos};
  const double backDist[]={vacNegGap,alNeg,terNeg,outNeg,clearNeg};
  const double sideLDist[]={vacLSide,alSide,terSide,outSide,clearSide};
  const double sideRDist[]={vacRSide,alSide,terSide,outSide,clearSide};
  const double topDist[]={vacTop,alTop,terTop,outTop,clearTop};
  const double baseDist[]={vacBase,alBase,terBase,outBase,clearBase};

  const double* DPtr[]={frontDist,backDist,sideLDist,
			sideRDist,topDist,baseDist};

  const Geometry::Vec3D XYZ=getDirection(SI);

  double sumVec(0.0);
  for(size_t i=0;i<=layer;i++)
    sumVec+=DPtr[SI][i];
  
  return BVec[SI]+XYZ*sumVec;
}


void
VacVessel::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)

  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Previously build  moderator
    \param sideIndex :: 
  */
{
  ELog::RegMethod RegA("VacVessel","createAll");
  populate(System.getDataBase());
 
  createUnitVector(FC,sideIndex);
  doubleBoundaryCheck();
  createSurfaces();
  createObjects(System); 
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE moderatorSystem
