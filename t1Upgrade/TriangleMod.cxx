/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Upgrade/TriangleMod.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
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
#include "KGroup.h"
#include "Source.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "TriangleMod.h"

namespace moderatorSystem
{

TriangleMod::TriangleMod(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,5),
  triIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(triIndex+1),CornerPts(3)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

TriangleMod::TriangleMod(const TriangleMod& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  triIndex(A.triIndex),cellIndex(A.cellIndex),xStep(A.xStep),
  yStep(A.yStep),zStep(A.zStep),xyAngle(A.xyAngle),
  zAngle(A.zAngle),CornerPts(A.CornerPts),height(A.height),
  wallThick(A.wallThick),flatClearance(A.flatClearance),
  topClearance(A.topClearance),baseClearance(A.baseClearance),
  innerStep(A.innerStep),innerMat(A.innerMat),
  poisonStep(A.poisonStep),poisonThick(A.poisonThick),
  pCladThick(A.pCladThick),modTemp(A.modTemp),
  modMat(A.modMat),wallMat(A.wallMat),pCladMat(A.pCladMat),
  poisonMat(A.poisonMat)
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
      CornerPts=A.CornerPts;
      height=A.height;
      wallThick=A.wallThick;
      flatClearance=A.flatClearance;
      topClearance=A.topClearance;
      baseClearance=A.baseClearance;
      innerStep=A.innerStep;
      innerMat=A.innerMat;
      poisonStep=A.poisonStep;
      poisonThick=A.poisonThick;
      pCladThick=A.pCladThick;
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

  CornerPts[0]=Control.EvalVar<Geometry::Vec3D>(keyName+"Corner1");
  CornerPts[1]=Control.EvalVar<Geometry::Vec3D>(keyName+"Corner2");
  CornerPts[2]=Control.EvalVar<Geometry::Vec3D>(keyName+"Corner3");

  height=Control.EvalVar<double>(keyName+"Height");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  flatClearance=Control.EvalVar<double>(keyName+"FlatClearance");
  topClearance=Control.EvalVar<double>(keyName+"TopClearance");
  baseClearance=Control.EvalVar<double>(keyName+"BaseClearance");

  innerStep=Control.EvalVar<double>(keyName+"InnerStep");
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");

// Poison
  poisonStep=Control.EvalVar<double>(keyName+"PoisonStep");
  poisonThick=Control.EvalVar<double>(keyName+"PoisonThick");
  pCladThick=Control.EvalVar<double>(keyName+"PCladThick");
  pCladMat=ModelSupport::EvalMat<int>(Control,keyName+"PCladMat");
  poisonMat=ModelSupport::EvalMat<int>(Control,keyName+"PoisonMat");
//

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
TriangleMod::createLinks()
  /*!
    Construct links for the triangel moderator
  */
{
  ELog::RegMethod RegA("TriangleMod","createLinks");

  const double OSize=wallThick+flatClearance;
  for(size_t i=0;i<3;i++)
    {
      FixedComp::setConnect(i,
	 (corner(i,OSize)+corner(i+1,OSize))/2.0,
			    -midNorm(i+2));
    }
  FixedComp::setConnect(3,Origin-Z*(height/2.0+OSize),-Z);
  FixedComp::setConnect(4,Origin+Z*(height/2.0+OSize),Z);

  FixedComp::setLinkSurf(0,-SMap.realSurf(triIndex+21));
  FixedComp::setLinkSurf(1,-SMap.realSurf(triIndex+22));
  FixedComp::setLinkSurf(2,-SMap.realSurf(triIndex+23));
  FixedComp::setLinkSurf(3,-SMap.realSurf(triIndex+25));
  FixedComp::setLinkSurf(4,SMap.realSurf(triIndex+26));

  return;
}
  
Geometry::Vec3D 
TriangleMod::corner(const size_t Index,
		    const double scale) const
  /*!
    Corner of the version 
    \param Index :: Corner point
    \param scale :: Distance out from the corner
    \return corner point or outside point
  */
{
  Geometry::Vec3D CPoint=
    Origin + X * CornerPts[Index%3].X() + Y*CornerPts[Index%3].Y();
  if (fabs(scale)>Geometry::zeroTol)
    CPoint+=midNorm(Index%3)*scale;
  return CPoint;
}

Geometry::Vec3D 
TriangleMod::midNorm(const size_t Index) const
  /*!
    Calculate the normal going from teh centre of the triangle
    to the corner
    \param Index :: Corner to calculate normal through
    \return Normal
  */
{
  Geometry::Vec3D Mid;
  for(size_t i=0;i<3;i++)
    Mid += ((Index%3)==i) ? corner(i,0.0) : -corner(i,0.0)/2.0;

  return Mid.unit();
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

  // Each blade starts at 101

  const double PSteps[]={pCladThick,poisonThick,pCladThick,0.0};
  for(size_t i=0;i<3;i++)
    {
      const int ii(static_cast<int>(i));
      double PDepth(poisonStep);
      for(int j=0;j<4;j++)
	{
	  ModelSupport::buildPlane(SMap,triIndex+201+100*j+ii,
				   corner(i,-innerStep-PDepth),
				   corner(i+1,-innerStep-PDepth),
				   corner(i+1,-innerStep-PDepth)+Z,
				   midNorm(i+2));
	  PDepth+=PSteps[j];
	}

      ModelSupport::buildPlane(SMap,triIndex+101+ii,
			       corner(i,-innerStep),
			       corner(i+1,-innerStep),
			       corner(i+1,-innerStep)+Z,
			       midNorm(i+2));

      ModelSupport::buildPlane(SMap,triIndex+1+ii,
			       corner(i,0.0),
			       corner(i+1,0.0),
			       corner(i+1,0.0)+Z,
			       midNorm(i+2));

      ModelSupport::buildPlane(SMap,triIndex+11+ii,
			       corner(i,wallThick),
			       corner(i+1,wallThick),
			       corner(i+1,wallThick)+Z,
			       midNorm(i+2));

      ModelSupport::buildPlane(SMap,triIndex+21+ii,
			       corner(i,wallThick+flatClearance),
			       corner(i+1,wallThick+flatClearance),
			       corner(i+1,wallThick+flatClearance)+Z,
			       midNorm(i+2));
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


  return;
}

void
TriangleMod::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("TriangleMod","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,triIndex,"501 502 503 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,modTemp,Out));

  Out=ModelSupport::getComposite(SMap,triIndex,
				 "401 402 403 5 -6 (-501:-502:-503)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pCladMat,modTemp,Out));

  Out=ModelSupport::getComposite(SMap,triIndex,
				 "301 302 303 5 -6 (-401:-402:-403)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,poisonMat,modTemp,Out));

  Out=ModelSupport::getComposite(SMap,triIndex,
				 "201 202 203 5 -6 (-301:-302:-303)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pCladMat,modTemp,Out));

  Out=ModelSupport::getComposite(SMap,triIndex,
				 "101 102 103 5 -6 (-201:-202:-203)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,modTemp,Out));

  Out=ModelSupport::getComposite(SMap,triIndex,"1 2 3 5 -6 (-101:-102:-103)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));

  Out=ModelSupport::getComposite(SMap,triIndex,
				 "11 12 13 15 -16 (-1:-2:-3:-5:6)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,modTemp,Out));


  Out=ModelSupport::getComposite(SMap,triIndex,
				 "21 22 23 25 -26 (-11:-12:-13:-15:16)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));


  Out=ModelSupport::getComposite(SMap,triIndex,"21 22 23 25 -26");
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
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  return;
}
  
}  // NAMESPACE moderatorSystem
