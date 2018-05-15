/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/beamSlot.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "SimProcess.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "beamSlot.h"

namespace delftSystem
{

beamSlot::beamSlot(const std::string& Key,const int SN)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key+StrFunc::makeString(SN),6),
  baseName(Key),
  surfIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param SN :: slot number  (id number)
  */
{}

beamSlot::beamSlot(const beamSlot& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  baseName(A.baseName),surfIndex(A.surfIndex),
  cellIndex(A.cellIndex),xyAngle(A.xyAngle),zAngle(A.zAngle),
  xStep(A.xStep),zStep(A.zStep),xSize(A.xSize),
  zSize(A.zSize)
  /*!
    Copy constructor
    \param A :: beamSlot to copy
  */
{}

beamSlot&
beamSlot::operator=(const beamSlot& A)
  /*!
    Assignment operator
    \param A :: beamSlot to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      xStep=A.xStep;
      zStep=A.zStep;
      xSize=A.xSize;
      zSize=A.zSize;
    }
  return *this;
}


beamSlot::~beamSlot() 
 /*!
   Destructor
 */
{}

void
beamSlot::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Variable data base
 */
{
  ELog::RegMethod RegA("beamSlot","populate");
  
  // First get inner widths:
  axisAngle=Control.EvalPair<double>(keyName+"AxisAngle",keyName+"AxisAngle");
  
  xStep=Control.EvalPair<double>(keyName+"XStep",baseName+"XStep");
  zStep=Control.EvalPair<double>(keyName+"ZStep",baseName+"ZStep");
  xyAngle=Control.EvalPair<double>(keyName+"XYAngle",baseName+"XYAngle");
  zAngle=Control.EvalPair<double>(keyName+"ZAngle",baseName+"ZAngle");

  
  xSize=Control.EvalPair<double>(keyName+"XSize",baseName+"XSize");
  zSize=Control.EvalPair<double>(keyName+"ZSize",baseName+"ZSize");

  endThick=Control.EvalPair<double>(keyName+"EndThick",baseName+"EndThick");
  divideThick=Control.EvalPair<double>(keyName+"DivideThick",
				       baseName+"DivideThick");

  NChannels=Control.EvalPair<size_t>(keyName,baseName,"NChannels");

  glassMat=ModelSupport::EvalMat<int>(Control,keyName+"GlassMat",
				      baseName+"GlassMat");

  return;
}
  
void
beamSlot::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param FC :: A Contained FixedComp to use as basis set
  */
{
  ELog::RegMethod RegA("beamSlot","createUnitVector");

  FixedComp::createUnitVector(FC);

  // PROCESS Origin of a point
  Origin+=X*xStep+Z*zStep;

  if (fabs(axisAngle)>Geometry::zeroTol || 
      fabs(xyAngle)>Geometry::zeroTol || 
      fabs(zAngle)>Geometry::zeroTol)
    {
      const Geometry::Quaternion Qaxis=
	Geometry::Quaternion::calcQRotDeg(axisAngle,Y);
      const Geometry::Quaternion Qz=
	Geometry::Quaternion::calcQRotDeg(zAngle,X);
      const Geometry::Quaternion Qxy=
	Geometry::Quaternion::calcQRotDeg(xyAngle,Z);
  
      Qaxis.rotate(X);
      Qaxis.rotate(Z);
      Qz.rotate(X);
      Qz.rotate(Y);
      Qz.rotate(Z);
      Qxy.rotate(Y);
      Qxy.rotate(X);
      Qxy.rotate(Z); 
    }
  return;
}

void
beamSlot::createSurfaces(const attachSystem::FixedComp& FC)
  /*!
    Create All the surfaces
    \param FC :: FixedComp for front/back
  */
{
  ELog::RegMethod RegA("beamSlot","createSurfaces");

  SMap.addMatch(surfIndex+1,FC.getLinkSurf(1));
  SMap.addMatch(surfIndex+2,FC.getLinkSurf(2));

  ModelSupport::buildPlane(SMap,surfIndex+3,Origin-X*(xSize/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+4,Origin+X*(xSize/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+5,Origin-Z*zSize/2.0,Z);
  ModelSupport::buildPlane(SMap,surfIndex+6,Origin+Z*zSize/2.0,Z);


  ModelSupport::buildPlane(SMap,surfIndex+13,
			   Origin-X*(xSize/2.0-endThick),X);
  ModelSupport::buildPlane(SMap,surfIndex+14,
  			   Origin+X*(xSize/2.0-endThick),X);

  const double gap=(zSize-static_cast<double>(NChannels+1)*divideThick)/
    static_cast<double>(NChannels);
  double zPoint(-zSize/2.0);
  int surfOffset(surfIndex+10);
  for(size_t i=0;i<NChannels;i++)
    {
      // Glass: Air
      ModelSupport::buildPlane(SMap,surfOffset+5,
			     Origin+Z*(zPoint+divideThick),Z);
      // Air: Glass: 
      ModelSupport::buildPlane(SMap,surfOffset+15,
			     Origin+Z*(zPoint+divideThick+gap),Z);
      surfOffset+=20;
      zPoint+=divideThick+gap;
    }
    
  return;
}

void
beamSlot::createObjects(Simulation& System)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
  */
{
  ELog::RegMethod RegA("beamSlot","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,surfIndex," 3 -4 5 -6 ");
  addOuterSurf(Out);

  
  // End plates
  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 3 -13 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,glassMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 14 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,glassMat,0.0,Out));

  int surfOffset(surfIndex);
  const std::string baseOut=
    ModelSupport::getComposite(SMap,surfIndex," 1 -2 13 -14 ");
  
  for(size_t i=0;i<NChannels;i++)
    {
      Out=baseOut+ModelSupport::getComposite(SMap,surfOffset," 5 -15 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,glassMat,0.0,Out));
      Out=baseOut+ModelSupport::getComposite(SMap,surfOffset," 15 -25 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      surfOffset+=20;
    }
  Out=baseOut+ModelSupport::getComposite(SMap,surfIndex,
					 surfOffset," 5M -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,glassMat,0.0,Out));
  
  return;
}

void
beamSlot::createLinks()
  /*!
    Create All the links:
    - 0 : First surface
    - 1 : Exit surface
    - 2 : Inner face
  */
{
  ELog::RegMethod RegA("beamSlot","createLinks");

  FixedComp::setConnect(0,Origin,-Y); 
  FixedComp::setConnect(1,Origin,Y);  
  FixedComp::setConnect(2,Origin-X*xSize,X); 
  FixedComp::setConnect(3,Origin+X*xSize,X); 
  FixedComp::setConnect(4,Origin-Z*zSize/2.0,-Z); 
  FixedComp::setConnect(5,Origin+Z*zSize/2.0,Z); 

  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(surfIndex+2));
  for(size_t i=2;i<6;i++)
    {
      const int sN(surfIndex+static_cast<int>(i+1));
      FixedComp::setLinkSurf(i,SMap.realSurf(sN));
    }

  return;
}

void
beamSlot::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add slot to
    \param FC :: BeamInsert Object
  */
{
  ELog::RegMethod RegA("beamSlot","createAll");
  populate(System.getDataBase());

  createUnitVector(FC);
  createSurfaces(FC);
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE delftSystem
