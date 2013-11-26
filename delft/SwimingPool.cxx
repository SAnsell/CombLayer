/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delft/SwimingPool.cxx
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
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "SwimingPool.h"

namespace delftSystem
{

SwimingPool::SwimingPool(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,1),
  poolIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(poolIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

SwimingPool::SwimingPool(const SwimingPool& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  poolIndex(A.poolIndex),cellIndex(A.cellIndex),
  base(A.base),surface(A.surface),
  waterMat(A.waterMat)
  /*!
    Copy constructor
    \param A :: SwimingPool to copy
  */
{}

SwimingPool&
SwimingPool::operator=(const SwimingPool& A)
  /*!
    Assignment operator
    \param A :: SwimingPool to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      base=A.base;
      surface=A.surface;
      waterMat=A.waterMat;
    }
  return *this;
}

SwimingPool::~SwimingPool() 
 /*!
   Destructor
 */
{}

void
SwimingPool::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("SwimingPool","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  // First get inner widths:

  base=Control.EvalVar<double>(keyName+"Base");
  surface=Control.EvalVar<double>(keyName+"Surface");

  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");

  frontWidth=Control.EvalVar<double>(keyName+"FrontWidth");
  backWidth=Control.EvalVar<double>(keyName+"BackWidth");
  beamSide=Control.EvalVar<double>(keyName+"BeamSide");
  extendBeamSide=Control.EvalVar<double>(keyName+"ExtendBeamSide");
  beamSideBackLen=Control.EvalVar<double>(keyName+"BeamSideBackLength");
  beamSideFrontLen=Control.EvalVar<double>(keyName+"BeamSideFrontLength");
  beamSideExtendLen=Control.EvalVar<double>(keyName+"BeamSideExtendLength");
  frontLength=Control.EvalVar<double>(keyName+"FrontLength");
  doorWidth=Control.EvalVar<double>(keyName+"DoorWidth");
  doorLength=Control.EvalVar<double>(keyName+"DoorLength");

  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");

  return;
}
  
void
SwimingPool::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param FC :: A Contained FixedComp to use as basis set
  */
{
  ELog::RegMethod RegA("SwimingPool","createUnitVector");
  
  attachSystem::FixedComp::createUnitVector(FC);
  attachSystem::FixedComp::applyShift(xStep,yStep,zStep);
  return;
}

void
SwimingPool::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("SwimingPool","createSurfaces");

  // Pool layer
  ModelSupport::buildPlane(SMap,poolIndex+1,Origin-Y*frontWidth,Y);
  ModelSupport::buildPlane(SMap,poolIndex+2,Origin+Y*backWidth,Y);


  ModelSupport::buildPlane(SMap,poolIndex+12,
			   Origin+Y*(backWidth+doorWidth),Y);
  
  ModelSupport::buildPlane(SMap,poolIndex+3,Origin-X*beamSide/2.0,X);
  ModelSupport::buildPlane(SMap,poolIndex+13,Origin-X*extendBeamSide/2.0,X);

  ModelSupport::buildPlane(SMap,poolIndex+4,Origin+X*beamSide/2.00,X);
  ModelSupport::buildPlane(SMap,poolIndex+14,Origin+X*extendBeamSide/2.0,X);

  // door
  ModelSupport::buildPlane(SMap,poolIndex+23,Origin-X*doorLength/2.0,X);
  ModelSupport::buildPlane(SMap,poolIndex+24,Origin+X*doorLength/2.0,X);


  ModelSupport::buildPlane(SMap,poolIndex+5,Origin-Z*base,Z);
  ModelSupport::buildPlane(SMap,poolIndex+6,Origin+Z*surface,Z);

  // Front angles
  ModelSupport::buildPlane(SMap,poolIndex+7,
			   Origin-X*beamSide/2.0-Y*beamSideFrontLen,
			   Origin-X*beamSide/2.0-Y*beamSideFrontLen+Z,
			   Origin-X*frontLength/2.0-Y*frontWidth,
			   X);
  ModelSupport::buildPlane(SMap,poolIndex+8,
			   Origin+X*beamSide/2.0-Y*beamSideFrontLen,
			   Origin+X*beamSide/2.0-Y*beamSideFrontLen+Z,
			   Origin+X*frontLength/2.0-Y*frontWidth,
			   X);
  // Back angles
  ModelSupport::buildPlane(SMap,poolIndex+17,
			   Origin-X*beamSide/2.0+Y*beamSideBackLen,
			   Origin-X*beamSide/2.0+Y*beamSideBackLen+Z,
			   Origin-X*extendBeamSide/2.0+Y*beamSideExtendLen,
			   X);
  ModelSupport::buildPlane(SMap,poolIndex+18,
   			   Origin+X*beamSide/2.0+Y*beamSideBackLen,
   			   Origin+X*beamSide/2.0+Y*beamSideBackLen+Z,
   			   Origin+X*extendBeamSide/2.0+Y*beamSideExtendLen,
   			   X);
			   

  
  // Outer layers

  return;
}

void
SwimingPool::createObjects(Simulation& System,
			   const attachSystem::ContainedComp& RG)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
    \param RG :: Reactor grid object (inner container)
  */
{
  ELog::RegMethod RegA("SwimingPool","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,poolIndex,
				 " 1 (-2 : (23 -24 -12)) 7 -8 13 -14 "
				 " (17:3) (-18:-4) 5 -6 ");
  addOuterSurf(Out);
  Out+=RG.getExclude();
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  
  return;
}


void
SwimingPool::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const attachSystem::ContainedComp& CC)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Moderator Object
    \param CC :: Interal object
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("SwimingPool","createAll");
  populate(System);

  createUnitVector(FC);
  createSurfaces();
  createObjects(System,CC);
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE delftSystem
