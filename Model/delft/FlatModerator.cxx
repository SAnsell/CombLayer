/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/FlatModerator.cxx
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
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "virtualMod.h"
#include "FlatModerator.h"

namespace delftSystem
{

FlatModerator::FlatModerator(const std::string& Key)  :
  virtualMod(Key),
  flatIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(flatIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

FlatModerator::FlatModerator(const FlatModerator& A) : 
  virtualMod(A),
  flatIndex(A.flatIndex),cellIndex(A.cellIndex),
  frontRadius(A.frontRadius),backRadius(A.backRadius),
  frontRoundRadius(A.frontRoundRadius),backRoundRadius(A.backRoundRadius),
  frontWallThick(A.frontWallThick),backWallThick(A.backWallThick),
  wingAngle(A.wingAngle),viewExtent(A.viewExtent),
  depth(A.depth),length(A.length),radius(A.radius),
  sideThick(A.sideThick),modTemp(A.modTemp),gasTemp(A.gasTemp),
  modMat(A.modMat),gasMat(A.gasMat),alMat(A.alMat),
  HCell(A.HCell)
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
      cellIndex=A.cellIndex;
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
      HCell=A.HCell;
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
  
  FixedOffset::populate(Control);

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
FlatModerator::createUnitVector(const attachSystem::FixedComp& CUnit,
				const long int sideIndex)
  /*!
    Create the unit vectors
    Origin is the back point of the moderator
    \param CUnit :: Fixed unit that it is connected to 
    \param sideIndex :: link point						
  */
{
  ELog::RegMethod RegA("FlatModerator","createUnitVector");
  FixedComp::createUnitVector(CUnit,sideIndex);
  applyOffset();
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
  ModelSupport::buildSphere(SMap,flatIndex+7,RCentre,backRadius+backWallThick);
  ModelSupport::buildSphere(SMap,flatIndex+17,RCentre,backRadius);

  // Outer on origin:
  ModelSupport::buildSphere(SMap,flatIndex+27,RCentre,frontRadius+frontWallThick);
  ModelSupport::buildSphere(SMap,flatIndex+37,RCentre,frontRadius);

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

      
      ModelSupport::buildCone(SMap,flatIndex+8,BConePt,Y,wingAngle);
      ModelSupport::buildCone(SMap,flatIndex+18,BConePt+Y*backWShift,Y,wingAngle);
      ModelSupport::buildCone(SMap,flatIndex+28,FConePt-Y*frontWShift,Y,wingAngle);
      ModelSupport::buildCone(SMap,flatIndex+38,FConePt,Y,wingAngle);

    }
  else  // cylinder join
    {
      ModelSupport::buildCylinder(SMap,flatIndex+8,Origin,Y,backView+backWallThick);
      ModelSupport::buildCylinder(SMap,flatIndex+18,Origin,Y,backView);
      ModelSupport::buildCylinder(SMap,flatIndex+28,Origin,Y,frontView+frontWallThick);
      ModelSupport::buildCylinder(SMap,flatIndex+38,Origin,Y,frontView);
    }

  // last planes
  ModelSupport::buildPlane(SMap,flatIndex+1,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,flatIndex+11,Origin+Y*(length+frontWallThick),Y);

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

  std::string Out;  

  Out=ModelSupport::getComposite(SMap,flatIndex,"-7 -8 -1");
  addOuterSurf(Out);

  Out=ModelSupport::getComposite(SMap,flatIndex," -7 -8 17 -1");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));
  
  Out=ModelSupport::getComposite(SMap,flatIndex," -17 -8 18 -1");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  Out=ModelSupport::getComposite(SMap,flatIndex," -17 -18 27 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));

  Out=ModelSupport::getComposite(SMap,flatIndex," -27 -18 28 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));


  Out=ModelSupport::getComposite(SMap,flatIndex," -27 -28 37 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modMat,Out));

  Out=ModelSupport::getComposite(SMap,flatIndex," -37 -28 38 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  Out=ModelSupport::getComposite(SMap,flatIndex," -37 -38 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  return;
  // build Simple case [cylinder]:
  

  Out=ModelSupport::getComposite(SMap,flatIndex," -37 -38 -1");
  System.addCell(MonteCarlo::Qhull(cellIndex++,gasMat,gasTemp,Out));

  // Cap :
  Out=ModelSupport::getComposite(SMap,flatIndex," 1 -11 -8");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

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
