/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/ReflectRods.cxx
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
#include <list>
#include <vector>
#include <set>
#include <map>
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
#include "Surface.h"
#include "surfRegister.h"
#include "SurInter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Rules.h"
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
#include "ContainedComp.h"
#include "FixedRotate.h"
#include "gridUnit.h"
#include "hexUnit.h"
#include "ReflectRods.h"

namespace ts1System
{

ReflectRods::ReflectRods(const std::string& Key,const size_t index)  :
  attachSystem::FixedRotate(Key+std::to_string(index),0),
  attachSystem::ContainedComp(),
  baseName(Key),
  topSurf(0),baseSurf(0),RefObj(0)  
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param index :: unit number
  */
{}

ReflectRods::ReflectRods(const ReflectRods& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  baseName(A.baseName),populated(A.populated),
  outerMat(A.outerMat),
  innerMat(A.innerMat),linerMat(A.linerMat),HexHA(A.HexHA),
  HexHB(A.HexHB),HexHC(A.HexHC),topCentre(A.topCentre),
  baseCentre(A.baseCentre),radius(A.radius),
  linerThick(A.linerThick),
  centSpc(A.centSpc),topSurf(A.topSurf),baseSurf(A.baseSurf),
  RefObj(A.RefObj),OPSurf(A.OPSurf),HVec(A.HVec)
  /*!
    Copy constructor
    \param A :: ReflectRods to copy
  */
{}

ReflectRods&
ReflectRods::operator=(const ReflectRods& A)
  /*!
    Assignment operator
    \param A :: ReflectRods to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      cellIndex=A.cellIndex;
      outerMat=A.outerMat;
      innerMat=A.innerMat;
      linerMat=A.linerMat;
      HexHA=A.HexHA;
      HexHB=A.HexHB;
      HexHC=A.HexHC;
      topCentre=A.topCentre;
      baseCentre=A.baseCentre;
      radius=A.radius;
      linerThick=A.linerThick;
      centSpc=A.centSpc;
      HVec=A.HVec;
    }
  return *this;
}

ReflectRods::~ReflectRods() 
  /*!
    Destructor
  */
{
  clearHVec();
}

void
ReflectRods::copyHoleCentre(const MTYPE& AMap)
  /*!
    Create a copy of the hole centres
    \param AMap :: Vector of holes to copy
  */
{
  ELog::RegMethod RegA("ReflectRods","copyHoleCentre");

  clearHVec();
  for(const MTYPE::value_type& mv : AMap)
    HVec.insert(MTYPE::value_type
		(mv.first,new constructSystem::hexUnit(*mv.second)));  
  return;
}

const Geometry::Vec3D&
ReflectRods::getHexAxis(const size_t Index) const
/*!
    Calculate the direction based on a Index 
    \param Index :: Index value [0-6]
    \return Vec3D for normal of plane
  */
{
  static const Geometry::Vec3D* VUnit[3]=
    {&HexHA,&HexHB,&HexHC};
  return *(VUnit[Index % 3]);
}

const Geometry::Vec3D
ReflectRods::getHexPoint(const size_t Index) const
/*!
    Calculate the point on the corners of the hexagon
    \param Index :: Index value [0-5]
    \return Vec3D of the point on the hex corner
  */
{
  // Extra 2.0 to deal with average of 2 unit vectors
  // and cos(30)
  const double PRadius(centSpc/3.0);
  switch (Index % 6)
    {
    case 0:
      return (HexHA+HexHB)*PRadius;
    case 1:
      return (HexHB+HexHC)*PRadius;
    case 2:
      return (HexHC-HexHA)*PRadius;
    case 3:
      return (-HexHA-HexHB)*PRadius;
    case 4:
      return (-HexHB-HexHC)*PRadius;
    case 5:
      return (-HexHC+HexHA)*PRadius;
    }
  // Should NEVER get here!!
  return HexHC;
}

void
ReflectRods::clearHVec() 
  /*!
    Delete the hole centres
  */
{
  ELog::RegMethod RegA("ReflectRods","clearHVec");

  for(MTYPE::value_type& mv : HVec)
    delete mv.second;
  
  HVec.clear();
  return;
}

void
ReflectRods::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("ReflectRods","populate");

  FixedRotate::populate(baseName,Control);
  
  centSpc=Control.EvalTail<double>(keyName,baseName,"CentSpace");
  radius=Control.EvalTail<double>(keyName,baseName,"Radius");
  linerThick=Control.EvalTail<double>(keyName,baseName,"LinerThick");
  
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat",
				      baseName+"InnerMat");
  linerMat=ModelSupport::EvalMat<int>(Control,keyName+"LinerMat",
				      baseName+"LinerMat");
  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat",
				      baseName+"OuterMat");
  

  
  return;
}

void
ReflectRods::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: FixedComponent to attach this object to.
  */
{
  ELog::RegMethod RegA("ReflectRods","createUnitVector");

  // Origin is in the wrong place as it is at the EXIT:
  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  HexHA=X*cos(M_PI*60.0/180.0)-Y*sin(M_PI*60.0/180.0);
  HexHB=X;
  HexHC=X*cos(M_PI*60.0/180.0)+Y*sin(M_PI*60.0/180.0);


  return;
}

void
ReflectRods::getZSurf()
  /*!
    Get extreme Z surfaces
  */
{
  ELog::RegMethod RegA("ReflectRods","getZSurf");

  HeadRule HR=RefObj->getHeadRule();

  const int baseZ=HR.findAxisPlane(-Z,0.95);
  const int topZ=HR.findAxisPlane(Z,0.95);
  baseSurf=dynamic_cast<const Geometry::Plane*>(HR.getSurface(baseZ));
  topSurf=dynamic_cast<const Geometry::Plane*>(HR.getSurface(topZ));


  if (!topSurf || !baseSurf)
    {
      ELog::EM<<"Failed to find top/Base surface " <<ELog::endCrit;  
      throw ColErr::NumericalAbort("Plane not found in Z intersect");
    }
  
  ELog::EM<<"Found base == "<<*baseSurf<<ELog::endDiag;
  ELog::EM<<"Found top == "<<*topSurf<<ELog::endDiag;

  return;
}

void
ReflectRods::calcCentre()
  /*!
    Calculate the centre point
  */
{
  ELog::RegMethod RegA("ReflectRods","calcCentre");

  // Extract Container items  
  const std::set<const Geometry::Surface*>& SL=
    RefObj->getSurfPtrSet();

  std::set<const Geometry::Plane*> PSet;  
  for(const Geometry::Surface* const& sPtr : SL)
    {
      const Geometry::Plane* PL=
	dynamic_cast<const Geometry::Plane*>(sPtr);
      if (PL && fabs(PL->getNormal().dotProd(Z))<0.5)
	PSet.emplace(PL);
    }

  // Loop over all pair to find possible surfaces
  std::vector<Geometry::Vec3D> OutA;
  topCentre=Geometry::Vec3D(0,0,0);
  baseCentre=Geometry::Vec3D(0,0,0);
  int cntA(0);

  std::set<const Geometry::Plane*>::const_iterator ac,bc;
  for(ac=PSet.begin();ac!=PSet.end();ac++)
    for(bc=PSet.begin();bc!=ac;bc++)
      {
	OutA=SurInter::makePoint(topSurf,*ac,*bc);
	if (!OutA.empty())
	  {
	    const Geometry::Vec3D FP=OutA.front();
	    if (RefObj->isValid(FP))
	      {
		topCentre+=FP;
		cntA++;
	      }
	  }
      }
  
  if (!cntA)
    {
      ELog::EM<<"Failed to find centre points "<<ELog::endErr;
      return;
    }
  topCentre/=cntA;
  if (baseSurf)
    {
      baseCentre=SurInter::getLinePoint(topCentre,Z,*baseSurf);
      
      ELog::EM<<"Centre[T/B] == "<<topCentre<<" : " 
	      <<baseCentre<<ELog::endDiag;
    }
  return;
}

void 
ReflectRods::createCentres(const Geometry::Plane* PX)
  /*!
    Calculate the centres
    \param PX :: Dividing plane to test against.
  */
{
  ELog::RegMethod RegA("ReflectRods","createCentres");

  int acceptFlag(1);
  int step(0);

  clearHVec();
  const int pNum(PX->getName());
  splitRefObj();

  std::vector<Geometry::Vec3D> TopPts;
  std::vector<Geometry::Vec3D> BasePts;
  Geometry::Vec3D InPt;
  
  Geometry::Vec3D DebugPT(11.46,41.29,-22.477);
  while(acceptFlag || step<2)
    {
      acceptFlag=0;
      for(int i=-step;i<=step;i++)
	for(int j=-step;j<=step;j++)
	  {
	    if (abs(i)==step || abs(j)==step)
	      {
		// Note tube *2 because separation is diameter
		const Geometry::Vec3D topC=topCentre+HexHA*(i*centSpc)+
		  HexHB*(j*centSpc);
		const Geometry::Vec3D baseC=baseCentre+HexHA*(i*centSpc)+
		  HexHB*(j*centSpc);
		const Geometry::Vec3D midC=(topC+baseC)/2.0;
		const int topBFlag= checkCorners(pNum,topC,InPt,TopPts);
		const int baseBFlag= checkCorners(pNum,baseC,InPt,TopPts);
		const int midBFlag= checkCorners(pNum,midC,InPt,TopPts);
		if (topBFlag!= -1 || baseBFlag!= -1 || midBFlag!=-1)
		  {
		    // top/base flag zero on no cut
		    const bool cutFlag((!baseBFlag && 
					!topBFlag && !midBFlag) ? 0 : 1);
		    constructSystem::hexUnit* HPtr=
		      new constructSystem::hexUnit(i,j,cutFlag,
						   (topC+baseC)/2.0);
		    if (cutFlag==1)
		      {
			HPtr->setCutSurf(calcCornerCut(InPt,TopPts));
		      }
		    acceptFlag=1;
		    HVec.insert(MTYPE::value_type(i*1000+j,HPtr));
		  }
	      }
	  }
      step++;
    }
  return;
}

int
ReflectRods::checkCorners(const int surfN,
			  const Geometry::Vec3D& C) const
  /*!
    Check if the corners of the cube fall in the object
    \param surfN :: Surface number (front/back plate)
    \param C :: Centre of cube
    \retval 0 if all inside
    \retval 1 if part outside
    \retval -1 if all outside
   */
{
  ELog::RegMethod RegA("ReflectRods","checkCorners");

  int validCnt(0);
  for(size_t k=0;k<6;k++)
    {
      const Geometry::Vec3D CX=C+getHexPoint(k);
      validCnt+=RefObj->isSideValid(CX,surfN);
    }
  if (validCnt==6) return 0;
  return (validCnt) ? 1 : -1;
}

int
ReflectRods::checkCorners(const int surfN,
			  const Geometry::Vec3D& C,
			  Geometry::Vec3D& inPt,
			  std::vector<Geometry::Vec3D>& outPts) const

  /*!
    Check if the corners of the cube fall in the object
    \param surfN :: Surface number (front/back plate)
    \param C :: Centre of cube
    \param inPt :: Points that was hit
    \param outPts :: Points that don't hit [ INCREMENTED NO CLEAR]
    \retval 0 if all inside
    \retval 1 if part outside
    \retval -1 if all outside
   */
{
  ELog::RegMethod RegA("ReflectRods","checkCorners<Vec,Vec>");

  int validCnt(0);
  for(size_t k=0;k<6;k++)
    {
      const Geometry::Vec3D CX=C+getHexPoint(k);
      if (RefObj->isSideValid(CX,surfN)) 
	{
	  validCnt++;
	  inPt=CX;
	}
      else
	outPts.push_back(CX);
    }
  if (validCnt==6) return 0;
  return (validCnt) ? 1 : -1;
}

void
ReflectRods::joinHoles()
  /*!
    Create the links between the hole centres
    \todo Optimize by sorting the holeCentre list
   */
{
  ELog::RegMethod RegA("ReflectRods","joinHoles");

  // Clear all the links first:
  for(MTYPE::value_type& mv : HVec)
    mv.second->clearLinks();

  return;
}

void
ReflectRods::createLinkSurf()
  /*!
    Create the link surfaces
  */
{
  ELog::RegMethod RegA("ReflectRods","createLinkSurf");

  // List of surfaces constructed relative to 0,0
  std::map<int,int> surfContruct;

  MTYPE::iterator ac;
  MTYPE::iterator bc;
  int planeIndex(buildIndex+5001);

  for(ac=HVec.begin();ac!=HVec.end();ac++)
    {
      // iterate over the index set [6]
      constructSystem::hexUnit* APtr = ac->second;
      for(size_t i=0;i<6;i++)
	{
	  if (!APtr->hasLink(i))
	    {
	      const Geometry::Vec3D& HAxis=getHexAxis(i);  // unsigned
	      int surNum;
	      if (i>2) 
		{
		  const Geometry::Vec3D DCent=APtr->getCentre()-
		    HAxis*(centSpc/2.0);
		  ModelSupport::buildPlane(SMap,planeIndex,DCent,HAxis);
		  surNum=SMap.realSurf(planeIndex);
		}
	      else
		{
		  const Geometry::Vec3D DCent=APtr->getCentre()+
		    HAxis*(centSpc/2.0);
		  ModelSupport::buildPlane(SMap,planeIndex,DCent,HAxis);
		  surNum=SMap.realSurf(planeIndex);
		  surNum*=-1;
		}
	      const int JA=ac->first+APtr->gridIndex(i);
	      bc=HVec.find(JA);
	      if (bc!=HVec.end())
		bc->second->setSurf((i+3) % 6,-surNum);
	      APtr->setSurf(i,surNum);
	    }	      
	  planeIndex++;

	}
    }
  return;
}


void
ReflectRods::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("ReflectRods","createSurface");
  
  createLinkSurf();

  // Create actual holes :
  Geometry::Vec3D Pt,Axis;

  int index(buildIndex);
  MTYPE::const_iterator mc;
  for(MTYPE::value_type& mc : HVec)
    {
      constructSystem::hexUnit* HPtr(mc.second);
      if (!HPtr->isEmpty())
	{
	  // cylinder
	  ModelSupport::buildCylinder(SMap,index+7,
				      HPtr->getCentre(),Z,radius);
	  HPtr->setCyl(index+7);
	  if (linerThick>Geometry::zeroTol)
	    {
	      ModelSupport::buildCylinder(SMap,index+8,HPtr->getCentre(),
					  Z,radius+linerThick);
	      HPtr->addCyl(index+8);
	    }	
	}
      index+=10;
    }
  return;
}

HeadRule
ReflectRods::plateRule() const
  /*!
    Calcuate the string for the +/- top/base surfaces
    - Allows those surfaces to be either direction
    \return surf rulle
  */
{
  std::ostringstream cx;
  
  const int tSign= (topSurf->getNormal().dotProd(Z)>0.0) ? -1 : 1;
  const int bSign= (baseSurf->getNormal().dotProd(Z)>0.0) ? 1 : -1;

  cx<<" "<<tSign*topSurf->getName()<<" "
    <<bSign*baseSurf->getName()<<" ";
  return HeadRule(cx.str());
}

void
ReflectRods::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ReflectRods","createObjects");
  int cylIndex(buildIndex);

  const HeadRule plates=plateRule();

  const int iLayer((linerThick>Geometry::zeroTol) ? 1 : 0);

  HeadRule HR;
  for(const MTYPE::value_type& mc : HVec)
    {
      const constructSystem::hexUnit* APtr= mc.second;

      const HeadRule CylA(SMap,cylIndex,-7);
      const HeadRule CylB=ModelSupport::getHeadRule(SMap,cylIndex,"7 -8");
	
      HR=APtr->getInner()*HeadRule(SMap,cylIndex+iLayer,7);

      if (!APtr->isCut())
	{
	  System.addCell(cellIndex++,innerMat,0.0,CylA*plates);
	  if (iLayer)
	    System.addCell(cellIndex++,linerMat,0.0,CylB*plates);
	  HR*=plates;
	}
      else 
	{
	  const HeadRule HRX= APtr->getCut();
	  System.addCell(cellIndex++,innerMat,0.0,CylA*plates*HRX);
	  if (iLayer)
	    System.addCell(cellIndex++,linerMat,0.0,CylB*HRX);
	  HR*=HRX*plates;
	}
      
      System.addCell(cellIndex++,outerMat,0.0,HR);
      cylIndex+=10;
    }

  System.removeCell(RefObj->getName());
  RefObj=0;
  return;
}

void
ReflectRods::splitRefObj() 
  /*!
    Split the RefObj into its constituent parts
  */
{
  ELog::RegMethod RegA("ReflectorRods","splitRefObj");
  if (RefObj) 
    {
      RefItems.clear();
      const HeadRule& HR=RefObj->getHeadRule();
      const size_t NL=HR.countNLevel(0);
      for(size_t i=1;i<=NL;i++)
	RefItems.push_back(HR.findNode(0,i));
    }
  return;
}

HeadRule
ReflectRods::calcCornerCut(const Geometry::Vec3D& InPt,
			   const std::vector<Geometry::Vec3D>& OutPt) const
  /*!
    Given a hex unit that definately cuts the edge
    calculate those extra edges that are needed from the main
    reflector unit
    \param InPt :: Good point
    \param OutPt :: Failed points
    \return string of surfaces
  */
{
  ELog::RegMethod RegA("ReflectorRods","calcCornerCut");

  HeadRule HR;
  for(const Rule* RP :  RefItems)
    {
      for(const Geometry::Vec3D& TP : OutPt)
	{
	  if (RP->isValid(InPt)!=RP->isValid(TP))
	    {
	      HR.addIntersection(RP);
	      break;
	    }
	}
    }
  return HR;
}


void
ReflectRods::printHoles() const
  /*!
    Debug function to write out holes
   */
{
  // Clear all the links first:
  for(const MTYPE::value_type& mc : HVec)
    {
      mc.second->write(ELog::EM.Estream());
    }
  ELog::EM<<ELog::endDiag;

  return;
}
  
void
ReflectRods::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Twin component to set axis etc
    \param Index :: will be orientated direction
 */
{
  ELog::RegMethod RegA("ReflectRods","createAll");
  populate(System.getDataBase());
  if (radius>Geometry::zeroTol)
    {
      createUnitVector(FC,sideIndex);
      getZSurf();
      calcCentre();
      createCentres(topSurf);
      createSurfaces();
      createObjects(System);
    }
  return;
}
  
}  // NAMESPACE ts1System
