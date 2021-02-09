/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   muon/targSimpleShield.cxx
*
 * Copyright (c) 2004-2018 by Stuart Ansell/Goran Skoro
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "targSimpleShield.h"

namespace muSystem
{

targSimpleShield::targSimpleShield(const std::string& Key)  : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}

targSimpleShield::targSimpleShield(const targSimpleShield& A) : 
  attachSystem::FixedRotate(A),attachSystem::ContainedComp(A),
  height(A.height),depth(A.depth),width(A.width),
  backThick(A.backThick),forwThick(A.forwThick),
  leftThick(A.leftThick),rightThick(A.rightThick),
  baseThick(A.baseThick),topThick(A.topThick),mat(A.mat)
  /*!
    Copy constructor
    \param A :: targSimpleShield to copy
  */
{}

targSimpleShield&
targSimpleShield::operator=(const targSimpleShield& A)
  /*!
    Assignment operator
    \param A :: targSimpleShield to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      height=A.height;
      depth=A.depth;
      width=A.width;
      backThick=A.backThick;
      forwThick=A.forwThick;
      leftThick=A.leftThick;
      rightThick=A.rightThick;
      baseThick=A.baseThick;
      topThick=A.topThick;
      mat=A.mat;
    }
  return *this;
}

targSimpleShield::~targSimpleShield() 
  /*!
    Destructor
  */
{}

void
targSimpleShield::populate(const FuncDataBase& Control)  
  /*!
    Populate all the variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("targSimpleShield","populate");

  FixedRotate::populate(Control);

  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  width=Control.EvalVar<double>(keyName+"Width");
  
  backThick=Control.EvalVar<double>(keyName+"BackThick");  
  forwThick=Control.EvalVar<double>(keyName+"ForwThick"); 
  leftThick=Control.EvalVar<double>(keyName+"LeftThick");    
  rightThick=Control.EvalVar<double>(keyName+"RightThick");
  baseThick=Control.EvalVar<double>(keyName+"BaseThick");
  topThick=Control.EvalVar<double>(keyName+"TopThick");
       
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");    
       
  return;
}

void
targSimpleShield::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("targSimpleShield","createSurface");

  // outer layer
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height/2.0,Z);
  
  // shield layer
  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-Y*(depth/2.0-backThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(depth/2.0-forwThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(width/2.0-leftThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(width/2.0-rightThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(height/2.0-baseThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(height/2.0-topThick),Z);  


  return;
}

void
targSimpleShield::createObjects(Simulation& System)
  /*!
    Create the basic object
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("targSimpleShield","createObjects");
  
  std::string Out;
  std::string Out1;

    // shield
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);
  addBoundarySurf(Out);
  Out1=ModelSupport::getComposite(SMap,buildIndex,
				  "(-11:12:-13:14:-15:16) ");  
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out+Out1));

    // hole
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  
  return;
}

void
targSimpleShield::createLinks()
  /*!
    Create links
   */
{
  ELog::RegMethod RegA("targSimpleShield","createLinks");

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  // shield layer
  FixedComp::setConnect(0,Origin-Y*(depth/2.0-backThick),-Y);
  FixedComp::setConnect(1,Origin+Y*(depth/2.0-forwThick),Y);
  FixedComp::setConnect(2,Origin-X*(width/2.0-leftThick),-X);
  FixedComp::setConnect(3,Origin+X*(width/2.0-rightThick),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0-baseThick),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0-topThick),Z);  

  return;
}

void
targSimpleShield::createAll(Simulation& System,
			    const attachSystem::FixedComp& FC,
			    const long int sideIndex)

  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("targSimpleShield","createAll");
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE shutterSystem
