/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/BilbaoWheel.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "EllipticCyl.h"
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
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "General.h"
#include "Plane.h"
#include "SurInter.h"

#include "BilbaoWheelCassette.h"
#include "WheelBase.h"
#include "BilbaoWheel.h"


namespace essSystem
{

BilbaoWheel::BilbaoWheel(const std::string& Key) :
  WheelBase(Key)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

BilbaoWheel::BilbaoWheel(const BilbaoWheel& A) :
  WheelBase(A),
  targetHeight(A.targetHeight),
  targetInnerHeight(A.targetHeight),
  targetInnerHeightRadius(A.targetHeight),
  voidTungstenThick(A.voidTungstenThick),
  steelTungstenThick(A.steelTungstenThick),
  steelTungstenInnerThick(A.steelTungstenInnerThick),
  coolantThick(A.coolantThick),
  caseThick(A.caseThick),voidThick(A.voidThick),
  innerRadius(A.innerRadius),
  innerHoleHeight(A.innerHoleHeight),
  innerHoleSize(A.innerHoleSize),
  innerHoleXYangle(A.innerHoleXYangle),
  coolantRadiusIn(A.coolantRadiusIn),
  coolantRadiusOut(A.coolantRadiusOut),
  caseRadius(A.caseRadius),voidRadius(A.voidRadius),
  aspectRatio(A.aspectRatio),
  mainTemp(A.mainTemp),nSectors(A.nSectors),
  nLayers(A.nLayers),radius(A.radius),
  matTYPE(A.matTYPE),shaftHeight(A.shaftHeight),
  nShaftLayers(A.nShaftLayers),shaftRadius(A.shaftRadius),
  shaftMat(A.shaftMat),shaft2StepHeight(A.shaft2StepHeight),
  shaft2StepConnectionHeight(A.shaft2StepConnectionHeight),
  shaft2StepConnectionDist(A.shaft2StepConnectionDist),
  shaft2StepConnectionRadius(A.shaft2StepConnectionRadius),
  shaftCFRingHeight(A.shaftCFRingHeight),
  shaftCFRingRadius(A.shaftCFRingRadius),
  shaftUpperBigStiffLength(A.shaftUpperBigStiffLength),
  shaftUpperBigStiffHeight(A.shaftUpperBigStiffHeight),
  shaftUpperBigStiffThick(A.shaftUpperBigStiffThick),
  shaftUpperBigStiffHomoMat(A.shaftUpperBigStiffHomoMat),
  shaftLowerBigStiffShortLength(A.shaftLowerBigStiffShortLength),
  shaftLowerBigStiffLongLength(A.shaftLowerBigStiffLongLength),
  shaftLowerBigStiffHeight(A.shaftLowerBigStiffHeight),
  shaftLowerBigStiffThick(A.shaftLowerBigStiffThick),
  shaftLowerBigStiffHomoMat(A.shaftLowerBigStiffHomoMat),
  shaftHoleHeight(A.shaftHoleHeight),
  shaftHoleSize(A.shaftHoleSize),
  shaftHoleXYangle(A.shaftHoleXYangle),
  shaftBaseDepth(A.shaftBaseDepth),
  catcherTopSteelThick(A.catcherTopSteelThick),
  catcherRadius(A.catcherRadius),
  catcherBaseHeight(A.catcherBaseHeight),
  catcherBaseRadius(A.catcherBaseRadius),
  catcherBaseAngle(A.catcherBaseAngle),
  catcherNotchRadius(A.catcherNotchRadius),
  catcherNotchBaseThick(A.catcherNotchBaseThick),
  circlePipesBigRad(A.circlePipesBigRad),
  circlePipesRad(A.circlePipesRad),
  circlePipesWallThick(A.circlePipesWallThick),
  homoWMat(A.homoWMat),heMat(A.heMat),
  steelMat(A.steelMat),ssVoidMat(A.ssVoidMat),
  innerMat(A.innerMat)
  /*!
    Copy constructor
    \param A :: BilbaoWheel to copy
  */
{}

BilbaoWheel&
BilbaoWheel::operator=(const BilbaoWheel& A)
  /*!
    Assignment operator
    \param A :: BilbaoWheel to copy
    \return *this
  */
{
  if (this!=&A)
    {
      WheelBase::operator=(A);
      targetHeight=A.targetHeight;
      targetInnerHeight=A.targetInnerHeight;
      targetInnerHeightRadius=A.targetInnerHeightRadius;
      voidTungstenThick=A.voidTungstenThick;
      steelTungstenThick=A.steelTungstenThick;
      steelTungstenInnerThick=A.steelTungstenInnerThick;
      coolantThick=A.coolantThick;
      caseThick=A.caseThick;
      voidThick=A.voidThick;
      innerRadius=A.innerRadius;
      innerHoleHeight=A.innerHoleHeight;
      innerHoleSize=A.innerHoleSize;
      innerHoleXYangle=A.innerHoleXYangle;
      coolantRadiusIn=A.coolantRadiusIn;
      coolantRadiusOut=A.coolantRadiusOut;
      caseRadius=A.caseRadius;
      voidRadius=A.voidRadius;
      aspectRatio=A.aspectRatio;
      mainTemp=A.mainTemp;
      nSectors=A.nSectors;
      nLayers=A.nLayers;
      radius=A.radius;
      matTYPE=A.matTYPE;
      shaftHeight=A.shaftHeight;
      nShaftLayers=A.nShaftLayers;
      shaftRadius=A.shaftRadius;
      shaftMat=A.shaftMat;
      shaft2StepHeight=A.shaft2StepHeight;
      shaft2StepConnectionHeight=A.shaft2StepConnectionHeight;
      shaft2StepConnectionDist=A.shaft2StepConnectionDist;
      shaft2StepConnectionRadius=A.shaft2StepConnectionRadius;
      shaftCFRingHeight=A.shaftCFRingHeight;
      shaftCFRingRadius=A.shaftCFRingRadius;
      shaftUpperBigStiffLength=A.shaftUpperBigStiffLength;
      shaftUpperBigStiffHeight=A.shaftUpperBigStiffHeight;
      shaftUpperBigStiffThick=A.shaftUpperBigStiffThick;
      shaftUpperBigStiffHomoMat=A.shaftUpperBigStiffHomoMat;
      shaftLowerBigStiffShortLength=A.shaftLowerBigStiffShortLength;
      shaftLowerBigStiffLongLength=A.shaftLowerBigStiffLongLength;
      shaftLowerBigStiffHeight=A.shaftLowerBigStiffHeight;
      shaftLowerBigStiffThick=A.shaftLowerBigStiffThick;
      shaftLowerBigStiffHomoMat=A.shaftLowerBigStiffHomoMat;
      shaftHoleHeight=A.shaftHoleHeight;
      shaftHoleSize=A.shaftHoleSize;
      shaftHoleXYangle=A.shaftHoleXYangle;
      shaftBaseDepth=A.shaftBaseDepth;
      catcherTopSteelThick=A.catcherTopSteelThick;
      catcherRadius=A.catcherRadius;
      catcherBaseHeight=A.catcherBaseHeight;
      catcherBaseRadius=A.catcherBaseRadius;
      catcherBaseAngle=A.catcherBaseAngle;
      catcherNotchRadius=A.catcherNotchRadius;
      catcherNotchBaseThick=A.catcherNotchBaseThick;
      circlePipesBigRad=A.circlePipesBigRad;
      circlePipesRad=A.circlePipesRad;
      circlePipesWallThick=A.circlePipesWallThick;
      homoWMat=A.homoWMat;
      heMat=A.heMat;
      steelMat=A.steelMat;
      ssVoidMat=A.ssVoidMat;
      innerMat=A.innerMat;
    }
  return *this;
}

BilbaoWheel::~BilbaoWheel()
  /*!
    Destructor
   */
{}

BilbaoWheel*
BilbaoWheel::clone() const
  /*!
    Virtual copy constructor
    \return new(this)
   */
{
  return new BilbaoWheel(*this);
}

void
BilbaoWheel::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("BilbaoWheel","populate");

  // Master values
  FixedRotate::populate(Control);
  
  nSectors=Control.EvalDefVar<size_t>(keyName+"NSectors",3);
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  double R;
  int M;
  for(size_t i=0;i<nLayers;i++)
    {
      const std::string nStr=std::to_string(i+1);
      R=Control.EvalVar<double>(keyName+"Radius"+nStr);
      M = Control.EvalVar<int>(keyName+"MatTYPE"+nStr);

      if (i && R-radius.back()<Geometry::zeroTol)
	ELog::EM<<"Radius["<<i+1<<"] not ordered "
		<<R<<" "<<radius.back()<<ELog::endErr;
      radius.push_back(R);
      matTYPE.push_back(M);
    }

  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  innerHoleHeight=Control.EvalVar<double>(keyName+"InnerHoleHeight");
  innerHoleSize=Control.EvalVar<double>(keyName+"InnerHoleSize");
  if (innerHoleSize>1.0)
    throw ColErr::RangeError<double>(innerHoleSize, 0, 1,
				     keyName+"InnerHoleSize is the fraction of"
				     " hole angular length with respect to"
				     " hole+steel length.");
  innerHoleXYangle=Control.EvalVar<double>(keyName+"InnerHoleXYAngle");

  coolantRadiusIn=Control.EvalVar<double>(keyName+"CoolantRadiusIn");
  coolantRadiusOut=Control.EvalVar<double>(keyName+"CoolantRadiusOut");
  caseRadius=Control.EvalVar<double>(keyName+"CaseRadius");
  voidRadius=Control.EvalVar<double>(keyName+"VoidRadius");
  aspectRatio=Control.EvalVar<double>(keyName+"AspectRatio");
  mainTemp=Control.EvalVar<double>(keyName+"Temp");

  nSectors=Control.EvalVar<size_t>(keyName+"NSectors");

  targetHeight=Control.EvalVar<double>(keyName+"TargetHeight");
  targetInnerHeight=Control.EvalVar<double>(keyName+"TargetInnerHeight");
  targetInnerHeightRadius=Control.EvalVar<double>(keyName+"TargetInnerHeightRadius");
  voidTungstenThick=Control.EvalVar<double>(keyName+"VoidTungstenThick");
  steelTungstenThick=Control.EvalVar<double>(keyName+"SteelTungstenThick");
  steelTungstenInnerThick=Control.EvalVar<double>(keyName+"SteelTungstenInnerThick");
  coolantThick=Control.EvalVar<double>(keyName+"CoolantThick");
  caseThick=Control.EvalVar<double>(keyName+"CaseThick");
  voidThick=Control.EvalVar<double>(keyName+"VoidThick");

  nShaftLayers=Control.EvalVar<size_t>(keyName+"NShaftLayers");
  for (size_t i=0; i<nShaftLayers; i++)
    {
      const std::string nStr=std::to_string(i+1);
      R = Control.EvalVar<double>(keyName+"ShaftRadius"+nStr);
      M = ModelSupport::EvalMat<int>(Control,keyName+"ShaftMat"+nStr);
      if (i && R-shaftRadius.back()<Geometry::zeroTol)
	ELog::EM<<"ShaftRadius["<<i+1<<"] not ordered "
		<<R<<" "<<shaftRadius.back()<<ELog::endErr;
      shaftRadius.push_back(R);
      shaftMat.push_back(M);
    }


  shaftHeight=Control.EvalVar<double>(keyName+"ShaftHeight");
  shaft2StepHeight=Control.EvalVar<double>(keyName+"Shaft2StepHeight");
  shaft2StepConnectionHeight=Control.EvalVar<double>(keyName+"Shaft2StepConnectionHeight");
  shaft2StepConnectionDist=Control.EvalVar<double>(keyName+"Shaft2StepConnectionDist");
  shaft2StepConnectionRadius=Control.EvalVar<double>(keyName+"Shaft2StepConnectionRadius");
  shaftCFRingHeight=Control.EvalVar<double>(keyName+"ShaftConnectionFlangeRingHeight");
  shaftCFRingRadius=Control.EvalVar<double>(keyName+"ShaftConnectionFlangeRingRadius");
  shaftUpperBigStiffLength=Control.EvalVar<double>(keyName+"ShaftUpperBigStiffLength");
  shaftUpperBigStiffHeight=Control.EvalVar<double>(keyName+"ShaftUpperBigStiffHeight");
  shaftUpperBigStiffThick=Control.EvalVar<double>(keyName+"ShaftUpperBigStiffThick");
  shaftUpperBigStiffHomoMat=ModelSupport::EvalMat<int>(Control,
						       keyName+"ShaftUpperBigStiffHomoMat");

  shaftLowerBigStiffShortLength=Control.EvalVar<double>(keyName+"ShaftLowerBigStiffShortLength");
  shaftLowerBigStiffLongLength=Control.EvalVar<double>(keyName+"ShaftLowerBigStiffLongLength");
  shaftLowerBigStiffHeight=Control.EvalVar<double>(keyName+"ShaftLowerBigStiffHeight");
  shaftLowerBigStiffThick=Control.EvalVar<double>(keyName+"ShaftLowerBigStiffThick");
  shaftLowerBigStiffHomoMat=ModelSupport::EvalMat<int>(Control,
						       keyName+"ShaftLowerBigStiffHomoMat");

  if (shaft2StepConnectionRadius<shaftRadius[nShaftLayers-1])
    throw ColErr::RangeError<double>(shaft2StepConnectionRadius,shaftRadius[nShaftLayers-1],
				     INFINITY,
				     "Shaft2StepConnectionRadius must exceed outer ShaftRadius");
  shaftHoleHeight=Control.EvalVar<double>(keyName+"ShaftHoleHeight");
  shaftHoleSize=Control.EvalVar<double>(keyName+"ShaftHoleSize");
  if (shaftHoleSize>1.0)
    throw ColErr::RangeError<double>(shaftHoleSize, 0, 1,
				     keyName+"ShaftHoleSize is the fraction of"
				     " hole angular length with respect to"
				     " hole+steel length.");
  shaftHoleXYangle=Control.EvalVar<double>(keyName+"ShaftHoleXYAngle");

  shaftBaseDepth=Control.EvalVar<double>(keyName+"ShaftBaseDepth");

  catcherTopSteelThick=Control.EvalVar<double>(keyName+"CatcherTopSteelThick");

  catcherRadius=Control.EvalVar<double>(keyName+"CatcherRadius");
  if (catcherRadius>radius[0]+voidThick)
    throw ColErr::RangeError<double>(catcherRadius, 0, radius[0]+voidThick,
				     "CatcherRadius must not exceed Radius1 + VoidThick");

  catcherBaseHeight=Control.EvalVar<double>(keyName+"CatcherBaseHeight");
  catcherBaseRadius=Control.EvalVar<double>(keyName+"CatcherBaseRadius");

  catcherBaseAngle=Control.EvalVar<double>(keyName+"CatcherBaseAngle");
  catcherNotchRadius=Control.EvalVar<double>(keyName+"CatcherNotchRadius");

  catcherNotchBaseThick=Control.EvalVar<double>(keyName+"CatcherNotchBaseThick");

  circlePipesBigRad=Control.EvalVar<double>(keyName+"CirclePipesBigRad");
  circlePipesRad=Control.EvalVar<double>(keyName+"CirclePipesRad");
  circlePipesWallThick=Control.EvalVar<double>(keyName+"CirclePipesWallThick");

  homoWMat=ModelSupport::EvalMat<int>(Control,keyName+"HomoWMat");
  heMat=ModelSupport::EvalMat<int>(Control,keyName+"HeMat");
  steelMat=ModelSupport::EvalMat<int>(Control,keyName+"SteelMat");
  ssVoidMat=ModelSupport::EvalMat<int>(Control,keyName+"SS316LVoidMat");
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");

  return;
}

void
BilbaoWheel::createShaftSurfaces()
  /*!
    Construct surfaces for the inner shaft
  */
{
  ELog::RegMethod RegA("BilbaoWheel","createShaftSurfaces");

  // divider
  const Geometry::Plane *px =
    ModelSupport::buildPlane(SMap,buildIndex+3,Origin,X);

  ModelSupport::buildPlane(SMap,buildIndex+2006,Origin+Z*shaftHeight,Z);

  int SI(buildIndex);
  for (size_t i=0; i<nShaftLayers; i++)
    {
      ModelSupport::buildCylinder(SMap,SI+2007,Origin,Z,shaftRadius[i]);
      SI+=10;
    }


  double H(wheelHeight()/2.0+caseThick);


  // 2nd void step
  H = shaft2StepHeight;
  ModelSupport::buildPlane(SMap,buildIndex+2115,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,buildIndex+2116,Origin+Z*H,Z);

  H += voidThick;
  ModelSupport::buildPlane(SMap,buildIndex+2125,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,buildIndex+2126,Origin+Z*H,Z);

  double H1 = H;

  H -= voidThick;
  H -= radius[0]-innerRadius;
  ModelSupport::buildPlane(SMap,buildIndex+2135,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,buildIndex+2136,Origin+Z*H,Z);

  double R = radius[0]+voidThick;
  ModelSupport::buildCylinder(SMap,buildIndex+2118,Origin,Z,R);

  // Connection flange ring [ESS-0124024 pages 22-23 (DW-TRGT-ESS-0102)]
  H = H1-voidThick;
  H += shaftUpperBigStiffHeight;
  ModelSupport::buildPlane(SMap,buildIndex+2146,Origin+Z*H,Z);
  H += shaftCFRingHeight;
  ModelSupport::buildPlane(SMap,buildIndex+2156,Origin+Z*H,Z);
  R = shaftCFRingRadius;
  ModelSupport::buildCylinder(SMap,buildIndex+2147,Origin,Z,R);

  R = std::max(shaftCFRingRadius,shaft2StepConnectionRadius)+voidThick;
  ModelSupport::buildCylinder(SMap,buildIndex+2157,Origin,Z,R);

  // Connection flange stiffeners
  // [ESS-0124024 page 19 (DW-TRGT-ESS-0102.05)]
  const double stiffTheta(atan(shaftUpperBigStiffHeight/shaftUpperBigStiffLength)*180.0/M_PI);
  const double stiffL(shaftRadius[nShaftLayers-2]+shaftUpperBigStiffLength);
  const double stiffH(tan(stiffTheta*M_PI/180)*stiffL+shaft2StepHeight);

  // cone divider
  ModelSupport::buildPlane(SMap,buildIndex+6148,Origin+Z*stiffH,Z);  // cone divider
  ModelSupport::buildCone(SMap,buildIndex+2148,Origin+Z*stiffH,Z,90.0-stiffTheta);

  if (engActive) // create surfaces for large upper/lower stiffeners
    createRadialSurfaces(buildIndex+3000, nSectors/2, shaftUpperBigStiffThick);

  H = stiffH+voidThick/cos(stiffTheta);
  ModelSupport::buildPlane(SMap,buildIndex+6149,Origin+Z*H,Z);  // cone divider
  ModelSupport::buildCone(SMap,buildIndex+2149,Origin+Z*H,Z,90.0-stiffTheta);

  H = H1-voidThick;
  H += shaft2StepConnectionDist;
  ModelSupport::buildPlane(SMap,buildIndex+2166,Origin+Z*H,Z);

  H += shaft2StepConnectionHeight;
  ModelSupport::buildPlane(SMap,buildIndex+2176,Origin+Z*H,Z);

  H += voidThick;
  ModelSupport::buildPlane(SMap,buildIndex+2186,Origin+Z*H,Z);

  R = shaft2StepConnectionRadius;
  ModelSupport::buildCylinder(SMap,buildIndex+2127,Origin,Z,R);

  // shaft base
  H = shaftBaseDepth;
  const Geometry::Plane *p2205 =
    ModelSupport::buildPlane(SMap,buildIndex+2205,Origin-Z*H,Z);

  // shaft base - catcher
  H = shaft2StepHeight + shaftLowerBigStiffHeight + catcherNotchBaseThick;
  const Geometry::Plane *p2215 =
    ModelSupport::buildPlane(SMap,buildIndex+2215,Origin-Z*H,Z);

  //  H -= catcherNotchBaseThick;
  H = shaft2StepHeight + shaftLowerBigStiffHeight;
  ModelSupport::buildPlane(SMap,buildIndex+2225,Origin-Z*H,Z);

  H = shaftBaseDepth-catcherBaseHeight;
  ModelSupport::buildPlane(SMap,buildIndex+2245,Origin-Z*H,Z);

  R = catcherBaseRadius-2*catcherBaseHeight/tan(catcherBaseAngle*M_PI/180);
  ModelSupport::buildCylinder(SMap,buildIndex+2207,Origin,Z,R);

  H = shaftBaseDepth-catcherBaseRadius*tan(catcherBaseAngle*M_PI/180);
  ModelSupport::buildPlane(SMap,buildIndex+6208,Origin-Z*H,Z);  // cone divider
  ModelSupport::buildCone(SMap, buildIndex+2208, Origin-Z*H,
			  Z, 90.0-catcherBaseAngle);

  const double gap(p2215->getDistance()-p2205->getDistance());
  H -= gap/sin(catcherBaseAngle*M_PI/180);
  ModelSupport::buildPlane(SMap,buildIndex+6218,Origin-Z*H,Z);  // cone divider
  ModelSupport::buildCone(SMap, buildIndex+2218,Origin-Z*H,
			  Z,90.0-catcherBaseAngle);

  R = catcherNotchRadius;
  ModelSupport::buildCylinder(SMap,buildIndex+2217,Origin,Z,R);

  const Geometry::Vec3D nearPt(0,0,0);
  R += catcherNotchBaseThick;
  ModelSupport::buildCylinder(SMap,buildIndex+2227,Origin,Z,R);

  R = shaftRadius[nShaftLayers-2] + shaftLowerBigStiffShortLength;
  ModelSupport::buildCylinder(SMap,buildIndex+2237,Origin,Z,R);

  const Geometry::Vec3D C(SurInter::getPoint(SMap.realSurfPtr(buildIndex+2225),
					     SMap.realSurfPtr(buildIndex+2237),
					     px,nearPt));

  const double L(Origin.Y()-C.Y());
  const double lowerBigStiffTheta(atan(shaftLowerBigStiffHeight/
      (shaftLowerBigStiffLongLength-shaftLowerBigStiffShortLength))*180/M_PI);

  H = L * tan(lowerBigStiffTheta*M_PI/180)-C.Z();
  ModelSupport::buildPlane(SMap,buildIndex+6238,Origin-Z*H,Z);  // cone divider
  ModelSupport::buildCone(SMap, buildIndex+2238, Origin-Z*H,
			  -Z, 90.0-lowerBigStiffTheta);

  H+= voidThick/cos(lowerBigStiffTheta*M_PI/180);
  ModelSupport::buildPlane(SMap,buildIndex+6239,Origin-Z*H,Z);  // cone divider
  ModelSupport::buildCone(SMap, buildIndex+2239, Origin-Z*H,
			  -Z, 90.0-lowerBigStiffTheta);

  R += voidThick;
  ModelSupport::buildCylinder(SMap,buildIndex+2247,Origin,Z,R);

  // circle of pipes
  R = circlePipesBigRad-circlePipesRad-circlePipesWallThick;
  ModelSupport::buildCylinder(SMap,buildIndex+2307,Origin,Z,R);
  R = circlePipesBigRad+circlePipesRad+circlePipesWallThick;
  ModelSupport::buildCylinder(SMap,buildIndex+2317,Origin,Z,R);

  int SJ(buildIndex+2300);
  double theta(0.0), x0, y0;
  const double dTheta(360.0/static_cast<double>(nSectors));
  R = circlePipesBigRad;
  for (size_t j=0; j<nSectors; j++)
    {
      x0 = R*sin(theta * M_PI/180.0);
      y0 = -R*cos(theta * M_PI/180.0);
      ModelSupport::buildCylinder(SMap,SJ+8,Origin+X*x0+Y*y0,Z,circlePipesRad);
      ModelSupport::buildCylinder(SMap,SJ+9,Origin+X*x0+Y*y0,Z,
				  circlePipesRad+circlePipesWallThick);
      // dummy plane to separate voids between circles:
      ModelSupport::buildPlaneRotAxis(SMap, SJ+1, Origin, X, Z, theta-dTheta/2);

      theta += dTheta;
      SJ+=10;
    }
  // add 1st surface again with reversed normal - to simplify building cells
  SMap.addMatch(SJ+1,SMap.realSurf(buildIndex+2301));

  return;
}

void
BilbaoWheel::createShaftObjects(Simulation& System)
  /*!
    Construct the objects
    \param System :: Simulation object
   */
{
  ELog::RegMethod RegA("BilbaoWheel","createShaftObjects");
  HeadRule HR;

  // layer with circle of pipes
  buildCirclePipes(System,
		   ModelSupport::getHeadRule
		   (SMap,buildIndex,buildIndex+20,"2007M -7"),
		   ModelSupport::getHeadRule(SMap,buildIndex,"115 -105"),
		   ModelSupport::getHeadRule(SMap,buildIndex,"106 -116"),
		   ModelSupport::getHeadRule(SMap,buildIndex,"105 -106"),
		   ModelSupport::getHeadRule(SMap,buildIndex,"115 -116"));
  // void below
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,buildIndex+20,"-7 35 -115 2127 ");
  System.addCell(cellIndex++,0,mainTemp,HR);

  const int SJ(buildIndex+(static_cast<int>(nShaftLayers)-1)*10);

  // steel inside - outer radial part
  HR=ModelSupport::getHeadRule(SMap,buildIndex,buildIndex+20,SJ-10,
			       "35 -115 -2127 2007M");
  System.addCell(cellIndex++,steelMat,mainTemp,HR);
  // steel inside - central part
  HR=ModelSupport::getHeadRule(SMap,buildIndex,buildIndex+20,SJ-10,
				 "35 -5 -2007M");
  System.addCell(cellIndex++,steelMat,mainTemp,HR);

  // lower cell
  HR=ModelSupport::getHeadRule(SMap,buildIndex, "-7 2115 -2135");
  System.addCell(cellIndex++,steelMat,mainTemp,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex, "-7 2135 -35");
  System.addCell(cellIndex++,0,mainTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, "7 -17 2115 -25");
  System.addCell(cellIndex++,steelMat,mainTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, "17 -2118 2115 -45");
  System.addCell(cellIndex++,0,mainTemp,HR);

  // 2nd void step
  // upper cell - inner steel - outer side layer
  HR=ModelSupport::getHeadRule(SMap,buildIndex,SJ,
				 "7 -17 26 -2116 2007M");
  System.addCell(cellIndex++,steelMat,mainTemp,HR);
  // upper cell - inner steel - top layer
  HR=ModelSupport::getHeadRule(SMap,buildIndex,SJ-10,"-7 2136 -2116 2007M");
  System.addCell(cellIndex++,steelMat,mainTemp,HR);

  // upper cell - inner steel - inner void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,buildIndex+20,
				 "-7 116 -2136 2007M");
  System.addCell(cellIndex++,0,mainTemp,HR);

  // upper cell - void around inner steel - vertical part
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2118 17 46 -2116");
  System.addCell(cellIndex++,0,mainTemp,HR);

  // upper/lower connection
  HR=ModelSupport::getHeadRule(SMap,buildIndex,SJ-10,
				 "-2127 2007M 2166 -2176");
  System.addCell(cellIndex++,steelMat,mainTemp,HR);

  // shaft outer surface
  const HeadRule RsurfHR=ModelSupport::getHeadRule(SMap,SJ-10,"2007");

  // Connection flange:
  //   stiffener
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2116 -2146 -2148 -6148");
  if (engActive)
    buildStiffeners(System,HR*RsurfHR,buildIndex+3000);
  else
    System.addCell(cellIndex++,shaftUpperBigStiffHomoMat,mainTemp,HR*RsurfHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2116 -2126 2148 -6148 -2118 2157");
  System.addCell(cellIndex++,0,mainTemp,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex, "2126 2148 -6148 -2149 -6149 2157");
  System.addCell(cellIndex++,0,mainTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, "2116 -2146 2148 -6148 -2157");
  System.addCell(cellIndex++,0,mainTemp,HR);

  //   ring
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2146 -2156 -2147");
  System.addCell(cellIndex++,steelMat,mainTemp,HR*RsurfHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2146 -2156 2147 -2157");
  System.addCell(cellIndex++,0,mainTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2156 -2166 -2157");
  System.addCell(cellIndex++,0,mainTemp,HR*RsurfHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2166 -2186 2127 -2157");
  System.addCell(cellIndex++,0,mainTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,SJ-10,
				 "-2127 2007M 2176 -2186");
  System.addCell(cellIndex++,0,mainTemp,HR);

  // wheel catcher

  HR=ModelSupport::getHeadRule(SMap,buildIndex, "2247 2238 6238 -2239 6239 -2125");
  System.addCell(cellIndex++,0,mainTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, "2125 -2115  2238 6238 -2118");
  System.addCell(cellIndex++,0,mainTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, " 2237 -2247 2238 6238 2205");
  System.addCell(cellIndex++,0,mainTemp,HR);

  //     floor gap
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2147 2208 -6208 2205 -2215");
  System.addCell(cellIndex++,0,mainTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2205 -2245 -2208 -6208");
  System.addCell(cellIndex++,steelMat,mainTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2217 2205 -2245 2208 -6208");
  System.addCell(cellIndex++,0,mainTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2217 2205 2245 -2115 ");
  System.addCell(cellIndex++,0,mainTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2227 2217 2215 -2115 2218 -6218");
  System.addCell(cellIndex++,steelMat,mainTemp,HR);

  // small cell between base cones
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2217 -2218 -6218 2208 -6208 2215 ");
  System.addCell(cellIndex++,0,mainTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, " 2227 -2237 2215 -2225 ");
  System.addCell(cellIndex++,steelMat,mainTemp,HR);

  // notch: big conical cell
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2238 6238 2227 2225 -2115");
  if (engActive)
    buildStiffeners(System,HR,buildIndex+3000);
  else
    System.addCell(cellIndex++,shaftLowerBigStiffHomoMat,mainTemp,HR);


  // shaft layers
  int SI(buildIndex);
  const HeadRule roofHR(SMap,buildIndex,-2006);

  HeadRule floorHR,innerHR;
  for (size_t i=0; i<nShaftLayers; i++)
    {
      const HeadRule outerHR=HeadRule(SMap,SI,-2007);

      if (i<3)
	floorHR = HeadRule(SMap,buildIndex,5);
      else if (i==nShaftLayers-1)
	floorHR = HeadRule(SMap,buildIndex,2186);
      else
	floorHR = HeadRule(SMap,buildIndex,2136);

      if (i==2)
	{
	  buildHoles(System,innerHR*outerHR,floorHR,roofHR,
		     shaftMat[i],shaftHoleSize,shaftHoleXYangle,shaftHoleHeight,
		     0.0, 1000);
	  SI += 10;
	}
      else
	{
	  System.addCell(cellIndex++,shaftMat[i],mainTemp,
			 innerHR*outerHR*floorHR*roofHR);
	  SI += 10;
	}
      innerHR=outerHR.complement();
    }

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,SI-10,
     " (-2007M -2006 2125) : "    // connection
     " (2126 -2149 -6149) : "     // upper stiffeners
     " (2126 -2186 -2157) : "     // above upper stiffeners
     " (-2118 2125 -2126) : "     // 2nd step
     " (((-2239 6239):-2247) 2205 -2125)"); // base
  addOuterSurf("Shaft",HR);
  return;
}

std::string
BilbaoWheel::getSQSurface(const double R, const double e)
  /*
    Return MCNP(X) surface card for SQ ellipsoids
  */
{
  std::ostringstream cx;
  cx<<"sq "<<1.0/(R*R)<<" "<<1.0/(R*R)<<" "<<
    e<<" 0 0 0 -1 "<<Origin;
  return cx.str();
}


void
BilbaoWheel::createRadialSurfaces(const int SI, const size_t n,const double w)
/*!
  Create planes for radial plate surfaces (stiffeners)
  \param SI :: surface offset
  \param n  :: number of sectors/plates
  \param w  :: plate width (if 0 then only surfaces for sectors are defined)
 */
{
  ELog::RegMethod RegA("BilbaoWheel","createRadialSurfaces");

  if (n<2) return;

  int SJ(SI);
  double theta(0.0);
  const double dTheta = 360.0/static_cast<double>(n);

  for (size_t j=0; j<n; j++)
    {
      if (w<Geometry::zeroTol) // just divide radial
	{
	  ModelSupport::buildPlaneRotAxis(SMap, SJ+1, Origin, X, Z, theta);
	}
      else // plates
	{
	  Geometry::Vec3D myX(X);
	  Geometry::Vec3D divider(-Y);
	  Geometry::Quaternion::calcQRotDeg(theta,Z).rotate(myX);
	  Geometry::Quaternion::calcQRotDeg(theta,Z).rotate(divider);

	  ModelSupport::buildPlane(SMap,SJ+2,Origin,divider);
	  ModelSupport::buildPlane(SMap,SJ+3,Origin-myX*w/2.0,myX);
	  ModelSupport::buildPlane(SMap,SJ+4, Origin+myX*w/2.0,myX);
	}
      theta += dTheta;
      SJ += 10;
    }
  // add 1st surface(s) again with reversed normal - to simplify building cells
  if (w<Geometry::zeroTol)
    SMap.addMatch(SJ+1,SMap.realSurf(SI+1));
  else
    {
      SMap.addMatch(SJ+3,SMap.realSurf(SI+3));
      SMap.addMatch(SJ+4,SMap.realSurf(SI+4));
    }
  return;
}

void
BilbaoWheel::divideRadial(Simulation& System,
			  const HeadRule& sidesHR,
			  const int mat)
/*!
    Divide wheel by sectors to help cell splitting
    \param System :: Simulation
    \param sides :: top/bottom and side surfaces
    \param mat :: material
  */
{
  ELog::RegMethod RegA("BilbaoWheel","divideRadial");

  if (nSectors<2)
    {
      System.addCell(cellIndex++,mat,mainTemp,sidesHR);
      return;
    }

  HeadRule HR;
  int SJ(buildIndex+3000);
  for(size_t j=0;j<nSectors;j++)
    {
      HR=ModelSupport::getHeadRule(SMap,SJ,"1 -11");
      System.addCell(cellIndex++,mat,mainTemp,HR*sidesHR);
      SJ+=10;
    }
  return;
}

void
BilbaoWheel::buildStiffeners(Simulation& System,
			     const HeadRule sidesHR,
			     const int SI)
  /*!
    Divide wheel by sectors to help cell splitting
    \param System :: Simulation
    \param sides :: top/bottom and side surfaces
    \param SI :: surface offset
  */
{
  ELog::RegMethod RegA("BilbaoWheel","buildStiffeners");

  HeadRule HR;
  if (nSectors<2)
    {
      System.addCell(cellIndex++,steelMat,mainTemp,sidesHR);
      return;
    }

  int SJ(SI);
  for(size_t j=0;j<nSectors/2;j++)
    {
      HR=ModelSupport::getHeadRule(SMap,SJ,"2 3 -4");
      System.addCell(cellIndex++,steelMat,mainTemp,HR*sidesHR);

      HR=ModelSupport::getHeadRule(SMap,SJ,"2 4 -13");
      System.addCell(cellIndex++,heMat,mainTemp,HR*sidesHR);

      SJ+=10;
    }
  return;
}

void
BilbaoWheel::buildHoles(Simulation& System,
			const HeadRule& sidesHR,
			const HeadRule& baseHR,
			const HeadRule& topHR,
			const int mat,
			const double hs,
			const double xyAngle,
			const double height,
			const double z0,
			const int surfOffset)
/*!
  Create surfaces and cells for the holes in the given layer
  \param System :: Simulation
  \param sides  :: side surfaces
  \param bot    :: bottom surface
  \param top    :: top surface
  \param mat    :: material
  \param hs     :: hole size (fraction of dTheta)
  \param xyAngle :: XY offset
  \param height :: hole height
  \param z0     :: center of the holes along the axis collinear with 'height'
  \param surfOffset :: surface offset (needed since we call this method several times)
 */
{
  ELog::RegMethod RegA("BilbaoWheel","buildHoles");

  if (nSectors<2)
    {
      System.addCell(cellIndex++,mat,mainTemp,sidesHR*topHR*baseHR);
      return;
    }
  const int SI0(buildIndex+30000+surfOffset);
  const int SIstep(10);

  // create surfaces
  int SI(SI0);
  double theta(xyAngle);

  // angular length of hole+mat
  const double dTheta = 360.0/static_cast<double>(nSectors);
  const double dThetaHole = dTheta * hs; // angular length of the hole

  for (size_t j=0; j<nSectors; j++)
    {
      theta += dTheta;
      ModelSupport::buildPlaneRotAxis(SMap, SI+1, Origin, X, Z, theta);
      ModelSupport::buildPlaneRotAxis
	(SMap, SI+2, Origin, X, Z, theta+dThetaHole);

      SI += SIstep;
    }
  // add 1st surface again with reversed normal - to simplify building cells
  SMap.addMatch(SI+1,SMap.realSurf(SI0+1));

  HeadRule HR;
  SI=SI0;

  if (height>0.0)
    {
      // hole top/bottom
      ModelSupport::buildPlane(SMap,SI0+5,Origin+Z*(z0-height/2.0),Z);
      ModelSupport::buildPlane(SMap,SI0+6,Origin+Z*(z0+height/2.0),Z);

      // cell below holes
      HR=HeadRule(SMap,SI0,-5);
      System.addCell(cellIndex++,mat,mainTemp,HR*baseHR*sidesHR);

      // cell above holes
      HR=HeadRule(SMap,SI0,6);
      System.addCell(cellIndex++,mat,mainTemp,HR*sidesHR*topHR);

      // holes
      for(size_t j=0;j<nSectors;j++)
	{
	  HR=ModelSupport::getHeadRule(SMap,SI,SI0,"1 -2 5M -6M");
	  System.addCell(cellIndex++,0,mainTemp,HR*sidesHR);

	  HR=ModelSupport::getHeadRule(SMap,SI,SI0,"2 -11 5M -6M");
	  System.addCell(cellIndex++,mat,mainTemp,HR*sidesHR);

	  SI+=SIstep;
	}
    } else // if height <= 0 => build full height holes
    {
      for(size_t j=0;j<nSectors;j++)
	{
	  HR=ModelSupport::getHeadRule(SMap,SI,SI0,"1 -2");
	  System.addCell(cellIndex++,0,mainTemp,HR*sidesHR*topHR*baseHR);

	  HR=ModelSupport::getHeadRule(SMap,SI,SI0,"2 -11");
	  System.addCell(cellIndex++,mat,mainTemp,HR*sidesHR*topHR*baseHR);

	  SI+=SIstep;
	}
    }
  return;
}

void
BilbaoWheel::buildCirclePipes(Simulation& System,
			      const HeadRule& sidesHR,
			      const HeadRule& floorHR,
			      const HeadRule& roofHR,
			      const HeadRule& innerHR,
			      const HeadRule& outerHR)
  /*!
    Build circle of pipes
    \param System :: Simulation
    \param sides  :: side surfaces
    \param floor :: floor rule (top+bottom)
    \param roof :: roof rule (top+bottom)
    \param inner :: inner rule (top+bottom)
    \param outer :: outer rule (top+bottom)
  */
{
  ELog::RegMethod RegA("BilbaoWheel","buildCirclePipes");


  if (nSectors<2)
    {
      System.addCell(cellIndex++,innerMat,mainTemp,sidesHR*innerHR);
      return;
    }

  HeadRule HR;
  
  int SJ(buildIndex+2300);
  for (size_t j=0; j<nSectors; j++)
    {
      HR=HeadRule(SMap,SJ,-8);
      System.addCell(cellIndex++,innerMat,mainTemp,HR*outerHR);
  
      HR=ModelSupport::getHeadRule(SMap,SJ,"8 -9");
      System.addCell(cellIndex++,steelMat,mainTemp,HR*outerHR);
      
      HR=ModelSupport::getHeadRule(SMap,SJ,SJ+10,"9 1 -1M");
      System.addCell(cellIndex++,innerMat,mainTemp,
					HR*sidesHR*innerHR);
      
      HR=ModelSupport::getHeadRule(SMap,SJ,SJ+10,"9 1 -1M");
      System.addCell(cellIndex++,steelMat,mainTemp,
					HR*sidesHR*floorHR);
      System.addCell(cellIndex++,steelMat,mainTemp,
					HR*sidesHR*roofHR);
      SJ+=10;
    }
  return;
}

void
BilbaoWheel::createSurfaces()
  /*!
    Create surfaces for Wheel but not Shaft
  */
{
  ELog::RegMethod RegA("BilbaoWheel","createSurfaces");

  // Dividing surface
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin,X);

  double H(targetHeight/2.0);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*H,Z);

  H+= voidTungstenThick;
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*H,Z);

  H+= steelTungstenThick;
  ModelSupport::buildPlane(SMap,buildIndex+25,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*H,Z);

  H+=coolantThick;
  ModelSupport::buildPlane(SMap,buildIndex+35,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,buildIndex+36,Origin+Z*H,Z);

  H+=caseThick;
  ModelSupport::buildPlane(SMap,buildIndex+245,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,buildIndex+246,Origin+Z*H,Z);

  H+=voidThick;
  ModelSupport::buildPlane(SMap,buildIndex+45,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,buildIndex+46,Origin+Z*H,Z);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,innerRadius);

  H  = targetHeight/2.0;
  H += voidTungstenThick;
  H += steelTungstenThick;
  H += coolantThick;
  H += caseThick;
  ModelSupport::buildPlane(SMap,buildIndex+125,Origin-Z*H,Z);  // 22 : 23
  ModelSupport::buildPlane(SMap,buildIndex+126,Origin+Z*H,Z);


  int SI(buildIndex+10);
  for(size_t i=0;i<nLayers;i++)
    {
      ModelSupport::buildCylinder(SMap,SI+7,Origin,Z,radius[i]);
      SI+=10;
    }

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  Geometry::General *GA;

  GA = SurI.createUniqSurf<Geometry::General>(buildIndex+517);
  GA->setSurface(getSQSurface(coolantRadiusOut, aspectRatio));
  SMap.registerSurf(GA);

  GA = SurI.createUniqSurf<Geometry::General>(buildIndex+527);
  GA->setSurface(getSQSurface(caseRadius, aspectRatio));
  SMap.registerSurf(GA);

  Geometry::EllipticCyl* ECPtr=
    ModelSupport::buildEllipticCyl(SMap,buildIndex+528,Origin,Z,
				   X,caseRadius,caseRadius);

  GA->normalizeGEQ(0);
  //  ELog::EM<<"XXN ="<<*GA<<ELog::endDiag;
  //  ELog::EM<<"ASPECT ="<<1/aspectRatio<<ELog::endDiag;
  ECPtr->normalizeGEQ(0);
  //  ELog::EM<<"YY ="<<*ECPtr<<ELog::endDiag;

  ModelSupport::buildCylinder(SMap,buildIndex+537,Origin,Z,voidRadius);

  createRadialSurfaces(buildIndex+3000, nSectors);


  // inner vertical layers
  H=targetInnerHeight/2.0;
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*H,Z);
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Z,
			      targetInnerHeightRadius);

  H+=steelTungstenInnerThick;
  ModelSupport::buildPlane(SMap,buildIndex+115,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,buildIndex+116,Origin+Z*H,Z);
  ModelSupport::buildCylinder(SMap,buildIndex+117,Origin,Z,
			      targetInnerHeightRadius+steelTungstenInnerThick);

  return;
}

void
BilbaoWheel::createObjects(Simulation& System)
  /*!
    Create the wheel cells
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("BilbaoWheel","createObjects");

  const int matNum[4]={0,steelMat,heMat,homoWMat};
  std::string Out, Out1;
  //
  // Loop through each item and build inner section
  //

  int SI(buildIndex);
  int nInner(0); // number of inner cells (must be 1)
  int mat(0);
  HeadRule HR,sideHR;
  for(size_t i=0;i<nLayers;i++)
    {
      mat = matNum[matTYPE[i]];
      sideHR = ModelSupport::getHeadRule(SMap,SI," 7 -17 ");

      if (i==0)
	{
	  buildHoles(System,sideHR,
		     HeadRule(SMap,buildIndex,116),
		     HeadRule(SMap,buildIndex,-26),
		     mat,innerHoleSize,innerHoleXYangle,-1,
		     0.0, 4000);

	  buildHoles(System,sideHR,
		     HeadRule(SMap,buildIndex,25),
		     HeadRule(SMap,buildIndex,-115),
		     mat,innerHoleSize,innerHoleXYangle,-1,
		     0.0, 5000);


	  buildHoles(System,sideHR,
		     HeadRule(SMap,buildIndex,115),
		     HeadRule(SMap,buildIndex,-116),
		     mat,innerHoleSize,innerHoleXYangle,innerHoleHeight,
		     0.0, 0);
	}
      else if (i==1)
	{
	  // Layer before Tungsten bricks:
	  // Do not build it with engActive=1 since the cassette goes there
	  if (!engActive)
	    {
	      HR=ModelSupport::getHeadRule(SMap,buildIndex,SI,"117 -17M 5 -6");
	      System.addCell(cellIndex++,mat,mainTemp,HR);
	    }

	  sideHR=ModelSupport::getHeadRule(SMap,buildIndex,"-117 107");

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"105 -106");
	  System.addCell(cellIndex++,mat,mainTemp,HR*sideHR);
	  
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"106 -26");
	  System.addCell(cellIndex++,steelMat,mainTemp,HR*sideHR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-105 25");
	  System.addCell(cellIndex++,steelMat,mainTemp,HR*sideHR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-25 35");
	  System.addCell(cellIndex++,mat,mainTemp,HR*sideHR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"26 -36");
	  System.addCell(cellIndex++,mat,mainTemp,HR*sideHR);

	  sideHR=ModelSupport::getHeadRule(SMap,buildIndex,SI,"7M -107");
	  
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"105 -106");
	  System.addCell(cellIndex++,mat,mainTemp,HR*sideHR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"106 -116");
	  System.addCell(cellIndex++,steelMat,mainTemp,HR*sideHR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-105 115");
	  System.addCell(cellIndex++,steelMat,mainTemp,HR*sideHR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"116 -36");
	  System.addCell(cellIndex++,mat,mainTemp,HR*sideHR);

	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-115 35");
	  System.addCell(cellIndex++,mat,mainTemp,HR*sideHR);
	}
      else if (mat==homoWMat)
	{
	  if (nInner>0)
	    ELog::EM << "More than one spallation layer" << ELog::endErr;

	  // Do not build the layer with Tungsten if !engActive
	  // It is created by the buildSectors() method later.
	  if (!engActive)
	    {
	      //divideRadial(System, Out, mat); // geocheck fails, but MCNP - not. why???
	      HR=ModelSupport::getHeadRule(SMap,buildIndex,SI,"5 -6");
	      System.addCell(cellIndex++,mat,mainTemp,HR*sideHR);
	    }
	  nInner++;
	}
      else // never actually called with standard geometry since nLayers=3
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,SI,"5 -6");
	  System.addCell(cellIndex++,mat,mainTemp,HR*sideHR);
	}

      SI+=10;
    }

  // front coolant:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,SI," 7M -517 35 -36");
  if (engActive)
    divideRadial(System,HR,heMat);
  else
    System.addCell(cellIndex++,heMat,mainTemp,HR);

  sideHR=ModelSupport::getHeadRule(SMap,buildIndex,SI,"-7M 117");

  // Void above W
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"6 -16");
  System.addCell(cellIndex++,0,mainTemp,HR*sideHR);
  // Void below W
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"15 -5");
  System.addCell(cellIndex++,0,mainTemp,HR*sideHR);

  // Steel above W
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"16 -26");
  System.addCell(cellIndex++,steelMat,mainTemp,HR*sideHR);

  // Steel below W
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"25 -15");
  System.addCell(cellIndex++,steelMat,mainTemp,HR*sideHR);

  // Coolant above steel
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"26 -36");
  makeCell("CoolantAboveSteel",System,cellIndex++,
	   ssVoidMat,mainTemp,HR*sideHR);

  // Coolant below steel
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"35 -25" );
  System.addCell(cellIndex++,ssVoidMat,mainTemp,HR*sideHR);

  // Metal surround [ UNACCEPTABLE JUNK CELL]
  // Metal front:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-527 517 35 -36");
  if (engActive)
    divideRadial(System, HR, steelMat);
  else
    System.addCell(cellIndex++,steelMat,mainTemp,HR);

  // forward Main sections:
  sideHR=ModelSupport::getHeadRule(SMap,buildIndex,"-527 17");

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-126 36");  // outer above W
  System.addCell(cellIndex++,steelMat,mainTemp,HR*sideHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 125 -35 ");
  System.addCell(cellIndex++,steelMat,mainTemp,HR*sideHR); // outer below W

  // Void surround
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"17 45 -46 -537 (-125:126:527)");
  if (engActive)
    divideRadial(System,HR,0);
  else
    System.addCell(cellIndex++,0,mainTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-537 45 -46");
  addOuterSurf("Wheel",HR);

  return;
}


void
BilbaoWheel::createLinks()
  /*!
    Creates a full attachment set
    Surfaces pointing outwards
  */
{
  ELog::RegMethod RegA("BilbaoWheel","createLinks");
  // set Links :: Inner links:

  FixedComp::setConnect(0,Origin-Y*innerRadius,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+537));
  FixedComp::setBridgeSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*innerRadius,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+537));
  FixedComp::setBridgeSurf(1,SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(2,Origin-Y*voidRadius,-Y);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+537));
  FixedComp::setBridgeSurf(2,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(3,Origin+Y*voidRadius,Y);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+537));
  FixedComp::setBridgeSurf(3,SMap.realSurf(buildIndex+1));

  const double H=wheelHeight()/2.0;
  FixedComp::setConnect(4,Origin-Z*H,-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+45));

  FixedComp::setConnect(5,Origin+Z*H,Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+46));


  nameSideIndex(4,"VoidBase");
  nameSideIndex(5,"VoidTop");
  
  // inner links (normally) point towards
  // top/bottom of the spallation material (innet cell)
  const double TH=targetHeight/2.0;
  FixedComp::setConnect(6,Origin-Z*TH,Z);
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(7,Origin+Z*TH,-Z);
  FixedComp::setLinkSurf(7,-SMap.realSurf(buildIndex+6));

  FixedComp::setConnect(12, Origin-Y*(radius[0]+voidThick),-Y);
  FixedComp::setLinkSurf(12, SMap.realSurf(buildIndex+2118));
  FixedComp::setBridgeSurf(12,-SMap.realSurf(buildIndex+1));

  int SI(buildIndex);
  size_t i;
  for (i=0; i<nLayers && matTYPE[i]!=3; SI+=10,i++) ;
  if (matTYPE[i]==3) // Tungsten layer
    {
      // 8 and 9 - the layer before Tungsten (He)
      FixedComp::setConnect(8, Origin-Y*(targetInnerHeightRadius +
					 steelTungstenInnerThick), -Y);
      FixedComp::setLinkSurf(8, SMap.realSurf(buildIndex+117));
      
      FixedComp::setConnect(9, Origin-Y*radius[i-2], Y);
      FixedComp::setLinkSurf(9, -SMap.realSurf(SI-10+7));
      
      // 10 and 11 - Tungsten layer
      FixedComp::setConnect(10, Origin-Y*radius[i-1], -Y);
      FixedComp::setLinkSurf(10, SMap.realSurf(SI+7));
      
      FixedComp::setConnect(11, Origin-Y*radius[i], Y);
      FixedComp::setLinkSurf(11, -SMap.realSurf(SI+17));
    }

  FixedComp::setConnect(13, Origin-Y*voidRadius,Y);
  FixedComp::setLinkSurf(13,SMap.realSurf(buildIndex+537));
  nameSideIndex(13,"VoidRadius");
  
  return;
}

void
BilbaoWheel::buildSectors(Simulation& System) const
  /*!
    Build Tungsten sectors. Must be called after createLinks.
    \param Systew :: Simulation to use
   */
{
  ELog::RegMethod RegA("BilbaoWheel","buildSectors");

  ModelSupport::objectRegister& OR=ModelSupport::objectRegister::Instance();

  const HeadRule innerHR=getFullRule(9);
  const HeadRule vHR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -6");
  const HeadRule fHR=getFullRule(12);

  for (size_t i=0; i<nSectors; i++)
    {
      std::shared_ptr<BilbaoWheelCassette>
	cassetteUnit(new BilbaoWheelCassette(keyName,"Sec",i));
      OR.addObject(cassetteUnit);
      cassetteUnit->setRotAngle(static_cast<double>(i)*360.0/
			     static_cast<double>(nSectors));
      cassetteUnit->setLinkCopy("Inner",*this,9);
      cassetteUnit->setLinkCopy("Outer",*this,12);
      cassetteUnit->setCutSurf("InnerCyl",innerHR);
      cassetteUnit->setCutSurf("VerticalCut",vHR);
      cassetteUnit->setCutSurf("OuterCyl",fHR);
      cassetteUnit->createAll(System,*this,0);
    }
}

void
BilbaoWheel::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: sideIndex
   */
{
  ELog::RegMethod RegA("BilbaoWheel","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createShaftSurfaces();

  createObjects(System);
  createShaftObjects(System);

  createLinks();
  if (engActive)
    buildSectors(System);

  insertObjects(System);

  return;
}

}  // NAMESPACE essSystem
