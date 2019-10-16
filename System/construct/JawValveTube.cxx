/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/JawValveTube
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"
#include "SurInter.h"

#include "JawUnit.h"
#include "JawValveBase.h"
#include "JawValveTube.h" 

namespace constructSystem
{

JawValveTube::JawValveTube(const std::string& Key) :
  JawValveBase(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

JawValveTube::JawValveTube(const JawValveTube& A) :
  JawValveBase(A),innerRadius(A.innerRadius)
  /*!
    Copy constructor
    \param A :: JawValveTube to copy
  */
{}

JawValveTube&
JawValveTube::operator=(const JawValveTube& A)
  /*!
    Assignment operator
    \param A :: JawValveTube to copy
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


JawValveTube::~JawValveTube() 
  /*!
    Destructor
  */
{}

void
JawValveTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("JawValveTube","populate");

  length=Control.EvalVar<double>(keyName+"Length");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");

  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  // set values to zero:
  portARadius=0.0;
  portAThick=0.0;
  portALen=0.0;
  portBRadius=0.0;
  portBThick=0.0;
  portBLen=0.0;

  return;
}



void
JawValveTube::createSurfaces()
  /*!
    Create the surfaces
    If front/back given it is at portLen from the wall and 
    length/2+portLen from origin.
  */
{
  ELog::RegMethod RegA("JawValveTube","createSurfaces");

  //  JawValveBase::createSurfaces();

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    }
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
    }

  ModelSupport::buildCylinder 
    (SMap,buildIndex+7,Origin,Y,innerRadius);
  
  ModelSupport::buildCylinder
    (SMap,buildIndex+17,Origin,Y,innerRadius+wallThick);
  
  return;
}

void
JawValveTube::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("JawValveTube","createObjects");

  std::string Out;

  const std::string fbSurf=
    FrontBackCut::frontRule()+FrontBackCut::backRule();
  // Void 
  Out=ModelSupport::getComposite(SMap,buildIndex,"-7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+fbSurf);

  // Main body
  Out=ModelSupport::getComposite(SMap,buildIndex,"  -17 7 ");
  makeCell("Body",System,cellIndex++,wallMat,0.0,Out+fbSurf);


  Out=ModelSupport::getComposite(SMap,buildIndex,"-17 ");
  addOuterSurf(Out+fbSurf);

  return;
}
     
}  // NAMESPACE constructSystem
