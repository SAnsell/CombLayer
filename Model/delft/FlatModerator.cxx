/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/FlatModerator.cxx
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "virtualMod.h"
#include "FlatModerator.h"

namespace delftSystem
{

FlatModerator::FlatModerator(const std::string& Key)  :
  virtualMod(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

FlatModerator::FlatModerator(const FlatModerator& A) : 
  virtualMod(A),
  frontRadius(A.frontRadius),backRadius(A.backRadius),
  frontRoundRadius(A.frontRoundRadius),backRoundRadius(A.backRoundRadius),
  frontWallThick(A.frontWallThick),backWallThick(A.backWallThick),
  wingAngle(A.wingAngle),viewExtent(A.viewExtent),
  depth(A.depth),length(A.length),radius(A.radius),
  sideThick(A.sideThick),modTemp(A.modTemp),gasTemp(A.gasTemp),
  modMat(A.modMat),gasMat(A.gasMat),alMat(A.alMat)
  /*!
    Copy constructor
    \param A :: FlatModerator to copy
  */
{}

FlatModerator&
FlatModerator::operator=(const FlatModerator& A)
  /*!
    Assignment operator
    \param A :: FlatModerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      virtualMod::operator=(A);
      frontRadius=A.frontRadius;
      backRadius=A.backRadius;
      frontRoundRadius=A.frontRoundRadius;
      backRoundRadius=A.backRoundRadius;
      frontWallThick=A.frontWallThick;
      backWallThick=A.backWallThick;
      wingAngle=A.wingAngle;
      viewExtent=A.viewExtent;
      depth=A.depth;
      length=A.length;
      radius=A.radius;
      sideThick=A.sideThick;
      modTemp=A.modTemp;
      gasTemp=A.gasTemp;
      modMat=A.modMat;
      gasMat=A.gasMat;
      alMat=A.alMat;
    }
  return *this;
}


FlatModerator*
FlatModerator::clone() const
  /*!
    Clone copy constructor
    \return new this
  */
{
  return new FlatModerator(*this); 
}


FlatModerator::~FlatModerator() 
  /*!
    Destructor
  */
{}

void
FlatModerator::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase 
  */
{
  ELog::RegMethod RegA("FlatModerator","populate");
  
  FixedRotate::populate(Control);

  depth=Control.EvalVar<double>(keyName+"FocusDepth");
  frontRadius=Control.EvalVar<double>(keyName+"FrontRadius");
  backRadius=Control.EvalVar<double>(keyName+"BackRadius");
  frontWallThick=Control.EvalVar<double>(keyName+"FrontWallThick");
  backWallThick=Control.EvalVar<double>(keyName+"BackWallThick");

  frontRoundRadius=Control.EvalVar<double>(keyName+"FrontRoundRadius");
  backRoundRadius=Control.EvalVar<double>(keyName+"BackRoundRadius");
  
  wingAngle=Control.EvalVar<double>(keyName+"WingAngle");
  viewExtent=Control.EvalVar<double>(keyName+"ViewExtent");
  
  length=Control.EvalVar<double>(keyName+"Length");

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  gasTemp=Control.EvalDefVar<double>(keyName+"GasTemp",modTemp);

  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  gasMat=ModelSupport::EvalMat<int>(Control,keyName+"GasMat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");

  return;
}

  
void
FlatModerator::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("FlatModerator","createSurfaces");

  // Inner on origin:
  const Geometry::Vec3D RCentre(Origin+Y*depth);
  ModelSupport::buildSphere(SMap,buildIndex+7,RCentre,backRadius+backWallThick);
  ModelSupport::buildSphere(SMap,buildIndex+17,RCentre,backRadius);

  // Outer on origin:
  ModelSupport::buildSphere(SMap,buildIndex+27,RCentre,frontRadius+frontWallThick);
  ModelSupport::buildSphere(SMap,buildIndex+37,RCentre,frontRadius);

  // Cone Wings:
  // calculate the distance from surface of sphere  to dividing plane

  // vertical distance at cone impact
  const double frontView(viewExtent/2.0+frontRoundRadius);
  const double backView(viewExtent/2.0+backRoundRadius);

  // dividing plane
  if (wingAngle>0.1)
    {
      const double TA(tan(M_PI*wingAngle/180.0));
      const double SA(sin(M_PI*wingAngle/180.0));

      // step from circle centre
      const double frontStep=
	sqrt(frontRadius*frontRadius-frontView*frontView)+frontView/TA;
      const double backStep=
	sqrt(backRadius*backRadius-backView*backView)+backView/TA;
      
      const Geometry::Vec3D FConePt(RCentre-Y*frontStep);
      const Geometry::Vec3D BConePt(RCentre-Y*backStep);

      // wall shifts
      const double frontWShift(frontWallThick/SA);
      const double backWShift(backWallThick/SA);

      
      ModelSupport::buildCone(SMap,buildIndex+8,BConePt,Y,wingAngle);
      ModelSupport::buildCone(SMap,buildIndex+18,BConePt+Y*backWShift,Y,wingAngle);
      ModelSupport::buildCone(SMap,buildIndex+28,FConePt-Y*frontWShift,Y,wingAngle);
      ModelSupport::buildCone(SMap,buildIndex+38,FConePt,Y,wingAngle);

    }
  else  // cylinder join
    {
      ModelSupport::buildCylinder(SMap,buildIndex+8,Origin,Y,backView+backWallThick);
      ModelSupport::buildCylinder(SMap,buildIndex+18,Origin,Y,backView);
      ModelSupport::buildCylinder(SMap,buildIndex+28,Origin,Y,frontView+frontWallThick);
      ModelSupport::buildCylinder(SMap,buildIndex+38,Origin,Y,frontView);
    }

  // last planes
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*(length+frontWallThick),Y);

  return;
}

void
FlatModerator::createLinks()
  /*!
    Create links
  */
{
  ELog::RegMethod RegA("FlatModerator","createLinks");


  return;
}
  
void
FlatModerator::createObjects(Simulation& System)
  /*!
    Build the main components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("FlatModerator","createObjects");

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 -8 17 -1");
  System.addCell(cellIndex++,alMat,modTemp,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 -8 18 -1");
  System.addCell(cellIndex++,alMat,modTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 -18 27 -1");
  System.addCell(cellIndex++,modMat,modTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-27 -18 28 -1");
  System.addCell(cellIndex++,modMat,modTemp,HR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-27 -28 37 -1");
  System.addCell(cellIndex++,alMat,modMat,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-37 -28 38 -1");
  System.addCell(cellIndex++,alMat,modTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-37 -38 -1");
  System.addCell(cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 -8 -1");
  addOuterSurf(HR);
  
  return;
  // build Simple case [cylinder]:
  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-37 -38 -1");
  System.addCell(cellIndex++,gasMat,gasTemp,HR);

  // Cap :
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -11 -8");
  System.addCell(cellIndex++,alMat,modTemp,HR);
  
  return;
}

void
FlatModerator::postCreateWork(Simulation&)
  /*!
    Add pipework
   */
{
  return;
}
  
void
FlatModerator::createAll(Simulation& System,
			 const attachSystem::FixedComp& FUnit,
			 const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("FlatModerator","createAll");
  populate(System.getDataBase());

  createUnitVector(FUnit,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
