/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/BilbaoWheel.cxx
 *
 * Copyright (c) 2004-2018 by Konstantin Batkov
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
#include "EllipticCyl.h"
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
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "General.h"
#include "Plane.h"
#include "BaseMap.h"
#include "CellMap.h"
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
  engActive(A.engActive),
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
  shaftLowerBigStiffHomoMat(A.shaftLowerBigStiffHomoMat),
  shaftHoleHeight(A.shaftHoleHeight),
  shaftHoleSize(A.shaftHoleSize),
  shaftHoleXYangle(A.shaftHoleXYangle),
  shaftBaseDepth(A.shaftBaseDepth),
  catcherTopSteelThick(A.catcherTopSteelThick),
  catcherGap(A.catcherGap),
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
      engActive=A.engActive;
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
      shaftLowerBigStiffHomoMat=A.shaftLowerBigStiffHomoMat;
      shaftHoleHeight=A.shaftHoleHeight;
      shaftHoleSize=A.shaftHoleSize;
      shaftHoleXYangle=A.shaftHoleXYangle;
      shaftBaseDepth=A.shaftBaseDepth;
      catcherTopSteelThick=A.catcherTopSteelThick;
      catcherGap=A.catcherGap;
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
  FixedOffset::populate(Control);

  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");

  nSectors=Control.EvalDefVar<size_t>(keyName+"NSectors",3);
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  double R;
  int M;
  for(size_t i=0;i<nLayers;i++)
    {
      const std::string nStr=StrFunc::makeString(i+1);
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
  innerHoleXYangle=Control.EvalVar<double>(keyName+"InnerHoleXYangle");

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
      const std::string nStr=StrFunc::makeString(i+1);
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
  shaftHoleXYangle=Control.EvalVar<double>(keyName+"ShaftHoleXYangle");

  shaftBaseDepth=Control.EvalVar<double>(keyName+"ShaftBaseDepth");

  catcherTopSteelThick=Control.EvalVar<double>(keyName+"CatcherTopSteelThick");

  catcherGap=Control.EvalVar<double>(keyName+"CatcherGap");
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
  const Geometry::Plane *px = ModelSupport::buildPlane(SMap,wheelIndex+3,Origin,X);

  ModelSupport::buildPlane(SMap,wheelIndex+2006,Origin+Z*shaftHeight,Z);

  int SI(wheelIndex);
  for (size_t i=0; i<nShaftLayers; i++)
    {
      ModelSupport::buildCylinder(SMap,SI+2007,Origin,Z,shaftRadius[i]);
      SI+=10;
    }

  double H(wheelHeight()/2.0+caseThick);

  // 2nd void step
  H = shaft2StepHeight;
  ModelSupport::buildPlane(SMap,wheelIndex+2115,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,wheelIndex+2116,Origin+Z*H,Z);

  H += voidThick;
  //  ModelSupport::buildPlane(SMap,wheelIndex+2125,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,wheelIndex+2126,Origin+Z*H,Z);

  double H1 = H;

  H -= voidThick;
  H -= radius[0]-innerRadius;
  ModelSupport::buildPlane(SMap,wheelIndex+2135,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,wheelIndex+2136,Origin+Z*H,Z);

  double R = radius[0]+voidThick;
  ModelSupport::buildCylinder(SMap,wheelIndex+2118,Origin,Z,R);

  // Connection flange ring [ESS-0124024 pages 22-23 (DW-TRGT-ESS-0102)]
  H = H1-voidThick;
  H += shaftUpperBigStiffHeight;
  ModelSupport::buildPlane(SMap,wheelIndex+2146,Origin+Z*H,Z);
  H += shaftCFRingHeight;
  ModelSupport::buildPlane(SMap,wheelIndex+2156,Origin+Z*H,Z);
  R = shaftCFRingRadius;
  ModelSupport::buildCylinder(SMap,wheelIndex+2147,Origin,Z,R);

  R = std::max(shaftCFRingRadius,shaft2StepConnectionRadius)+voidThick;
  ModelSupport::buildCylinder(SMap,wheelIndex+2157,Origin,Z,R);

  // Connection flange stiffeners
  // [ESS-0124024 page 19 (DW-TRGT-ESS-0102.05)]
  const double stiffTheta(atan(shaftUpperBigStiffHeight/shaftUpperBigStiffLength)*180.0/M_PI);
  const double stiffL(shaftRadius[nShaftLayers-2]+shaftUpperBigStiffLength);
  const double stiffH(tan(stiffTheta*M_PI/180)*stiffL+shaft2StepHeight);
  ModelSupport::buildCone(SMap,wheelIndex+2148,Origin+Z*(stiffH),Z,90-stiffTheta,-1);

  if (engActive) // create surfaces for large upper/lower stiffeners
    createRadialSurfaces(wheelIndex+3000, nSectors/2, shaftUpperBigStiffThick);

  H = stiffH+voidThick/cos(stiffTheta);
  ModelSupport::buildCone(SMap,wheelIndex+2149,Origin+Z*(H),Z,90-stiffTheta,-1);

  H = H1-voidThick;
  H += shaft2StepConnectionDist;
  ModelSupport::buildPlane(SMap,wheelIndex+2166,Origin+Z*H,Z);

  H += shaft2StepConnectionHeight;
  ModelSupport::buildPlane(SMap,wheelIndex+2176,Origin+Z*H,Z);

  H += voidThick;
  ModelSupport::buildPlane(SMap,wheelIndex+2186,Origin+Z*H,Z);

  R = shaft2StepConnectionRadius;
  ModelSupport::buildCylinder(SMap,wheelIndex+2127,Origin,Z,R);

  // shaft base
  H = shaftBaseDepth;
  ModelSupport::buildPlane(SMap,wheelIndex+2205,Origin-Z*H,Z);

  // shaft base - catcher
  H -= catcherGap;
  ModelSupport::buildPlane(SMap,wheelIndex+2215,Origin-Z*H,Z);

  H -= catcherNotchBaseThick;
  ModelSupport::buildPlane(SMap,wheelIndex+2225,Origin-Z*H,Z);

  H = shaftBaseDepth-catcherBaseHeight;
  ModelSupport::buildPlane(SMap,wheelIndex+2245,Origin-Z*H,Z);

  R = catcherBaseRadius-2*catcherBaseHeight/tan(catcherBaseAngle*M_PI/180);
  ModelSupport::buildCylinder(SMap,wheelIndex+2207,Origin,Z,R);

  H = shaftBaseDepth-catcherBaseRadius*tan(catcherBaseAngle*M_PI/180);
  ModelSupport::buildCone(SMap, wheelIndex+2208, Origin-Z*(H),
			  Z, 90-catcherBaseAngle, -1);

  H -= catcherGap/sin(catcherBaseAngle*M_PI/180);
  ModelSupport::buildCone(SMap, wheelIndex+2218, Origin-Z*(H),
			  Z, 90-catcherBaseAngle, -1);

  R = catcherNotchRadius;
  ModelSupport::buildCylinder(SMap,wheelIndex+2217,Origin,Z,R);

  const Geometry::Vec3D nearPt(0,0,0);
  R += catcherNotchBaseThick;
  ModelSupport::buildCylinder(SMap,wheelIndex+2227,Origin,Z,R);

  R = shaftRadius[nShaftLayers-2] + shaftLowerBigStiffShortLength;
  ModelSupport::buildCylinder(SMap,wheelIndex+2237,Origin,Z,R);

  const Geometry::Vec3D C(SurInter::getPoint(SMap.realSurfPtr(wheelIndex+2225),
					     SMap.realSurfPtr(wheelIndex+2237),
					     px,nearPt));

  const double L(Origin.Y()-C.Y());
  H = L * tan(catcherBaseAngle*M_PI/180)-C.Z();
  ModelSupport::buildCone(SMap, wheelIndex+2238, Origin-Z*(H),
			  -Z, 90-catcherBaseAngle, -1);

  H+= voidThick/cos(catcherBaseAngle*M_PI/180);
  ModelSupport::buildCone(SMap, wheelIndex+2239, Origin-Z*(H),
			  -Z, 90-catcherBaseAngle, -1);

  R += voidThick;
  ModelSupport::buildCylinder(SMap,wheelIndex+2247,Origin,Z,R);

  // circle of pipes
  R = circlePipesBigRad-circlePipesRad-circlePipesWallThick;
  ModelSupport::buildCylinder(SMap,wheelIndex+2307,Origin,Z,R);
  R = circlePipesBigRad+circlePipesRad+circlePipesWallThick;
  ModelSupport::buildCylinder(SMap,wheelIndex+2317,Origin,Z,R);

  int SJ(wheelIndex+2300);
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
  SMap.addMatch(SJ+1,SMap.realSurf(wheelIndex+2301));

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
  std::string Out;

  // layer with circle of pipes
  buildCirclePipes(System,
		   ModelSupport::getComposite(SMap,wheelIndex,wheelIndex+20,
					      " 2007M -7 "),
		   ModelSupport::getComposite(SMap,wheelIndex," 115 -105 "),
		   ModelSupport::getComposite(SMap,wheelIndex," 106 -116 "),
		   ModelSupport::getComposite(SMap,wheelIndex," 105 -106 "),
		   ModelSupport::getComposite(SMap,wheelIndex," 115 -116 "));

  // void below
  Out=ModelSupport::getComposite(SMap,wheelIndex,wheelIndex+20,"-7 35 -115 2127 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  const int SJ(wheelIndex+(static_cast<int>(nShaftLayers)-1)*10);

  // steel inside - outer radial part
  Out=ModelSupport::getComposite(SMap,wheelIndex,wheelIndex+20,SJ-10,
				 " 35 -115 -2127 2007M ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));
  // steel inside - central part
  Out=ModelSupport::getComposite(SMap,wheelIndex,wheelIndex+20,SJ-10,
				 " 35 -5 -2007M ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  // lower cell
  Out=ModelSupport::getComposite(SMap,wheelIndex, " -7 2115 -2135" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));
  Out=ModelSupport::getComposite(SMap,wheelIndex, " -7 2135 -35" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex, " 7 -17 2115 -25" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex, " 17 -2118 2115 -45 " );
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  // 2nd void step
  // upper cell - inner steel - outer side layer
  Out=ModelSupport::getComposite(SMap,wheelIndex,SJ,
				 " 7 -17 26 -2116 2007M" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));
  // upper cell - inner steel - top layer
  Out=ModelSupport::getComposite(SMap,wheelIndex,SJ-10," -7 2136 -2116 2007M" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  // upper cell - inner steel - inner void
  Out=ModelSupport::getComposite(SMap,wheelIndex,wheelIndex+20,
				 " -7 116 -2136 2007M" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  // upper cell - void around inner steel - vertical part
  Out=ModelSupport::getComposite(SMap,wheelIndex, " -2118 17 46 -2116 " );
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  // upper/lower connection
  Out=ModelSupport::getComposite(SMap,wheelIndex,SJ-10,
				 " -2127 2007M 2166 -2176 " );
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  // shaft outer surface
  const std::string Rsurf(ModelSupport::getComposite(SMap,SJ-10," 2007 "));

  // Connection flange:
  //   stiffener
  Out=ModelSupport::getComposite(SMap,wheelIndex, " 2116 -2146 -2148 ");
  if (engActive)
    buildStiffeners(System,Out+Rsurf,wheelIndex+3000,nSectors/2,steelMat);
  else
    System.addCell(MonteCarlo::Qhull(cellIndex++,shaftUpperBigStiffHomoMat,mainTemp,Out+Rsurf));

  Out=ModelSupport::getComposite(SMap,wheelIndex, " 2116 -2126 2148 -2118 2157 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));
  Out=ModelSupport::getComposite(SMap,wheelIndex, " 2126 2148 -2149 2157 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex, " 2116 -2146 2148 -2157 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  //   ring
  Out=ModelSupport::getComposite(SMap,wheelIndex, " 2146 -2156 -2147 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out+Rsurf));

  Out=ModelSupport::getComposite(SMap,wheelIndex, " 2146 -2156 2147 -2157");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex," 2156 -2166 -2157 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out+Rsurf));

  Out=ModelSupport::getComposite(SMap,wheelIndex," 2166 -2186 2127 -2157 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex,SJ-10,
				 " -2127 2007M 2176 -2186 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  // wheel catcher

  Out=ModelSupport::getComposite(SMap,wheelIndex, " 2247 2238 -2239 -2115 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex, " 2237 -2247 2238 2205 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  //     floor gap
  Out=ModelSupport::getComposite(SMap,wheelIndex, " -2147 2208 2205 -2215 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex, " 2205 -2245 -2208 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex, " -2217 2205 -2245 2208 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex, " -2217 2205 2245 -2115 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex, " -2227 2217 2215 -2115 2218 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  // small cell between base cones
  Out=ModelSupport::getComposite(SMap,wheelIndex, " 2217 -2218 2208 2215 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex, " 2227 -2237 2215 -2225 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  // notch: big conical cell
  Out=ModelSupport::getComposite(SMap,wheelIndex, " -2238 2227 2225 -2115 ");
  if (engActive)
    buildStiffeners(System,Out,wheelIndex+3000,nSectors/2,steelMat);
  else
    System.addCell(MonteCarlo::Qhull(cellIndex++,shaftLowerBigStiffHomoMat,mainTemp,Out));


  // shaft layers
  int SI(wheelIndex);
  const std::string roof(ModelSupport::getComposite(SMap,wheelIndex," -2006 "));
  std::string floor;
  for (size_t i=0; i<nShaftLayers; i++)
    {
      const std::string outer(ModelSupport::getComposite(SMap,SI," -2007 "));
      const std::string inner(i==0?"":ModelSupport::getComposite(SMap,SI-10,
								 " 2007 "));

      if (i<3)
	floor = " 5 ";
      else if (i==nShaftLayers-1)
	floor = " 2186 ";
      else
	floor = " 2136 ";

      floor = ModelSupport::getComposite(SMap,wheelIndex,floor);

      if (i==2)
	{
	  buildHoles(System,inner+outer,floor,roof,
		     shaftMat[i],shaftHoleSize,shaftHoleXYangle,shaftHoleHeight,
		     0.0, 1000);
	  SI += 10;
	  continue;
	}

      System.addCell(MonteCarlo::Qhull(cellIndex++,shaftMat[i],mainTemp,
				       inner+outer+floor+roof));
      SI += 10;
    }

  Out=ModelSupport::getComposite(SMap,wheelIndex,SI-10,
				 " (-2007M -2006 2115) : " // connection
				 " (2126 -2149) : " // upper stiffeners
				 " (2126 -2186 -2157) : "  // above upper stiffeners
				 " (-2118 2115 -2126) : " // 2nd step
				 " ((-2239:-2247) 2205 -2115) "); // base
  addOuterSurf("Shaft",Out);

  return;
}

std::string
BilbaoWheel::getSQSurface(const double R, const double e)
  /*
    Return MCNP(X) surface card for SQ ellipsoids
  */
{
  std::string surf = "sq " + StrFunc::makeString(1./std::pow(R,2)) + " " +
    StrFunc::makeString(1./std::pow(R,2)) + " " +
    StrFunc::makeString(e) + " 0 0 0 -1 " +
    StrFunc::makeString(Origin[0]) + " " + // X
    StrFunc::makeString(Origin[1]) + " " + // Y
    StrFunc::makeString(Origin[2]);        // Z

  return surf;
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
  const double dTheta = 360.0/n;

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
			  const std::string& sides,
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
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,sides));
      return;
    }

  std::string Out;
  int SJ(wheelIndex+3000);
  for(size_t j=0;j<nSectors;j++)
    {
      Out=ModelSupport::getComposite(SMap,SJ," 1 -11 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out+sides));
      SJ+=10;
    }
  return;
}

void
BilbaoWheel::buildStiffeners(Simulation& System,
			     const std::string& sides,
			     const int SI,const size_t n,
			     const int mat)
  /*!
    Divide wheel by sectors to help cell splitting
    \param System :: Simulation
    \param sides :: top/bottom and side surfaces
    \param SI :: surface offset
    \param n  :: number of sectors/plates
    \param mat :: material
  */
{
  ELog::RegMethod RegA("BilbaoWheel","buildStiffeners");

  if (nSectors<2)
    {
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,sides));
      return;
    }

  std::string Out;
  int SJ(SI);
  for(size_t j=0;j<n;j++)
    {
      Out=ModelSupport::getComposite(SMap,SJ," 2 3 -4 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out+sides));

      Out=ModelSupport::getComposite(SMap,SJ," 2 4 -13 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,mainTemp,Out+sides));

      SJ+=10;
    }
  return;
}

void
BilbaoWheel::buildHoles(Simulation& System,
			const std::string& sides,
			const std::string& bot,
			const std::string& top,
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
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,sides+top+bot));
      return;
    }
  const int SI0(wheelIndex+30000+surfOffset);
  const int SIstep(10);

  // create surfaces
  int SI(SI0);
  double theta(0.0);

  // angular length of hole+mat
  const double dTheta = 360.0/static_cast<double>(nSectors);
  const double dThetaHole = dTheta * hs; // angular length of the hole

  for (size_t j=0; j<nSectors; j++)
    {
      theta = j*dTheta+xyAngle;
      ModelSupport::buildPlaneRotAxis(SMap, SI+1, Origin, X, Z, theta);
      theta += dThetaHole;
      ModelSupport::buildPlaneRotAxis(SMap, SI+2, Origin, X, Z, theta);

      SI += SIstep;
    }
  // add 1st surface again with reversed normal - to simplify building cells
  SMap.addMatch(SI+1,SMap.realSurf(SI0+1));

  std::string Out;
  SI=SI0;

  if (height>0.0)
    {
      // hole top/bottom
      ModelSupport::buildPlane(SMap,SI0+5,Origin+Z*(z0-height/2.0),Z);
      ModelSupport::buildPlane(SMap,SI0+6,Origin+Z*(z0+height/2.0),Z);

      // cell below holes
      Out=ModelSupport::getComposite(SMap,SI0," -5 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out+bot+sides));

      // cell above holes
      Out=ModelSupport::getComposite(SMap,SI0," 6 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out+top+sides+bot));

      // holes
      for(size_t j=0;j<nSectors;j++)
	{
	  Out=ModelSupport::getComposite(SMap,SI,SI0," 1 -2 5M -6M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out+sides));

	  Out=ModelSupport::getComposite(SMap,SI,SI0," 2 -11 5M -6M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out+sides));

	  SI+=SIstep;
	}
    } else // if height <= 0 => build full height holes
    {
      for(size_t j=0;j<nSectors;j++)
	{
	  Out=ModelSupport::getComposite(SMap,SI,SI0," 1 -2 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out+sides+top+bot));

	  Out=ModelSupport::getComposite(SMap,SI,SI0," 2 -11 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out+sides+top+bot));

	  SI+=SIstep;
	}
    }
  return;
}

void
BilbaoWheel::buildCirclePipes(Simulation& System,
			      const std::string& sides,
			      const std::string& floor,
			      const std::string& roof,
			      const std::string& inner,
			      const std::string& outer)
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
  std::string Out;
  int SJ(wheelIndex+2300);
  if (nSectors<2)
    {
      Out=sides+inner;
      System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,mainTemp,Out));
    } else
    {
      for (size_t j=0; j<nSectors; j++)
	{
	  Out=ModelSupport::getComposite(SMap,SJ," -8 ") + outer;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,mainTemp,Out));
	  Out=ModelSupport::getComposite(SMap,SJ," 8 -9 ") + outer;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

	  Out=ModelSupport::getComposite(SMap,SJ,SJ+10," 9 1 -1M ") + inner;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,mainTemp,
					   Out+sides));

	  Out=ModelSupport::getComposite(SMap,SJ,SJ+10," 9 1 -1M ") + floor;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,
					   Out+sides));
	  Out=ModelSupport::getComposite(SMap,SJ,SJ+10," 9 1 -1M ") + roof;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,
					   Out+sides));
	  SJ+=10;
	}
    }
  return;
}

void
BilbaoWheel::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("BilbaoWheel","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);

  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

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
  ModelSupport::buildPlane(SMap,wheelIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,wheelIndex+2,Origin,X);

  double H(targetHeight/2.0);
  ModelSupport::buildPlane(SMap,wheelIndex+5,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,wheelIndex+6,Origin+Z*H,Z);

  H+= voidTungstenThick;
  ModelSupport::buildPlane(SMap,wheelIndex+15,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,wheelIndex+16,Origin+Z*H,Z);

  H+= steelTungstenThick;
  ModelSupport::buildPlane(SMap,wheelIndex+25,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,wheelIndex+26,Origin+Z*H,Z);

  H+=coolantThick;
  ModelSupport::buildPlane(SMap,wheelIndex+35,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,wheelIndex+36,Origin+Z*H,Z);

  H+=caseThick;
  ModelSupport::buildPlane(SMap,wheelIndex+245,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,wheelIndex+246,Origin+Z*H,Z);

  H+=voidThick;
  ModelSupport::buildPlane(SMap,wheelIndex+45,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,wheelIndex+46,Origin+Z*H,Z);

  ModelSupport::buildCylinder(SMap,wheelIndex+7,Origin,Z,innerRadius);

  H  = targetHeight/2.0;
  H += voidTungstenThick;
  H += steelTungstenThick;
  H += coolantThick;
  H += caseThick;
  ModelSupport::buildPlane(SMap,wheelIndex+125,Origin-Z*H,Z);  // 22 : 23
  ModelSupport::buildPlane(SMap,wheelIndex+126,Origin+Z*H,Z);


  int SI(wheelIndex+10);
  for(size_t i=0;i<nLayers;i++)
    {
      ModelSupport::buildCylinder(SMap,SI+7,Origin,Z,radius[i]);
      SI+=10;
    }

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  Geometry::General *GA;

  GA = SurI.createUniqSurf<Geometry::General>(wheelIndex+517);
  GA->setSurface(getSQSurface(coolantRadiusOut, aspectRatio));
  SMap.registerSurf(GA);

  GA = SurI.createUniqSurf<Geometry::General>(wheelIndex+527);
  GA->setSurface(getSQSurface(caseRadius, aspectRatio));
  SMap.registerSurf(GA);

  Geometry::EllipticCyl* ECPtr=
    ModelSupport::buildEllipticCyl(SMap,wheelIndex+528,Origin,Z,
				   X,caseRadius,caseRadius);

  GA->normalizeGEQ(0);
  //  ELog::EM<<"XXN ="<<*GA<<ELog::endDiag;
  //  ELog::EM<<"ASPECT ="<<1/aspectRatio<<ELog::endDiag;
  ECPtr->normalizeGEQ(0);
  //  ELog::EM<<"YY ="<<*ECPtr<<ELog::endDiag;

  ModelSupport::buildCylinder(SMap,wheelIndex+537,Origin,Z,voidRadius);

  createRadialSurfaces(wheelIndex+3000, nSectors);


  // inner vertical layers
  H=targetInnerHeight/2.0;
  ModelSupport::buildPlane(SMap,wheelIndex+105,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,wheelIndex+106,Origin+Z*H,Z);
  ModelSupport::buildCylinder(SMap,wheelIndex+107,Origin,Z,
			      targetInnerHeightRadius);

  H+=steelTungstenInnerThick;
  ModelSupport::buildPlane(SMap,wheelIndex+115,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,wheelIndex+116,Origin+Z*H,Z);
  ModelSupport::buildCylinder(SMap,wheelIndex+117,Origin,Z,
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

  int SI(wheelIndex);
  int nInner(0); // number of inner cells (must be 1)
  int mat(0);
  std::string side;
  for(size_t i=0;i<nLayers;i++)
    {
      mat = matNum[matTYPE[i]];
      side = ModelSupport::getComposite(SMap,SI," 7 -17 ");

      Out=side+ModelSupport::getComposite(SMap,wheelIndex,SI," 5 -6 ");
      if (i==0)
	{
	  buildHoles(System,side,
		     ModelSupport::getComposite(SMap,wheelIndex," 116 "),
		     ModelSupport::getComposite(SMap,wheelIndex," -26 "),
		     mat,innerHoleSize,innerHoleXYangle,-1,
		     0.0, 4000);

	  buildHoles(System,side,
		     ModelSupport::getComposite(SMap,wheelIndex," 25 "),
		     ModelSupport::getComposite(SMap,wheelIndex," -115 "),
		     mat,innerHoleSize,innerHoleXYangle,-1,
		     0.0, 5000);


	  buildHoles(System,side,
		     ModelSupport::getComposite(SMap,wheelIndex," 115 "),
		     ModelSupport::getComposite(SMap,wheelIndex," -116 "),
		     mat,innerHoleSize,innerHoleXYangle,innerHoleHeight,
		     0.0, 0);
	}
      else if (i==1)
	{
	  // Layer before Tungsten bricks:
	  // Do not build it with engActive=1 since the cassette goes there
	  if (!engActive)
	    {
	      Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 117 -17M 5 -6 ");
	      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out));
	    }

	  side=ModelSupport::getComposite(SMap,wheelIndex," -117 107 ");

	  Out=ModelSupport::getComposite(SMap,wheelIndex," 105 -106 ")+side;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out));

	  Out=ModelSupport::getComposite(SMap,wheelIndex," 106 -26 ")+side;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

	  Out=ModelSupport::getComposite(SMap,wheelIndex,"-105 25 ")+side;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

	  Out=ModelSupport::getComposite(SMap,wheelIndex," -25 35 ")+side;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out));

	  Out=ModelSupport::getComposite(SMap,wheelIndex," 26 -36 ")+side;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out));

	  side=ModelSupport::getComposite(SMap,wheelIndex,SI," 7M -107 ");

	  Out=ModelSupport::getComposite(SMap,wheelIndex," 105 -106 ")+side;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out));

	  Out=ModelSupport::getComposite(SMap,wheelIndex," 106 -116 ")+side;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

	  Out=ModelSupport::getComposite(SMap,wheelIndex," -105 115 ")+side;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

	  Out=ModelSupport::getComposite(SMap,wheelIndex," 116 -36 ")+side;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out));

	  Out=ModelSupport::getComposite(SMap,wheelIndex," -115 35 ")+side;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out));
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
	      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out));
	    }
	  nInner++;
	}
      else // never actually called with standard geometry since nLayers=3
	{
	  //divideRadial(System, Out, mat);
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out));
	}

      SI+=10;
    }

  // front coolant:
  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 7M -517 35 -36");
  if (engActive)
    divideRadial(System, Out, heMat);
  else
    System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,mainTemp,Out));

  side = ModelSupport::getComposite(SMap,wheelIndex,SI," -7M 117" );

  // Void above W
  Out=ModelSupport::getComposite(SMap,wheelIndex," 6 -16 ")+side;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));
  // Void below W
  Out=ModelSupport::getComposite(SMap,wheelIndex," 15 -5 ")+side;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  // Steel above W
  Out=ModelSupport::getComposite(SMap,wheelIndex," 16 -26 ")+side;
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  // Steel below W
  Out=ModelSupport::getComposite(SMap,wheelIndex," 25 -15 ")+side;
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  // Coolant above steel
  Out=ModelSupport::getComposite(SMap,wheelIndex," 26 -36 ")+side;
  System.addCell(MonteCarlo::Qhull(cellIndex++,ssVoidMat,mainTemp,Out));
  CellMap::setCell("CoolantAboveSteel",cellIndex-1);

  // Coolant below steel
  Out=ModelSupport::getComposite(SMap,wheelIndex," 35 -25 " )+side;
  System.addCell(MonteCarlo::Qhull(cellIndex++,ssVoidMat,mainTemp,Out));

  // Metal surround [ UNACCEPTABLE JUNK CELL]
  // Metal front:
  Out=ModelSupport::getComposite(SMap,wheelIndex,"-527 517 35 -36");
  if (engActive)
    divideRadial(System, Out, steelMat);
  else
    System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  // forward Main sections:
  side=ModelSupport::getComposite(SMap,wheelIndex,"-527 17 ");

  Out=ModelSupport::getComposite(SMap,wheelIndex," -126 36 ")+side;	 // outer above W
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex," 125 -35 ")+side;
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out)); // outer below W

  // Void surround
  Out=ModelSupport::getComposite(SMap,wheelIndex,
				 "17 45 -46 -537 (-125:126:527)");
  if (engActive)
    divideRadial(System, Out, 0);
  else
    System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex,"-537 45 -46");
  addOuterSurf("Wheel",Out);

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
  FixedComp::setLinkSurf(0,SMap.realSurf(wheelIndex+537));
  FixedComp::setBridgeSurf(0,-SMap.realSurf(wheelIndex+1));

  FixedComp::setConnect(1,Origin+Y*innerRadius,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(wheelIndex+537));
  FixedComp::setBridgeSurf(1,SMap.realSurf(wheelIndex+1));

  FixedComp::setConnect(2,Origin-Y*voidRadius,-Y);
  FixedComp::setLinkSurf(2,SMap.realSurf(wheelIndex+537));
  FixedComp::setBridgeSurf(2,-SMap.realSurf(wheelIndex+1));

  FixedComp::setConnect(3,Origin+Y*voidRadius,Y);
  FixedComp::setLinkSurf(3,SMap.realSurf(wheelIndex+537));
  FixedComp::setBridgeSurf(3,SMap.realSurf(wheelIndex+1));

  const double H=wheelHeight()/2.0;
  FixedComp::setConnect(4,Origin-Z*H,-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(wheelIndex+45));

  FixedComp::setConnect(5,Origin+Z*H,Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(wheelIndex+46));

  // inner links (normally) point towards
  // top/bottom of the spallation material (innet cell)
  const double TH=targetHeight/2.0;
  FixedComp::setConnect(6,Origin-Z*TH,Z);
  FixedComp::setLinkSurf(6,SMap.realSurf(wheelIndex+5));

  FixedComp::setConnect(7,Origin+Z*TH,-Z);
  FixedComp::setLinkSurf(7,-SMap.realSurf(wheelIndex+6));

  FixedComp::setConnect(12, Origin-Y*(radius[0]+voidThick),-Y);
  FixedComp::setLinkSurf(12, SMap.realSurf(wheelIndex+2118));
  FixedComp::setBridgeSurf(12,-SMap.realSurf(wheelIndex+1));

  int SI(wheelIndex);
  for (size_t i=0; i<nLayers; i++)
    {
      if (matTYPE[i]==3) // Tungsten layer
  	{
  	  // 8 and 9 - the layer before Tungsten (He)
  	  FixedComp::setConnect(8, Origin-Y*(targetInnerHeightRadius +
  					     steelTungstenInnerThick), -Y);
  	  FixedComp::setLinkSurf(8, SMap.realSurf(wheelIndex+117));

  	  FixedComp::setConnect(9, Origin-Y*radius[i-2], Y);
  	  FixedComp::setLinkSurf(9, -SMap.realSurf(SI-10+7));

  	  // 10 and 11 - Tungsten layer
  	  FixedComp::setConnect(10, Origin-Y*radius[i-1], -Y);
  	  FixedComp::setLinkSurf(10, SMap.realSurf(SI+7));

  	  FixedComp::setConnect(11, Origin-Y*radius[i], Y);
  	  FixedComp::setLinkSurf(11, -SMap.realSurf(SI+17));

  	  return; // !!! we assume that there is only one Tungsten layer
  	}
      SI+=10;
    }

  return;
}

void
BilbaoWheel::buildSectors(Simulation& System) const
{
  /*!
    Build Tungsten sectors. Must be called after createLinks.
   */
  ELog::RegMethod RegA("BilbaoWheel","buildSectors");

  ModelSupport::objectRegister& OR=ModelSupport::objectRegister::Instance();

  for (size_t i=0; i<nSectors; i++)
    {
      std::shared_ptr<BilbaoWheelCassette>
	c(new BilbaoWheelCassette(keyName,"Sec",i));
      OR.addObject(c);
      c->createAll(System,*this,0,
		   7,8,9,12,i*360.0/nSectors);
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
