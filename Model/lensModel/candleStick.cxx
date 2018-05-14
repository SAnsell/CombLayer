/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   lensModel/candleStick.cxx
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
#include "surfExpand.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "siModerator.h"
#include "candleStick.h"

namespace lensSystem
{

candleStick::candleStick(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,2),
  surIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surIndex+1),populated(0)
  /*!
    Constructor
  */
{}

candleStick::candleStick(const candleStick& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  surIndex(A.surIndex),cellIndex(A.cellIndex),
  populated(A.populated),TopBoundary(A.TopBoundary),BaseCent(A.BaseCent),
  StickCent(A.StickCent),CylCent(A.CylCent),baseThick(A.baseThick),
  baseWidth(A.baseWidth),baseLength(A.baseLength),baseXoffset(A.baseXoffset),
  baseYoffset(A.baseYoffset),vsWidth(A.vsWidth),vsDepth(A.vsDepth),
  vsHeight(A.vsHeight),vsXoffset(A.vsXoffset),vsYoffset(A.vsYoffset),
  flatWidth(A.flatWidth),flatLength(A.flatLength),flatThick(A.flatThick),
  vacLowMX(A.vacLowMX),vacLowPX(A.vacLowPX),vacLowMY(A.vacLowMY),
  vacLowPY(A.vacLowPY),vacBase(A.vacBase),vacTop(A.vacTop),
  vacZExt(A.vacZExt),vacRadius(A.vacRadius),xCyl(A.xCyl),
  yCyl(A.yCyl),alTopThick(A.alTopThick),alSideThick(A.alSideThick),
  alBaseThick(A.alBaseThick),skinForwardClear(A.skinForwardClear),
  skinBaseClear(A.skinBaseClear),skinBackClear(A.skinBackClear),
  skinPSideClear(A.skinPSideClear),skinMSideClear(A.skinMSideClear),
  skinCutBaseClear(A.skinCutBaseClear),skinCircleClear(A.skinCircleClear),
  skinCutSideClear(A.skinCutSideClear),waterAlThick(A.waterAlThick),
  waterHeight(A.waterHeight),supportMat(A.supportMat),alMat(A.alMat),
  outerDepth(A.outerDepth)
  /*!
    Copy constructor
    \param A :: candleStick to copy
  */
{}

candleStick&
candleStick::operator=(const candleStick& A)
  /*!
    Assignment operator
    \param A :: candleStick to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      TopBoundary=A.TopBoundary;
      BaseCent=A.BaseCent;
      StickCent=A.StickCent;
      CylCent=A.CylCent;
      baseThick=A.baseThick;
      baseWidth=A.baseWidth;
      baseLength=A.baseLength;
      baseXoffset=A.baseXoffset;
      baseYoffset=A.baseYoffset;
      vsWidth=A.vsWidth;
      vsDepth=A.vsDepth;
      vsHeight=A.vsHeight;
      vsXoffset=A.vsXoffset;
      vsYoffset=A.vsYoffset;
      flatWidth=A.flatWidth;
      flatLength=A.flatLength;
      flatThick=A.flatThick;
      vacLowMX=A.vacLowMX;
      vacLowPX=A.vacLowPX;
      vacLowMY=A.vacLowMY;
      vacLowPY=A.vacLowPY;
      vacBase=A.vacBase;
      vacTop=A.vacTop;
      vacZExt=A.vacZExt;
      vacRadius=A.vacRadius;
      xCyl=A.xCyl;
      yCyl=A.yCyl;
      alTopThick=A.alTopThick;
      alSideThick=A.alSideThick;
      alBaseThick=A.alBaseThick;
      skinForwardClear=A.skinForwardClear;
      skinBaseClear=A.skinBaseClear;
      skinBackClear=A.skinBackClear;
      skinPSideClear=A.skinPSideClear;
      skinMSideClear=A.skinMSideClear;
      skinCutBaseClear=A.skinCutBaseClear;
      skinCircleClear=A.skinCircleClear;
      skinCutSideClear=A.skinCutSideClear;
      waterAlThick=A.waterAlThick;
      waterHeight=A.waterHeight;
      supportMat=A.supportMat;
      alMat=A.alMat;
      outerDepth=A.outerDepth;
    }
  return *this;
}

void
candleStick::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y towards the normal of the target
    - X across the moderator
    - Z up / down (gravity)
    \param FC :: Fixed comp of the siModerator
  */
{
  ELog::RegMethod RegA("candleStick","createUnitVector");

  FixedComp::createUnitVector(FC);  

  // Centre Base centre to be half way into bar:
  BaseCent=FC.getLinkPt(5)+X*baseXoffset+Y*baseYoffset-
    Z*baseThick/2.0;

  // Set stick vector
  StickCent=X*vsXoffset+Y*vsYoffset+
    Z*(BaseCent.Z()+baseThick/2.0);

  CylCent=FC.getLinkPt(5)+Geometry::Vec3D(-xCyl,yCyl,0);

  return;
}

void
candleStick::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table
  */
{
  ELog::RegMethod RegA("candleStick","populate");

  baseThick=Control.EvalVar<double>(keyName+"BaseThick");
  baseWidth=Control.EvalVar<double>(keyName+"BaseWidth");
  baseLength=Control.EvalVar<double>(keyName+"BaseLength");
  baseXoffset=Control.EvalVar<double>(keyName+"BaseXoffset");
  baseYoffset=Control.EvalVar<double>(keyName+"BaseYoffset");

  vsHeight=Control.EvalVar<double>(keyName+"VSHeight");
  vsWidth=SimProcess::getDefVar<double>(Control,keyName+"VSWidth",baseWidth);
  vsDepth=Control.EvalVar<double>(keyName+"VSDepth");
  
  //
  // First we have to calculate the vsXYoffsets if no value found
  //  -- The stick is 

  vsXoffset=SimProcess::getDefVar<double>(Control,keyName+"VSXoffset",
					  baseXoffset+(vsDepth-baseLength)/2.0);
  vsYoffset=
    SimProcess::getDefVar<double>(Control,keyName+"VSYoffset",baseYoffset);

  //
  // Flat section:
  //
  flatWidth= SimProcess::getDefVar<double>
    (Control,keyName+"FlatWidth",vsWidth);
  flatLength=Control.EvalVar<double>(keyName+"FlatLength");
  flatThick= SimProcess::getDefVar<double>
    (Control,keyName+"FlatThick",vsDepth);
  
  vacLowMX=Control.EvalVar<double>(keyName+"VacLowMX");
  vacLowPX=Control.EvalVar<double>(keyName+"VacLowPX");
  vacLowMY=Control.EvalVar<double>(keyName+"VacLowMY");
  vacLowPY=Control.EvalVar<double>(keyName+"VacLowPY");
  vacBase=Control.EvalVar<double>(keyName+"VacBase");
  vacTop=Control.EvalVar<double>(keyName+"VacTop");
  vacRadius=Control.EvalVar<double>(keyName+"VacRadius");
  vacZExt=Control.EvalVar<double>(keyName+"VacZExt");
  xCyl=Control.EvalVar<double>(keyName+"VacCylX");
  yCyl=Control.EvalVar<double>(keyName+"VacCylY");
     
  alTopThick=Control.EvalVar<double>(keyName+"AlTopThick");
  alSideThick=Control.EvalVar<double>(keyName+"AlSideThick");
  alBaseThick=Control.EvalVar<double>(keyName+"AlBaseThick");

  skinForwardClear=Control.EvalVar<double>(keyName+"SkinForwardClear");
  skinBaseClear=Control.EvalVar<double>(keyName+"SkinBaseClear");
  skinBackClear=Control.EvalVar<double>(keyName+"SkinBackClear");
  skinPSideClear=Control.EvalVar<double>(keyName+"SkinPSideClear");
  skinMSideClear=Control.EvalVar<double>(keyName+"SkinMSideClear");
  skinCutBaseClear=Control.EvalVar<double>(keyName+"SkinCutBaseClear");
  skinCircleClear=Control.EvalVar<double>(keyName+"SkinCircleClear");
  skinCutSideClear=Control.EvalVar<double>(keyName+"SkinCutSideClear");

  waterHeight=Control.EvalVar<double>(keyName+"WaterHeight");
  waterAlThick=Control.EvalVar<double>(keyName+"WaterAlThick");
  
  supportMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  populated=1;
  return;
}

void 
candleStick::createSurfaces()
  /*!
    Construct all the surfaces
  */
{
  ELog::RegMethod RegA("candleStick","constructSurfaces");

  //Base:
  ModelSupport::buildPlane(SMap,surIndex+1,BaseCent-X*(baseLength/2.0),X);
  ModelSupport::buildPlane(SMap,surIndex+2,BaseCent+X*(baseLength/2.0),X);
  ModelSupport::buildPlane(SMap,surIndex+3,BaseCent-Y*(baseWidth/2.0),Y);
  ModelSupport::buildPlane(SMap,surIndex+4,BaseCent+Y*(baseWidth/2.0),Y);
  ModelSupport::buildPlane(SMap,surIndex+5,BaseCent-Z*(baseThick/2.0),Z);
  ModelSupport::buildPlane(SMap,surIndex+6,BaseCent+Z*(baseThick/2.0),Z);

  // Support
  ModelSupport::buildPlane(SMap,surIndex+11,StickCent-X*(vsDepth/2.0),X);
  ModelSupport::buildPlane(SMap,surIndex+12,StickCent+X*(vsDepth/2.0),X);
  ModelSupport::buildPlane(SMap,surIndex+13,StickCent-Y*(vsWidth/2.0),Y);
  ModelSupport::buildPlane(SMap,surIndex+14,StickCent+Y*(vsWidth/2.0),Y);
  ModelSupport::buildPlane(SMap,surIndex+16,StickCent+Z*vsHeight,Z);
  ModelSupport::buildPlane(SMap,surIndex+15,StickCent,Z);

  // create same surf as Support
  //  SMap.addMatch(surIndex+21,surIndex+11);
  ModelSupport::buildPlane(SMap,surIndex+21,StickCent-X*(vsDepth/2.0),X);
  ModelSupport::buildPlane(SMap,surIndex+22,
			   StickCent-X*(flatLength-vsDepth/2.0),X);
  ModelSupport::buildPlane(SMap,surIndex+23,
			   StickCent-Y*(flatWidth/2.0),Y);
  ModelSupport::buildPlane(SMap,surIndex+24,
			   StickCent+Y*(flatWidth/2.0),Y);
  ModelSupport::buildPlane(SMap,surIndex+25,
			   StickCent+Z*vsHeight,Z);
  ModelSupport::buildPlane(SMap,surIndex+26,
			   StickCent+Z*(vsHeight+flatThick),Z);

  // Vac layer
  ModelSupport::buildPlane(SMap,surIndex+31,-X*vacLowMX,X);
  ModelSupport::buildPlane(SMap,surIndex+32,X*vacLowPX,X);
  ModelSupport::buildPlane(SMap,surIndex+33,-Y*vacLowMY,Y);
  ModelSupport::buildPlane(SMap,surIndex+34,Y*vacLowPY,Y);
  ModelSupport::buildPlane(SMap,surIndex+35,-Z*vacBase,Z);
  ModelSupport::buildPlane(SMap,surIndex+36,Z*vacTop,Z);

  // Top Vacuum
  // -- This is a divider
  ModelSupport::buildPlane(SMap,surIndex+42,X*CylCent.X(),X);
  ModelSupport::buildCylinder(SMap,surIndex+47,CylCent,Z,vacRadius);
  ModelSupport::buildPlane(SMap,surIndex+45,Z*vacZExt,Z);

  // AL Skin:
  // -- PLANAR BOX SECTIONS [20000]:
  ModelSupport::buildPlane(SMap,surIndex+51,-X*(vacLowMX+alSideThick),X);
  ModelSupport::buildPlane(SMap,surIndex+52,X*(vacLowPX+alSideThick),X);
  ModelSupport::buildPlane(SMap,surIndex+53,-Y*(vacLowMY+alSideThick),Y);
  ModelSupport::buildPlane(SMap,surIndex+54,Y*(vacLowPY+alSideThick),Y);
  ModelSupport::buildPlane(SMap,surIndex+55,-Z*(vacBase+alBaseThick),Z);
  ModelSupport::buildPlane(SMap,surIndex+56,Z*(vacTop+alTopThick),Z);

  ModelSupport::buildCylinder(SMap,surIndex+67,CylCent,Z,vacRadius+alSideThick);
  ModelSupport::buildPlane(SMap,surIndex+65,Z*(vacZExt-alBaseThick),Z);
 
  ModelSupport::buildPlane(SMap,surIndex+71,
			   -X*(vacLowMX+alSideThick+skinMSideClear),X);
  ModelSupport::buildPlane(SMap,surIndex+72,
			   X*(vacLowPX+alSideThick+skinPSideClear),X);
  ModelSupport::buildPlane(SMap,surIndex+73,
			   -Y*(vacLowMY+alSideThick+skinForwardClear),Y);
  ModelSupport::buildPlane(SMap,surIndex+74,
			   Y*(vacLowPY+alSideThick+skinBackClear),Y);
  ModelSupport::buildPlane(SMap,surIndex+75,
			   -Z*(vacBase+alBaseThick+skinBaseClear),Z);

  ModelSupport::buildPlane(SMap,surIndex+83,
			   -Y*(vacLowMY+alSideThick+skinCutSideClear),Y);
  ModelSupport::buildPlane(SMap,surIndex+84,
			   Y*(vacLowPY+alSideThick+skinCutSideClear),Y);
  ModelSupport::buildPlane(SMap,surIndex+85,
			   Z*(vacZExt-alBaseThick-skinCutBaseClear),Z);
  ModelSupport::buildCylinder(SMap,surIndex+87,
			      CylCent,Z,vacRadius+alSideThick+skinCircleClear);

  // WATER ALUMINIUM:
  ModelSupport::buildPlane
    (SMap,surIndex+91,-X*(vacLowMX+alSideThick+skinMSideClear+waterAlThick),X);
  ModelSupport::buildPlane
    (SMap,surIndex+92,X*(vacLowPX+alSideThick+skinPSideClear+waterAlThick),X);
  ModelSupport::buildPlane
    (SMap,surIndex+93,
     -Y*(vacLowMY+alSideThick+skinForwardClear+waterAlThick),Y);
  ModelSupport::buildPlane
    (SMap,surIndex+94,Y*(vacLowPY+alSideThick+skinBackClear+waterAlThick),Y);
  ModelSupport::buildPlane
    (SMap,surIndex+95,-Z*(vacBase+alBaseThick+skinBaseClear+waterAlThick),Z);
  ModelSupport::buildPlane
    (SMap,surIndex+96,Z*(waterHeight+waterAlThick),Z);

  return;
}

void 
candleStick::createObjects(Simulation& System,
			   const attachSystem::ContainedComp& CC)
  /*!
    Construct the base and create item
    \param System :: simulation for objects
    \param CC :: Contained Component (siModerator)
  */
{
  ELog::RegMethod RegA("candleStick","createObjects");

  // Base system:
  std::string Out=
    ModelSupport::getComposite(SMap,surIndex,"1 -2 3 -4 5 -6");  
  System.addCell(MonteCarlo::Qhull(cellIndex++,supportMat,0.0,Out));
  // Support
  Out=ModelSupport::getComposite(SMap,surIndex,"11 -12 13 -14 15 -16");
  System.addCell(MonteCarlo::Qhull(cellIndex++,supportMat,0.0,Out));

  // Plate
  Out=ModelSupport::getComposite(SMap,surIndex,"-12 22 23 -24 25 -26");
  System.addCell(MonteCarlo::Qhull(cellIndex++,supportMat,0.0,Out));

  // VACUUM:
  Out=ModelSupport::getComposite(SMap,surIndex,"31 -32 33 -34 35 -36");    
  // last three object 
  Out+=ModelSupport::getComposite(SMap,surIndex,"(-1:2:-3:4:-5:6)");
  Out+=ModelSupport::getComposite(SMap,surIndex,"(-11:12:-13:14:-15:16)");
  Out+=ModelSupport::getComposite(SMap,surIndex,"(12:-22:-23:24:-25:26)");
  // And si moderator 
  Out+=CC.getExclude();
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // TOP VACUUM
  // Make outer Al system
  Out=ModelSupport::getComposite(SMap,surIndex,"45 -36 -31 ((42 33 -34):-47)");
  Out+=ModelSupport::getComposite(SMap,surIndex," (-22:-23:24:-25:26)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // AL SKIN:
  Out= ModelSupport::getComposite(SMap,surIndex,"51 -52 53 -54 55 -56");
  Out+=ModelSupport::getComposite(SMap,surIndex,
				  "( -31 : 32 : -33 : 34 : -35 : 36 )" 
				  "(-45 : 36 : -33 : 34 : 31) ");  
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));

  // TOP Rounded section
  // Make outer Al system
  Out=ModelSupport::getComposite(SMap,surIndex,"65 -56 -51 ((42 -54 53):-67)")+
    ModelSupport::getComposite(SMap,surIndex,
			       "#(45 -36 -31 ((42 33 -34):-47))");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));

  // Construct side clearance:
  Out=
    ModelSupport::getComposite(SMap,surIndex,"71 -72 73 -74 75 -56");
  Out+=
    (skinBaseClear>Geometry::zeroTol) ?
    ModelSupport::getComposite(SMap,surIndex,"( -51  : 52 : -53 : 54 : -55 )") :
    ModelSupport::getComposite(SMap,surIndex,"( -51  : 52 : -53 : 54 )");

  // Note that we dont need an explicit test here for cutClear
  // as the surface is required if and only if its the old Al surface
  // and register surface will find it.
  Out+=
    ModelSupport::getComposite(SMap,surIndex,"( 51  : -85 : -83 : 84)");
 
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
			         
  // AL SKIN CLEARANCE
  Out= ModelSupport::getComposite(SMap,surIndex,
				  "85 -56 -51 ((42 -84 83) : -87)");
  addOuterSurf(Out);   // old 70000 virtual
  Out+=ModelSupport::getComposite(SMap,surIndex,
				  "#(65 -56 -51 ((42 -54 53) : -67))");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // WATER AL
  Out=ModelSupport::getComposite(SMap,surIndex,"91 -92 93 -94 95 -96 ");
  TopBoundary.addOuterSurf(Out);
  Out+=(skinBaseClear>Geometry::zeroTol) ? 
    ModelSupport::getComposite(SMap,surIndex,
			       "( -71 : 72 : -73 : 74 : -75 ) ") : 
    ModelSupport::getComposite(SMap,surIndex,
			       "( -71 : 72 : -73 : 74 : -55 ) "); 
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));


  Out=ModelSupport::getComposite(SMap,surIndex,"71 -72 73 -74 96 -56");
  HeadBoundary.addOuterSurf(Out);
  return;
}  

void
candleStick::specialExclude(Simulation& System,const int cellNum) const
  /*!
    Exclude the outer parts of the candlestick
    \param System :: Simulation 
    \param cellNum :: Cell number
  */
{
  ELog::RegMethod RegA("candleStick","specialExclude");

  MonteCarlo::Qhull* outerObj=System.findQhull(cellNum);
  if (outerObj)
    {
      outerObj->addSurfString(getExclude());
      outerObj->addSurfString(TopBoundary.getExclude());
      outerObj->addSurfString(HeadBoundary.getExclude());
    }
  else
    ELog::EM<<"Failed to find outerObject: "<<cellNum<<ELog::endErr;
  
  return;
}

void
candleStick::createLinks()
  /*!
    Creates all the links
  */
{
  ELog::RegMethod RegA("candleStick","createLinks");

  FixedComp::setConnect(0,-Y*(vacLowMY+alSideThick+
			      skinForwardClear+waterAlThick),-Y);
  FixedComp::setConnect(1,Y*(vacLowMY+alSideThick+
			     skinForwardClear+waterAlThick),Y);

  FixedComp::setLinkSurf(0,SMap.realSurf(surIndex+93));
  FixedComp::setLinkSurf(1,SMap.realSurf(surIndex+94));

  return;
}

  
void
candleStick::createAll(Simulation& System,const siModerator& SMod)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param SMod :: siModerator to support
   */
{
  ELog::RegMethod RegA("candleStick","createAll");

  populate(System.getDataBase());
  createUnitVector(SMod);
  createSurfaces();
  createObjects(System,SMod);
  createLinks();
  return;
}

} // NAMESPACE LensSystem

