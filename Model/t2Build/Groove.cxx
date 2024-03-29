/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t2Build/Groove.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "Groove.h"

namespace moderatorSystem
{

Groove::Groove(const std::string& Key)  :
  attachSystem::FixedRotate(Key,7),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


Groove::~Groove() 
  /*!
    Destructor
  */
{}

void
Groove::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("Groove","populate");

  FixedRotate::populate(Control);

  width=Control.EvalVar<double>(keyName+"MethWidth");
  height=Control.EvalVar<double>(keyName+"MethHeight");
  depth=Control.EvalVar<double>(keyName+"MethDepth");

  innerRadius=Control.EvalVar<double>(keyName+"Radius");
  innerXShift=Control.EvalVar<double>(keyName+"InnerXShift");
  innerZShift=Control.EvalVar<double>(keyName+"InnerZShift");
  innerWidth=Control.EvalVar<double>(keyName+"InnerWidth");
  innerHeight=Control.EvalVar<double>(keyName+"InnerHeight");
  innerCut=Control.EvalVar<double>(keyName+"InnerCut");
  innerSideAngleE1=Control.EvalVar<double>(keyName+"IXYAngleE1");
  innerSideAngleE5=Control.EvalVar<double>(keyName+"IXYAngleE5");

  alInnerCurve=Control.EvalVar<double>(keyName+"AlInnerCurve");
  alInnerSides=Control.EvalVar<double>(keyName+"AlInnerSides");
  alInnerUpDown=Control.EvalVar<double>(keyName+"AlInnerUpDown");
  alFront=Control.EvalVar<double>(keyName+"AlFront"); 
  alTop=Control.EvalVar<double>(keyName+"AlTop"); 
  alBase=Control.EvalVar<double>(keyName+"AlBase"); 
  alSide=Control.EvalVar<double>(keyName+"AlSide"); 

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  
  return;
}
  

void
Groove::createUnitVector(const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Points down the Groove direction
    - X Across the Groove
    - Z up (towards the target)
    \param FC :: FixedComp for origin/axis
    \param sideIndex :: linkPoint
  */
{
  ELog::RegMethod RegA("Groove","createUnitVector");

  FixedRotate::createUnitVector(FC,sideIndex);
  // Groove Centre:
  // -- Step in by cut, and  
  const double MStep=(depth-innerCut)-
    sqrt(innerRadius*innerRadius-innerWidth*innerWidth/4.0);
  GCentre=Origin+Y*MStep+X*innerXShift+Z*innerZShift;
  return;
}
  
void
Groove::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Groove","createSurface");

  // INNER DIVIDE PLANE
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  SurfMap::addSurf("DividePlane",SMap.realSurf(buildIndex+1));
  FixedComp::addLinkSurf(0,-SMap.realSurf(buildIndex+1));
  // Simple box planes

  // Inner Methane levels:
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*depth,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height/2.0,Z);


  // Inner groove part:
  ModelSupport::buildCylinder(SMap,buildIndex+11,GCentre,Z,innerRadius);
  ModelSupport::buildPlane(SMap,buildIndex+15,GCentre-Z*innerHeight/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,GCentre+Z*innerHeight/2.0,Z);

  // Calc intercep point at corner of curve + diagonal sides:
  const double yfStep(sqrt(innerRadius*innerRadius-
			   innerWidth*innerWidth/4.0));
  const Geometry::Vec3D IPtA(GCentre+Y*yfStep+X*innerWidth/2.0);
  const Geometry::Vec3D IPtB(GCentre+Y*yfStep-X*innerWidth/2.0);
  
  Geometry::Vec3D ADirc(X);   
  Geometry::Vec3D BDirc(X);
  Geometry::Quaternion::calcQRotDeg(-innerSideAngleE5,Z).rotate(ADirc);
  Geometry::Quaternion::calcQRotDeg(innerSideAngleE1,Z).rotate(BDirc);

  // E1 side [left]
  ModelSupport::buildPlane(SMap,buildIndex+13,IPtA,ADirc);
  ModelSupport::buildPlane(SMap,buildIndex+14,IPtB,BDirc);


  // Aluminium Coating: [cold layer]  
  // Outer Al Skin
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*(depth+alFront),Y);
  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(width/2.0+alSide),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(width/2.0+alSide),X);
  ModelSupport::buildPlane(SMap,buildIndex+25,Origin-Z*(height/2.0+alTop),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*(height/2.0+alTop),Z);


  // Inner groove part:
  ModelSupport::buildCylinder(SMap,buildIndex+31,
			      GCentre+Y*alInnerCurve,Z,innerRadius);
  ModelSupport::buildPlane(SMap,buildIndex+35,
			   GCentre-Z*(innerHeight/2.0-alInnerUpDown),Z);
  ModelSupport::buildPlane(SMap,buildIndex+36,
			   GCentre+Z*(innerHeight/2.0-alInnerUpDown),Z);

  // E1 side [left]
  ModelSupport::buildPlane(SMap,buildIndex+33,IPtA-ADirc*alInnerSides,ADirc);
  ModelSupport::buildPlane(SMap,buildIndex+34,IPtB+BDirc*alInnerSides,BDirc);

  return;
}

void
Groove::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Groove","createObjects");

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6 "
				 "(13 : -14 : -11 : -15 : 16)");
  System.addCell(cellIndex++,modMat,modTemp,HR);

  // void in groove
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -22 -33 34 31 35 -36");
  System.addCell(cellIndex++,0,0.0,HR);

  // Al layers :
  // - Outer skin
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -22 23 -24 25 -26 "
				 " (2 : -3 : 4 : -5 : 6 ) "
                                 " (33 : -34 : -35 : 36 )");
  System.addCell(cellIndex++,alMat,modTemp,HR);
  
  // - Inner skin
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -13 14 11 15 -16 "
				 "( 33 : -34 : -31 : -35 : 36 )");
  System.addCell(cellIndex++,alMat,modTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -22 23 -24 25 -26");
  addOuterSurf(HR);

  return;
}

void
Groove::createLinks()
  /*!
    Create links
  */
{
  ELog::RegMethod RegA("Groove","createLinks");

  // set Links:
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setConnect(1,Origin+Y*depth,Y);
  FixedComp::setConnect(2,Origin-X*(alSide+width/2.0),-X);
  FixedComp::setConnect(3,Origin+X*(alSide+width/2.0),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0+alBase),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0+alTop),Z);

  // Centre of groove
  FixedComp::setConnect(6,GCentre+Y*(alInnerCurve+innerRadius),Y);

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  int signVal(1);
  for(int i=1;i<6;i++)
    {
      FixedComp::setLinkSurf(static_cast<size_t>(i),
			     signVal*SMap.realSurf(buildIndex+i+21));
      signVal*=-1;
    }
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+31));
  return;
}

int
Groove::getDividePlane() const
  /*!
    Get the dividing plane
    \return Dividing plane [pointing out]
  */
{
  return SMap.realSurf(buildIndex+1);
}

int
Groove::viewSurf() const
  /*!
    View surface 
    \return view surface [pointing out]
   */
{
  return SMap.realSurf(buildIndex+11);
}
  
Geometry::Vec3D
Groove::getViewPoint() const
  /*!
    Return Centre point
    \return Mid point in CH4 groove
  */
{
  return GCentre+Y*innerRadius;
}

Geometry::Vec3D
Groove::getBackGroove() const
  /*!
    Base point of the groove
    \return Base point
  */
{
  return Origin+X*innerXShift+Z*innerZShift;
}


void
Groove::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp for origin/axisx
  */
{
  ELog::RegMethod RegA("Groove","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
