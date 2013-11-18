/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delft/BeamTube.cxx
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
#include <numeric>
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
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "BeamTube.h"

namespace delftSystem
{

BeamTube::BeamTube(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::TwinComp(Key,3),
  flightIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(flightIndex+1),innerVoid(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BeamTube::BeamTube(const BeamTube& A) : 
  attachSystem::ContainedComp(A),attachSystem::TwinComp(A),
  flightIndex(A.flightIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),zAngle(A.zAngle),length(A.length),
  innerRadius(A.innerRadius),innerWall(A.innerWall),
  outerRadius(A.outerRadius),outerWall(A.outerWall),
  frontWall(A.frontWall),frontGap(A.frontGap),frontIWall(A.frontIWall),
  interThick(A.interThick),interFrontWall(A.interFrontWall),
  interWallThick(A.interWallThick),interYOffset(A.interYOffset),
  interMat(A.interMat),wallMat(A.wallMat),gapMat(A.gapMat),
  innerVoid(A.innerVoid)
  /*!
    Copy constructor
    \param A :: BeamTube to copy
  */
{}

BeamTube&
BeamTube::operator=(const BeamTube& A)
  /*!
    Assignment operator
    \param A :: BeamTube to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::TwinComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      length=A.length;
      innerRadius=A.innerRadius;
      innerWall=A.innerWall;
      outerRadius=A.outerRadius;
      outerWall=A.outerWall;
      frontWall=A.frontWall;
      frontGap=A.frontGap;
      frontIWall=A.frontIWall;
      interThick=A.interThick;
      interFrontWall=A.interFrontWall;
      interWallThick=A.interWallThick;
      interYOffset=A.interYOffset;
      interMat=A.interMat;
      wallMat=A.wallMat;
      gapMat=A.gapMat;
      innerVoid=A.innerVoid;
    }
  return *this;
}

BeamTube::~BeamTube() 
 /*!
   Destructor
 */
{}

void
BeamTube::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("BeamTube","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  // First get inner widths:
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");

  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
  zAngle=Control.EvalVar<double>(keyName+"ZAngle");

  length=Control.EvalVar<double>(keyName+"Length");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  innerWall=Control.EvalVar<double>(keyName+"InnerWall");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  outerWall=Control.EvalVar<double>(keyName+"OuterWall");

  frontWall=Control.EvalVar<double>(keyName+"FrontWall");
  frontGap=Control.EvalVar<double>(keyName+"FrontGap");
  frontIWall=Control.EvalVar<double>(keyName+"FrontIWall");

  interThick=Control.EvalVar<double>(keyName+"InterThick");
  interWallThick=Control.EvalVar<double>(keyName+"InterWallThick");
  interYOffset=Control.EvalVar<double>(keyName+"InterYOffset");
  interFrontWall=Control.EvalVar<double>(keyName+"InterFrontWall");
  interMat=Control.EvalVar<int>(keyName+"InterMat");

  wallMat=Control.EvalVar<int>(keyName+"WallMat");
  gapMat=Control.EvalVar<int>(keyName+"GapMat");

  return;
}
  
void
BeamTube::createUnitVector(const attachSystem::FixedComp& FC,
			   const Geometry::Vec3D& PAxis)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param FC :: A Contained FixedComp to use as basis set
    \param PAxis :: Primary axis
  */
{
  ELog::RegMethod RegA("BeamTube","createUnitVector");
  
  // PROCESS Origin of a point
  attachSystem::FixedComp::createUnitVector(FC);
  attachSystem::FixedComp::applyShift(xStep,yStep,zStep);


  bZ=Z= FC.getZ();
  bY=Y= PAxis;
  bX=X= Y*Z;

  ELog::EM<<"F:"<<keyName<<" "<<Origin<<" :: "<<Y<<ELog::endDebug;

  if (fabs(xyAngle)>Geometry::zeroTol || 
      fabs(zAngle)>Geometry::zeroTol)
    {
      const Geometry::Quaternion Qz=
	Geometry::Quaternion::calcQRotDeg(zAngle,X);
      const Geometry::Quaternion Qxy=
	Geometry::Quaternion::calcQRotDeg(xyAngle,Z);
  
      Qz.rotate(bY);
      Qz.rotate(bZ);
      Qxy.rotate(bY);
      Qxy.rotate(bX);
      Qxy.rotate(bZ); 
    }
  bEnter=Origin;
  return;
}

void
BeamTube::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BeamTube","createSurfaces");

  // Outer layers
  ModelSupport::buildPlane(SMap,flightIndex+1,Origin,Y);
  ModelSupport::buildCylinder(SMap,flightIndex+7,
			      Origin+Y*frontWall,bY,outerRadius+outerWall);
  ModelSupport::buildPlane(SMap,flightIndex+2,Origin+bY*length,bY);
  // Outer Wall
  ModelSupport::buildPlane(SMap,flightIndex+11,Origin+Y*frontWall,Y);
  ModelSupport::buildCylinder(SMap,flightIndex+17,Origin+Y*frontWall,
			      bY,outerRadius);
  // Gap layer
  ModelSupport::buildPlane(SMap,flightIndex+21,
			   Origin+Y*frontWall+bY*frontGap,bY);
  ModelSupport::buildCylinder(SMap,flightIndex+27,
			      Origin+Y*frontWall,
			      bY,innerRadius+innerWall);

  // Gap layer
  ModelSupport::buildPlane(SMap,flightIndex+31,Origin+Y*frontWall+
			   bY*(frontGap+frontIWall),bY);
  ModelSupport::buildCylinder(SMap,flightIndex+37,
			      Origin+Y*frontWall,
			      bY,innerRadius);

  // Inter layer
  ModelSupport::buildPlane(SMap,flightIndex+41,
			   Origin+bY*interYOffset,bY);
  ModelSupport::buildPlane(SMap,flightIndex+51,
			   Origin+bY*(interYOffset+interFrontWall),bY);
  // Layers [wall : Mid : Outer ]
  ModelSupport::buildCylinder(SMap,flightIndex+47,
			      Origin+Y*frontWall,bY,
			      outerRadius-interWallThick);
  ModelSupport::buildCylinder(SMap,flightIndex+48,
			      Origin+Y*frontWall,bY,
			      outerRadius-(interWallThick+interThick));
  ModelSupport::buildCylinder(SMap,flightIndex+49,
			      Origin+Y*frontWall,bY,
			      outerRadius-(2.0*interWallThick+interThick));

  return;
}

void
BeamTube::createObjects(Simulation& System)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
  */
{
  ELog::RegMethod RegA("BeamTube","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,flightIndex," 1 -2 -7 ");
  addOuterSurf(Out);

  Out=ModelSupport::getComposite(SMap,flightIndex," 1 -2 -7 (-11:17)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // Void (exclude inter wall)
  Out=ModelSupport::getComposite(SMap,flightIndex,
				 "11 -2 -17 (-21:27) (-41:-49)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,gapMat,0.0,Out));

  // Inner wall
  Out=ModelSupport::getComposite(SMap,flightIndex," 21 -2 -27 (-31:37)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  
  // Inner Void
  Out=ModelSupport::getComposite(SMap,flightIndex," 31 -2 -37 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  innerVoid=cellIndex-1;

  // Inter wall
  Out=ModelSupport::getComposite(SMap,flightIndex," -47 48 51 -2 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,interMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,flightIndex,
				 "41 -17 49 (47:-48:-51) -2 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));


  
  return;
}

void
BeamTube::createLinks()
  /*!
    Create All the links:
    - 0 : First surface
    - 1 : Exit surface
    - 2 : Inner face
  */
{
  SecondTrack::setBeamExit(Origin+bY*length,bY);
  setExit(Origin+bY*length,bY);

  FixedComp::setConnect(0,Origin,-Y);      // Note always to the reactor
  FixedComp::setConnect(2,Origin+Y*frontWall+bY*frontGap,bY);

  FixedComp::setLinkSurf(0,SMap.realSurf(flightIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(flightIndex+2));
  FixedComp::setLinkSurf(2,SMap.realSurf(flightIndex+21));

  return;
}

void
BeamTube::createAll(Simulation& System,const attachSystem::FixedComp& FC,
		    const Geometry::Vec3D& PAxis)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Moderator Object
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("BeamTube","createAll");
  populate(System);

  createUnitVector(FC,PAxis);
  createSurfaces();
  createObjects(System);
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE delftSystem
