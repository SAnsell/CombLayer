/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/TubeMod.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
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
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "TubeMod.h"

namespace photonSystem
{
      
TubeMod::TubeMod(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  modIndex(ModelSupport::objectRegister::Instance().cell(Key)), 
  cellIndex(modIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

TubeMod::TubeMod(const TubeMod& A) :
   attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  modIndex(A.modIndex),cellIndex(A.cellIndex),xStep(A.xStep),
  yStep(A.yStep),zStep(A.zStep),xyAngle(A.xyAngle),
  zAngle(A.zAngle),outerRadius(A.outerRadius),outerHeight(A.outerHeight),
  outerMat(A.outerMat),Tubes(A.Tubes)
  /*!
    Copy constructor
    \param A :: TubeMod to copy
  */
{}

TubeMod&
TubeMod::operator=(const TubeMod& A)
  /*!
    Assignment operator
    \param A :: TubeMod to copy
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
      outerRadius=A.outerRadius;
      outerHeight=A.outerHeight;
      outerMat=A.outerMat;
      Tubes=A.Tubes;
    }
  return *this;
}

TubeMod::~TubeMod()
  /*!
    Destructor
  */
{}

TubeMod*
TubeMod::clone() const
  /*!
    Clone copy constructor
    \return copy of this
  */
{
  return new TubeMod(*this);
}

void
TubeMod::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("TubeMod","populate");

    // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  outerHeight=Control.EvalVar<double>(keyName+"OuterHeight");
  
  const size_t nTubes=Control.EvalVar<size_t>(keyName+"NTubes");

  for(size_t i=0;i<nTubes;i++)
    {
      const std::string KN=keyName+StrFunc::makeString(i);
      TUnit TI;
      TI.Offset=Control.EvalPair<Geometry::Vec3D>
	(KN+"Offset",keyName+"Offset");
      TI.radius=Control.EvalPair<double>(KN+"Radius",keyName+"Radius");
      TI.height=Control.EvalPair<double>(KN+"Height",keyName+"Height");
      
      TI.mat=ModelSupport::EvalMat<int>(Control,KN+"Mat",keyName+"Mat");
      Tubes.push_back(TI);
    }
  return;
}

void
TubeMod::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: Link point surface to use as origin/basis.
  */
{
  ELog::RegMethod RegA("TubeMod","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

  return;
}

void
TubeMod::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("TubeMod","createSurfaces");

  // Outer surfaces:
  ModelSupport::buildPlane(SMap,modIndex+1,Origin,Y);  
  ModelSupport::buildPlane(SMap,modIndex+2,Origin+Y*outerHeight,Y);  
  ModelSupport::buildCylinder(SMap,modIndex+7,
			      Origin,Y,outerRadius);

  int tIndex(modIndex+100);
  for(const TUnit& TI : Tubes)
    {
      const Geometry::Vec3D OR=Origin+X*TI.Offset.X()+
	Y*TI.Offset.Y()+Z*TI.Offset.Z();
      ModelSupport::buildPlane(SMap,tIndex+1,OR,Y);
      ModelSupport::buildPlane(SMap,tIndex+2,OR+Y*TI.height,Y);
      ModelSupport::buildCylinder(SMap,tIndex+7,OR,Y,TI.radius);
      tIndex+=100;
    }
  return; 
}

void
TubeMod::createObjects(Simulation& System)
  /*!
    Create the tubed moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("TubeMod","createObjects");

  std::string Out;
  HeadRule TubeCollection;
  int tIndex(modIndex+100);

  for(const TUnit& TI : Tubes)
    {
      Out=ModelSupport::getComposite(SMap,tIndex," 1 -2 -7 ");
      TubeCollection.addUnion(Out);
      System.addCell(MonteCarlo::Qhull(cellIndex++,TI.mat,0.0,Out));
      tIndex+=100;
    }
  Out=ModelSupport::getComposite(SMap,modIndex,modIndex," 1 -2 -7 ");
  addOuterSurf(Out);
  // Now exclude tubes
  TubeCollection.makeComplement();
  Out+=TubeCollection.display();
  System.addCell(MonteCarlo::Qhull(cellIndex++,outerMat,0.0,Out));
  return; 
}

void
TubeMod::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("TubeMod","createLinks");
  
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(modIndex+1));

  FixedComp::setConnect(1,Origin+Y*outerHeight,Y);
  FixedComp::setLinkSurf(1,-SMap.realSurf(modIndex+2));

  FixedComp::setConnect(2,Origin+Y*(outerHeight/2.0)-X*outerRadius,-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(modIndex+18));
  
  FixedComp::setConnect(3,Origin+Y*(outerHeight/2.0)-Z*outerRadius,-Z);
  FixedComp::setLinkSurf(3,SMap.realSurf(modIndex+18));

  FixedComp::setConnect(4,Origin+Y*(outerHeight/2.0)+X*outerRadius,X);
  FixedComp::setLinkSurf(4,SMap.realSurf(modIndex+18));

  FixedComp::setConnect(5,Origin+Y*(outerHeight/2.0)+Z*outerRadius,Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(modIndex+18));

  return;
}

void
TubeMod::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: surface offset
   */
{
  ELog::RegMethod RegA("TubeMod","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE photonSystem
