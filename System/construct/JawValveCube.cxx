/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/JawValveCube
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"
#include "SurInter.h"

#include "JawUnit.h"
#include "JawValveBase.h"
#include "JawValveCube.h" 

namespace constructSystem
{

JawValveCube::JawValveCube(const std::string& Key) : 
  JawValveBase(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

JawValveCube::JawValveCube(const JawValveCube& A) :
  JawValveBase(A),
  width(A.width),height(A.height)
  /*!
    Copy constructor
    \param A :: JawValveCube to copy
  */
{}

JawValveCube&
JawValveCube::operator=(const JawValveCube& A)
  /*!
    Assignment operator
    \param A :: JawValveCube to copy
    \return *this
  */
{
  if (this!=&A)
    {
      JawValveBase::operator=(A);
      width=A.width;
      height=A.height;
    }
  return *this;
}


JawValveCube::~JawValveCube() 
  /*!
    Destructor
  */
{}

void
JawValveCube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("JawValveCube","populate");
  
  JawValveBase::populate(Control);

  // Void + Fe special:
  depth=Control.EvalVar<double>(keyName+"Depth");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");


  return;
}

void
JawValveCube::createSurfaces()
  /*!
    Create the surfaces
    If front/back given it is at portLen from the wall and 
    length/2+portLen from origin.
  */
{
  ELog::RegMethod RegA("JawValveCube","createSurfaces");

  JawValveBase::createSurfaces();

  // sides
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(depth+wallThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height+wallThick),Z);

  return;
}

void
JawValveCube::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("JawValveCube","createObjects");

  std::string Out;

  // Void 
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1 -2 3 -4 5 -6 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  // Main body
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1 -2 13 -14 15 -16 (-3:4:-5:6) ");
  makeCell("Body",System,cellIndex++,wallMat,0.0,Out);

  // front plate
  Out=ModelSupport::getComposite(SMap,buildIndex," -1 11 13 -14 15 -16 117 ");
  makeCell("FrontPlate",System,cellIndex++,wallMat,0.0,Out);

       
  // back plate
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 13 -14 15 -16 117 ");
  makeCell("BackPlate",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 13 -14 15 -16 ");
  addOuterSurf(Out);

  JawValveBase::createOuterObjects(System);
  
  return;
}
  
  
}  // NAMESPACE constructSystem
