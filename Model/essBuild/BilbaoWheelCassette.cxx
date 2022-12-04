/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/BilbaoWheelCassette.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell / Konstantin Batkov
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
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <numeric>
#include <algorithm>
#include <memory>

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
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "SurInter.h"

#include "BilbaoWheelCassette.h"

namespace essSystem
{

BilbaoWheelCassette::BilbaoWheelCassette(const std::string& baseKey,
					 const std::string& extraKey,
					 const size_t& Index)  :
  attachSystem::FixedRotate(baseKey+extraKey+std::to_string(Index),40),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  baseName(baseKey),
  commonName(baseKey+extraKey)
    /*!
    Constructor BUT ALL variable are left unpopulated.
    \param baseKey :: Name for item in search
    \param extraKey :: individual key name
    \param Index :: counter of unit
  */
{
  FixedComp::nameSideIndex({
      {"Inner", 0},
      {"Outer", 1}
    });      
}

BilbaoWheelCassette::BilbaoWheelCassette(const BilbaoWheelCassette& A) :
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  baseName(A.baseName),
  commonName(A.commonName),
  wallThick(A.wallThick),delta(A.delta),temp(A.temp),
  nWallSeg(A.nWallSeg),
  wallSegLength(A.wallSegLength),
  wallSegDelta(A.wallSegDelta),
  wallSegThick(A.wallSegThick),
  homoWMat(A.homoWMat),
  homoSteelMat(A.homoSteelMat),
  wallMat(A.wallMat),
  heMat(A.heMat),
  floor(A.floor),
  roof(A.roof),
  back(A.back),
  front(A.front),
  nBricks(A.nBricks),
  brickWidth(A.brickWidth),
  brickLength(A.brickLength),
  brickGap(A.brickGap),
  brickSteelMat(A.brickSteelMat),
  brickWMat(A.brickWMat),
  nSteelRows(A.nSteelRows),
  pipeCellThick(A.pipeCellThick),
  pipeCellMat(A.pipeCellMat)
  /*!
    Copy constructor
    \param A :: BilbaoWheelCassette to copy
  */
{}

BilbaoWheelCassette&
BilbaoWheelCassette::operator=(const BilbaoWheelCassette& A)
  /*!
    Assignment operator
    \param A :: BilbaoWheelCassette to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      wallThick=A.wallThick;
      delta=A.delta;
      temp=A.temp;
      nWallSeg=A.nWallSeg;
      wallSegLength=A.wallSegLength;
      wallSegDelta=A.wallSegDelta;
      wallSegThick=A.wallSegThick;
      homoWMat=A.homoWMat;
      homoSteelMat=A.homoSteelMat;
      wallMat=A.wallMat;
      heMat=A.heMat;
      floor=A.floor;
      roof=A.roof;
      back=A.back;
      front=A.front;
      nBricks=A.nBricks;
      brickWidth=A.brickWidth;
      brickLength=A.brickLength;
      brickGap=A.brickGap;
      brickSteelMat=A.brickSteelMat;
      brickWMat=A.brickWMat;
      nSteelRows=A.nSteelRows;
      pipeCellThick=A.pipeCellThick;
      pipeCellMat=A.pipeCellMat;
    }
  return *this;
}

BilbaoWheelCassette*
BilbaoWheelCassette::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new BilbaoWheelCassette(*this);
}

BilbaoWheelCassette::~BilbaoWheelCassette()
  /*!
    Destructor
  */
{}

double
BilbaoWheelCassette::getSegWallArea() const
  /*!
    Calculate segmented wall area.
   */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","getSegWallArea");

  // for the innermost segment without bricks:
  // get segmented wall area:


  //  double h1(wallSegThick); // downstream thick

  // for the inner segment with  no bricks
  const double h3 = wallSegLength[0]*tan(wallSegDelta*M_PI/180.0);
  const double s1 = wallSegThick*wallSegLength[0]; // rectangle
  const double s2 = wallSegLength[0]*h3/2.0; // triangle

  // it's a sum of rectangle (s1) and triangle (s2)
  double s(s1+s2);

  // for the segments with bricks:
  double R(innerCylRadius);
  for (size_t j=1; j<nWallSeg; j++)
    {
      R += wallSegLength[j]*cos(wallSegDelta*M_PI/180.0);
      const double L = 2*R*sin(wallSegDelta*M_PI/180.0);
      const double bg(getBrickGapThick(j));
      s += (L-bg)*wallSegLength[j]/2.0;
    }

  return s;
}

double
BilbaoWheelCassette::getSegWallThick() const
  /*!
    Calculate averate wall thickness for simplified (non-detailed)
    wall geometry. Used when bricksActive is false.
    Just calculates the thickness of a rectangular wall in order to
    have the same area as the segmented one.
   */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","getSegWallThick");

  double sum(0.0);
  for(const double D : wallSegLength)
    sum+=D;

  return getSegWallArea()/sum;
}

double
BilbaoWheelCassette::getBrickGapThick(const size_t& j) const
  /*!
    Calculate total thick of bricks + gaps in the given segment
    Used to calculate wall thickness when bricksActive is true.
    \param j :: segment number
   */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","getBrickGapThick(size_t&)");

  if (j==0)
    throw ColErr::RangeError<size_t>(j, 1, nWallSeg-1, "segment index");

  return static_cast<double>(nBricks[j])*brickWidth +
    static_cast<double>(nBricks[j]-1)*brickGap;
}

void
BilbaoWheelCassette::buildBricks()
  /*!
    Build the bricks for the given segment
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","buildBricks");
  return;
}

void
BilbaoWheelCassette::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","populate");

  FixedRotate::populate(Control);
  zAngle+=rotAngle;
  
  bricksActive=Control.EvalDefTail<int>(keyName,commonName,"BricksActive",0);
  const double nSectors = Control.EvalVar<double>(baseName+"NSectors");
  delta = 360.0/nSectors;

  wallThick=Control.EvalTail<double>(keyName,commonName,"WallThick");
  wallThick /= 2.0; // there is half wall from each side of neighbouring sectors
  wallMat=ModelSupport::EvalMat<int>(Control,commonName+"WallMat",
				     keyName+"WallMat");
  heMat=ModelSupport::EvalMat<int>(Control,baseName+"HeMat");
  homoWMat=ModelSupport::EvalMat<int>(Control,baseName+"HomoWMat");
  homoSteelMat=ModelSupport::EvalMat<int>(Control,baseName+"HomoSteelMat");
  temp=Control.EvalVar<double>(baseName+"Temp");

  // for detailed wall geometry (if bricksActive=true)
  nWallSeg=Control.EvalTail<size_t>(keyName,commonName,"NWallSeg");
  for (size_t i=0; i<nWallSeg; i++)
    {
      wallSegLength.push_back(Control.EvalTail<double>
			      (keyName,commonName,
			       "WallSegLength"+std::to_string(i)));
      nBricks.push_back(Control.EvalTail<size_t>
			(keyName,commonName,
			 "WallSegNBricks"+std::to_string(i)));
    }
  wallSegDelta=delta/2.0; // otherwise wall planes near bricks are not parallel
  wallSegThick=Control.EvalTail<double>(keyName,commonName,"WallSegThick");

  brickWidth=Control.EvalTail<double>(keyName,commonName,"BrickWidth");
  brickLength=Control.EvalTail<double>(keyName,commonName,"BrickLength");
  brickGap=Control.EvalTail<double>(keyName,commonName,"BrickGap");
  brickSteelMat=ModelSupport::EvalMat<int>(Control,commonName+"BrickSteelMat",
					   keyName+"BrickSteelMat");
  brickWMat=ModelSupport::EvalMat<int>(Control,commonName+"BrickWMat",
				       keyName+"BrickWMat");
  nSteelRows=Control.EvalTail<size_t>(keyName,commonName,"NSteelRows");

  pipeCellThick=Control.EvalTail<double>(keyName,commonName,"PipeCellThick");
  pipeCellMat=ModelSupport::EvalMat<int>(Control,commonName+"PipeCellMat",
					 keyName+"PipeCellMat");


  innerCylRadius=getLinkDistance("Inner","Origin");

  return;
}

void
BilbaoWheelCassette::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createSurfaces");

  // divider
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  // outer sides
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+3,Origin,X,Z,-delta/2.0);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+4,Origin,X,Z,delta/2.0);

  const double dw = getSegWallThick()+wallThick;
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+13,Origin+X*dw,X,Z,-delta/2.0);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+14,Origin-X*dw,X,Z,delta/2.0);

  const double R(innerCylRadius);

  // bricks start from this cylinder:
  ModelSupport::buildCylinder(SMap, buildIndex+7, Origin, Z,
			      R+wallSegLength[0]);

  double rSteel(R+wallSegLength[0]); // outer radius of steel bricks
  for (size_t i=0; i<nSteelRows; i++)
    rSteel += wallSegLength[i+1];
  ModelSupport::buildCylinder(SMap, buildIndex+17, Origin, Z, rSteel);

  // front plane
  const double outerCylRadius=
    FixedComp::getLinkDistance("Outer","Origin");

  const Geometry::Vec3D offset = Origin-Y*outerCylRadius;
  ModelSupport::buildPlane(SMap,buildIndex+11,offset,Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,offset+Y*pipeCellThick,Y);

  return;
}

void
BilbaoWheelCassette::createSurfacesBricks()
  /*!
    Create All the surfaces with bricks
    \param FC :: wheel object
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createSurfacesBricks");

  // divider
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin,Z);
  // outer sides
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+3,Origin,X,Z,-delta/2.0);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+4,Origin,X,Z,delta/2.0);

  int SI(buildIndex+100);

  // // detailed wall
  // Geometry::Cylinder *backCyl =
  //   SMap.realPtr<Geometry::Cylinder>(FC.getLinkSurf(back));

  const double d2(M_PI*wallSegDelta/180.0); // half of the sector opening angle
  double R(innerCylRadius);

  int SJ(SI);
  
  // only for the inner segment with no bricks:
  const double dx = R*sin(d2)-wallSegThick-wallThick;
  Geometry::Vec3D orig13=Origin-Y*(R*cos(d2)) - X*dx;
  Geometry::Vec3D orig14=Origin-Y*(R*cos(d2)) + X*dx;

  // ERROR HERE :: Y is not correct???
  for (size_t j=0; j<nWallSeg; j++)
    {
      R += wallSegLength[j]*cos(wallSegDelta*M_PI/180.0);
      Geometry::Vec3D offset = Origin-Y*R;
      ModelSupport::buildPlane(SMap,SJ+11,offset,Y);
      ModelSupport::buildPlane(SMap,SJ+12,offset-Y*brickGap,Y);
      if (j==nWallSeg-2)
	  ModelSupport::buildPlane(SMap,SJ+21,offset-Y*(brickLength+brickGap),Y);

      if (j==0) // for the inner segment with no bricks
	{
	  ModelSupport::buildPlaneRotAxis(SMap,SJ+13,
					  orig13,X,Z,
					  delta/2.0-wallSegDelta);
	  ModelSupport::buildPlaneRotAxis(SMap,SJ+14,
					  orig14,X,Z,
					  wallSegDelta-delta/2.0);
	}
      else // build the bricks
	{
	  // total space for bricks in the current segment along the x-axis:
	  const double L(getBrickGapThick(j));
	  ModelSupport::buildPlane(SMap,SJ+13,Origin-X*L/2.0,X);
	  ModelSupport::buildPlane(SMap,SJ+14,Origin+X*L/2.0,X);

	  int SBricks(SJ+100);
	  double bOffset(brickWidth);
	  for (size_t i=0; i<nBricks[j]; i++) // bricks
	    {
	      ModelSupport::buildShiftedPlane(SMap,SBricks+3,
					      SMap.realPtr<Geometry::Plane>(SJ+13),
					      bOffset);
	      ModelSupport::buildShiftedPlane(SMap,SBricks+4,
					      SMap.realPtr<Geometry::Plane>(SBricks+3),
					      brickGap);
	      SBricks += 10;
	      bOffset += brickWidth+brickGap;
	    }
	}

      SJ += 1000;
    }

  return;
}

void
BilbaoWheelCassette::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createObjects");

  const HeadRule tbHR=getRule("VerticalCut");
  const HeadRule frontHR=tbHR*getRule("OuterCyl");
  const HeadRule innerHR=tbHR*getRule("InnerCyl");
  const HeadRule outerHR=frontHR*innerHR*tbHR;

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -13 -1");
  System.addCell(cellIndex++,wallMat,temp,HR*outerHR);

  if (nSteelRows>0)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"13 -14 12 17");
      System.addCell(cellIndex++,homoWMat,temp,HR*tbHR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"13 -14 -17 7");
      System.addCell(cellIndex++,homoSteelMat,temp,HR*tbHR);
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"13 -14 12 7");
      System.addCell(cellIndex++,homoWMat,temp,HR*tbHR);
    }

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"13 -14 -7");
  System.addCell(cellIndex++,heMat,temp,HR*outerHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"13 -14 -12");
  System.addCell(cellIndex++,pipeCellMat,temp,HR*outerHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"14 -4 -1");
  System.addCell(cellIndex++,wallMat,temp,HR*outerHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 -1");
  addOuterSurf(HR*outerHR);

  return;
}

void
BilbaoWheelCassette::createObjectsBricks(Simulation& System)
  /*!
    Adds the all the components with bricks
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createObjectsBricks");

  const HeadRule tbHR=getRule("VerticalCut");

  const HeadRule frontHR=tbHR*getRule("OuterCyl");
  const HeadRule innerHR=getRule("InnerCyl");
  const HeadRule outerHR=frontHR*innerHR*tbHR;

  HeadRule HR;
  HeadRule bfHR; // back-front surfaces of each segment
  HeadRule bfBrickHR; // back-front surfaces of bricks

  int SJ(buildIndex+1100);
  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 111 113 -114");
  System.addCell(cellIndex++,heMat,temp,HR*tbHR*innerHR);
  /// create side walls
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 111 3 -113");
  System.addCell(cellIndex++,wallMat,temp,HR*tbHR*innerHR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 111 114 -4 ");
  System.addCell(cellIndex++,wallMat,temp,HR*tbHR*innerHR);

  for (size_t j=1; j<nWallSeg; j++)
    {
      if (j==nWallSeg-1)
	{
	  bfHR = HeadRule(SMap,SJ-1000,-11)*innerHR;
	  bfBrickHR=ModelSupport::getHeadRule(SMap,SJ-1000,"21 -12");
	  
	  HR=ModelSupport::getHeadRule(SMap,SJ,SJ-1000,"13 -14 -21M");
	  System.addCell(cellIndex++,pipeCellMat,temp,
					    HR*tbHR*innerHR);
	}
      else
	{
	  bfHR = ModelSupport::getHeadRule(SMap,SJ,SJ-1000,"11 -11M");
	  bfBrickHR=ModelSupport::getHeadRule(SMap,SJ,SJ-1000,"11 -12M");
	}
      
      int SBricks(SJ+100);
      HeadRule prevHR=HeadRule(SMap,SJ,13);
      for (size_t i=0; i<nBricks[j]; i++) // create brick cells
	{
	  if (i!=nBricks[j]-1)
	    {
	      // gap
	      HR=ModelSupport::getHeadRule(SMap,SBricks,"3 -4");
	      System.addCell(cellIndex++,heMat,temp,HR*bfBrickHR*tbHR);
	      // brick
	      HR=ModelSupport::getHeadRule(SMap,SBricks,"-3");
	    }
	  else // build only brick
	    HR=ModelSupport::getHeadRule(SMap,SJ,"-14");

	  const int brickMat=(j<=nSteelRows) ? brickSteelMat : brickWMat;
	  System.addCell(cellIndex++,brickMat,temp,HR*bfBrickHR+tbHR*prevHR);
	  
	  prevHR =HeadRule(SMap,SBricks,4);
	  SBricks += 10;
	}
      // gap b/w bricks in y-direction
      HR=ModelSupport::getHeadRule(SMap,SJ,SJ-1000,"13 -14 12M -11M");
      System.addCell(cellIndex++,heMat,temp,HR*tbHR);
      

      /// create side walls
      HR=ModelSupport::getHeadRule(SMap,buildIndex,SJ,"3 -13M");
      System.addCell(cellIndex++,wallMat,temp,HR*tbHR*innerHR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,SJ,"14M -4");
      System.addCell(cellIndex++,wallMat,temp,HR*tbHR*innerHR);
      
      SJ += 1000;
    }
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 -1");
  addOuterSurf(HR*tbHR*innerHR*outerHR);

  return;
}


void
BilbaoWheelCassette::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createLinks");

  if (bricksActive)
    {
      int SJ(buildIndex);
      size_t i(4);
      for (size_t j=0; j<nWallSeg; j++)
	{
	  Geometry::Vec3D Pt =
	    SurInter::getPoint(SMap.realSurfPtr(SJ+111),
			       SMap.realSurfPtr(SJ+113),
			       SMap.realSurfPtr(buildIndex+5));
	  
	  Pt += Y*(wallSegLength[j]/2.0);
	  FixedComp::setConnect(i,Pt,X);
	  FixedComp::setLinkSurf(i,SMap.realSurf(SJ+113));

	  i++;

	  Pt =
	    SurInter::getPoint(SMap.realSurfPtr(SJ+111),
			       SMap.realSurfPtr(SJ+114),
			       SMap.realSurfPtr(buildIndex+5));
	  
	  Pt += Y*(wallSegLength[j]/2.0);
	  FixedComp::setConnect(i,Pt,-X);
	  FixedComp::setLinkSurf(i,-SMap.realSurf(SJ+114));

	  SJ += 1000;
	  i++;
	}
    }

  return;
}

void
BilbaoWheelCassette::createAll(Simulation& System,
			       const attachSystem::FixedComp& FC,
			       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  
  if (!bricksActive)
    {
      createSurfaces();
      createObjects(System);
    }
  else
    {
      createSurfacesBricks();
      createObjectsBricks(System);
    }
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
