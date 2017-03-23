/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/Motor.cxx
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
#include <memory>
#include <array>

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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "FrontBackCut.h"
#include "Motor.h"

namespace constructSystem
{

Motor::Motor(const std::string& Key) : 
  attachSystem::FixedOffset(Key,4),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::FrontBackCut(),
  motorIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(motorIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
   \param Key :: KeyName
  */
{}


Motor::~Motor() 
  /*!
    Destructor
  */
{}

void
Motor::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("Motor","populate");

  FixedOffset::populate(Control);
  //  + Fe special:
  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}

void
Motor::createUnitVector(const attachSystem::FixedComp& FC,
                              const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("Motor","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}


void
Motor::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("Motor","createSurfaces");


  ModelSupport::buildPlane(SMap,motorIndex+2,Origin+Y*length,Y);
  ModelSupport::buildCylinder(SMap,motorIndex+7,Origin,Y,radius);
    
  return;
}
  
void
Motor::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
    */
{
  ELog::RegMethod RegA("Motor","createObjects");

  if (frontActive())
    {
      std::string Out;
	
      // Main void
      Out=ModelSupport::getComposite(SMap,motorIndex," -2 -7 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out+frontRule()));
      addCell("Motor",cellIndex-1);
  
      addOuterSurf(Out+frontRule());  
    }
  return;
}

void
Motor::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("Motor","createLinks");

  return;
}

void
Motor::createAll(Simulation& System,
                       const attachSystem::FixedComp& motorFC,
                       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param motorFC :: FixedComp at the motor centre
    \param FIndex :: side index
  */
{
  ELog::RegMethod RegA("Motor","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(motorFC,FIndex);
  
  if (!frontActive())
    FrontBackCut::setFront(motorFC,FIndex);
  createSurfaces();    
  createObjects(System);  
  createLinks();
  insertObjects(System);   


  return;
}
  
}  // NAMESPACE constructSystem
