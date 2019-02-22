/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/TwisterModule.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell/Konstantin Batkov
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "TwisterModule.h"

namespace essSystem
{

TwisterModule::TwisterModule(const std::string& Key) :
  attachSystem::ContainedGroup("PlugFrame","Shaft","ShaftBearing"),
  attachSystem::FixedOffset(Key,16),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
}

TwisterModule::TwisterModule(const TwisterModule& A) : 
  attachSystem::ContainedGroup(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),  
  shaftRadius(A.shaftRadius),
  shaftHeight(A.shaftHeight),shaftWallThick(A.shaftWallThick),
  shaftBearingRadius(A.shaftBearingRadius),
  shaftBearingHeight(A.shaftBearingHeight),
  shaftBearingWallThick(A.shaftBearingWallThick),
  plugFrameRadius(A.plugFrameRadius),plugFrameHeight(A.plugFrameHeight),
  plugFrameDepth(A.plugFrameDepth),plugFrameAngle(A.plugFrameAngle),
  plugFrameWallThick(A.plugFrameWallThick),
  plugFrameMat(A.plugFrameMat),plugFrameWallMat(A.plugFrameWallMat),
  shaftMat(A.shaftMat),shaftWallMat(A.shaftWallMat)
  /*!
    Copy constructor
    \param A :: TwisterModule to copy
  */
{}

TwisterModule&
TwisterModule::operator=(const TwisterModule& A)
  /*!
    Assignment operator
    \param A :: TwisterModule to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      CellMap::operator=(A);
      cellIndex=A.cellIndex;
      shaftRadius=A.shaftRadius;
      shaftHeight=A.shaftHeight;
      shaftWallThick=A.shaftWallThick;
      shaftBearingRadius=A.shaftBearingRadius;
      shaftBearingHeight=A.shaftBearingHeight;
      shaftBearingWallThick=A.shaftBearingWallThick;

      plugFrameRadius=A.plugFrameRadius;
      plugFrameHeight=A.plugFrameHeight;
      plugFrameDepth=A.plugFrameDepth;
      plugFrameAngle=A.plugFrameAngle;
      plugFrameWallThick=A.plugFrameWallThick;

      plugFrameMat=A.plugFrameMat;
      plugFrameWallMat=A.plugFrameWallMat;
      shaftMat=A.shaftMat;
      shaftWallMat=A.shaftWallMat;
    }
  return *this;
}
  
  TwisterModule*
  TwisterModule::clone() const
  /*!
    Clone self
    \return new (this)
   */
  {
    return new TwisterModule(*this);
  }
  

TwisterModule::~TwisterModule()
  /*!
    Destructor
   */
{}

void
TwisterModule::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("TwisterModule","populate");

  FixedOffset::populate(Control);
  
  shaftRadius=Control.EvalVar<double>(keyName+"ShaftRadius");   
  shaftHeight=Control.EvalVar<double>(keyName+"ShaftHeight");   
  shaftWallThick=Control.EvalVar<double>(keyName+"ShaftWallThick");

  shaftBearingRadius=Control.EvalVar<double>(keyName+"ShaftBearingRadius");
  shaftBearingHeight=Control.EvalVar<double>(keyName+"ShaftBearingHeight");
  shaftBearingWallThick=Control.EvalVar<double>(keyName+"ShaftBearingWallThick");

  plugFrameRadius=Control.EvalVar<double>(keyName+"PlugFrameRadius");   
  plugFrameHeight=Control.EvalVar<double>(keyName+"PlugFrameHeight");   
  plugFrameDepth=Control.EvalVar<double>(keyName+"PlugFrameDepth");
  plugFrameAngle=Control.EvalVar<double>(keyName+"PlugFrameAngle");
  plugFrameWallThick=Control.EvalVar<double>(keyName+"PlugFrameWallThick");

  plugFrameMat=ModelSupport::EvalMat<int>(Control,keyName+"PlugFrameMat");
  plugFrameWallMat=ModelSupport::EvalMat<int>(Control,keyName+"PlugFrameWallMat");

  shaftMat=ModelSupport::EvalMat<int>(Control,keyName+"ShaftMat");
  shaftWallMat=ModelSupport::EvalMat<int>(Control,keyName+"ShaftWallMat");

  return;
}

void
TwisterModule::createUnitVector(const attachSystem::FixedComp& FC,
                                const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: Link point [signed]
  */
{
  ELog::RegMethod RegA("TwisterModule","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  
  return;
}

void
TwisterModule::createSurfaces()
  /*!
    Create Surfaces for the Be
  */
{
  ELog::RegMethod RegA("TwisterModule","createSurfaces");

  // dividers
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin,X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*plugFrameDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*plugFrameHeight,Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+
                           Z*(plugFrameHeight+shaftHeight),Z);

  ModelSupport::buildPlane(SMap,buildIndex+25,Origin-
                           Z*(plugFrameDepth-plugFrameWallThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+
                           Z*(plugFrameHeight-plugFrameWallThick),Z);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,shaftRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,
                              shaftRadius+shaftWallThick);

  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Z,plugFrameRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+37,Origin,Z,
                              plugFrameRadius-plugFrameWallThick);

  // sector walls
  const double angle = plugFrameAngle/2.0*M_PI/180;
  double R = shaftRadius;

  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+1,
				  Origin-Y*R*sin(angle)+X*R*cos(angle),
				  X,-Z,plugFrameAngle/2.0);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+11,
				  Origin-Y*R*sin(angle)-X*R*cos(angle),
				  X,-Z,-plugFrameAngle/2.0);

  R += shaftWallThick;
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin-Y*(R*sin(angle)),Y); // auxiliary dividing plane
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+21,
				  Origin-Y*R*sin(angle)+X*R*cos(angle),
				  X,-Z,plugFrameAngle/2.0);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+31,
				  Origin-Y*R*sin(angle)-X*R*cos(angle),
				  X,-Z,-plugFrameAngle/2.0);

  // shaft bearing
  ModelSupport::buildPlane(SMap,buildIndex+35,Origin-Z*(plugFrameDepth+shaftBearingHeight),Z);
  ModelSupport::buildCylinder(SMap,buildIndex+47,Origin,Z,shaftBearingRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+57,Origin,Z,shaftBearingRadius+shaftBearingWallThick);
  

  return; 
}

void
TwisterModule::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("TwisterModule","createObjects");

  std::string Out;
  // shaft
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 25 -16 ");
  System.addCell(MonteCarlo::Object(cellIndex++,shaftMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex," -17 7 25 -16 ");
  System.addCell(MonteCarlo::Object(cellIndex++,shaftWallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex," -17 5 -16 ");
  addOuterSurf("Shaft", Out);

  // plug frame
  //  inside sector
  Out=ModelSupport::getComposite(SMap,buildIndex," -37 25 -26 17 2 -1 11");
  System.addCell(MonteCarlo::Object(cellIndex++,plugFrameMat,0.0,Out));

  //  inside sector wall x+
  Out=ModelSupport::getComposite(SMap,buildIndex," -37 25 -26 17 2 -21 1");
  System.addCell(MonteCarlo::Object(cellIndex++,plugFrameWallMat,0.0,Out));

  //  inside sector wall x-
  Out=ModelSupport::getComposite(SMap,buildIndex," -37 25 -26 17 2 31 -11");
  System.addCell(MonteCarlo::Object(cellIndex++,plugFrameWallMat,0.0,Out));

  // outer wall inside sector
  Out=ModelSupport::getComposite(SMap,buildIndex," -27 5 -6 (37:-25:26) 17 2 -21 31 ");
  System.addCell(MonteCarlo::Object(cellIndex++,plugFrameWallMat,0.0,Out));

  // bottom wall of the shaft
  Out=ModelSupport::getComposite(SMap,buildIndex," -17 47 5 -25 ");
  System.addCell(MonteCarlo::Object(cellIndex++,shaftWallMat,0.0,Out));

  // Next line is DNF of " 5 -6  ((-17 31 -21) : (-27 31 -21 2))"
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 5 -6 -17 31 -21 : 5 -6 -27 2 31 -21 ");
  addOuterSurf("PlugFrame", Out);

  // shaft bearing
  Out=ModelSupport::getComposite(SMap,buildIndex, " -47 35 -25 ");
  System.addCell(MonteCarlo::Object(cellIndex++,shaftMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex, " -57 47 35 -5 ");
  System.addCell(MonteCarlo::Object(cellIndex++,shaftWallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex, " -57 35 -5 ");
  addOuterSurf("ShaftBearing", Out);
    
  return; 
}

void
TwisterModule::createLinks()
  /*!
    Creates a full attachment set
    Links/directions going outwards true.
  */
{
  ELog::RegMethod RegA("TwisterModule","createLinks");

  // SHAFT
  FixedComp::setConnect(0,Origin+Y*shaftRadius,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+17));
  FixedComp::addLinkSurf(0,-SMap.realSurf(buildIndex+101));

  FixedComp::setConnect(1,Origin+Y*shaftRadius,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+17));
  FixedComp::addLinkSurf(1,SMap.realSurf(buildIndex+101));

  FixedComp::setConnect(2,Origin+X*shaftRadius,-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+17));
  FixedComp::addLinkSurf(2,-SMap.realSurf(buildIndex+103));
  
  FixedComp::setConnect(3,Origin+X*shaftRadius,X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+17));
  FixedComp::addLinkSurf(3,SMap.realSurf(buildIndex+103));
  
  FixedComp::setConnect(4,Origin+Z*(plugFrameHeight+shaftHeight),Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+16));

  // SHAFT BEARING
  FixedComp::setConnect(5,Origin+Y*shaftBearingRadius,-Y);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+57));
  FixedComp::addLinkSurf(5,-SMap.realSurf(buildIndex+101));

  FixedComp::setConnect(6,Origin+Y*shaftBearingRadius,Y);
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+57));
  FixedComp::addLinkSurf(6,SMap.realSurf(buildIndex+101));

  FixedComp::setConnect(7,Origin+X*shaftBearingRadius,-X);
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+57));
  FixedComp::addLinkSurf(7,-SMap.realSurf(buildIndex+103));
  
  FixedComp::setConnect(8,Origin+X*shaftBearingRadius,X);
  FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+57));
  FixedComp::addLinkSurf(8,SMap.realSurf(buildIndex+103));
  
  FixedComp::setConnect(9,Origin-Z*(plugFrameDepth+shaftBearingHeight),-Z);
  FixedComp::setLinkSurf(9,-SMap.realSurf(buildIndex+35));

  // PLUG FRAME
  FixedComp::setConnect(10,Origin+Y*plugFrameRadius,-Y);
  FixedComp::setLinkSurf(10,SMap.realSurf(buildIndex+27));
  FixedComp::addLinkSurf(10,-SMap.realSurf(buildIndex+101));

  FixedComp::setConnect(11,Origin+Y*plugFrameRadius,Y);
  FixedComp::setLinkSurf(11,SMap.realSurf(buildIndex+27));
  FixedComp::addLinkSurf(11,SMap.realSurf(buildIndex+101));

  FixedComp::setConnect(12,Origin+X*plugFrameRadius,-X);
  FixedComp::setLinkSurf(12,SMap.realSurf(buildIndex+27));
  FixedComp::addLinkSurf(12,-SMap.realSurf(buildIndex+103));
  
  FixedComp::setConnect(13,Origin+X*plugFrameRadius,X);
  FixedComp::setLinkSurf(13,SMap.realSurf(buildIndex+27));
  FixedComp::addLinkSurf(13,SMap.realSurf(buildIndex+103));
  
  FixedComp::setConnect(14,Origin-Z*plugFrameDepth,-Z);
  FixedComp::setLinkSurf(14,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(14,Origin+Z*plugFrameHeight,Z);
  FixedComp::setLinkSurf(14,SMap.realSurf(buildIndex+6));

  // plug frame side rule
  FixedComp::setConnect(15,Origin+Y*plugFrameRadius,Y);
  FixedComp::setLinkSurf(15,-SMap.realSurf(buildIndex+21));
  FixedComp::addLinkSurf(15,SMap.realSurf(buildIndex+31));
  FixedComp::addLinkSurf(15,-SMap.realSurf(buildIndex+27));

  return;
}


void
TwisterModule::createAll(Simulation& System,
                         const attachSystem::FixedComp& FC,
                         const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: Link point [signed]
  */
{
  ELog::RegMethod RegA("TwisterModule","createAll");
  populate(System.getDataBase());
  

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE essSystem
