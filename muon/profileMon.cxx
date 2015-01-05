/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muon/profileMon.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell/Goran Skoro
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
#include <iterator>
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
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "Convex.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "profileMon.h"

namespace muSystem
{

profileMon::profileMon(const std::string& Key)  : 
  attachSystem::FixedComp(Key,6),attachSystem::ContainedComp(),
  profMonIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(profMonIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}

profileMon::profileMon(const profileMon& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  profMonIndex(A.profMonIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),height(A.height),depth(A.depth),
  width(A.width),steelMat(A.steelMat)
  /*!
    Copy constructor
    \param A :: profileMon to copy
  */
{}

profileMon&
profileMon::operator=(const profileMon& A)
  /*!
    Assignment operator
    \param A :: profileMon to copy
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
      height=A.height;
      depth=A.depth;
      width=A.width;
      steelMat=A.steelMat;
    }
  return *this;
}

profileMon::~profileMon() 
  /*!
    Destructor
  */
{}

void
profileMon::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: database for variables
  */
{
  ELog::RegMethod RegA("profileMon","populate");

  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");

  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  width=Control.EvalVar<double>(keyName+"Width");
  
  steelMat=ModelSupport::EvalMat<int>(Control,keyName+"SteelMat");    
       
  return;
}

void
profileMon::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
  */
{
  ELog::RegMethod RegA("profileMon","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,0);    
  
  return;
}

void
profileMon::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("profileMon","createSurface");

  // steel box
  ModelSupport::buildPlane(SMap,profMonIndex+1,Origin-Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,profMonIndex+2,Origin+Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,profMonIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,profMonIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,profMonIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,profMonIndex+6,Origin+Z*height/2.0,Z);
  
  return;
}

void
profileMon::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("profileMon","createObjects");
  
  std::string Out;

    // Steel
  Out=ModelSupport::getComposite(SMap,profMonIndex,"1 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

  addOuterSurf(Out);
  addBoundarySurf(Out);
  
  return;
}


void
profileMon::createLinks()
  /*!
    Create links
   */
{
  ELog::RegMethod RegA("profileMon","createLinks");

  FixedComp::setLinkSurf(0,-SMap.realSurf(profMonIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(profMonIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(profMonIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(profMonIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(profMonIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(profMonIndex+6));

  FixedComp::setConnect(0,Origin-Y*depth/2.0,-Y);
  FixedComp::setConnect(1,Origin+Y*depth/2.0,Y);
  FixedComp::setConnect(2,Origin-X*width/2.0,-X);
  FixedComp::setConnect(3,Origin+X*width/2.0,X);
  FixedComp::setConnect(4,Origin-Z*height/2.0,-Z);
  FixedComp::setConnect(5,Origin+Z*height/2.0,Z);

  return;
}

void
profileMon::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC)

  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("profileMon","createAll");

  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE muSystem
