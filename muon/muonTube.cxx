/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muon/muonTube.cxx
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
#include <iterator>
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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "World.h"
#include "muonTube.h"

namespace muSystem
{

muonTube::muonTube(const std::string& Key)  : 
  attachSystem::FixedComp(Key,3),attachSystem::ContainedComp(),
  tubeIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(tubeIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}


muonTube::~muonTube() 
  /*!
    Destructor
  */
{}

void
muonTube::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("muonTube","populate");

  const FuncDataBase& Control=System.getDataBase();

  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");   

  radius=Control.EvalVar<double>(keyName+"Radius");   
  thick=Control.EvalVar<double>(keyName+"Thick");
  length=Control.EvalVar<double>(keyName+"Length");     

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
       
  return;
}

void
muonTube::createUnitVector()
  /*!
    Create the unit vectors
  */
{
  ELog::RegMethod RegA("muonTube","createUnitVector");

  attachSystem::FixedComp::createUnitVector(World::masterOrigin());
  applyShift(xStep,yStep,zStep);
  
  return;
}

void
muonTube::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("muonTube","createSurface");

  //  :
  ModelSupport::buildPlane(SMap,tubeIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,tubeIndex+2,Origin+Y*length,Y);
  ModelSupport::buildCylinder(SMap,tubeIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,tubeIndex+17,Origin,Y,radius-thick);      


  return;
}

void
muonTube::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("muonTube","createObjects");
  
  std::string Out;
  std::string Out1;  

     // Steel
  Out=ModelSupport::getComposite(SMap,tubeIndex,"1 -2 -7 ");
  addOuterSurf(Out);
  addBoundarySurf(Out);
  Out1=ModelSupport::getComposite(SMap,tubeIndex,"17 ");  
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out+Out1));

    // hole
  Out=ModelSupport::getComposite(SMap,tubeIndex,"1 -2 -17 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  return;
}


void
muonTube::createLinks()
  /*!
    Create links
   */
{
  ELog::RegMethod RegA("muonTube","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setConnect(1,Origin+
			Y*length,Y);

  FixedComp::setLinkSurf(0,-SMap.realSurf(tubeIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(tubeIndex+2));  

  return;
}

void
muonTube::createAll(Simulation& System)
  /*!
    Create the shutter
    \param System :: Simulation to process
  */
{
  ELog::RegMethod RegA("muonTube","createAll");
  populate(System);
  createUnitVector();
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE shutterSystem
