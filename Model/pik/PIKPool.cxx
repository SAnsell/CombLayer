/*********************************************************************
  CombLayer : MCNP(X) Input builder

  * File:   Model/pik/PIKPool.cxx
  *
  * Copyright (c) 2004-2020 by Konstantin Batkov
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"


#include "PIKPool.h"

namespace pikSystem
{

  PIKPool::PIKPool(const std::string& Key)  :
    attachSystem::ContainedComp(),
    attachSystem::FixedRotate(Key,6),
    attachSystem::CellMap()
    /*!
      Constructor BUT ALL variable are left unpopulated.
      \param Key :: Name for item in search
    */
  {}

  PIKPool::PIKPool(const PIKPool& A) :
    attachSystem::ContainedComp(A),
    attachSystem::FixedRotate(A),
    attachSystem::CellMap(A),
    depth(A.depth),
    height(A.height),
    innerShieldRadius(A.innerShieldRadius),
    innerShieldWallThick(A.innerShieldWallThick),
    outerShieldRadius(A.outerShieldRadius),
    outerShieldWidth(A.outerShieldWidth),
    innerShieldMat(A.innerShieldMat),
    innerShieldWallMat(A.innerShieldWallMat),
    outerShieldMat(A.outerShieldMat),
    tankDepth(A.tankDepth),
    tankHeight(A.tankHeight),
    tankNLayers(A.tankNLayers),
    tankRadius(A.tankRadius),
    tankThick(A.tankThick),
    tankMat(A.tankMat)
    /*!
      Copy constructor
      \param A :: PIKPool to copy
    */
  {}

  PIKPool&
  PIKPool::operator=(const PIKPool& A)
  /*!
    Assignment operator
    \param A :: PIKPool to copy
    \return *this
  */
  {
    if (this!=&A)
      {
	attachSystem::ContainedComp::operator=(A);
	attachSystem::FixedRotate::operator=(A);
	attachSystem::CellMap::operator=(A);
	depth=A.depth;
	height=A.height;
	innerShieldRadius=A.innerShieldRadius;
        innerShieldWallThick=A.innerShieldWallThick;
	outerShieldRadius=A.outerShieldRadius;
	outerShieldWidth=A.outerShieldWidth;
	innerShieldMat=A.innerShieldMat;
        innerShieldWallMat=A.innerShieldWallMat;
	outerShieldMat=A.outerShieldMat;
	tankDepth=A.tankDepth;
        tankHeight=A.tankHeight;
        tankNLayers=A.tankNLayers;
        tankRadius=A.tankRadius;
        tankThick=A.tankThick;
        tankMat=A.tankMat;
      }
    return *this;
  }

  PIKPool*
  PIKPool::clone() const
  /*!
    Clone self
    \return new (this)
  */
  {
    return new PIKPool(*this);
  }

  PIKPool::~PIKPool()
  /*!
    Destructor
  */
  {}

  void
  PIKPool::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
  {
    ELog::RegMethod RegA("PIKPool","populate");

    FixedRotate::populate(Control);

    depth=Control.EvalVar<double>(keyName+"Depth");
    height=Control.EvalVar<double>(keyName+"Height");
    innerShieldRadius=Control.EvalVar<double>(keyName+"InnerShieldRadius");
    innerShieldWallThick=Control.EvalVar<double>(keyName+"InnerShieldWallThick");
    outerShieldRadius=Control.EvalVar<double>(keyName+"OuterShieldRadius");
    outerShieldWidth=Control.EvalVar<double>(keyName+"OuterShieldWidth");

    innerShieldMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerShieldMat");
    innerShieldWallMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerShieldWallMat");
    outerShieldMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterShieldMat");
    tankDepth=Control.EvalVar<double>(keyName+"TankDepth");
    tankHeight=Control.EvalVar<double>(keyName+"TankHeight");
    tankNLayers=Control.EvalVar<size_t>(keyName+"TankNLayers");
    for (size_t i=0; i<tankNLayers; ++i)
      {
	tankRadius.push_back(Control.EvalVar<double>(keyName+"TankRadius" + std::to_string(i)));
	tankThick.push_back(Control.EvalVar<double>(keyName+"TankThick" + std::to_string(i)));
      }
    tankMat=ModelSupport::EvalMat<int>(Control,keyName+"TankMat");

    return;
  }

  void
  PIKPool::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
  {
    ELog::RegMethod RegA("PIKPool","createUnitVector");

    FixedComp::createUnitVector(FC,sideIndex);
    applyOffset();

    return;
  }

  void
  PIKPool::createSurfaces()
  /*!
    Create All the surfaces
  */
  {
    ELog::RegMethod RegA("PIKPool","createSurfaces");

    ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,innerShieldRadius);
    ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,innerShieldRadius+innerShieldWallThick);
    ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Z,outerShieldRadius);

    ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(outerShieldWidth/2.0),X);
    ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(outerShieldWidth/2.0),X);

    ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(depth),Z);
    ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height),Z);

    // iron-water protection tank
    ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(tankDepth),Z);
    ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(tankHeight),Z);

    int SI(buildIndex+100);
    for (size_t i=0; i<tankNLayers; ++i)
      {
	ModelSupport::buildCylinder(SMap,SI+7,Origin,Z,tankRadius[i]);
	ModelSupport::buildCylinder(SMap,SI+17,Origin,Z,tankRadius[i]+tankThick[i]);
	SI += 20;
      }

    return;
  }

  void
  PIKPool::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
  {
    ELog::RegMethod RegA("PIKPool","createObjects");

    std::string Out;

    Out=ModelSupport::getComposite(SMap,buildIndex," -7 5 -105 ");
    makeCell("InnerShieldBottom",System,cellIndex++,innerShieldMat,0.0,Out);

    Out=ModelSupport::getComposite(SMap,buildIndex," -7 106 -6 ");
    makeCell("InnerShieldTop",System,cellIndex++,innerShieldMat,0.0,Out);

    Out=ModelSupport::getComposite(SMap,buildIndex," -17 7 5 -6 ");
    makeCell("InnerShieldWall",System,cellIndex++,innerShieldWallMat,0.0,Out);

    Out=ModelSupport::getComposite(SMap,buildIndex," -27 17 5 -6 3 -4 ");
    makeCell("OuterShield",System,cellIndex++,outerShieldMat,0.0,Out);

    Out=ModelSupport::getComposite(SMap,buildIndex," -27 5 -6 -3 ");
    makeCell("OuterShieldVoidLeft",System,cellIndex++,0,0.0,Out);

    Out=ModelSupport::getComposite(SMap,buildIndex," -27 5 -6 4 ");
    makeCell("OuterShieldVoidRight",System,cellIndex++,0,0.0,Out);

    // iron-water protection tank
    // Out=ModelSupport::getComposite(SMap,buildIndex," -7 105 -106 ");
    // makeCell("InnerShieldTank",System,cellIndex++,innerShieldMat,0.0,Out);
    const std::string tb=ModelSupport::getComposite(SMap,buildIndex," 105 -106 ");

    int SI(buildIndex+100);
    Out=ModelSupport::getComposite(SMap,SI," -7 ");
    makeCell("InnerShieldCentral",System,cellIndex++,innerShieldMat,0.0,Out+tb);

    for (size_t i=0; i<tankNLayers; ++i)
      {
    	Out=ModelSupport::getComposite(SMap,SI," 7 -17 ");
    	makeCell("InnerShieldTank"+std::to_string(i),
		 System,cellIndex++,tankMat,0.0,Out+tb);

	if (i!=tankNLayers-1) {
	  Out=ModelSupport::getComposite(SMap,SI," 17 -27 ");
	  makeCell("InnerShield"+std::to_string(i),
		   System,cellIndex++,innerShieldMat,0.0,Out+tb);
	  SI += 20;
	}
      }

    Out=ModelSupport::getComposite(SMap,buildIndex,SI," -7 17M ");
    makeCell("InnerShieldOuter",System,cellIndex++,innerShieldMat,0.0,Out+tb);

    Out=ModelSupport::getComposite(SMap,buildIndex," -27 5 -6 ");
    addOuterSurf(Out);

    return;
  }


  void
  PIKPool::createLinks()
  /*!
    Create all the linkes
  */
  {
    ELog::RegMethod RegA("PIKPool","createLinks");

    // inner links
    int SI(buildIndex+100);
    FixedComp::setConnect(0,Origin-Y*(tankRadius[0]),Y);
    FixedComp::setLinkSurf(0,-SMap.realSurf(SI+7));

    FixedComp::setConnect(1,Origin-Z*(tankDepth),Z);
    FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+105));

    FixedComp::setConnect(2,Origin+Z*(tankHeight),-Z);
    FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+106));
    return;
  }

  void
  PIKPool::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
  {
    ELog::RegMethod RegA("PIKPool","createAll");

    populate(System.getDataBase());
    createUnitVector(FC,sideIndex);
    createSurfaces();
    createObjects(System);
    createLinks();
    insertObjects(System);

    return;
  }

}  // pikSystem
