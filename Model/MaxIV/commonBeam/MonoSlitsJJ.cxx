/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/MonoSlitsJJ.cxx
 *
 * Copyright (c) 2026 by Udo Friman-Gayer
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "Vec3D.h"
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
#include "FixedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "MonoSlitsJJ.h"

namespace xraySystem
{

MonoSlitsJJ::MonoSlitsJJ(const std::string& Key) :
  attachSystem::FixedRotate(Key,3),
  attachSystem::ContainedComp(),
  attachSystem::FrontBackCut(),
  attachSystem::SurfMap(),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

void MonoSlitsJJ::setBladePositions(){
  if(bladeM1PosFlag == 0){
    bladeM1Pos = bladeM1InPos;
  } else if(bladeM1PosFlag == 1){
    bladeM1Pos = bladeM1OutPos;
  }

  if(bladeM2PosFlag == 0){
    bladeM2Pos = bladeM2InPos;
  } else if(bladeM2PosFlag == 1){
    bladeM2Pos = bladeM2OutPos;
  }

  if(bladeM3PosFlag == 0){
    bladeM3Pos = bladeM3InPos;
  } else if(bladeM3PosFlag == 1){
    bladeM3Pos = bladeM3OutPos;
  }

  if(bladeM4PosFlag == 0){
    bladeM4Pos = bladeM4InPos;
  } else if(bladeM4PosFlag == 1){
    bladeM4Pos = bladeM4OutPos;
  }
}

void
MonoSlitsJJ::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("MonoSlitsJJ","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");

  adapterInnerRadius=Control.EvalVar<double>(keyName+"AdapterInnerRadius");
  adapterThick=Control.EvalVar<double>(keyName+"AdapterThick");

  auxPortAngleStep=Control.EvalVar<double>(keyName+"AuxPortAngleStep");
  auxPortCenterAngle=Control.EvalVar<double>(keyName+"AuxPortCenterAngle");
  auxPortFlangeLength=Control.EvalVar<double>(keyName+"AuxPortFlangeLength");
  auxPortFlangeRadius=Control.EvalVar<double>(keyName+"AuxPortFlangeRadius");
  auxPortInnerRadius=Control.EvalVar<double>(keyName+"AuxPortInnerRadius");
  auxPortOpticalAxisOffset=Control.EvalVar<double>(keyName+"AuxPortOpticalAxisOffset");
  auxPortWallThick=Control.EvalVar<double>(keyName+"AuxPortWallThick");

  mainInnerRadius=Control.EvalVar<double>(keyName+"MainInnerRadius");
  mainWallThick=Control.EvalVar<double>(keyName+"MainWallThick");

  baseDepth=Control.EvalVar<double>(keyName+"BaseDepth");
  baseThick=Control.EvalVar<double>(keyName+"BaseThick");
  baseWidth=Control.EvalVar<double>(keyName+"BaseWidth");

  bladeAngle=Control.EvalVar<double>(keyName+"BladeAngle");
  bladeM1Pos=Control.EvalVar<double>(keyName+"BladeM1Pos");
  bladeM2Pos=Control.EvalVar<double>(keyName+"BladeM2Pos");
  bladeM3Pos=Control.EvalVar<double>(keyName+"BladeM3Pos");
  bladeM4Pos=Control.EvalVar<double>(keyName+"BladeM4Pos");
  bladeLongEdge=Control.EvalVar<double>(keyName+"BladeLongEdge");
  bladePortCenterDist=Control.EvalVar<double>(keyName+"BladePortCenterDist");
  bladePortDist=Control.EvalVar<double>(keyName+"BladePortDist");
  bladePortFlangeLength=Control.EvalVar<double>(keyName+"BladePortFlangeLength");
  bladePortFlangeRadius=Control.EvalVar<double>(keyName+"BladePortFlangeRadius");
  bladePortInnerRadius=Control.EvalVar<double>(keyName+"BladePortInnerRadius");
  bladePortLength=Control.EvalVar<double>(keyName+"BladePortLength");
  bladePortWallThick=Control.EvalVar<double>(keyName+"BladePortWallThick");
  bladeShortEdge=Control.EvalVar<double>(keyName+"BladeShortEdge");
  bladeThick=Control.EvalVar<double>(keyName+"BladeThick");
  bladeThreadLength=Control.EvalVar<double>(keyName+"BladeThreadLength");
  bladeThreadRadius=Control.EvalVar<double>(keyName+"BladeThreadRadius");
  bladeToThreadDist=Control.EvalVar<double>(keyName+"BladeToThreadDist");

  bladeMat=ModelSupport::EvalMat<int>(Control,keyName+"BladeMat");
  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");

  bladeM1PosFlag=Control.EvalVar<int>(keyName+"BladeM1PosFlag");
  bladeM2PosFlag=Control.EvalVar<int>(keyName+"BladeM2PosFlag");
  bladeM3PosFlag=Control.EvalVar<int>(keyName+"BladeM3PosFlag");
  bladeM4PosFlag=Control.EvalVar<int>(keyName+"BladeM4PosFlag");
  setBladePositions();

  return;
}


void
MonoSlitsJJ::createSurfaces()
{
  ELog::RegMethod RegA("MonoSlitsJJ","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setFront(SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
      setBack(-SMap.realSurf(buildIndex+2));
    }

  // Auxiliary planes
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin,Z);

  // Adapter (main vessel to in/out pipe)
  ModelSupport::buildPlane(SMap,buildIndex+21,Origin+Y*adapterThick,Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*(length-adapterThick),Y);

  // Main vessel
  ModelSupport::buildCylinder(
    SMap,buildIndex+7,Origin,Y,mainInnerRadius+mainWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,mainInnerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,adapterInnerRadius);

  // Base plate
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin+X*(baseWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin-X*(baseWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin-Z*baseDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin-Z*(baseDepth-baseThick),Z);

  // Blades and their respective ports/threads.
  // Surfaces of M1, ... M4 have index offsets of +100, ...,  +400, respectively.
  // Surfaces related to the blades have index offsets of +150, ..., +450, 
  // respectively.
  // Whenever there are shared surfaces between M1/M2 and M3/M4, the lower offset is
  // used, i.e. there is no surface with the index 201 because the M2 port uses 101 
  // from the M1 port.

  // M1 and M2 - common surfaces for ports
  ModelSupport::buildPlane(
    SMap,buildIndex+101,Origin+Z*(bladePortLength+bladePortFlangeLength),Z);
  ModelSupport::buildPlane(
    SMap,buildIndex+111,Origin+Z*bladePortLength,Z);
  ModelSupport::buildPlane(
    SMap,buildIndex+121,Origin+Z*(bladePortLength-bladePortFlangeLength),Z);
  ModelSupport::buildPlane(
    SMap,buildIndex+112,Origin+Z*(bladePortLength-bladeThreadLength),Z);
  
  // M1 - blade
  ModelSupport::buildPlane(
    SMap,buildIndex+151,
    Origin+Y*(
      length/2.0+bladePortCenterDist-bladeThreadRadius-bladeToThreadDist-bladeThick
    ),Y);
  ModelSupport::buildPlane(
    SMap,buildIndex+152,
    Origin+Y*(length/2.0+bladePortCenterDist-bladeThreadRadius-bladeToThreadDist),Y);
  ModelSupport::buildPlane(
    SMap,buildIndex+155,Origin-Z*(bladeM1Pos+bladeShortEdge),Z);
  Geometry::Vec3D Zp = Z;
  Zp.rotate(X,-bladeAngle*M_PI/180.0);
  ModelSupport::buildPlane(
    SMap,buildIndex+156,Origin
    +Y*(length/2.0+bladePortCenterDist-bladeThreadRadius-bladeToThreadDist-bladeThick)
    -Z*(bladeM1Pos),Zp);
  ModelSupport::buildPlane(
    SMap,buildIndex+153,Origin-X*(bladeLongEdge/2.0),X);
  ModelSupport::buildPlane(
    SMap,buildIndex+154,Origin+X*(bladeLongEdge/2.0),X);

  // M1 - port and thread
  Geometry::Vec3D portOrigin = (
    Origin+Y*(length/2.0+bladePortCenterDist)+X*(bladePortDist/2.0)
  );
  ModelSupport::buildCylinder(
    SMap,buildIndex+107,portOrigin,Z,bladePortFlangeRadius);
  ModelSupport::buildCylinder(
    SMap,buildIndex+117,portOrigin,Z,bladePortInnerRadius+bladePortWallThick);
  ModelSupport::buildCylinder(
    SMap,buildIndex+127,portOrigin,Z,bladePortInnerRadius);
  ModelSupport::buildCylinder(
    SMap,buildIndex+137,portOrigin,Z,bladeThreadRadius);

  // M2 - blade
  ModelSupport::buildPlane(
      SMap,buildIndex+251,
      Origin+Y*(
        length/2.0+bladePortCenterDist+bladeThreadRadius+bladeToThreadDist
      ),Y);
  ModelSupport::buildPlane(
      SMap,buildIndex+252,
      Origin+Y*(
        length/2.0+bladePortCenterDist+bladeThreadRadius+bladeToThreadDist+bladeThick
      ),Y);
  Zp = Z;
  Zp.rotate(X,bladeAngle*M_PI/180.0);
  ModelSupport::buildPlane(
    SMap,buildIndex+255,Origin
    +Y*(length/2.0+bladePortCenterDist+bladeThreadRadius+bladeToThreadDist)
    +Z*bladeM1Pos,Zp);
  ModelSupport::buildPlane(
    SMap,buildIndex+256,Origin+Z*(bladeM1Pos+bladeShortEdge),Z);

  // M2 = port and thread
  portOrigin = (
    Origin+Y*(length/2.0+bladePortCenterDist)-X*(bladePortDist/2.0)
  );
  ModelSupport::buildCylinder(
    SMap,buildIndex+207,portOrigin,Z,bladePortFlangeRadius);
  ModelSupport::buildCylinder(
    SMap,buildIndex+217,portOrigin,Z,bladePortInnerRadius+bladePortWallThick);
  ModelSupport::buildCylinder(
    SMap,buildIndex+227,portOrigin,Z,bladePortInnerRadius);
  ModelSupport::buildCylinder(
    SMap,buildIndex+237,portOrigin,Z,bladeThreadRadius);

  // M3 and M4 - common surfaces for ports
  ModelSupport::buildPlane(
    SMap,buildIndex+301,Origin+X*(bladePortLength+bladePortFlangeLength),X);
  ModelSupport::buildPlane(
    SMap,buildIndex+311,Origin+X*bladePortLength,X);
  ModelSupport::buildPlane(
    SMap,buildIndex+321,Origin+X*(bladePortLength-bladePortFlangeLength),X);
  ModelSupport::buildPlane(
    SMap,buildIndex+312,Origin+X*(bladePortLength-bladeThreadLength),X);

  // M3 - blade
  ModelSupport::buildPlane(
    SMap,buildIndex+351,
    Origin+Y*(length/2.0-bladePortCenterDist+bladeThreadRadius+bladeToThreadDist),Y);
  ModelSupport::buildPlane(
    SMap,buildIndex+352,
    Origin+Y*(
      length/2.0-bladePortCenterDist+bladeThreadRadius
      +bladeToThreadDist+bladeThick
    ),Y);
  Geometry::Vec3D Xp = X;
  Xp.rotate(Z,bladeAngle*M_PI/180.0);
  ModelSupport::buildPlane(
    SMap,buildIndex+353,Origin-X*(bladeM3Pos+bladeShortEdge),X);
  ModelSupport::buildPlane(
    SMap,buildIndex+354,Origin
    +Y*(length/2.0-bladePortCenterDist+bladeThreadRadius+bladeToThreadDist)
    -X*(bladeM3Pos),Xp);
  ModelSupport::buildPlane(
    SMap,buildIndex+355,Origin-Z*(bladeLongEdge/2.0),Z);
  ModelSupport::buildPlane(
    SMap,buildIndex+356,Origin+Z*(bladeLongEdge/2.0),Z);

  // M3 - port and thread
  portOrigin = (
    Origin+Y*(length/2.0-bladePortCenterDist)+Z*(bladePortDist/2.0)
  );
  ModelSupport::buildCylinder(
    SMap,buildIndex+307,portOrigin,X,bladePortFlangeRadius);
  ModelSupport::buildCylinder(
    SMap,buildIndex+317,portOrigin,X,bladePortInnerRadius+bladePortWallThick);
  ModelSupport::buildCylinder(
    SMap,buildIndex+327,portOrigin,X,bladePortInnerRadius);
  ModelSupport::buildCylinder(
    SMap,buildIndex+337,portOrigin,X,bladeThreadRadius);

  // M4 - blade
  ModelSupport::buildPlane(
    SMap,buildIndex+451,
    Origin
    +Y*(length/2.0-bladePortCenterDist-bladeThreadRadius-bladeToThreadDist-bladeThick),
    Y);
  ModelSupport::buildPlane(
    SMap,buildIndex+452,
    Origin+Y*(length/2.0-bladePortCenterDist-bladeThreadRadius-bladeToThreadDist),Y);
  Xp = X;
  Xp.rotate(Z,-bladeAngle*M_PI/180.0);
  ModelSupport::buildPlane(
    SMap,buildIndex+453,Origin
    +Y*(length/2.0-bladePortCenterDist-bladeThreadRadius-bladeToThreadDist-bladeThick)
    +X*(bladeM4Pos),Xp);
  ModelSupport::buildPlane(
    SMap,buildIndex+454,Origin+X*(bladeM4Pos+bladeShortEdge),X);

  // M4 - port and thread
  portOrigin = (
    Origin+Y*(length/2.0-bladePortCenterDist)-Z*(bladePortDist/2.0)
  );
  ModelSupport::buildCylinder(
    SMap,buildIndex+407,portOrigin,X,bladePortFlangeRadius);
  ModelSupport::buildCylinder(
    SMap,buildIndex+417,portOrigin,X,bladePortInnerRadius+bladePortWallThick);
  ModelSupport::buildCylinder(
    SMap,buildIndex+427,portOrigin,X,bladePortInnerRadius);
  ModelSupport::buildCylinder(
    SMap,buildIndex+437,portOrigin,X,bladeThreadRadius);

  // Surfaces for auxiliary ports, using the following index offsets:
  // Center : +500
  // Top    : +600
  // Bottom : +700

  const double auxPortCenterAngleRad = auxPortCenterAngle * M_PI / 180.0;
  const double auxPortAngleStepRad = auxPortAngleStep * M_PI / 180.0;
  // In [1] or [2], it can be seen that the flanges of the auxiliary ports touch each 
  // other when projected on a 2D plane (in reality, they do not touch because they 
  // are offset along the beam axis).
  // From this information, the port length can be calculated.
  const double auxPortLength = (
    auxPortFlangeRadius / tan(auxPortAngleStepRad/2.0)+auxPortFlangeLength);

  // Auxiliary: Center port
  Geometry::Vec3D auxPortCenterOrigin = Origin+Y*(length/2.0+auxPortOpticalAxisOffset);
  Geometry::Vec3D auxPortCenterDir = (
    X*(-cos(auxPortCenterAngleRad))+Z*sin(auxPortCenterAngleRad));
  ModelSupport::buildPlane(
    SMap,buildIndex+501,
    auxPortCenterOrigin+auxPortCenterDir*(auxPortLength+auxPortFlangeLength),
    auxPortCenterDir
  );
  ModelSupport::buildPlane(
    SMap,buildIndex+511,
    auxPortCenterOrigin+auxPortCenterDir*auxPortLength,auxPortCenterDir
  );
  ModelSupport::buildPlane(
    SMap,buildIndex+521,
    auxPortCenterOrigin+auxPortCenterDir*(auxPortLength-auxPortFlangeLength),
    auxPortCenterDir
  );
  ModelSupport::buildCylinder(
    SMap,buildIndex+507,auxPortCenterOrigin,auxPortCenterDir,auxPortFlangeRadius
  );
  ModelSupport::buildCylinder(
    SMap,buildIndex+517,
    auxPortCenterOrigin,auxPortCenterDir,auxPortInnerRadius+auxPortWallThick
  );
  ModelSupport::buildCylinder(
    SMap,buildIndex+527,auxPortCenterOrigin,auxPortCenterDir,auxPortInnerRadius
  );

  // Auxiliary: Top port (using the same origin as the bottom port)
  Geometry::Vec3D auxPortTopBottomOrigin = (
    Origin+Y*(length/2.0-auxPortOpticalAxisOffset));

  Geometry::Vec3D auxPortTopDir = (
    X*(-cos(auxPortCenterAngleRad+auxPortAngleStepRad))
    +Z*sin(auxPortCenterAngleRad+auxPortAngleStepRad)
  );
  ModelSupport::buildPlane(
    SMap,buildIndex+601,
    auxPortTopBottomOrigin+auxPortTopDir*(auxPortLength+auxPortFlangeLength),
    auxPortTopDir
  );
  ModelSupport::buildPlane(
    SMap,buildIndex+611,auxPortTopBottomOrigin+auxPortTopDir*auxPortLength,
    auxPortTopDir
  );
  ModelSupport::buildPlane(
    SMap,buildIndex+621,
    auxPortTopBottomOrigin+auxPortTopDir*(auxPortLength-auxPortFlangeLength),
    auxPortTopDir
  );
  ModelSupport::buildCylinder(
    SMap,buildIndex+607,auxPortTopBottomOrigin,auxPortTopDir,auxPortFlangeRadius
  );
  ModelSupport::buildCylinder(
    SMap,buildIndex+617,auxPortTopBottomOrigin,auxPortTopDir,
    auxPortInnerRadius+auxPortWallThick
  );
  ModelSupport::buildCylinder(
    SMap,buildIndex+627,auxPortTopBottomOrigin,auxPortTopDir,auxPortInnerRadius
  );

  // Auxiliary: Bottom port
  Geometry::Vec3D auxPortBottomDir = (
    X*(-cos(auxPortCenterAngleRad-auxPortAngleStepRad))+
    Z*sin(auxPortCenterAngleRad-auxPortAngleStepRad)
  );
  ModelSupport::buildPlane(
    SMap,buildIndex+701,
    auxPortTopBottomOrigin+auxPortBottomDir*(auxPortLength+auxPortFlangeLength),
    auxPortBottomDir
  );
  ModelSupport::buildPlane(
    SMap,buildIndex+711,auxPortTopBottomOrigin+auxPortBottomDir*auxPortLength,
    auxPortBottomDir
  );
  ModelSupport::buildPlane(
    SMap,buildIndex+721,
    auxPortTopBottomOrigin+auxPortBottomDir*(auxPortLength-auxPortFlangeLength),
    auxPortBottomDir
  );
  ModelSupport::buildCylinder(
    SMap,buildIndex+707,auxPortTopBottomOrigin,auxPortBottomDir,auxPortFlangeRadius
  );
  ModelSupport::buildCylinder(
    SMap,buildIndex+717,auxPortTopBottomOrigin,auxPortBottomDir,
    auxPortInnerRadius+auxPortWallThick
  );
  ModelSupport::buildCylinder(
    SMap,buildIndex+727,auxPortTopBottomOrigin,auxPortBottomDir,auxPortInnerRadius
  );


  // Separating plane between the M2 port and the top auxiliary port.
  // Angle adjusted such that port dimensions/position can be adjusted slightly without
  // breaking the geometry.
  const double m2TopPortSeparatorAngle = (
    (auxPortCenterAngleRad+auxPortAngleStepRad)/2.0 + M_PI_4);
  ModelSupport::buildPlane(
    SMap,buildIndex+505,
    Origin+(
      X*(-cos(m2TopPortSeparatorAngle))
      +Z*sin(m2TopPortSeparatorAngle)
    )*(mainInnerRadius+mainWallThick),
    X*cos(M_PI_2-m2TopPortSeparatorAngle)
    +Z*sin(M_PI_2-m2TopPortSeparatorAngle)
  );
  // Separating planes between the auxiliary ports.
  ModelSupport::buildPlane(
    SMap,buildIndex+515,Origin,auxPortTopDir-auxPortCenterDir);
  ModelSupport::buildPlane(
    SMap,buildIndex+525,Origin,auxPortCenterDir-auxPortBottomDir);
}

void
MonoSlitsJJ::createObjects(Simulation& System)
  /*!
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("MonoSlitsJJ","createObjects");

  const HeadRule front = ExternalCut::getRule("front");
  const HeadRule back = ExternalCut::getRule("back");

  addOuterSurf(
    front*back
    *ModelSupport::getHeadRule(SMap,buildIndex,"6 -101 -301 -501 -601 -701")
  );

  makeCell("MainFrontRight",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-11 21 -7 17 3 327 427"));
  makeCell("MainFrontLeft",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-11 21 -7 17 -3 527 627 727"));
  makeCell("MainBackTopLeft",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-3 11 -22 -7 17 5 227 527 627"));
  makeCell("MainBackTopRight",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"3 11 -22 -7 17 5 127"));
  makeCell("MainBackBottomLeft",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-3 11 -22 -7 17 -5 527 727"));
  makeCell("MainBackBottomRight",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"3 11 -22 -7 17 -5"));

  makeCell("BasePlate",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"21 -22 -13 14 6 -16 7"));
  makeCell("BaseVoid",System,cellIndex++,0,0.0,
    front*back*
    ModelSupport::getHeadRule(SMap,buildIndex,"(-21:22:13:-14) -301 -701 6 -16 707"));

  makeCell("MainVoidFront",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"21 -451 -17"));
  makeCell("MainVoidM4",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"451 -452 (-453:454:-355:356) -17"));
  makeCell("MainVoidFrontThreads",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"452 -351 (437:-312) (337:-312) -17"));
  makeCell("MainVoidM3",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"351 -352 (-353:354:-355:356) -17"));
  makeCell("MainVoidCenter",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"352 -151 -17"));
  makeCell("MainVoidM1",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"151 -152 (-153:154:-155:156) -17"));
  makeCell("MainVoidBackThreads",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"152 -251 (237:-112) (137:-112) -17"));
  makeCell("MainVoidM2",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"251 -252 (-153:154:-255:256) -17"));
  makeCell("MainVoidBack",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"252 -22 -17"));

  makeCell("TopRightVoid",System,cellIndex++,0,0.0,
    front*back*ModelSupport::getHeadRule(SMap,buildIndex,"3 5 7 -101 107 -301 307"));
  makeCell("M2PortVoid",System,cellIndex++,0,0.0,
    front*back*ModelSupport::getHeadRule(SMap,buildIndex,"-3 7 -101 207 505"));
  makeCell("AuxPortTopVoid",System,cellIndex++,0,0.0,
    front*back
    *ModelSupport::getHeadRule(SMap,buildIndex,"-3 7 -101 207 -505 515 -601 607"));
  makeCell("AuxPortTopVoid",System,cellIndex++,0,0.0,
    front*back*ModelSupport::getHeadRule(SMap,buildIndex,"-3 7 515 -621 -607 617"));
  makeCell("AuxPortCenterVoid",System,cellIndex++,0,0.0,
    front*back*ModelSupport::getHeadRule(SMap,buildIndex,"-3 7 -515 525 -501 507"));
  makeCell("AuxPortCenterVoid",System,cellIndex++,0,0.0,
    front*back
    *ModelSupport::getHeadRule(SMap,buildIndex,"-3 7 -515 525 -521 -507 517"));
  makeCell("AuxPortBottomVoid",System,cellIndex++,0,0.0,
    front*back*ModelSupport::getHeadRule(SMap,buildIndex,"-3 16 7 -525 -701 707"));
  makeCell("AuxPortBottomVoid",System,cellIndex++,0,0.0,
    front*back*ModelSupport::getHeadRule(SMap,buildIndex,"-3 16 7 -525 -721 -707 717"));
  makeCell("BottomRightVoid",System,cellIndex++,0,0.0,
    front*back*ModelSupport::getHeadRule(SMap,buildIndex,"3 -5 16 7 -301 407"));

  makeCell("AdapterFront",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-7 27 -21")*front);
  makeCell("AdapterFrontVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-27 -21")*front);
  makeCell("AdapterBack",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-7 27 22")*back);
  makeCell("AdapterBackVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-27 22")*back);

  makeCell("BladeM1",System,cellIndex++,bladeMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"151 -152 153 -154 155 -156"));
  std::string portName = "BladePortM1";
  makeCell(portName+"Cap",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"3 -101 111 -107"));
  makeCell(portName+"OuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"3 5 7 -121 -107 117"));
  makeCell(portName+"Flange",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"3 -111 121 -107 127"));
  makeCell(portName+"Pipe",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"5 7 -121 -117 127"));
  makeCell(portName+"Thread",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-111 112 -137"));
  makeCell(portName+"InnerVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"5 17 -111 -127 137"));

  makeCell("BladeM2",System,cellIndex++,bladeMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"251 -252 153 -154 255 -256"));
  portName = "BladePortM2";
  makeCell(portName+"Cap",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-3 -101 111 -207"));
  makeCell(portName+"OuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-3 5 7 -121 -207 217"));
  makeCell(portName+"Flange",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-3 -111 121 -207 227"));
  makeCell(portName+"Pipe",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"5 7 -121 -217 227"));
  makeCell(portName+"Thread",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-111 112 -237"));
  makeCell(portName+"InnerVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"5 17 -111 -227 237"));

  makeCell("BladeM3",System,cellIndex++,bladeMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"351 -352 353 -354 355 -356"));
  portName = "BladePortM3";
  makeCell(portName+"Cap",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"5 -301 311 -307"));
  makeCell(portName+"OuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"3 5 7 -321 -307 317"));
  makeCell(portName+"Flange",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"5 -311 321 -307 327"));
  makeCell(portName+"Pipe",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"3 5 7 -321 -317 327"));
  makeCell(portName+"Thread",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-311 312 -337"));
  makeCell(portName+"InnerVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"3 17 -311 -327 337"));

  makeCell("BladeM4",System,cellIndex++,bladeMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"451 -452 453 -454 355 -356"));
  portName = "BladePortM4";
  makeCell(portName+"Cap",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-5 -301 311 -407"));
  makeCell(portName+"OuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"3 -5 7 -321 -407 417"));
  makeCell(portName+"Flange",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-5 -311 321 -407 427"));
  makeCell(portName+"Pipe",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"3 -5 7 -321 -417 427"));
  makeCell(portName+"Thread",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-311 312 -437"));
  makeCell(portName+"InnerVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"3 17 -311 -427 437"));

  makeCell("AuxPortCenterCap",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-3 7 -501 511 -507")
  );
  makeCell("AuxPortCenterFlange",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-3 7 -511 521 -507 527")
  );
  makeCell("AuxPortCenterPipe",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-3 7 -521 -517 527")
  );
  makeCell("AuxPortCenterInnerVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-3 17 -511 -527")
  );

  makeCell("AuxPortTopCap",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-3 7 -601 611 -607")
  );
  makeCell("AuxPortTopFlange",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-3 7 -611 621 -607 627")
  );
  makeCell("AuxPortTopPipe",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-3 7 -621 -617 627")
  );
  makeCell("AuxPortTopInnerVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-3 17 -611 -627")
  );

  makeCell("AuxPortBottomCap",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-3 7 -701 711 -707")
  );
  makeCell("AuxPortBottomFlange",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-3 7 -711 721 -707 727")
  );
  makeCell("AuxPortBottomPipe",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-3 7 -721 -717 727")
  );
  makeCell("AuxPortBottomInnerVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-3 17 -711 -727")
  );
}

void
MonoSlitsJJ::createLinks()
{
  ELog::RegMethod RControl("MonoSlitsJJ","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  return;
}

void
MonoSlitsJJ::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    External build everything
    \param System :: Simulation
    \param FC :: FixedComp for beam origin
    \param sideIndex :: link point of centre [and axis]
   */
{
  ELog::RegMethod RegA("MonoSlitsJJ","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
