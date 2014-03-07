/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delft/SpaceBlock.cxx
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
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "SpaceBlock.h"

namespace delftSystem
{

SpaceBlock::SpaceBlock(const std::string& Key,const size_t Index)  :
  attachSystem::FixedComp(StrFunc::makeString(Key,Index),6),
  attachSystem::ContainedComp(),baseName(Key),
  boxIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(boxIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

SpaceBlock::SpaceBlock(const SpaceBlock& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  baseName(A.baseName),boxIndex(A.boxIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),zAngle(A.zAngle),length(A.length),
  width(A.width),height(A.height),mat(A.mat)
  /*!
    Copy constructor
    \param A :: SpaceBlock to copy
  */
{}

SpaceBlock&
SpaceBlock::operator=(const SpaceBlock& A)
  /*!
    Assignment operator
    \param A :: SpaceBlock to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      length=A.length;
      width=A.width;
      height=A.height;
      mat=A.mat;
    }
  return *this;
}

SpaceBlock::~SpaceBlock() 
 /*!
   Destructor
 */
{}

int
SpaceBlock::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: FuncDatabase
   \return build status [false on failure]
 */
{
  ELog::RegMethod RegA("SpaceBlock","populate");
  

  const int flag=Control.EvalDefVar<int>(keyName+"Flag",-1);
  if (flag<1)  return flag;

  // First get inner widths:
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
  zAngle=Control.EvalVar<double>(keyName+"ZAngle");

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return 1;
}
  
void
SpaceBlock::createUnitVector(const attachSystem::FixedComp& FC,
			     const size_t sideIndex)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param FC :: A Contained FixedComp to use as basis set
    \param PAxis :: Primary axis
  */
{
  ELog::RegMethod RegA("SpaceBlock","createUnitVector");
  
  // PROCESS Origin of a point
  if (sideIndex)
    {
      const attachSystem::LinkUnit& LU=FC.getLU(sideIndex-1);
      attachSystem::FixedComp::createUnitVector(LU.getConnectPt(),
						LU.getAxis(),
						FC.getZ());
    }
  else
    attachSystem::FixedComp::createUnitVector(FC);
  
  attachSystem::FixedComp::applyShift(xStep,yStep,zStep);
  attachSystem::FixedComp::applyAngleRotate(xyAngle,zAngle);
  return;
}


void
SpaceBlock::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("SpaceBlock","createSurfaces");

  ModelSupport::buildPlane(SMap,boxIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,boxIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,boxIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,boxIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,boxIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,boxIndex+6,Origin+Z*(height/2.0),Z);

  return;
}

void
SpaceBlock::createObjects(Simulation& System)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
  */
{
  ELog::RegMethod RegA("SpaceBlock","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,boxIndex," 1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  
  return;
}

void
SpaceBlock::createLinks()
  /*!
    Create All the links:
  */
{
  ELog::RegMethod RegA("SpaceBlock","createLinks");

  FixedComp::setConnect(0,Origin,-Y);      
  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);

  
  FixedComp::setLinkSurf(0,-SMap.realSurf(boxIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(boxIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(boxIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(boxIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(boxIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(boxIndex+6));

  return;
}

int
SpaceBlock::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const size_t sideIndex)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Moderator Object
    \param sideIndex :: Side index [0 ==> FC origin]
    \return flag status
  */
{
  ELog::RegMethod RegA("SpaceBlock","createAll");

  const int flag=populate(System.getDataBase());
  if (flag>0)
    {
      createUnitVector(FC,sideIndex);
      createSurfaces();
      createObjects(System);
      insertObjects(System);       
    }
  return flag;
}

  
}  // NAMESPACE delftSystem
