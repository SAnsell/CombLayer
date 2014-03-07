/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muon/muonTube.cxx
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
#include "surfRegister.h"
#include "objectRegister.h"
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
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "muonTube.h"

namespace muSystem
{

muonTube::muonTube(const std::string& Key)  : 
  attachSystem::FixedComp(Key,6),attachSystem::ContainedComp(),
  tubeIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(tubeIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}

muonTube::muonTube(const muonTube& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  tubeIndex(A.tubeIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  radius(A.radius),thick(A.thick),length(A.length),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: muonTube to copy
  */
{}

muonTube&
muonTube::operator=(const muonTube& A)
  /*!
    Assignment operator
    \param A :: muonTube to copy
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
      radius=A.radius;
      thick=A.thick;
      length=A.length;
      mat=A.mat;
    }
  return *this;
}


muonTube::~muonTube() 
  /*!
    Destructor
  */
{}

void
muonTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Data base for variables
  */
{
  ELog::RegMethod RegA("muonTube","populate");

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
muonTube::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: FixedComp for unit vectors
  */
{
  ELog::RegMethod RegA("muonTube","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC);
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
    Adds the real objects and processes outer boundary
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("muonTube","createObjects");
  
  std::string Out;

  // Steel
  Out=ModelSupport::getComposite(SMap,tubeIndex,"1 -2 -7 17 ");  
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));

    // hole
  Out=ModelSupport::getComposite(SMap,tubeIndex,"1 -2 -17 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addBoundarySurf(Out);   // Inner part

  Out=ModelSupport::getComposite(SMap,tubeIndex,"1 -2 -7 ");
  addOuterSurf(Out);
  
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
  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setConnect(2,Origin-X*radius,-X);
  FixedComp::setConnect(3,Origin+X*radius,X);
  FixedComp::setConnect(4,Origin-Z*radius,-Z);
  FixedComp::setConnect(5,Origin+Z*radius,Z);

  FixedComp::setLinkSurf(0,-SMap.realSurf(tubeIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(tubeIndex+2));  
  FixedComp::setLinkSurf(2,SMap.realSurf(tubeIndex+17));
  FixedComp::setLinkSurf(3,SMap.realSurf(tubeIndex+17));
  FixedComp::setLinkSurf(4,SMap.realSurf(tubeIndex+17));
  FixedComp::setLinkSurf(5,SMap.realSurf(tubeIndex+17));

  return;
}

void
muonTube::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param FC :: Fixed Compent for origin/axis
  */
{
  ELog::RegMethod RegA("muonTube","createAll");
  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE shutterSystem
