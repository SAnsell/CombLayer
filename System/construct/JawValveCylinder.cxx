/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/JawValveCylinder
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"
#include "SurInter.h"

#include "JawUnit.h"
#include "JawValveBase.h"
#include "JawValveCylinder.h" 

namespace constructSystem
{

JawValveCylinder::JawValveCylinder(const std::string& Key) :
  JawValveBase(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

JawValveCylinder::JawValveCylinder(const JawValveCylinder& A) :
  JawValveBase(A),
  innerRadius(A.innerRadius)
  /*!
    Copy constructor
    \param A :: JawValveCylinder to copy
  */
{}

JawValveCylinder&
JawValveCylinder::operator=(const JawValveCylinder& A)
  /*!
    Assignment operator
    \param A :: JawValveCylinder to copy
    \return *this
  */
{
  if (this!=&A)
    {
      JawValveBase::operator=(A);
      innerRadius=A.innerRadius;
    }
  return *this;
}


JawValveCylinder::~JawValveCylinder() 
  /*!
    Destructor
  */
{}

void
JawValveCylinder::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("JawValveCylinder","populate");

  JawValveBase::populate(Control);
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");

  return;
}



void
JawValveCylinder::createSurfaces()
  /*!
    Create the surfaces
    If front/back given it is at portLen from the wall and 
    length/2+portLen from origin.
  */
{
  ELog::RegMethod RegA("JawValveCylinder","createSurfaces");

  JawValveBase::createSurfaces();
  

  ModelSupport::buildCylinder
    (SMap,buildIndex+7,Origin,Y,innerRadius);
  
  ModelSupport::buildCylinder
    (SMap,buildIndex+17,Origin,Y,innerRadius+wallThick);
  
  return;
}

void
JawValveCylinder::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("JawValveCylinder","createObjects");

  std::string Out;

  // Void 
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  // Main body
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -17 7 ");
  makeCell("Body",System,cellIndex++,wallMat,0.0,Out);

  // front plate
  Out=ModelSupport::getComposite(SMap,buildIndex," -1 11 -17 117 ");
  makeCell("FrontPlate",System,cellIndex++,wallMat,0.0,Out);

  // back plate
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 -17  217 ");
  makeCell("BackPlate",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 -17 ");
  addOuterSurf(Out);

  JawValveBase::createOuterObjects(System);
  
  return;
}
  
void
JawValveCylinder::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("JawValveCylinder","createLinks");

  JawValveBase::createLinks();

  return;
}

  
void
JawValveCylinder::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("JawValveCylinder","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  createJaws(System);
  
  return;
}
  
}  // NAMESPACE constructSystem
