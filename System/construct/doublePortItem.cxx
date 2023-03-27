/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/doublePortItem.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LineTrack.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"

#include "portItem.h"
#include "doublePortItem.h"

namespace constructSystem
{

doublePortItem::doublePortItem(const std::string& baseKey,
				 const std::string& Key) :
  portItem(baseKey,Key),
  lengthB(0.0),radiusB(0.0),wallB(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param baseKey :: Base name
    \param Key :: Primary name
  */
{}

doublePortItem::doublePortItem(const std::string& Key) :
  portItem(Key),
  lengthB(0.0),radiusB(0.0),wallB(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}
  
doublePortItem::~doublePortItem() 
  /*!
    Destructor
  */
{}
  
void
doublePortItem::populate(const FuncDataBase& Control)
  /*!
    Populate variables
    \param Control :: Variable DataBase
   */
{
  ELog::RegMethod RegA("doublePortItem","populate");

  portItem::populate(Control);
  lengthB=Control.EvalTail<double>(keyName,portBase,"ExtraLength");
  radiusB=Control.EvalTail<double>(keyName,portBase,"ExtraRadius");
  wallB=Control.EvalTail<double>(keyName,portBase,"ExtraWall");

  flangeRadius=Control.EvalTail<double>
    (keyName,portBase,"ExtraFlangeRadius");
  flangeLength=Control.EvalTail<double>
    (keyName,portBase,"ExtraFlangeLength");
  lengthA=length;
  length+=lengthB;

  return;
}


void
doublePortItem::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("doublePortItem","createSurfaces");

  portItem::createSurfaces();

  ModelSupport::buildCylinder(SMap,buildIndex+1007,Origin,Y,radiusB);
  ModelSupport::buildCylinder(SMap,buildIndex+1017,Origin,Y,radiusB+wallB);

  ModelSupport::buildPlane(SMap,buildIndex+1001,
			   Origin+Y*lengthA,Y);
  ModelSupport::buildPlane(SMap,buildIndex+1002,
			   Origin+Y*(lengthA+wall),Y);
    
  return;
}

void
doublePortItem::constructObject(Simulation& System,
				const HeadRule& innerSurf,
				const HeadRule& outerSurf)
/*!
    Construct a flange from the centre point
    \param System :: Simulation to use
    \param innerSurf :: Surface of main cell to cut (into the void typically)
    \param outerSurf :: wall Surface of main cell to cut 
*/
{
  if (radius>radiusB)
    constructObjectReducing(System,innerSurf,outerSurf);
  else
    constructObjectIncreasing(System,innerSurf,outerSurf);
  return;
}

  
void
doublePortItem::constructObjectReducing(Simulation& System,
					const HeadRule& innerSurf,
					const HeadRule& outerSurf)
  /*!
    Construct a flange from the centre point
    \param System :: Simulation to use
    \param innerSurf :: Surface of main cell to cut (into the void typically)
    \param outerSurf :: Surface of main cell to cut 
  */
{
  ELog::RegMethod RegA("doublePortItem","constructObjectReducing");

  const bool capFlag(capThick>Geometry::zeroTol);
  
  // construct inner volume:
  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -7 -1001");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*innerSurf);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1001 -1007 -2");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -17 7 -1001");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR*innerSurf);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1001 -17 1007 -1002");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1002 -2 1007 -1017");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);

  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 -27 1017 -2");
  makeCell("Flange",System,cellIndex++,wallMat,0.0,HR);
  
  if (outerFlag)
    {
      if (flangeRadius>=radius)
	{
	  HR=ModelSupport::getHeadRule
	    (SMap,buildIndex,"1017 -102 1002 -17");
	  makeCell("OutVoid",System,cellIndex++,outerVoidMat,0.0,HR);
	  
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 17 -27 -102");
	  makeCell("OutVoid",System,cellIndex++,outerVoidMat,0.0,HR*outerSurf);
	  
	  HR= (capFlag) ?
	    ModelSupport::getHeadRule(SMap,buildIndex,"-202 -27 1") :
	    ModelSupport::getHeadRule(SMap,buildIndex,"-2 -27  1");
	  
	  addOuterSurf(HR*outerSurf);
	}
      else 
	{
	  HR= (capFlag) ?
	    ModelSupport::getHeadRule(SMap,buildIndex,"-202 102 -17 27") :
	    ModelSupport::getHeadRule(SMap,buildIndex,"-2 -17 27 102");
	  makeCell("OutVoid",System,cellIndex++,outerVoidMat,0.0,HR);
	  
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1002 -17 1017 -102");
	  makeCell("OutVoid",System,cellIndex++,outerVoidMat,0.0,HR);
	  
	  HR= (capFlag) ?
	    ModelSupport::getHeadRule(SMap,buildIndex,"-202 -17 1") :
	    ModelSupport::getHeadRule(SMap,buildIndex,"-2 -17 1");
	  addOuterSurf(HR*outerSurf);	      
	}
    }
  else  // outerFlag  NOT set:
    {
      HR= (capFlag) ?
	ModelSupport::getHeadRule(SMap,buildIndex,"-202 -27 102") :
	ModelSupport::getHeadRule(SMap,buildIndex,"-2 -27 102");
      addOuterSurf(HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-102 -1017 1002");
      addOuterUnionSurf(HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -17 -1002");
      addOuterUnionSurf(HR*outerSurf);
    }
  return;
}


void
doublePortItem::constructObjectIncreasing(Simulation& System,
					  const HeadRule& innerSurf,
					  const HeadRule& outerSurf)
  /*!
    Construct a flange from the centre point
    \param System :: Simulation to use
    \param innerSurf :: inner Surface of main cell to cut
                   (into the void typically)
    \param outerSurf :: wall Surface of main cell to cut 
  */
{
  ELog::RegMethod RegA("doublePortItem","constructObjectIncreasing");

  const bool capFlag(capThick>Geometry::zeroTol);
  const bool windowFlag(windowThick>Geometry::zeroTol);
  
  // construct inner volume:
  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -7 -1002");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*innerSurf);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1002 -1007 -2");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -17 7 -1001");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR*innerSurf);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1001 7 -1017 -1002");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1002 -2 1007 -1017");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 -27 1017 -2");
  makeCell("Flange",System,cellIndex++,wallMat,0.0,HR);

  if (capFlag)
    {
      // we have window AND flange:
      if (windowFlag)
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-207 -211 2");
	  makeCell("BelowPlate",System,cellIndex++,voidMat,0.0,HR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-207 212 -202");
	  makeCell("AbovePlate",System,cellIndex++,outerVoidMat,0.0,HR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-207 211 -212");
	  makeCell("Plate",System,cellIndex++,windowMat,0.0,HR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-27 207 -202 2");
	  makeCell("PlateSurround",System,cellIndex++,capMat,0.0,HR);
	}
      else // just a cap
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-27 2 -202");
	  makeCell("Plate",System,cellIndex++,capMat,0.0,HR);
	}
    }  

  if (outerFlag)
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"1 17 -27 (-1001:1017) -102");
      makeCell("OutVoid",System,cellIndex++,outerVoidMat,0.0,HR*outerSurf);
      
      HR= (capFlag) ?
	ModelSupport::getHeadRule(SMap,buildIndex,"-202 -27 1") :
	ModelSupport::getHeadRule(SMap,buildIndex,"-2 -27  1");

      addOuterSurf(HR*outerSurf);
    }
  else
    {
      HR= (capFlag) ?
	ModelSupport::getHeadRule(SMap,buildIndex,"-202 -27 102") :
	ModelSupport::getHeadRule(SMap,buildIndex,"-2 -27 102");
      addOuterSurf(HR);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 -102 1");
      addOuterUnionSurf(HR*outerSurf);
    }
  return;
}

}  // NAMESPACE constructSystem
