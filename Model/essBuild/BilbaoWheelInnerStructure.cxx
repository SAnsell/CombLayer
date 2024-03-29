/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
  * File:   essBuild/BilbaoWheelInnerStructure.cxx
  *
  * Copyright (c) 2004-2024 by Stuart Ansell/Konstain Batkov
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
#include "surfRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "interPoint.h"
#include "Surface.h"
#include "Quadratic.h"
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
#include "SurInter.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Plane.h"
#include "BilbaoWheelInnerStructure.h"

namespace essSystem
{

BilbaoWheelInnerStructure::BilbaoWheelInnerStructure(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

BilbaoWheelInnerStructure::BilbaoWheelInnerStructure
(const BilbaoWheelInnerStructure& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  temp(A.temp),brickLen(A.brickLen),brickWidth(A.brickWidth),
  brickGapLen(A.brickGapLen),brickGapWidth(A.brickGapWidth),
  nSectors(A.nSectors),nBrickSectors(A.nBrickSectors),
  nBrickLayers(A.nBrickLayers),nSteelLayers(A.nSteelLayers),
  secSepThick(A.secSepThick),nBricks(A.nBricks),
  secSepMat(A.secSepMat),brickSteelMat(A.brickSteelMat),
  brickGapMat(A.brickGapMat),brickMat(A.brickMat)
  /*!
    Copy constructor
    \param A :: BilbaoWheelInnerStructure to copy
  */
{}

BilbaoWheelInnerStructure&
BilbaoWheelInnerStructure::operator=(const BilbaoWheelInnerStructure& A)
  /*!
    Assignment operator
    \param A :: BilbaoWheelInnerStructure to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      temp=A.temp;
      brickLen=A.brickLen;
      brickWidth=A.brickWidth;
      brickGapLen=A.brickGapLen;
      brickGapWidth=A.brickGapWidth;
      nSectors=A.nSectors;
      nBrickSectors=A.nBrickSectors;
      nBrickLayers=A.nBrickLayers;
      nSteelLayers=A.nSteelLayers;
      secSepThick=A.secSepThick;
      nBricks=A.nBricks;
      secSepMat=A.secSepMat;
      brickSteelMat=A.brickSteelMat;
      brickGapMat=A.brickGapMat;
      brickMat=A.brickMat;
    }
  return *this;
}

BilbaoWheelInnerStructure*
BilbaoWheelInnerStructure::clone() const
  /*!
    Clone self 
    \return new (this)
  */
{
  return new BilbaoWheelInnerStructure(*this);
}

BilbaoWheelInnerStructure::~BilbaoWheelInnerStructure()
/*!
  Destructor
*/
{}
  

void
BilbaoWheelInnerStructure::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("BilbaoWheelInnerStructure","populate");

  FixedOffset::populate(Control);
  brickLen=Control.EvalVar<double>(keyName+"BrickLength");
  brickWidth=Control.EvalVar<double>(keyName+"BrickWidth");
  brickMat=ModelSupport::EvalMat<int>(Control,keyName+"BrickMat");

  brickGapLen=Control.EvalVar<double>(keyName+"BrickGapLength");
  brickGapWidth=Control.EvalVar<double>(keyName+"BrickGapWidth");
  brickGapMat=ModelSupport::EvalMat<int>(Control,keyName+"BrickGapMat");

  nSectors=Control.EvalVar<size_t>(keyName+"NSectors");
  if (nSectors<1)
    ELog::EM << "NSectors must be >= 1" << ELog::endErr;
  secSepThick=Control.EvalVar<double>(keyName+"SectorSepThick");
  secSepMat=ModelSupport::EvalMat<int>(Control,keyName+"SectorSepMat");  

  nBrickSectors=Control.EvalVar<size_t>(keyName+"NBrickSectors");
  if (nBrickSectors>nSectors)
    throw ColErr::RangeError<size_t>(nBrickSectors,0,nSectors,
				     "nBrickSectors can not exceed nSectors:");

  nSteelLayers=Control.EvalVar<size_t>(keyName+"NSteelLayers");
  brickSteelMat=ModelSupport::EvalMat<int>(Control,keyName+"BrickSteelMat");  

  return;
}


void
BilbaoWheelInnerStructure::createSurfaces(const attachSystem::FixedComp& Wheel)
  /*!
    Create planes for the inner structure iside BilbaoWheel
    \param Wheel :: Wheel structure for inner build
  */
{
  ELog::RegMethod RegA("BilbaoWheelInnerStructure","createSurfaces");
  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin,Z);
  
  // segmentation
  const double dTheta = 360.0/static_cast<double>(nSectors);
  int SIsec(buildIndex+0);

  // -dTheta is needed to shoot a proton in the center of a sector,
  // but not between them
  // *3/2 is needed when 3 sectors with bricks are made,
  // so we need to put the 2nd one in the centre
  double theta(-1.5*dTheta);
  const Geometry::Plane* PLeft(0);
  const Geometry::Plane* PRight(0);
  const Geometry::Plane* PHold(0);

  for (size_t secIndex=0;secIndex<nSectors;secIndex++)
    {
      const double cosA(cos(theta*M_PI/180.0));
      const double sinA(sin(theta*M_PI/180.0));

            // invisible divider
      ModelSupport::buildPlaneRotAxis(SMap,SIsec+1,Origin,X,Z,theta+90.0);
      PLeft=
	ModelSupport::buildPlaneRotAxis(SMap,SIsec+3,
					Origin-X*(secSepThick/2.0*cosA)
					-Y*(secSepThick/2.0*sinA), X,Z,theta);

      
      // Mid brick surfaces if needed:
      if (secIndex && (secIndex-1)<nBrickSectors)
	createBrickSurfaces(Wheel,PRight,PLeft,secIndex-1);


      PRight=
	ModelSupport::buildPlaneRotAxis(SMap,SIsec+4,
					Origin+X*(secSepThick/2.0*cosA)
					+Y*(secSepThick/2.0*sinA), X,Z,theta);
      

      theta+=dTheta;
      SIsec += 10;
      if (!secIndex) PHold=PLeft;
    }

  // last division if needed:
  if (nSectors && nSectors==nBrickSectors)
    createBrickSurfaces(Wheel,PRight,PHold,nSectors-1);
  
  return; 
}


void
BilbaoWheelInnerStructure::createObjects(Simulation& System,
					 attachSystem::FixedComp& Wheel)
 /*!
   Create the objects.
   Note that wheel is NOt const as we need to delete its inner cell.
   \param System :: Simulation to add results
   \param Wheel :: Wheel object where the inner structure is to be added
   
 */
{
  ELog::RegMethod RegA("BilbaoWheelInnerStructure","createObjects");
  
  attachSystem::CellMap* CM =
    dynamic_cast<attachSystem::CellMap*>(&Wheel);
  if (!CM)
    throw ColErr::DynamicConv("FixedComp","CellMap",Wheel.getKeyName());
  
  const std::pair<int,double> MatInfo=
    CM->deleteCellWithData(System,"Inner");
  
  const int innerMat = MatInfo.first;
  temp = MatInfo.second;
  
  const HeadRule vertHR =
    Wheel.getFullRule(7)*
    Wheel.getFullRule(8); // top+bottom
  const HeadRule cylHR =
    Wheel.getFullRule(9)*
    Wheel.getFullRule(10); // min+max radii


  int SIsec(buildIndex+0), SI1;
  HeadRule HR;
  if (nSectors==1)
    System.addCell(cellIndex++,innerMat,temp,vertHR*cylHR); // same as "Inner" cell from BilbaoWheel
  else 
    {
      for (size_t j=0;j<nSectors;j++)
	{
	  // Tungsten
	  SI1 = (j!=nSectors-1) ? SIsec+10 : buildIndex+0;
	  HR=ModelSupport::getHeadRule(SMap,SIsec,SI1,"4 -3M");
	  if (j>=nBrickSectors)
	    System.addCell(cellIndex++,innerMat,temp,
			   HR*vertHR*cylHR);
	  else
	    {
	      createBricks(System, Wheel, 
			   HeadRule(SMap,SIsec,4), 
			   HeadRule(SMap,SI1,-3), j); // another side plane
	    }
	    
	    // Pieces of steel between Tungsten sectors
	    // -1 is needed since planes 3 and -4 cross Tunsten in two places,
	    //     so we need to select only one
	    HR = ModelSupport::getHeadRule(SMap,SIsec,"3 -4 -1");
	    System.addCell(cellIndex++,secSepMat,temp,HR*vertHR*cylHR);

	    SIsec+=10;
	}
    }
  return; 
}

void
BilbaoWheelInnerStructure::createBrickSurfaces
(const attachSystem::FixedComp& Wheel,const Geometry::Plane *pSide1,
 const Geometry::Plane *pSide2,const size_t sector)
  /*
    Creates surfaces for individual Tungsten bricks
    \pararm Wheel 
    \param pSide1 :: wheel segment side plane
    \parampSide2 :: wheel segment side plane
    \param sector :: number of sector for surface index offset
    \todo THIS METHOD IS SIMPLY A MESS.
  */
{
  ELog::RegMethod RegA("BilbaoWheelInnerStructure","createBrickSurfaces");
  
  const Geometry::Surface *innerCyl =
    SMap.realSurfPtr(Wheel.getLinkSurf(9));
  const Geometry::Surface *outerCyl =
    SMap.realSurfPtr(Wheel.getLinkSurf(10));
  
  const Geometry::Plane *pz = SMap.realPtr<Geometry::Plane>(buildIndex+5);
  
  const double sectorAngle = getSectorAngle(sector)*M_PI/180.0;
  Geometry::Vec3D nearPt(125*sin(sectorAngle), -125*cos(sectorAngle), 0);
  nearPt += Origin;
  
  Geometry::Vec3D p1 = SurInter::getPoint(pSide1, outerCyl, pz, nearPt);
  Geometry::Vec3D p2 = SurInter::getPoint(pSide2, outerCyl, pz, nearPt);
  Geometry::Vec3D p3 = p2 + Geometry::Vec3D(0.0, 0.0, 1.0);
  
  // radial planes
  int SI(buildIndex+1000*(static_cast<int>(sector)+1));
  // first (outermost) layer
  Geometry::Plane *prad1(0); // first radial plane of the bricks
  Geometry::Vec3D p4;        // intersection of radial plane and inner cylinder:
  size_t iLayer=0;
  for (;;) // while we are between outer and inner cylinders
    {
      if (iLayer==0)
	{
	  // CHANGE FROM (0,0,0) as that is a NORMAL!!!
	  prad1 = ModelSupport::buildPlane(SMap, SI+5, p1, p2, p3,Y);
	}
      else
	{
	  // plane which goes after brick and gap
	  ModelSupport::buildShiftedPlane
	    (SMap,SI+5,prad1,
	     -(brickLen+brickGapLen)*static_cast<double>(iLayer));
	}
      
      // back side of the brick
      Geometry::Plane *ptmp =ModelSupport::buildShiftedPlane
	(SMap,SI+6,prad1,-((brickLen+brickGapLen)*
			     static_cast<double>(iLayer)+brickLen));
      try
	{
	  p4 = SurInter::getPoint(ptmp,innerCyl,pz,nearPt);
	}
      catch (ColErr::IndexError<size_t>& IE)
	{
	  //	    std::cout << "does not intersept" << std::endl;
	}

      // if back of the brick crosses inner surface
      if (p4.abs()>Geometry::zeroTol) 
	{
	  nBrickLayers = iLayer;
	  break;
	}
      
      iLayer++;
      SI += 10;
    }
  
  // tangential (perpendicular to radial) planes
  // only 2 layers are needed since other layers use the same planes
  Geometry::Plane *ptan1 = 0; // first tangential plane of the bricks
  int SJ(buildIndex+1000*(static_cast<int>(sector)+1));
  for (int i=0; i<50; i++)
    {
      // 1st layer
      if (i==0)
	ptan1 = ModelSupport::buildRotatedPlane(SMap, SJ+1, prad1, 90, Z, p2);
      else // after brick and gap
	ModelSupport::buildShiftedPlane(SMap, SJ+1, ptan1,
					i*(brickWidth+brickGapWidth)); 
      
      // after brick
      ModelSupport::buildShiftedPlane(SMap, SJ+2, ptan1,
				      i*(brickWidth+brickGapWidth)+brickWidth);
      
      // 2nd layer
      // after brick
      ModelSupport::buildShiftedPlane(SMap, SJ+11, ptan1,
				      (2*i+1)*(brickWidth+brickGapWidth)/2.0-brickWidth-brickGapWidth);
      // after brick and gap
      ModelSupport::buildShiftedPlane(SMap, SJ+12, ptan1,
					(2*i+1)*(brickWidth+brickGapWidth)/2.0+brickWidth-brickWidth-brickGapWidth);
	
	SJ += 20;
      }
    return;
}
  
double
BilbaoWheelInnerStructure::sideIntersect
(HeadRule HR,
 const Geometry::Plane *plSide)
/*!
   Calculates intersect of a brick (void between bricks) 
   and one of the sector side surfaces.
   \param plSide :: plane for edge of system
   \return number of intersection points
 */
{
  ELog::RegMethod RegA("BilbaoWhieelInnerStructure","sideIntersect");
  
  std::vector<Geometry::interPoint> IPts;
  const Geometry::Plane *pz = SMap.realPtr<Geometry::Plane>(buildIndex+5);
  
  HR.populateSurf();
  
  Geometry::Vec3D Org = Origin-plSide->getNormal()*plSide->distance(Origin);
  Geometry::Vec3D Unit = -plSide->crossProd(*pz);
  
  if (!plSide->onSurface(Org))
    ELog::EM << "Origin of line is not on the surface" << ELog::endErr;
  
  const size_t n = HR.calcSurfIntersection(Org, Unit, IPts);
  double dist = -1.0;
  if (n>1)
    dist = IPts[0].Pt.Distance(IPts[1].Pt)+0.01; // 0.01 is a "safety" sum  and to get rid of the bricks where we cross in the corner. For some reason, 1st layer is not built without this number.
  return dist;
}
  

void
BilbaoWheelInnerStructure::createBricks(Simulation& System,
					const attachSystem::FixedComp& Wheel,
					const HeadRule& sideHRA,
					const HeadRule& sideHRB,
					const size_t sector)
  /*
    Create cells for bricks in the given sector
    \param System :: Simualation
    \param Wheel :: Model to place item in
   */
{
  ELog::RegMethod RegA("BilbaoWheelInnerStructure","createBricks");
  
  const Geometry::Plane* plSide1 =
    SMap.realPtr<Geometry::Plane>(sideHRA.getPrimarySurface());
  const Geometry::Plane* plSide2 =
    SMap.realPtr<Geometry::Plane>(sideHRB.getPrimarySurface());
  // //    const Geometry::Plane *pz =
  // SMap.realPtr<Geometry::Plane>(buildIndex+5);
  
  HeadRule sideHR(sideHRA*sideHRB);
  const HeadRule vertHR = Wheel.getFullRule(7)*
    Wheel.getFullRule(8); // top+bottom
  
  const HeadRule innerCyl = Wheel.getFullRule(9);
  const HeadRule outerCyl = Wheel.getFullRule(10);
  
  HeadRule HR;
  int SI(buildIndex+1000*(static_cast<int>(sector+1)));
  // He layer in front of the bricks
  HR = HeadRule(SMap,SI,5);
  System.addCell(cellIndex++,brickGapMat,temp,HR*vertHR*outerCyl);
  
  
  ELog::EM << "TODO: simplification of these multiple IFs needed" << ELog::endCrit;
  
  int mat(0);
  bool firstBrick(false);
  bool lastBrick(false);
  double dist; // distance b/w two intersection points

  // bricks
  HeadRule layerHR;
  for (size_t i=0; i<nBrickLayers; i++)
    {
      layerHR=ModelSupport::getHeadRule(SMap, SI,"-5  6");
      int SJ(buildIndex+1000*(static_cast<int>(sector)+1));
      firstBrick = false;
      lastBrick = false;
      for (int j=0; j<27; j++) // !!! TMP
	{
	  const int bOffset = i%2 ? SJ+10 : SJ; // alternate planes for odd/even brick layers
	  
	  mat = (i<nBrickLayers-nSteelLayers) ? brickMat : brickSteelMat;
	  
	  // brick
	  HR = ModelSupport::getHeadRule(SMap, bOffset,"1 -2");
	  dist = sideIntersect(HR*layerHR,plSide1);
	  if (dist>Geometry::zeroTol)
	    {
	      firstBrick = true;
	      if (dist>=brickLen) // side plane of the brick is not intersected
		HR= HeadRule(SMap,bOffset,-2);
	      sideHR = sideHRA;
	      mat = brickSteelMat;
	    }
	  else 
	    {
	      dist = sideIntersect(HR*layerHR,plSide2);
	      if (dist>Geometry::zeroTol)
		{
		  lastBrick = true;
		  HR = HeadRule(SMap,bOffset,1);
		  sideHR = sideHRB;
		  mat = brickSteelMat;
		}
	      else
		{
		  sideHR.reset();
		}
	    }
	  
	  if (firstBrick)
	    System.addCell(cellIndex++,mat, temp,
			   HR*layerHR*vertHR*sideHR);  // !!! sideStr is tmp
	  if (lastBrick) break;
	  
	  // gap
	  mat=brickGapMat;
	  HR=ModelSupport::getHeadRule(SMap,bOffset,bOffset+20,"2 -1M");
	  dist = sideIntersect(HR*layerHR,plSide1);
	  if (dist>Geometry::zeroTol) 
	    {
	      firstBrick = true;
	      if (dist>=brickLen) // side plane of the brick is not intersected
		HR = HeadRule(SMap,bOffset+20,-1);
	      sideHR = sideHRA;
	      mat=brickSteelMat;
	    }
          else
	    {
	      dist = sideIntersect(HR*layerHR,plSide2);
	      if (dist>Geometry::zeroTol)
		{
		  lastBrick = true;
		  HR=HeadRule(SMap,bOffset,2);
		  sideHR = sideHRB;
		  mat=brickSteelMat;
		}
              else
                sideHR.reset();
	    }
	  
	  if (firstBrick)
	    System.addCell(cellIndex++,mat,temp,HR*layerHR*vertHR+sideHR);
	  
	  if (lastBrick) break;
	  
	  SJ += 20;
	}
      
      if (i==nBrickLayers-1) 
	HR=HeadRule(SMap,SI,-6)*innerCyl;
      else
	HR=ModelSupport::getHeadRule(SMap,SI,SI+10,"-6 5M");
      System.addCell(cellIndex++,brickGapMat,temp,HR*vertHR*sideHRA*sideHRB);
      SI += 10;
    }
  return;
}
  
void
BilbaoWheelInnerStructure::createLinks()
/*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("BilbaoWheelInnerStructure","createLinks");
  
  
  return;
}
  
void
BilbaoWheelInnerStructure::createAll(Simulation& System,
				     attachSystem::FixedComp& WheelFC,
				     const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param WheelFC :: Attachment point and inner cell to remove
  */
{
  ELog::RegMethod RegA("BilbaoWheelInnerStructure","createAll");
  
  populate(System.getDataBase());
  createUnitVector(WheelFC,sideIndex);
  
  createSurfaces(WheelFC);
  createObjects(System, WheelFC);
  createLinks();
  
  insertObjects(System);       
  return;
}

}  // NAMESPACE essSystem
