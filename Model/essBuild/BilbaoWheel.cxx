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

#include "WheelBase.h"
#include "BilbaoWheelInnerStructure.h"
#include "BilbaoWheel.h"


namespace essSystem
{

BilbaoWheel::BilbaoWheel(const std::string& Key) :
  WheelBase(Key),
  InnerComp(new BilbaoWheelInnerStructure(Key + "InnerStructure"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

BilbaoWheel::BilbaoWheel(const BilbaoWheel& A) : 
  WheelBase(A),
  InnerComp(new BilbaoWheelInnerStructure(*A.InnerComp)),
  targetHeight(A.targetHeight),
  targetInnerHeight(A.targetHeight),
  targetInnerHeightRadius(A.targetHeight),
  voidTungstenThick(A.voidTungstenThick),
  steelTungstenThick(A.steelTungstenThick),
  steelTungstenInnerThick(A.steelTungstenInnerThick),
  caseThickIn(A.caseThickIn),coolantThick(A.coolantThick),
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
  shaftHoleHeight(A.shaftHoleHeight),
  shaftHoleSize(A.shaftHoleSize),
  shaftHoleXYangle(A.shaftHoleXYangle),
  shaftBaseDepth(A.shaftBaseDepth),
  catcherTopSteelThick(A.catcherTopSteelThick),
  catcherHeight(A.catcherHeight),
  catcherRadius(A.catcherRadius),
  catcherMiddleHeight(A.catcherMiddleHeight),
  catcherMiddleRadius(A.catcherMiddleRadius),
  catcherNotchDepth(A.catcherNotchDepth),
  catcherNotchRadius(A.catcherNotchRadius),
  catcherRingRadius(A.catcherRingRadius),
  catcherRingDepth(A.catcherRingDepth),
  catcherRingThick(A.catcherRingThick),
  circlePipesBigRad(A.circlePipesBigRad),
  circlePipesRad(A.circlePipesRad),
  circlePipesWallThick(A.circlePipesWallThick),
  wMat(A.wMat),heMat(A.heMat),
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
      *InnerComp= *A.InnerComp;
      targetHeight=A.targetHeight;
      targetInnerHeight=A.targetInnerHeight;
      targetInnerHeightRadius=A.targetInnerHeightRadius;
      voidTungstenThick=A.voidTungstenThick;
      steelTungstenThick=A.steelTungstenThick;
      steelTungstenInnerThick=A.steelTungstenInnerThick;
      caseThickIn=A.caseThickIn;
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
      shaftHoleHeight=A.shaftHoleHeight;
      shaftHoleSize=A.shaftHoleSize;
      shaftHoleXYangle=A.shaftHoleXYangle;
      shaftBaseDepth=A.shaftBaseDepth;
      catcherTopSteelThick=A.catcherTopSteelThick;
      catcherHeight=A.catcherHeight;
      catcherRadius=A.catcherRadius;
      catcherMiddleHeight=A.catcherMiddleHeight;
      catcherMiddleRadius=A.catcherMiddleRadius;
      catcherNotchDepth=A.catcherNotchDepth;
      catcherNotchRadius=A.catcherNotchRadius;
      catcherRingRadius=A.catcherRingRadius;
      catcherRingDepth=A.catcherRingDepth;
      catcherRingThick=A.catcherRingThick;
      circlePipesBigRad=A.circlePipesBigRad;
      circlePipesRad=A.circlePipesRad;
      circlePipesWallThick=A.circlePipesWallThick;
      wMat=A.wMat;
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

  FixedOffset::populate(Control);

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
  caseThickIn=Control.EvalVar<double>(keyName+"CaseThickIn");  
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
  if (shaft2StepConnectionRadius<shaftRadius[nShaftLayers-1])
    throw ColErr::RangeError<double>(shaft2StepConnectionRadius, shaftRadius[nShaftLayers-1], INFINITY, "Shaft2StepConnectionRadius must exceed outer ShaftRadius");
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
  
  catcherHeight=Control.EvalVar<double>(keyName+"CatcherHeight");
  catcherRadius=Control.EvalVar<double>(keyName+"CatcherRadius");
  if (catcherRadius>radius[0]+voidThick)
    throw ColErr::RangeError<double>(catcherRadius, 0, radius[0]+voidThick,
				     "CatcherRadius must not exceed Radius1 + VoidThick");

  catcherMiddleHeight=Control.EvalVar<double>(keyName+"CatcherMiddleHeight");
  catcherMiddleRadius=Control.EvalVar<double>(keyName+"CatcherMiddleRadius");
  if (catcherMiddleRadius>catcherRadius)
    throw ColErr::RangeError<double>(catcherMiddleRadius, 0, catcherRadius,
				     "CatcherMiddleRadius must not exceed CatcherRadius");

  catcherNotchDepth=Control.EvalVar<double>(keyName+"CatcherNotchDepth");
  if (catcherNotchDepth>catcherMiddleHeight)
    throw ColErr::RangeError<double>(catcherNotchDepth, 0, catcherMiddleHeight,
				     "CatcherNotchDepth must not exceed CatcherMiddleHeight");
  catcherNotchRadius=Control.EvalVar<double>(keyName+"CatcherNotchRadius");
  if (catcherNotchRadius>catcherMiddleRadius)
    throw ColErr::RangeError<double>(catcherNotchRadius, 0, catcherMiddleRadius,
				     "CatcherNotchRadius must not exceed CatcherMiddleRadius");

  catcherRingRadius=Control.EvalVar<double>(keyName+"CatcherRingRadius");
  catcherRingDepth=Control.EvalVar<double>(keyName+"CatcherRingDepth");
  catcherRingThick=Control.EvalVar<double>(keyName+"CatcherRingThick");

  circlePipesBigRad=Control.EvalVar<double>(keyName+"CirclePipesBigRad");
  circlePipesRad=Control.EvalVar<double>(keyName+"CirclePipesRad");
  circlePipesWallThick=Control.EvalVar<double>(keyName+"CirclePipesWallThick");

  wMat=ModelSupport::EvalMat<int>(Control,keyName+"WMat");  
  heMat=ModelSupport::EvalMat<int>(Control,keyName+"HeMat");  
  steelMat=ModelSupport::EvalMat<int>(Control,keyName+"SteelMat");  
  ssVoidMat=ModelSupport::EvalMat<int>(Control,keyName+"SS316LVoidMat");
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");  

  return;
}

void
BilbaoWheel::makeShaftSurfaces()
  /*!
    Construct surfaces for the inner shaft
  */
{
  ELog::RegMethod RegA("BilbaoWheel","makeShaftSurfaces");

  ModelSupport::buildPlane(SMap,wheelIndex+2006,Origin+Z*shaftHeight,Z);  

  int SI(wheelIndex);
  for (size_t i=0; i<nShaftLayers; i++)
    {
      ModelSupport::buildCylinder(SMap,SI+2007,Origin,Z,shaftRadius[i]);
      SI+=10;
    }

  // '-1.0' is needed - overwise TopAFlight will cut the outer void:
  double H(wheelHeight()/2.0-caseThick+caseThickIn-1.0);
  // first void step in the inner part
  ModelSupport::buildPlane(SMap,wheelIndex+2105,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,wheelIndex+2106,Origin+Z*H,Z);
  ModelSupport::buildCylinder(SMap,wheelIndex+2107,Origin,Z,
			      coolantRadiusIn+voidThick);

  ModelSupport::buildPlane(SMap,wheelIndex+2115,Origin-Z*(H+catcherTopSteelThick),Z);

  // 2nd void step
  H = shaft2StepHeight;
  ModelSupport::buildPlane(SMap,wheelIndex+2116,Origin+Z*H,Z);

  H += voidThick;
  //  ModelSupport::buildPlane(SMap,wheelIndex+2125,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,wheelIndex+2126,Origin+Z*H,Z);

  double H1 = H;

  H -= voidThick;
  H -= radius[0]-innerRadius;
  ModelSupport::buildPlane(SMap,wheelIndex+2136,Origin+Z*H,Z);

  double R = radius[0]+voidThick;
  ModelSupport::buildCylinder(SMap,wheelIndex+2118,Origin,Z,R);

  H = H1-voidThick;
  H += shaft2StepConnectionDist;
  ModelSupport::buildPlane(SMap,wheelIndex+2146,Origin+Z*H,Z);

  H += shaft2StepConnectionHeight;
  ModelSupport::buildPlane(SMap,wheelIndex+2156,Origin+Z*H,Z);

  H += voidThick;
  ModelSupport::buildPlane(SMap,wheelIndex+2166,Origin+Z*H,Z);

  R = shaft2StepConnectionRadius;
  ModelSupport::buildCylinder(SMap,wheelIndex+2127,Origin,Z,R);

  R += voidThick;
  ModelSupport::buildCylinder(SMap,wheelIndex+2137,Origin,Z,R);

  // shaft base
  H = 0.0;
  H += shaftBaseDepth;
  ModelSupport::buildPlane(SMap,wheelIndex+2205,Origin-Z*H,Z);
  
  // shaft base - catcher
  H -= catcherHeight;
  ModelSupport::buildPlane(SMap,wheelIndex+2215,Origin-Z*H,Z);
  R = catcherRadius;
  ModelSupport::buildCylinder(SMap,wheelIndex+2207,Origin,Z,R);

  H -= catcherMiddleHeight;
  ModelSupport::buildPlane(SMap,wheelIndex+2225,Origin-Z*H,Z);
  R = catcherMiddleRadius;
  ModelSupport::buildCylinder(SMap,wheelIndex+2217,Origin,Z,R);

  // catcher notch
  H += catcherNotchDepth;
  ModelSupport::buildPlane(SMap,wheelIndex+2235,Origin-Z*H,Z);
  R = catcherNotchRadius;
  ModelSupport::buildCylinder(SMap,wheelIndex+2227,Origin,Z,R);

  // catcher ring
  H = catcherRingDepth;
  ModelSupport::buildPlane(SMap,wheelIndex+2245,Origin-Z*H,Z);
  R = catcherRingRadius;
  ModelSupport::buildCylinder(SMap,wheelIndex+2237,Origin,Z,R);
  R += catcherRingThick;
  ModelSupport::buildCylinder(SMap,wheelIndex+2247,Origin,Z,R);


  // circle of pipes
  R = circlePipesBigRad-circlePipesRad-circlePipesWallThick;
  ModelSupport::buildCylinder(SMap,wheelIndex+2307,Origin,Z,R);
  R = circlePipesBigRad+circlePipesRad+circlePipesWallThick;
  ModelSupport::buildCylinder(SMap,wheelIndex+2317,Origin,Z,R);

  int SJ(wheelIndex+2300);
  double theta(0.0), x0, y0;
  const double dTheta = 360.0/static_cast<double>(nSectors);
  R = circlePipesBigRad;
  for (size_t j=0; j<nSectors; j++)
    {
      x0 = R*sin(theta * M_PI/180.0);
      y0 = -R*cos(theta * M_PI/180.0);
      ModelSupport::buildCylinder(SMap,SJ+8,Origin+X*x0+Y*y0,Z,circlePipesRad);
      ModelSupport::buildCylinder(SMap,SJ+9,Origin+X*x0+Y*y0,Z,
				  circlePipesRad+circlePipesWallThick);
      // dummy plane to separate voids between circles:
      ModelSupport::buildPlaneRotAxis(SMap, SJ+1, Origin, X, Z, theta);

      theta += dTheta;
      SJ+=10;
    }
  // add 1st surface again with reversed normal - to simplify building cells
  SMap.addMatch(SJ+9,SMap.realSurf(wheelIndex+2309));
  SMap.addMatch(SJ+1,SMap.realSurf(wheelIndex+2301));

  return;
}

void
BilbaoWheel::makeShaftObjects(Simulation& System)
  /*!
    Construct the objects
    \param System :: Simulation object
   */
{
  ELog::RegMethod RegA("BilbaoWheel","makeShaftObjects");
  std::string Out;

  // Main body [disk]
  // central voids. There is the layer with holes between them -
  // it is added in the for.. loop below (if (i==2))
  Out=ModelSupport::getComposite(SMap,wheelIndex,wheelIndex+10,
				 " -2007M 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,mainTemp,Out));

  // layer before (inside) circle of pipes
  //  Out=ModelSupport::getComposite(SMap,wheelIndex,wheelIndex+20," 105 -106 2007M -2307 ");
  //  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,mainTemp,Out));

  // layer with circle of pipes
  buildCirclePipes(System,
		   ModelSupport::getComposite(SMap,wheelIndex,wheelIndex+20,
					      " 2007M -7 "),
		   ModelSupport::getComposite(SMap,wheelIndex," 105 -106 "),0);

  // layer after (outside) circle of pipes
  //  Out=ModelSupport::getComposite(SMap,wheelIndex," 105 -106 2317 -7 ");
  //  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,mainTemp,Out));
  
  // steel above
  buildCirclePipes(System,
		   ModelSupport::getComposite(SMap,wheelIndex,wheelIndex+20,
					      " -7 2007M "),
		   ModelSupport::getComposite(SMap,wheelIndex," 106 -116 "),steelMat);
  // steel below
  //  Out=ModelSupport::getComposite(SMap,wheelIndex,wheelIndex+20,"-7 -105 115 2007M");
  //  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));
  buildCirclePipes(System,
		   ModelSupport::getComposite(SMap,wheelIndex,wheelIndex+20,
					      " -7 2007M "),
		   ModelSupport::getComposite(SMap,wheelIndex," -105 115 "),steelMat);

  
  // void below
  Out=ModelSupport::getComposite(SMap,wheelIndex,wheelIndex+20,"-7 35 -115 2127 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));
  // steel inside - outer radial part
  Out=ModelSupport::getComposite(SMap,wheelIndex,wheelIndex+20,
				 wheelIndex+(static_cast<int>(nShaftLayers)-2)*10,
				 " 35 -115 -2127 2007M ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));
  // steel inside - central part
  Out=ModelSupport::getComposite(SMap,wheelIndex,wheelIndex+20,
				 wheelIndex+(static_cast<int>(nShaftLayers)-2)*10,
				 " 35 -5 -2007M ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  // steel : inner shroud above/below
  // upper cell
  Out=ModelSupport::getComposite
    (SMap,wheelIndex," -1027 -46 36 17" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));
  // lower cell
  Out=ModelSupport::getComposite
    (SMap,wheelIndex," -1027 45 -35" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  // void (which connects to the Wheel void)
  // upper cell
  Out=ModelSupport::getComposite
    (SMap,wheelIndex, " -1027 -2106 46 2118" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));
  // side
  Out=ModelSupport::getComposite
    (SMap,wheelIndex, " -2107 1027 -2106 246 " );
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));

  // lower cell
  Out=ModelSupport::getComposite(SMap,wheelIndex, " -1027 17 2105 -45" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));
  Out=ModelSupport::getComposite(SMap,wheelIndex, " -17 2115 -45" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));
  Out=ModelSupport::getComposite(SMap,wheelIndex, " 17 -2118 2115 -2105 " );
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));
  // side
  Out=ModelSupport::getComposite
    (SMap,wheelIndex, " -2107 1027 2105 -245 " );
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));

  // 2nd void step
  // upper cell - inner steel - outer side layer
  Out=ModelSupport::getComposite(SMap,wheelIndex,
				 wheelIndex+(static_cast<int>(nShaftLayers)-1)*10,
				 " 7 -17 26 -2116 2007M" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));
  // upper cell - inner steel - top layer
  Out=ModelSupport::getComposite(SMap,wheelIndex,
				 wheelIndex+(static_cast<int>(nShaftLayers)-2)*10,
				 " -7 2136 -2116 2007M" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  // upper cell - inner steel - inner void
  Out=ModelSupport::getComposite(SMap,wheelIndex,wheelIndex+20,
				 " -7 116 -2136 2007M" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));
  Out=ModelSupport::getComposite(SMap,wheelIndex,wheelIndex+10,
				 " 6 -2136 -2007M  " );
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));

  // upper cell - void around inner steel - horizontal part
  Out=ModelSupport::getComposite(SMap,wheelIndex,
				 " -17 2116 -2126 2137 " );
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));
  // upper cell - void around inner steel - vertical part
  Out=ModelSupport::getComposite(SMap,wheelIndex, " -2118 17 46 -2126 " );
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));

  // upper/lower connection
  Out=ModelSupport::getComposite(SMap,wheelIndex,
				 wheelIndex+(static_cast<int>(nShaftLayers)-2)*10,
				 " -2127 2007M 2146 -2156 " );
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex,
				 wheelIndex+(static_cast<int>(nShaftLayers)-2)*10,
				 " -2137 2007M 2116 -2146 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex,
				 " 2127 -2137 2146 -2166 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex,
				 wheelIndex+(static_cast<int>(nShaftLayers)-2)*10,
				 " -2127 2007M 2156 -2166 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));

  // wheel catcher
  Out=ModelSupport::getComposite(SMap,wheelIndex, " -2118 2207 2205 -2115 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));

  // two cells are needed since there is CatcherRing between them
  Out=ModelSupport::getComposite(SMap,wheelIndex, " -2207 2247 2215 -2115 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));
  Out=ModelSupport::getComposite(SMap,wheelIndex, " -2237 2217 2215 -2115 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));
  
  Out=ModelSupport::getComposite(SMap,wheelIndex, " -2217 2225 -2115 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));

  // shaft base catcher
  Out=ModelSupport::getComposite(SMap,wheelIndex, " -2207 2205 -2215 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));
  // shaft base catcher - middle part
  Out=ModelSupport::getComposite(SMap,wheelIndex, " -2217 2215 -2235 "); // -2217 2215 -2225
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));
  Out=ModelSupport::getComposite(SMap,wheelIndex, " -2217 2227 2235 -2225 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));
  // notch
  Out=ModelSupport::getComposite(SMap,wheelIndex, " -2227 2235 -2225 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));
  // ring
  Out=ModelSupport::getComposite(SMap,wheelIndex, " 2237 -2247 2245 -2115 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));
  Out=ModelSupport::getComposite(SMap,wheelIndex, " 2237 -2247 -2245 2215 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));
  
  // shaft layers
  int SI(wheelIndex);
  for (size_t i=0; i<nShaftLayers; i++)
    {
      if (i==0)
	  Out=ModelSupport::getComposite(SMap,SI,wheelIndex," -2007 2136 -2006M ");
      else if (i==2)
	{
	Out=ModelSupport::getComposite
	  (SMap,SI,SI-10,wheelIndex," -2007 2007M 5N -2006N "); // layer with holes
	buildHoles(System,
		   ModelSupport::getComposite(SMap,SI,SI-10," -2007 2007M "),
		   ModelSupport::getComposite(SMap,wheelIndex," 5 "),
		   ModelSupport::getComposite(SMap,wheelIndex," -2006 "),
		   shaftMat[i],shaftHoleSize,shaftHoleXYangle,shaftHoleHeight,
		   0.0, 1000);
	SI += 10;
	continue;
	} else if (i==nShaftLayers-1)
	{
	  Out=ModelSupport::getComposite
	    (SMap,SI,SI-10,wheelIndex," -2007 2007M 2166N -2006N ");
	} else
	Out=ModelSupport::getComposite
	  (SMap,SI,SI-10,wheelIndex," -2007 2007M 2136N -2006N ");

      System.addCell(MonteCarlo::Qhull(cellIndex++,shaftMat[i],mainTemp,Out));

      if (i==nShaftLayers-1)
	{
	  Out=ModelSupport::getComposite(SMap,wheelIndex,SI,
					 " (((-2007M -2006) : " // top
                                         "   (-2107  -2106) : " // 1st step
                                         "   (-2137 -2166)) 2105) : " // connection
					 " (-2118 -2126 2205) "); // 2nd step and base
	  addOuterSurf("Shaft",Out);
	}

      SI += 10;
    }
  
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
BilbaoWheel::createRadialSurfaces()
  /*!
    Create planes for the inner structure inside BilbaoWheel
  */
{
  ELog::RegMethod RegA("BilbaoWheel","createRadialSurfaces");

  if (nSectors<2) return;

  int SI(wheelIndex+3000);
  double theta(0.0);
  const double dTheta = 360.0/static_cast<double>(nSectors);

  for (size_t j=0; j<nSectors; j++)
    {
      ModelSupport::buildPlaneRotAxis(SMap, SI+1, Origin, X, Z, theta);
      theta += dTheta;
      SI += 10;
    }
  // add 1st surface again with reversed normal - to simplify building cells
  SMap.addMatch(SI+1,SMap.realSurf(wheelIndex+3001));
  return; 
}


void
BilbaoWheel::divideRadial(Simulation& System,
			  const std::string& sides,
			  const int mat)
  /*!
    Divide wheel by sectors to help cell splitting
    \param System :: Simulation
    \param sides :: top/bottom and side cylinders
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
	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out+sides));

	  Out=ModelSupport::getComposite(SMap,SI,SI0," 2 -11 5M -6M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out+sides));

	  SI+=SIstep;
	}
    } else // if height <= 0 => build full height holes
    {
      for(size_t j=0;j<nSectors;j++)
	{
	  Out=ModelSupport::getComposite(SMap,SI,SI0," 1 -2 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out+sides+top+bot));

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
			      const std::string& bottop,
			      const int mat)
  /*!
    Build circle of pipes
    \param System :: Simulation
    \param sides  :: side surfaces
    \param bottop :: bottom and top surfaces
    \param mat    :: material between pipes
  */
{
  ELog::RegMethod RegA("BilbaoWheel","buildCirclePipes");
  std::string Out;
  int SJ(wheelIndex+2300);
  if (nSectors<2)
    {
      Out=sides+bottop;
      System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,mainTemp,Out));
    } else
    {
      for (size_t j=0; j<nSectors; j++)
	{
	  Out=ModelSupport::getComposite(SMap,SJ," -8 ") + bottop;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,mainTemp,Out));
	  Out=ModelSupport::getComposite(SMap,SJ," 8 -9 ") + bottop;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));
	  Out=ModelSupport::getComposite(SMap,SJ,SJ+10," (9 1) (9M -1M)");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,
					   Out+bottop+sides));
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
  applyOffset();

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

  H-=caseThick;
  H+=caseThickIn;
  ModelSupport::buildPlane(SMap,wheelIndex+45,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,wheelIndex+46,Origin+Z*H,Z);

  H-=caseThickIn;
  H+=caseThick;
  H+=voidThick;
  ModelSupport::buildPlane(SMap,wheelIndex+55,Origin-Z*H,Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+56,Origin+Z*H,Z);  

  // dummy planes (same ase 55,56) to attach shaft outer void
  // they will be revised when the inner void step is implemented
  ModelSupport::buildPlane(SMap,wheelIndex+65,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,wheelIndex+66,Origin+Z*H,Z);

  ModelSupport::buildCylinder(SMap,wheelIndex+7,Origin,Z,innerRadius);

  // step to outer radius:
  ModelSupport::buildCylinder(SMap,wheelIndex+1027,Origin,
			      Z,coolantRadiusIn);


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

  createRadialSurfaces();


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
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("BilbaoWheel","createObjects");

  // Inner Radius is 7
  
  const int matNum[4]={0,steelMat,heMat,wMat};
  std::string Out, Out1;
  // 
  // Loop through each item and build inner section
  // 

  int SI(wheelIndex);
  int nInner(0); // number of inner cells (must be 1)
  int mat(0);
  for(size_t i=0;i<nLayers;i++)
    {
      mat = matNum[matTYPE[i]];

      Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 7M -17M 5 -6 ");
      if (i==0)
	{
	  Out=ModelSupport::getComposite(SMap,SI," 7 -17 "); // sides
	  buildHoles(System,Out,
		     ModelSupport::getComposite(SMap,wheelIndex," 116 "),
		     ModelSupport::getComposite(SMap,wheelIndex," -26 "),
		     mat,innerHoleSize,innerHoleXYangle,-1,
		     0.0, 4000);


	  //	  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 7M -17M -115 25 ");
	  //	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));
	  Out=ModelSupport::getComposite(SMap,SI," 7 -17 ");
	  buildHoles(System,Out,
		     ModelSupport::getComposite(SMap,wheelIndex," 25 "),
		     ModelSupport::getComposite(SMap,wheelIndex," -115 "),
		     mat,innerHoleSize,innerHoleXYangle,-1,
		     0.0, 5000);

	  
	  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 7M -17M -25 35 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

	  Out=ModelSupport::getComposite(SMap,SI," 7 -17 "); // sides
	  buildHoles(System,Out,
		     ModelSupport::getComposite(SMap,wheelIndex," 115 "),
		     ModelSupport::getComposite(SMap,wheelIndex," -116 "),
		     mat,innerHoleSize,innerHoleXYangle,innerHoleHeight,
		     0.0, 0);
	}
      
      if (i==1)
	{
	  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 117 -17M 5 -6 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out));
	  
	  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," -117 107 105 -106 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out));
	  
	  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," -117 107 106 -26 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));
	  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," -117 107 -105 25 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));
	  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," -117 107 -25 35 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out));
	  
	  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," -117 107 26 -36 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out));
	  
	  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 7M -107 105 -106 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out));
	  
	  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 7M -107 106 -116 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));
	  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 7M -107 -105 115 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));
	  
	  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 7M -107 116 -36 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out));
	  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 7M -107 -115 35 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out));
	}
      else if (mat==wMat)   //engActive
	{
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out));
	  CellMap::setCell("Inner",cellIndex-1);
	  nInner++;
	  if (nInner>1)
	    ELog::EM << "More than one spallation layer" << ELog::endErr;
	}
      else if (i!=0)
	{
	  divideRadial(System, Out, mat);
	}
      SI+=10;
    }
  
  // front coolant:
  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 7M -517 35 -36");
  divideRadial(System, Out, heMat);

  // Void above W
  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," -7M 6 -16 117" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));
  // Void below W
  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," -7M 15 -5 117 " );
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0,Out));

  // Steel above W
  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," -7M 16 -26 117 " );
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  // Steel below W
  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," -7M 25 -15 117 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));


  // Coolant above steel
  Out=ModelSupport::getComposite(SMap,wheelIndex,SI, " -7M 26 -36 117 " );
  System.addCell(MonteCarlo::Qhull(cellIndex++,ssVoidMat,mainTemp,Out));
  CellMap::setCell("CoolantAboveSteel",cellIndex-1);

  // Coolant below steel
  Out=ModelSupport::getComposite(SMap,wheelIndex,SI, " -7M 35 -25 117 " );
  System.addCell(MonteCarlo::Qhull(cellIndex++,ssVoidMat,mainTemp,Out));

  // Metal surround [ UNACCEPTABLE JUNK CELL]
  // Metal front:
  Out=ModelSupport::getComposite(SMap,wheelIndex,"-527 517 35 -36");	
  divideRadial(System, Out, steelMat);

  // forward Main sections:
  Out=ModelSupport::getComposite(SMap,wheelIndex,"-527 1027 -126 36");	 // outer above W
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex,"-527 1027 125 -35");	
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out)); // outer below W

  // Void surround
  Out=ModelSupport::getComposite(SMap,wheelIndex,
				 "2107 55 -56 -537 (-125:126:527)");
  divideRadial(System, Out, 0);
  
  Out=ModelSupport::getComposite(SMap,wheelIndex,"-537 55 -56");	
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
  FixedComp::setLinkSurf(2,SMap.realSurf(wheelIndex+1037));
  FixedComp::setBridgeSurf(2,-SMap.realSurf(wheelIndex+1));

  FixedComp::setConnect(3,Origin+Y*voidRadius,Y);
  FixedComp::setLinkSurf(3,SMap.realSurf(wheelIndex+1037));
  FixedComp::setBridgeSurf(3,SMap.realSurf(wheelIndex+1));

  const double H=wheelHeight()/2.0;
  FixedComp::setConnect(4,Origin-Z*H,-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(wheelIndex+55));

  FixedComp::setConnect(5,Origin+Z*H,Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(wheelIndex+56));

    // inner links (normally) point towards
  // top/bottom of the spallation material (innet cell)
  const double TH=targetHeight/2.0;
  FixedComp::setConnect(6,Origin-Z*TH,Z);
  FixedComp::setLinkSurf(6,SMap.realSurf(wheelIndex+5));

  FixedComp::setConnect(7,Origin+Z*TH,-Z);
  FixedComp::setLinkSurf(7,-SMap.realSurf(wheelIndex+6));

  int SI(wheelIndex);
  for (size_t i=0; i<nLayers; i++)
    {
      if (matTYPE[i]==3) // Tungsten layer
	{
	  FixedComp::setConnect(8, Origin+Y*radius[i-1], Y);
	  FixedComp::setLinkSurf(8, SMap.realSurf(SI+7));

	  FixedComp::setConnect(9, Origin+Y*radius[i], -Y);
	  FixedComp::setLinkSurf(9, -SMap.realSurf(SI+17));

	  return; // !!! we assume that there is only one Tungsten layer
	}
      SI+=10;
    }

  return;
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
  makeShaftSurfaces();

  createObjects(System);
  makeShaftObjects(System);

  createLinks();
  insertObjects(System);       
  //  if (engActive)
  //    InnerComp->createAll(System, *this);

  return;
}

}  // NAMESPACE essSystem
