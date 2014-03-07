/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuild/BeRef.cxx
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
#include "BeRef.h"


namespace essSystem
{

BeRef::BeRef(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,3),
  refIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(refIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}


BeRef::~BeRef()
  /*!
    Destructor
   */
{}

void
BeRef::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("BeRef","populate");


    // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");
  radius=Control.EvalVar<double>(keyName+"Radius");   
  height=Control.EvalVar<double>(keyName+"Height");   
  wallThick=Control.EvalVar<double>(keyName+"WallThick");   

  refMat=ModelSupport::EvalMat<int>(Control,keyName+"RefMat");   
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");   
  
  targSepThick=Control.EvalVar<double>(keyName+"TargSepThick");   
  targSepMat=ModelSupport::EvalMat<int>
    (Control,StrFunc::makeString(keyName+"TargSepMat"));   

  return;
}

void
BeRef::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
{
  ELog::RegMethod RegA("BeRef","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);
  
  return;
}

void
BeRef::createSurfaces()
  /*!
    Create Surfaces for the Be
  */
{
  ELog::RegMethod RegA("BeRef","createSurfaces");
      
  ModelSupport::buildCylinder(SMap,refIndex+7,Origin,Z,radius);  
  ModelSupport::buildCylinder(SMap,refIndex+17,Origin,Z,radius+wallThick);  

  ModelSupport::buildPlane(SMap,refIndex+5,Origin-Z*(height/2.0),Z);  
  ModelSupport::buildPlane(SMap,refIndex+6,Origin+Z*(height/2.0),Z);  
  ModelSupport::buildPlane(SMap,refIndex+15,
			   Origin-Z*(height/2.0+wallThick),Z);  
  ModelSupport::buildPlane(SMap,refIndex+16,
			   Origin+Z*(height/2.0+wallThick),Z);  

  //define planes where the Be is substituted by Fe

  ModelSupport::buildPlane(SMap,refIndex+25,Origin-
			   Z*(targSepThick/2.0+wallThick),Z);  
  ModelSupport::buildPlane(SMap,refIndex+26,Origin+
			   Z*(targSepThick/2.0+wallThick),Z);  

  ModelSupport::buildPlane(SMap,refIndex+35,Origin-Z*(targSepThick/2.0),Z);  
  ModelSupport::buildPlane(SMap,refIndex+36,Origin+Z*(targSepThick/2.0),Z);  

  return; 
}

void
BeRef::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("BeRef","createObjects");

  std::string Out;
  
  // low segment
  Out=ModelSupport::getComposite(SMap,refIndex," -7 5 -35 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,refMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,refIndex," -17 35 -36");
  System.addCell(MonteCarlo::Qhull(cellIndex++,targSepMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,refIndex," -7 36 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,refMat,0.0,Out));
  
  Out=ModelSupport::getComposite(SMap,refIndex," -7 5 -6 ");

  if (wallThick>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,refIndex," -17 15 -35 (7:-5)");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

      // divide layer
      Out=ModelSupport::getComposite(SMap,refIndex," -17 -25 35 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

      // divide layer
      Out=ModelSupport::getComposite(SMap,refIndex," -17 26 -36 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));


      Out=ModelSupport::getComposite(SMap,refIndex," -17 -16 36 (7:6)");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

      Out=ModelSupport::getComposite(SMap,refIndex," -17 15 -16 ");
    }
      
  addOuterSurf(Out);
  return; 

}

void
BeRef::createLinks()
  /*!
    Creates a full attachment set
    Links/directions going outwards true.
  */
{
  FixedComp::setConnect(0,Origin+Y*radius,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(refIndex+17));

  FixedComp::setConnect(1,Origin-Z*(height/2.0+wallThick),-Z);
  FixedComp::setLinkSurf(1,-SMap.realSurf(refIndex+15));

  FixedComp::setConnect(2,Origin+Z*(height/2.0+wallThick),Z);
  FixedComp::setLinkSurf(2,SMap.realSurf(refIndex+16));

  return;
}


void
BeRef::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
   */
{
  ELog::RegMethod RegA("BeRef","createAll");
  populate(System.getDataBase());

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE instrumentSystem
