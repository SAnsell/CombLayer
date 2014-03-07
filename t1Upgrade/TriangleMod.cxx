/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Upgrade/TriangleMod.cxx 
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "Vert2D.h"
#include "Convex2D.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "TriangleMod.h"

namespace moderatorSystem
{

TriangleMod::TriangleMod(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,5),
  triIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(triIndex+1)

  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

TriangleMod::TriangleMod(const TriangleMod& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  triIndex(A.triIndex),cellIndex(A.cellIndex),xStep(A.xStep),
  yStep(A.yStep),zStep(A.zStep),xyAngle(A.xyAngle),
  zAngle(A.zAngle),nCorner(A.nCorner),centroid(A.centroid),
  CornerPts(A.CornerPts),nonConvex(A.nonConvex),
  nInnerCorner(A.nInnerCorner),InnerPts(A.InnerPts),
  height(A.height),wallThick(A.wallThick),flatClearance(A.flatClearance),
  topClearance(A.topClearance),baseClearance(A.baseClearance),
  innerStep(A.innerStep),innerWall(A.innerWall),
  innerMat(A.innerMat),modTemp(A.modTemp),modMat(A.modMat),
  wallMat(A.wallMat),pCladMat(A.pCladMat),poisonMat(A.poisonMat)
  /*!
    Copy constructor
    \param A :: TriangleMod to copy
  */
{}

TriangleMod&
TriangleMod::operator=(const TriangleMod& A)
  /*!
    Assignment operator
    \param A :: TriangleMod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      nCorner=A.nCorner;
      centroid=A.centroid;
      CornerPts=A.CornerPts;
      nonConvex=A.nonConvex;
      nInnerCorner=A.nInnerCorner;
      InnerPts=A.InnerPts;
      height=A.height;
      wallThick=A.wallThick;
      flatClearance=A.flatClearance;
      topClearance=A.topClearance;
      baseClearance=A.baseClearance;
      innerStep=A.innerStep;
      innerWall=A.innerWall;
      innerMat=A.innerMat;
      modTemp=A.modTemp;
      modMat=A.modMat;
      wallMat=A.wallMat;
      pCladMat=A.pCladMat;
      poisonMat=A.poisonMat;
    }
  return *this;
}

TriangleMod::~TriangleMod() 
  /*!
    Destructor
  */
{}

void
TriangleMod::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("TriangleMod","populate");
  
  const FuncDataBase& Control=System.getDataBase();
  
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  Geometry::Vec3D Pt;
  nCorner=Control.EvalDefVar<size_t>(keyName+"NCorner",3);
  if (nCorner<3)
    throw ColErr::RangeError<size_t>(nCorner,3,20,"nCorner out of range");

  CornerPts.clear();
  nonConvex.clear();
  for(size_t i=0;i<nCorner;i++)
    {
      Pt=Control.EvalVar<Geometry::Vec3D>(keyName+"Corner"+
					  StrFunc::makeString(i+1));
      CornerPts.push_back(Pt);
      nonConvex.push_back(1);
    }

  nInnerCorner=Control.EvalDefVar<size_t>(keyName+"NInnerCorner",0);
  InnerPts.clear();
  for(size_t i=0;i<nInnerCorner;i++)
    {
      Pt=Control.EvalVar<Geometry::Vec3D>(keyName+"InnerCorner"+
					  StrFunc::makeString(i+1));
      InnerPts.push_back(Pt);
    }

  height=Control.EvalVar<double>(keyName+"Height");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  flatClearance=Control.EvalVar<double>(keyName+"FlatClearance");
  topClearance=Control.EvalVar<double>(keyName+"TopClearance");
  baseClearance=Control.EvalVar<double>(keyName+"BaseClearance");

  innerStep=Control.EvalVar<double>(keyName+"InnerStep");
  if (innerStep>Geometry::zeroTol)
    {
      innerWall=Control.EvalVar<double>(keyName+"InnerWall");
      innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
    }

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  
  return;
}
  
void
TriangleMod::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Points down the TriangleMod direction
    - X Across the TriangleMod
    - Z up (towards the target)
    \param FC :: fixed Comp [and link comp]
  */
{
  ELog::RegMethod RegA("TriangleMod","createUnitVector");

  FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);
  return;
}

void
TriangleMod::createConvex()
  /*!
    Create convex hulls and resequence the points
   */
{
  ELog::RegMethod RegA("TriangleMod","createConvex");
  
  Geometry::Convex2D OuterCH;
  OuterCH.setPoints(CornerPts);
  OuterCH.constructHull();
  std::vector<Geometry::Vec3D> OutPt=OuterCH.getSequence();
  centroid=OuterCH.getCentroid();
  // Caluclate non-convex points:
  
  return;
}


void
TriangleMod::createLinks()
  /*!
    Construct links for the triangel moderator
  */
{
  ELog::RegMethod RegA("TriangleMod","createLinks");

  FixedComp::setNConnect(nCorner+2);
  const double OSize=wallThick+flatClearance;
  for(size_t i=0;i<nCorner;i++)
    {
      const int ii(static_cast<int>(i));
      std::pair<Geometry::Vec3D,Geometry::Vec3D> CP=
	cornerPair(CornerPts,i,i+1,OSize);
      FixedComp::setConnect(i,(CP.first+CP.second)/2.0,
			    sideNorm(CP));
      FixedComp::setLinkSurf(i,-SMap.realSurf(triIndex+101+ii));
    }
  FixedComp::setConnect(nCorner,Origin-Z*(height/2.0+OSize),-Z);
  FixedComp::setConnect(nCorner+1,Origin+Z*(height/2.0+OSize),Z);

  
  FixedComp::setLinkSurf(nCorner,-SMap.realSurf(triIndex+25));
  FixedComp::setLinkSurf(nCorner+1,SMap.realSurf(triIndex+26));

  return;
}
  
Geometry::Vec3D 
TriangleMod::corner(const size_t Index,
		    const double scale) const
  /*!
    Calculates a particular corner based on the basis set
    \param Index :: Corner point
    \param scale :: Distance out from the corner
    \return corner point or outside point
  */
{
  Geometry::Vec3D CPoint=
    Origin + X * CornerPts[Index % nCorner].X() + 
    Y*CornerPts[Index% nCorner].Y();
  if (fabs(scale)>Geometry::zeroTol)
    CPoint+=midNorm(Index)*scale;
  return CPoint;
}

std::pair<Geometry::Vec3D,Geometry::Vec3D>
TriangleMod::cornerPair(const std::vector<Geometry::Vec3D>& CPts,
			const size_t IndexA,const size_t IndexB,
			const double scale) const
  /*!
    Given two corners which define a normal to line.
    Step out by the distance scale.
    \param CPts :: Corner/Inner points 
    \param IndexA :: First corner
    \param IndexB :: Second corner
    \param scale :: Distance out from the surface
    \return shifted points
  */
{
  ELog::RegMethod RegA("TriangleMod","cornerPair");
  const size_t nC(CPts.size());

  Geometry::Vec3D APoint=
    Origin + X * CPts[IndexA % nC].X() + 
    Y*CPts[IndexA % nC].Y();
  Geometry::Vec3D BPoint=
    Origin + X * CPts[IndexB % nC].X() + 
    Y*CPts[IndexB % nCorner].Y();

  if (fabs(scale)>Geometry::zeroTol)
    {
      Geometry::Vec3D ASide=(BPoint-APoint).unit();
      ASide=Z*ASide;
      APoint+=ASide*scale;
      BPoint+=ASide*scale;
    }
  return std::pair<Geometry::Vec3D,Geometry::Vec3D>
    (APoint,BPoint);
}


Geometry::Vec3D 
TriangleMod::midNorm(const size_t Index) const
  /*!
    Calculate the normal going from the centre of the triangle
    to the corner
    \param Index :: Corner to calculate normal through
    \return Normal
  */
{
  const size_t INum(Index % nCorner);
  Geometry::Vec3D Mid(corner(INum,0.0)*2.0);
  // -ve number
  Mid-=(!INum) ? corner(nCorner-1,0.0) : corner(INum-1,0.0);

  Mid-=corner((INum+1) % nCorner,0.0);
  return (nonConvex[INum]) ? Mid.unit() : -Mid.unit();
}

Geometry::Vec3D 
TriangleMod::sideNorm(const std::pair<Geometry::Vec3D,
		      Geometry::Vec3D>& CPts) const
  /*!
    Calculate the normal leaving the side of the triangle
    \param CPts :: Pair of points making up surface (with +Z)
    \return Normal
  */
{
  ELog::RegMethod RegA("TriangleMod","sideNorm");

  // Now get cross vector
  const Geometry::Vec3D ASide=(CPts.second-CPts.first).unit();
  return Z*ASide;
}

void
TriangleMod::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("TriangleMod","createSurface");

  // Surfaces 1-6 are the FC container
  // Surfaces 11-16 are the outer blades etc

  const size_t maxLayers(5);  // Number of layers
  const double PSteps[maxLayers]={flatClearance,wallThick,innerStep,
				innerWall,0.0};
  
  const size_t nLayers((innerStep>Geometry::zeroTol) ? maxLayers : 3);
  for(size_t i=0;i<nCorner;i++)
    {
      const int ii(static_cast<int>(i));
      double PDepth(wallThick+flatClearance);  // + from origin
      int triOffset(triIndex+101);
      for(size_t j=0;j<nLayers;j++)
	{
	  const std::pair<Geometry::Vec3D,Geometry::Vec3D>
	    CPts(cornerPair(CornerPts,i,i+1,PDepth));
	  // Note :: Side norm faces out
	  ModelSupport::buildPlane(SMap,triOffset+ii,
				   CPts.first,CPts.second,
				   CPts.second+Z,
				   -sideNorm(CPts));
	  triOffset+=100;
	  PDepth-=PSteps[j];
	}
    }

  ModelSupport::buildPlane(SMap,triIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,triIndex+6,Origin+Z*height/2.0,Z);

  ModelSupport::buildPlane(SMap,triIndex+15,
			   Origin-Z*(height/2.0+wallThick),Z);
  ModelSupport::buildPlane(SMap,triIndex+16,
			   Origin+Z*(height/2.0+wallThick),Z);

  ModelSupport::buildPlane(SMap,triIndex+25,
			   Origin-Z*(height/2.0+wallThick+baseClearance),Z);
  ModelSupport::buildPlane(SMap,triIndex+26,
			   Origin+Z*(height/2.0+wallThick+topClearance),Z);


  // NOW CREATE INTERNAL ITEM:

  for(size_t i=0;i<nInnerCorner;i++)
    {
      const int ii(static_cast<int>(i));
      double PDepth(innerWall);  // + from origin
      int triOffset(triIndex+3001);
      for(size_t j=0;j<2;j++)
	{
	  const std::pair<Geometry::Vec3D,Geometry::Vec3D>
	    CPts(cornerPair(InnerPts,i,i+1,PDepth));
	  // Note :: Side norm faces out
	  ModelSupport::buildPlane(SMap,triOffset+ii,
				   CPts.first,CPts.second,
				   CPts.second+Z,
				   -sideNorm(CPts));
	  triOffset+=100;
	  PDepth-=innerWall;
	}
    }
  

  return;
}

std::string
TriangleMod::getOuterString() const
  /*!
    Process main string [to be updated for non-convex]
    \return string of outer part
   */
{
  std::ostringstream cx;
  for(size_t i=0;i<nCorner;i++)
    cx<<" "<<i+1;
  cx<<" ";
  return cx.str();
}

std::string
TriangleMod::getInnerString(const std::string& extraUnion) const
  /*!
    Process inner string [to be updated for non-convex]
    \param extraUnion :: inner part
    \return string of outer part
   */
{
  std::ostringstream cx;
  cx<<" ( -101";
  for(int i=1;i < static_cast<int>(nCorner);i++)
    {
      cx<<" : "<<-(i+101);
    }
  if (!extraUnion.empty())
    cx<<" : "<<extraUnion;
  cx<<" ) ";
  return cx.str();
}

std::string
TriangleMod::createInnerObject(Simulation& System)
  /*!
    Create the inner void space if required
    \param System :: Simulation 
  */
{
  ELog::RegMethod RegA("TriangleMod","createInnerObject");

  if (!nInnerCorners)
    return "";

  return ""
}

void
TriangleMod::createObjects(Simulation& System)
  /*!
    Adds the main components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("TriangleMod","createObjects");

  std::string Out;
  const std::string outSurf=getOuterString();
  std::string inSurf=getInnerString("");
  int TI(triIndex+500);
  if (innerStep>Geometry::zeroTol)
    {
      // Centre Mat
      Out=ModelSupport::getComposite(SMap,TI,triIndex,outSurf+" 5M -6M ");
      Out+=createInnerObject(System);
      System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,modTemp,Out));
      TI-=100;
      // Centre wall
      
      Out=ModelSupport::getComposite(SMap,TI,triIndex,
				     outSurf+inSurf+" 5M -6M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,modTemp,Out));
      TI-=100;
      // Outer Material
      Out=ModelSupport::getComposite(SMap,TI,triIndex,
				     outSurf+inSurf+" 5M -6M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));
    }
  else
    {
      TI=triIndex+300;
      // Full moderator
      Out=ModelSupport::getComposite(SMap,TI,triIndex,outSurf+" 5M -6M ");
      Out+=createInnerObject(System);
      System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));
    }

  // Wall       
  inSurf=getInnerString("-5M : 6M");
  TI=triIndex+200;
  Out=ModelSupport::getComposite(SMap,TI,triIndex,
				 outSurf+" 15M -16M "+inSurf);
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,modTemp,Out));
  // Clearance
  TI=triIndex+100;  
  inSurf=getInnerString("-15M : 16M");
  Out=ModelSupport::getComposite(SMap,TI,triIndex,
				 outSurf+" 25M -26M "+inSurf);
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Outer 
  Out=ModelSupport::getComposite(SMap,TI,triIndex,outSurf+" 25M -26M ");
  addOuterSurf(Out);

  return;
}

int
TriangleMod::getExitWindow(const size_t outIndex,
			   std::vector<int>& window) const
  /*!
    Generic exit window system : 
    -- Requires at least 5 surfaces
    -- Requires 3-5 to be sign surf
    \param outIndex :: Direction 0  for entry 1 for exit
    \param window :: window vector of paired planes
    \return Viewed surface
  */
{
  ELog::RegMethod RegA("TriangleMod","getExitWindow");
  if (LU.size()<5)
    throw ColErr::IndexError<size_t>(LU.size(),5,"Link size too small");
  if (outIndex>LU.size())
    throw ColErr::IndexError<size_t>(outIndex,5,"outIndex too big");


  window.clear();
  for(size_t i=0;i<5;i++)
    if (i!=outIndex)
      window.push_back(std::abs(getLinkSurf(i)));

  window.push_back(0);
  return std::abs(SMap.realSurf(getLinkSurf(outIndex)));
}

  
void
TriangleMod::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
  */
{
  ELog::RegMethod RegA("TriangleMod","createAll");
  populate(System);
  createUnitVector(FC);
  
  createConvex();
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  return;
}
  
}  // NAMESPACE moderatorSystem
