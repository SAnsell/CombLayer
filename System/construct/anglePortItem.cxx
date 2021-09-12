/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/anglePortItem.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
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
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LineTrack.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "portItem.h"
#include "anglePortItem.h"

namespace constructSystem
{

anglePortItem::anglePortItem(const std::string& baseKey,
		   const std::string& Key) :
  portItem(baseKey,Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param baseKey :: Base name
    \param Key :: KeyName
  */
{}

anglePortItem::anglePortItem(const std::string& Key) :
  portItem(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

anglePortItem::anglePortItem(const anglePortItem& A) : 
  portItem(A),bAxis(A.bAxis),
  lengthA(A.lengthA),lengthB(A.lengthB)
  /*!
    Copy constructor
    \param A :: anglePortItem to copy
  */
{}

anglePortItem&
anglePortItem::operator=(const anglePortItem& A)
  /*!
    Assignment operator
    \param A :: anglePortItem to copy
    \return *this
  */
{
  if (this!=&A)
    {
      portItem::operator=(A);
      bAxis=A.bAxis;
      lengthA=A.lengthA;
      lengthB=A.lengthB;
    }
  return *this;
}

  
anglePortItem::~anglePortItem()
  /*!
    Destructor
  */
{}


void
anglePortItem::populate(const FuncDataBase& Control)
  /*!
    Populate variables
    \param Control :: Variable DataBase
   */
{
  ELog::RegMethod RegA("anglePortItem","populate");

  portItem::populate(Control);

  bAxis=Control.EvalTail<Geometry::Vec3D>(keyName,portBase,"BAxis");
  lengthA=Control.EvalTail<double>(keyName,portBase,"Length");
  lengthB=Control.EvalTail<double>(keyName,portBase,"LengthB");

  return;
}



void
anglePortItem::createSurfaces()
  /*!
    Create the surfaces
    This is stand alone from portItem because 
    otherwize we polute the space with flanges etc.
  */
{
  ELog::RegMethod RegA("anglePortItem","createSurfaces");

  // divider surface if needeed :
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  if (flangeRadius-Geometry::zeroTol<=radius+wall)
    throw ColErr::SizeError<double>(flangeRadius,wall+radius,
				    "Wall Radius<FlangeRadius");
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wall);

  // y-prime Axis
  const Geometry::Vec3D bY=bAxis.getInBasis(X,Y,Z);
  // bisecting vector
  const Geometry::Vec3D bsectY((bY+Y)/2.0);
  // end point
  const Geometry::Vec3D midPt(Origin+Y*externalLength);
  // bisecting plane
  ModelSupport::buildPlane(SMap,buildIndex+1000,midPt,bsectY);

  ModelSupport::buildCylinder(SMap,buildIndex+1007,midPt,bY,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+1017,midPt,bY,radius+wall);
  ModelSupport::buildCylinder(SMap,buildIndex+1027,midPt,bY,flangeRadius);

  
  // Final outer
  ModelSupport::buildPlane(SMap,buildIndex+1002,midPt+bY*lengthB,bY);

  ModelSupport::buildPlane(SMap,buildIndex+1102,
			   midPt+bY*(lengthB-flangeLength),bY);

  const bool capFlag(capThick>Geometry::zeroTol);
  const bool windowFlag (capFlag &&
			 windowThick>Geometry::zeroTol &&
			 windowThick+Geometry::zeroTol <capThick &&
			 windowRadius>Geometry::zeroTol &&
			 windowRadius+Geometry::zeroTol < flangeRadius);
  if (!windowFlag) windowThick=-1.0;
  // 
  // This builds a window cap if required:
  // 
  if (capFlag)
    {
      Geometry::Vec3D capPt(midPt+bY*(lengthB+capThick));
      ModelSupport::buildPlane(SMap,buildIndex+1202,capPt,bY);
      // if we have a cap we might have a window:
      if (windowFlag)
	{
	  capPt-= bY*(capThick/2.0);   // move to mid point
	  ModelSupport::buildPlane
	    (SMap,buildIndex+1211,capPt-bY*(windowThick/2.0),bY);
	  ModelSupport::buildPlane
	    (SMap,buildIndex+1212,capPt+bY*(windowThick/2.0),bY);
	  ModelSupport::buildCylinder
	    (SMap,buildIndex+1207,capPt,bY,windowRadius);
	}
    }
  /// ----  END : Cap/Window
 
  return;
}


void
anglePortItem::constructObject(Simulation& System,
			   const HeadRule& innerSurf,
			   const HeadRule& outerSurf)
/*!
    Construct a flange from the centre point
    \param System :: Simulation to use
    \param innerSurf :: Inner surface of void 
    \param outerSurf :: Outer surface of container
  */
{
  ELog::RegMethod RegA("portItem","constructObject");

  const bool capFlag(capThick>Geometry::zeroTol);
  const bool windowFlag(windowThick>Geometry::zeroTol);

  HeadRule HR;


  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -7 -1000");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*innerSurf);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 7 -17 -1000");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR*innerSurf);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1007 1000 -1002");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1007 -1017 1000 -1002");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR*innerSurf);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1102 -1027 1017 -1002 ");
  makeCell("Flange",System,cellIndex++,wallMat,0.0,HR);

  if (capFlag)
    {
      // we have window AND flange:
      if (windowFlag)
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1207 -1211  1002");
	  makeCell("BelowPlate",System,cellIndex++,voidMat,0.0,HR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1207 1212 -1202");
	  makeCell("AbovePlate",System,cellIndex++,outerVoidMat,0.0,HR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1207 1211 -1212");
	  makeCell("Plate",System,cellIndex++,windowMat,0.0,HR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1027 1207 -1202 1002");
	  makeCell("PlateSurround",System,cellIndex++,capMat,0.0,HR);
	}
      else // just a cap
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex," -1027 1002 -1202 ");
	  makeCell("Plate",System,cellIndex++,capMat,0.0,HR);
	}
    }

  // first section:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -17 -1000");
  addOuterSurf(HR*outerSurf);

  if (outerFlag)
    {      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1000 1017  -1027 -1102");
      makeCell("OutVoid",System,cellIndex++,outerVoidMat,0.0,HR);

      HR= (!capFlag) ?
	ModelSupport::getHeadRule(SMap,buildIndex,"1000 -1002 -1027") :
	ModelSupport::getHeadRule(SMap,buildIndex,"1000 -1202 -1027");
      addOuterUnionSurf(HR);
    }
  else
    {
      // first section:
      // second section:
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1017 1000 -1102");
      addOuterUnionSurf(HR);
      ELog::EM<<"Cap flag = "<<capFlag<<ELog::endDiag;
      HR= (!capFlag) ?
	ModelSupport::getHeadRule(SMap,buildIndex,"1102 -1002 -1027") :
	ModelSupport::getHeadRule(SMap,buildIndex,"1102 -1202 -1027");
      addOuterUnionSurf(HR);
    }

  return;
}
  
void
anglePortItem::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
    \param LT :: Line track
    \param AIndex :: start of high density material
    \param BIndex :: end of high density material
  */
{
  ELog::RegMethod RegA("anglePortItem","createLinks");

  FixedComp::nameSideIndex(0,"BasePoint");
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::nameSideIndex(1,"OuterPlate");
  
  if (capThick>Geometry::zeroTol)
    {
      FixedComp::setConnect(1,Origin+Y*(externalLength+capThick),Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+202));
    }
  else
    {
      FixedComp::setConnect(1,Origin+Y*externalLength,Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
    }

  FixedComp::nameSideIndex(2,"InnerRadius");
  FixedComp::setConnect(2,Origin+Y*(externalLength/2.0)+X*radius,X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+7));
  FixedComp::setBridgeSurf(2,SMap.realSurf(buildIndex+1));

  FixedComp::nameSideIndex(3,"OuterRadius");
  FixedComp::setConnect(3,Origin+Y*(externalLength/2.0)+X*(wall+radius),X);
  FixedComp::setLinkSurf(3,-SMap.realSurf(buildIndex+17));
  FixedComp::setBridgeSurf(3,SMap.realSurf(buildIndex+1));

  FixedComp::nameSideIndex(4,"InnerPlate");
  FixedComp::setConnect(4,Origin+Y*externalLength,-Y);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+2));

  FixedComp::nameSideIndex(5,"VoidRadius");
  FixedComp::setConnect(5,Origin+Y*externalLength,-Y);
  FixedComp::setLinkSurf(5,-SMap.realSurf(buildIndex+27));
  FixedComp::setBridgeSurf(5,SMap.realSurf(buildIndex+1));

  FixedComp::nameSideIndex(6,"FlangePlate");
  const Geometry::Vec3D flangePoint=
    Origin+Y*(externalLength-flangeLength);
  
  FixedComp::setConnect(6,flangePoint,Y);
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+102));

  return;
}


void
anglePortItem::addFlangeCut(MonteCarlo::Object* obj) const
  /*!
    Adds the port exclude to the main tube
    \param mainTube :: object for main tube
  */
{
  ELog::RegMethod RegA("anglePortItem","addFlangeCut");
  if (!outerFlag)
    {
      const HeadRule HR=
	ModelSupport::getHeadRule(SMap,buildIndex," ( 1027 : -1102 )");
      obj->addIntersection(HR);
    }
  return;
}



void
anglePortItem::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Build the system assuming outer cells have been added
    \param System :: Simulation to use
    \param FC :: Fixed comp
    \param sideIndex :: Link point
   */
{
  ELog::RegMethod RegA("anglePortItem","createAll");

  constructAxis(System,FC,sideIndex);
  
  return;
}



}  // NAMESPACE constructSystem
