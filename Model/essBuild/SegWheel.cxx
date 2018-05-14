/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/SegWheel.cxx
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
#include "Quadratic.h"
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
#include "BaseMap.h"
#include "CellMap.h"
#include "WheelBase.h"
#include "SegWheel.h"


namespace essSystem
{

SegWheel::SegWheel(const std::string& Key) :
  WheelBase(Key)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

SegWheel::SegWheel(const SegWheel& A) : 
  WheelBase(A),lh2Index(A.lh2Index),mainShaftCell(A.mainShaftCell),
  targetHeight(A.targetHeight),
  targetSectorOffsetX(A.targetSectorOffsetX),
  targetSectorOffsetY(A.targetSectorOffsetY), 
  targetSectorOffsetZ(A.targetSectorOffsetZ),
  targetSectorAngleXY(A.targetSectorAngleXY),
  targetSectorAngleZ(A.targetSectorAngleZ),
  targetSectorApertureXY(A.targetSectorApertureXY),
  targetSectorApertureZ(A.targetSectorApertureZ),
  targetSectorNumber(A.targetSectorNumber),coolantThickOut(A.coolantThickOut),
  coolantThickIn(A.coolantThickIn),caseThickZ(A.caseThickZ),
  caseThickX(A.caseThickX),voidThick(A.voidThick),
  innerRadius(A.innerRadius),coolantRadiusOut(A.coolantRadiusOut),
  coolantRadiusIn(A.coolantRadiusIn),caseRadius(A.caseRadius),
  voidRadius(A.voidRadius),nLayers(A.nLayers),radius(A.radius),
  matTYPE(A.matTYPE),shaftTopHeight(A.shaftTopHeight),
  shaftBaseHeight(A.shaftBaseHeight),shaftJointThick(A.shaftJointThick),
  shaftCoolThick(A.shaftCoolThick),shaftCladThick(A.shaftCladThick),
  shaftVoidThick(A.shaftVoidThick),shaftSupportRadius(A.shaftSupportRadius),
  shaftSupportThick(A.shaftSupportThick),shaftBaseThick(A.shaftBaseThick),
  shaftBaseFootThick(A.shaftBaseFootThick),wMat(A.wMat),
  heMat(A.heMat),steelMat(A.steelMat),innerMat(A.innerMat),
  mainShaftMat(A.mainShaftMat),cladShaftMat(A.cladShaftMat),
  coolingShaftMatInt(A.coolingShaftMatInt),coolingShaftMatExt(A.coolingShaftMatExt)
  /*!
    Copy constructor
    \param A :: SegWheel to copy
  */
{}

SegWheel&
SegWheel::operator=(const SegWheel& A)
  /*!
    Assignment operator
    \param A :: SegWheel to copy
    \return *this
  */
{
  if (this!=&A)
    {
      WheelBase::operator=(A);
      lh2Index=A.lh2Index;
      mainShaftCell=A.mainShaftCell;
      targetHeight=A.targetHeight;
      targetSectorOffsetX=A.targetSectorOffsetX;
      targetSectorOffsetY=A.targetSectorOffsetY;
      targetSectorOffsetZ=A.targetSectorOffsetZ;
      targetSectorAngleXY=A.targetSectorAngleXY;
      targetSectorAngleZ=A.targetSectorAngleZ;
      targetSectorApertureXY=A.targetSectorApertureXY;
      targetSectorApertureZ=A.targetSectorApertureZ;
      targetSectorNumber=A.targetSectorNumber;
      coolantThickOut=A.coolantThickOut;
      coolantThickIn=A.coolantThickIn;
      caseThickZ=A.caseThickZ;
      caseThickX=A.caseThickX;
      voidThick=A.voidThick;
      innerRadius=A.innerRadius;
      coolantRadiusOut=A.coolantRadiusOut;
      coolantRadiusIn=A.coolantRadiusIn;
      caseRadius=A.caseRadius;
      voidRadius=A.voidRadius;
      nLayers=A.nLayers;
      radius=A.radius;
      matTYPE=A.matTYPE;
      shaftTopHeight=A.shaftTopHeight;
      shaftBaseHeight=A.shaftBaseHeight;
      shaftJointThick=A.shaftJointThick;
      shaftCoolThick=A.shaftCoolThick;
      shaftCladThick=A.shaftCladThick;
      shaftVoidThick=A.shaftVoidThick;
      shaftSupportRadius=A.shaftSupportRadius;
      shaftSupportThick=A.shaftSupportThick;
      shaftBaseThick=A.shaftBaseThick;
      shaftBaseFootThick=A.shaftBaseFootThick;
      wMat=A.wMat;
      heMat=A.heMat;
      steelMat=A.steelMat;
      innerMat=A.innerMat;
      mainShaftMat=A.mainShaftMat;
      cladShaftMat=A.cladShaftMat;
      coolingShaftMatInt=A.coolingShaftMatInt;
      coolingShaftMatExt=A.coolingShaftMatExt;
    }
  return *this;
}

SegWheel::~SegWheel()
  /*!
    Destructor
   */
{}

SegWheel*
SegWheel::clone() const
  /*!
    Virtual copy constructor
    \return new(this)
   */
{
  return new SegWheel(*this);
}

void
SegWheel::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("SegWheel","populate");
  FixedOffset::populate(Control);

  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");   

  double R;
  for(size_t i=0;i<nLayers;i++)
    {
      R=Control.EvalVar<double>(StrFunc::makeString(keyName+"Radius",i+1));   
      if (i && R<=radius.back())
        ELog::EM<<"Radius["<<i+1<<"] not ordered "
                <<R<<" "<<radius.back()<<ELog::endErr;

      radius.push_back(R);
      matTYPE.push_back(Control.EvalVar<int>
                        (StrFunc::makeString(keyName+"MatTYPE",i+1)));   
    }

  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");  
  coolantRadiusOut=Control.EvalVar<double>(keyName+"CoolantRadiusOut");  
  coolantRadiusIn=Control.EvalVar<double>(keyName+"CoolantRadiusIn");
  caseRadius=Control.EvalVar<double>(keyName+"CaseRadius");  
  voidRadius=Control.EvalVar<double>(keyName+"VoidRadius");
 
  targetHeight=Control.EvalVar<double>(keyName+"TargetHeight");
  targetSectorOffsetX=Control.EvalVar<double>(keyName+"TargetSectorOffsetX");  
  targetSectorOffsetY=Control.EvalVar<double>(keyName+"TargetSectorOffsetY");  
  targetSectorOffsetZ=Control.EvalVar<double>(keyName+"TargetSectorOffsetZ");  
  targetSectorAngleXY=Control.EvalVar<double>(keyName+"TargetSectorAngleXY");  
  targetSectorAngleZ=Control.EvalVar<double>(keyName+"TargetSectorAngleZ");  
  targetSectorApertureXY=Control.EvalVar<double>
    (keyName+"TargetSectorApertureXY");
  
  targetSectorApertureZ=Control.EvalVar<double>(keyName+"TargetSectorApertureZ");  
  targetSectorNumber=Control.EvalVar<size_t>(keyName+"TargetSectorNumber");  
 
  coolantThickOut=Control.EvalVar<double>(keyName+"CoolantThickOut");  
  coolantThickIn=Control.EvalVar<double>(keyName+"CoolantThickIn");  

  caseThickZ=Control.EvalVar<double>(keyName+"CaseThickZ");  
  caseThickX=Control.EvalVar<double>(keyName+"CaseThickX");  

  voidThick=Control.EvalVar<double>(keyName+"VoidThick");  

  // SHAFT
  // shaftRadius=Control.EvalVar<double>(keyName+"ShaftRadius");
  shaftTopHeight=Control.EvalVar<double>(keyName+"ShaftTopHeight");
  shaftBaseHeight=Control.EvalVar<double>(keyName+"ShaftBaseHeight");

  shaftJointThick=Control.EvalVar<double>(keyName+"ShaftJointThick");
  shaftCoolThick=Control.EvalVar<double>(keyName+"ShaftCoolThick");
  shaftCladThick=Control.EvalVar<double>(keyName+"ShaftCladThick");
  shaftVoidThick=Control.EvalVar<double>(keyName+"ShaftVoidThick");

  shaftSupportRadius=Control.EvalVar<double>(keyName+"ShaftSupportRadius");
  shaftSupportThick=Control.EvalVar<double>(keyName+"ShaftSupportThick");

  shaftBaseThick=Control.EvalVar<double>(keyName+"ShaftBaseThick");
  shaftBaseFootThick=Control.EvalVar<double>(keyName+"ShaftBaseFootThick");

  wMat=ModelSupport::EvalMat<int>(Control,keyName+"WMat");  
  heMat=ModelSupport::EvalMat<int>(Control,keyName+"HeMat");  
  steelMat=ModelSupport::EvalMat<int>(Control,keyName+"SteelMat");  
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");  
  cladShaftMat=ModelSupport::EvalMat<int>(Control,keyName+"CladShaftMat");  
  coolingShaftMatInt=ModelSupport::EvalMat<int>(Control,
						keyName+"CoolingShaftMatInt");  
  coolingShaftMatExt=ModelSupport::EvalMat<int>(Control,
						keyName+"CoolingShaftMatExt");  

  return;
}

void
SegWheel::makeShaftSurfaces()
  /*!
    Construct surfaces for the inner shaft
  */
{
  ELog::RegMethod RegA("SegWheel","makeShaftSurfaces");
  
  ModelSupport::buildPlane(SMap,wheelIndex+1005,Origin+Z*shaftBaseHeight,Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+1006,Origin+Z*shaftTopHeight,Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+1015,Origin+Z*
			   (shaftBaseHeight-shaftBaseThick),Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+1025,Origin+Z*
			   (shaftBaseHeight-shaftBaseThick-shaftBaseFootThick),Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+1035,Origin+Z*
			   (shaftBaseHeight-shaftBaseThick
                            -shaftBaseFootThick-shaftVoidThick),Z);  

  ModelSupport::buildPlane(SMap,wheelIndex+1045,Origin-Z*
                          (targetHeight/2+coolantThickIn+
                           caseThickZ+shaftSupportThick),Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+1046,Origin+Z*
                           (targetHeight/2+coolantThickIn+
                           caseThickZ+shaftSupportThick),Z);   
  ModelSupport::buildPlane(SMap,wheelIndex+1055,Origin-Z*
                           (targetHeight/2+coolantThickIn+
                           caseThickZ+shaftSupportThick+shaftVoidThick),Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+1056,Origin+Z*
                           (targetHeight/2+coolantThickIn+
                           caseThickZ+shaftSupportThick+shaftVoidThick),Z);  

  ModelSupport::buildCylinder(SMap,wheelIndex+1007,Origin,
                              Z,innerRadius-shaftJointThick);  
  ModelSupport::buildCylinder(SMap,wheelIndex+1017,Origin,
                              Z,innerRadius+shaftCoolThick);  
  ModelSupport::buildCylinder(SMap,wheelIndex+1027,Origin,
                              Z,innerRadius+shaftCoolThick+shaftCladThick);
  ModelSupport::buildCylinder(SMap,wheelIndex+1037,Origin,
                              Z,innerRadius+shaftCoolThick+
                              shaftCladThick+shaftVoidThick);  
  ModelSupport::buildCylinder(SMap,wheelIndex+1047,Origin,
                              Z,shaftSupportRadius);  
  ModelSupport::buildCylinder(SMap,wheelIndex+1057,Origin,
                              Z,shaftSupportRadius+shaftVoidThick);    

  return;
}

void
SegWheel::makeShaftObjects(Simulation& System)
  /*!
    Construct the objects
    \param System :: Simulation object
   */
{
  ELog::RegMethod RegA("SegWheel","makeShaftObjects");

  std::string Out;
  std::string Out0;
  std::string Out1;
  std::string Out2;
  std::string Out3;

  // inner he body
  Out=ModelSupport::getComposite(SMap,wheelIndex,
				 "1005 -1006 -1017 ( -1007: -1045 ) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,0.0,Out));

  // inner steel body
   Out=ModelSupport::getComposite(SMap,wheelIndex,"(-1006 1045 1007 -7):(-1047 1017 1045 -45) : "
				  "(-1006 1017 -1027 -1045 1025):(-1017 -1005 1015) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,cladShaftMat,0.0,Out));

  // outer wheel support
  Out=ModelSupport::getComposite(SMap,wheelIndex,"(-1006 -1047 1017 46 ( -1027 : -1046)) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,cladShaftMat,0.0,Out));

  // wheel cooling downcomer
  Out=ModelSupport::getComposite(SMap,wheelIndex," 7 -1017 -1006 1046 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,0.0,Out));

  // shaft cooling plate: Down for downcomer (open top), Up for upstream (open bottom)
  int SI0(wheelIndex);
  const std::string shaftCoolingDown=
  ModelSupport::getComposite(SMap,wheelIndex," 7 -1017 26 -1046 ");	
  const std::string shaftCoolingUp=
  ModelSupport::getComposite(SMap,wheelIndex," 7 -1017 -25 1045 ");	

  // the loop is very similar to the one for the wheel sectors (see below); surfaces from wheel::createSurfaces
  
  for(size_t i=0;i<targetSectorNumber;i++)
    {

      if (i<targetSectorNumber/4-1)
	{
	  Out0=ModelSupport::getComposite(SMap,wheelIndex,SI0," 503 -501  4M -3M ");
	  Out1=ModelSupport::getComposite(SMap,wheelIndex,SI0," 503  1M -2M ");
	  Out2=ModelSupport::getComposite(SMap,wheelIndex,SI0," 503 -501 12M -4M ");
	  Out3=ModelSupport::getComposite(SMap,wheelIndex,SI0," 503  -1M 3M ");  
	}
      else if (i>=targetSectorNumber/4-1 && i<=targetSectorNumber/2-1)
	{
	  Out0=ModelSupport::getComposite(SMap,wheelIndex,SI0," -501 -503 4M -3M ");
	  Out1=ModelSupport::getComposite(SMap,wheelIndex,SI0," -501 -1M 2M ");
	  Out2=ModelSupport::getComposite(SMap,wheelIndex,SI0," -501 -503 11M -4M ");
	  Out3=ModelSupport::getComposite(SMap,wheelIndex,SI0," -501  -2M 3M ");   
	}
      else if (i>=targetSectorNumber/2-1 && i<=targetSectorNumber*3/4-1)
	{
	  Out0=ModelSupport::getComposite(SMap,wheelIndex,SI0," -503  4M -3M ");
	  Out1=ModelSupport::getComposite(SMap,wheelIndex,SI0," -503 -1M 2M ");
	  Out2=ModelSupport::getComposite(SMap,wheelIndex,SI0," -503  11M -4M ");
	  Out3=ModelSupport::getComposite(SMap,wheelIndex,SI0," -503  -2M 3M");  
	}
      else
	{
	  Out0=ModelSupport::getComposite(SMap,wheelIndex,SI0," 501  4M -3M ");
	  Out1=ModelSupport::getComposite(SMap,wheelIndex,SI0," 501  1M -2M ");
	  Out2=ModelSupport::getComposite(SMap,wheelIndex,SI0," 501  12M -4M ");
	  Out3=ModelSupport::getComposite(SMap,wheelIndex,SI0," 501  -1M 3M");  
	  if(i==targetSectorNumber-1)
	    Out2=ModelSupport::getComposite(SMap,wheelIndex,SI0,"503 2 -4M");
	}
    
    System.addCell(MonteCarlo::Qhull(cellIndex++,cladShaftMat,
				     0.0,Out0+shaftCoolingDown));
    System.addCell(MonteCarlo::Qhull(cellIndex++,cladShaftMat,
				     0.0,Out0+shaftCoolingUp));

    System.addCell(MonteCarlo::Qhull(cellIndex++,cladShaftMat,
				     0.0,Out1+shaftCoolingDown));
    System.addCell(MonteCarlo::Qhull(cellIndex++,cladShaftMat,
				     0.0,Out1+shaftCoolingUp));

    System.addCell(MonteCarlo::Qhull(cellIndex++,cladShaftMat,
				     0.0,Out2+shaftCoolingDown));
    System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,
				     0.0,Out2+shaftCoolingUp));

    System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,
				     0.0,Out3+shaftCoolingDown));
    System.addCell(MonteCarlo::Qhull(cellIndex++,cladShaftMat,
				     0.0,Out3+shaftCoolingUp));
 
    SI0+=10;
   }

  // void in shaft foot
  Out=ModelSupport::getComposite(SMap,wheelIndex,"-1017 -1015 1025");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // void around shaft
  Out=ModelSupport::getComposite(SMap,wheelIndex," (-1037 1056 -1006 1027) : "
				 "(-1057 -1056 1027 46 (1046:1047)): (-1057 1027 -45 1055 (-1045:1047)): "
				 "(-1055 -1037 1035 (-1025:1027) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex,"-18 1035 -1006 (-1037:(-1056 1055))");
  addOuterSurf("Shaft",Out);  

  return;
}


void
SegWheel::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("SegWheel","createSurfaces");

  //  loop angles in sectors
  // sector aperture
  const double psi1(360.0/static_cast<double>(targetSectorNumber));
  const double psi2(targetSectorAngleXY);
  double psi(0.0);

  //  loop focal points for He wedge in sectors 
  double FPX(0.0);
  double FPY(targetSectorOffsetY);

  Geometry::Vec3D dirX(X);
  int SI1(wheelIndex);

  for(size_t i=0;i<targetSectorNumber;i++)
    { 
      Geometry::Quaternion::calcQRotDeg(psi,Z).rotate(dirX);
      ModelSupport::buildPlane(SMap,SI1+1,Origin-X*caseThickX,dirX);  
      ModelSupport::buildPlane(SMap,SI1+2,Origin+X*caseThickX,dirX);              
      Geometry::Quaternion::calcQRotDeg(-psi2,Z).rotate(dirX);
      ModelSupport::buildPlane(SMap,SI1+3,Origin-X*FPX+Y*FPY,dirX);
      
      FPX=targetSectorOffsetY*sin(static_cast<double>(i+1)*psi1*M_PI/180);
      FPY=targetSectorOffsetY*cos(static_cast<double>(i+1)*psi1*M_PI/180);

      psi=psi1+2.0*psi2;  
      
      Geometry::Quaternion::calcQRotDeg(psi,Z).rotate(dirX);   
      ModelSupport::buildPlane(SMap,SI1+4,Origin+Y*FPY-X*FPX,dirX);
      
      SI1+=10;
      
      psi=-psi2;
    }

  ModelSupport::buildPlane(SMap,wheelIndex+501,Origin,X);  
  ModelSupport::buildPlane(SMap,wheelIndex+503,Origin,Y);  

  double H(targetHeight/2.0);
  double HIn(targetHeight/2.0);
  double HOut(targetHeight/2.0);

  ModelSupport::buildPlane(SMap,wheelIndex+5,Origin-Z*H,Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+6,Origin+Z*H,Z);  

  HOut+=coolantThickOut; 
  ModelSupport::buildPlane(SMap,wheelIndex+15,Origin-Z*HOut,Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+16,Origin+Z*HOut,Z);  

  HIn+=coolantThickIn; 
  ModelSupport::buildPlane(SMap,wheelIndex+25,Origin-Z*HIn,Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+26,Origin+Z*HIn,Z);  

  HOut+=caseThickZ; 
  ModelSupport::buildPlane(SMap,wheelIndex+35,Origin-Z*HOut,Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+36,Origin+Z*HOut,Z);  

  HIn+=caseThickZ; 
  ModelSupport::buildPlane(SMap,wheelIndex+45,Origin-Z*HIn,Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+46,Origin+Z*HIn,Z);  

  H+=voidThick;
  ModelSupport::buildPlane(SMap,wheelIndex+55,Origin-Z*H,Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+56,Origin+Z*H,Z);  
  
  ModelSupport::buildCylinder(SMap,wheelIndex+7,Origin,Z,innerRadius);  
  
  int SI2(wheelIndex+10);
  for(size_t i=0;i<nLayers;i++)
    {
      ModelSupport::buildCylinder(SMap,SI2+7,Origin,Z,radius[i]);  
      SI2+=10;
    }
  
  ModelSupport::buildCylinder(SMap,wheelIndex+8,Origin,Z,coolantRadiusOut);  
  ModelSupport::buildCylinder(SMap,wheelIndex+9,Origin,Z,coolantRadiusIn);
  ModelSupport::buildCylinder(SMap,wheelIndex+10,Origin,Z,caseRadius);  
  ModelSupport::buildCylinder(SMap,wheelIndex+18,Origin,Z,shaftSupportRadius+shaftVoidThick);
  // 28 is duplicate of surface 17, but it adds to wheelIndex; it is the target limit
  ModelSupport::buildCylinder(SMap,wheelIndex+28,Origin,Z,radius[0]);
  ModelSupport::buildCylinder(SMap,wheelIndex+38,Origin,Z,coolantRadiusIn+caseThickZ);  
  ModelSupport::buildCylinder(SMap,wheelIndex+48,Origin,Z,voidRadius);  
  // 58 is duplicate of surface 257, but it adds to wheelIndex 
  ModelSupport::buildCylinder(SMap,wheelIndex+58,Origin,Z,radius[24]);  
  // 68 duplicates shaft surface 1017
  ModelSupport::buildCylinder(SMap,wheelIndex+68,Origin,Z,innerRadius+shaftCoolThick);  

  return; 
}

void
SegWheel::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("SegWheel","createObjects");

  // Inner Radius is 7
  
  const int matNum[4]={0,steelMat,heMat,wMat};
  std::string Out;
  std::string Out1;
  std::string Out2;
 
  // Loop through each item and build inner section

  // target top-base  
  const std::string TopBaseW=
    ModelSupport::getComposite(SMap,wheelIndex," 5 -6 ");	

  // 9 is surface dividing front and back target
  const std::string TopBaseSteelFront=
    ModelSupport::getComposite(SMap,wheelIndex," 15 -16 9 ");	
  
  const std::string TopBaseHeFront=
    ModelSupport::getComposite(SMap,wheelIndex," : -5: 6) 15 -16 9");	
  
  // 28 divides target from wheel body	
  const std::string TopBaseSteelBack=
    ModelSupport::getComposite(SMap,wheelIndex," 25 -26 -9 28");

  const std::string TopBaseHeBack=
    ModelSupport::getComposite(SMap,wheelIndex,": -5 :6) 25 -26 -9 28");

  const std::string TopBaseHeBody=
  ModelSupport::getComposite(SMap,wheelIndex,") 25 -26 -28");
  
  const std::string TopBaseSteelBody=
    ModelSupport::getComposite(SMap,wheelIndex,") 25 -26  -28");
  
  int SI3(wheelIndex);

  for(size_t i0=0;i0<targetSectorNumber;i0++)
    {
      int SI4(wheelIndex);
      for(size_t i1=0;i1<nLayers;i1++)
	{    
	  Out=ModelSupport::getComposite(SMap,SI3,SI4,"  7M -17M 4 -3");
	  if (matTYPE[i1]!=1)
	    Out+=TopBaseW;
	  else
	    Out+=TopBaseSteelBody;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,matNum[matTYPE[i1]],0.0,Out));  
	  SI4+=10;
	}

      std::string wheelQuarter;
      if(i0<targetSectorNumber/4-1)
	{ 
	  wheelQuarter = ModelSupport::getComposite(SMap,wheelIndex," 503 7 -8 ");
	  // add steel frame between sectors   
	  Out=ModelSupport::getComposite(SMap,SI3,"1 -2 ");
	  // coolant around target; 7: external to penultimate layer
	  Out1=ModelSupport::getComposite(SMap,SI4,SI3," -1M 12M (3M : -4M :  7");
	}
      else if(i0>=targetSectorNumber/4-1 && i0<=targetSectorNumber/2-1)
	{
	  wheelQuarter= ModelSupport::getComposite(SMap,wheelIndex," -501 7 -8 ");
	  Out=ModelSupport::getComposite(SMap,SI3," -1 2");
	  Out1=ModelSupport::getComposite(SMap,SI4,SI3," 11M -2M (3M : -4M : 7 ");
	}
      else if(i0>=targetSectorNumber/2-1 && i0<=targetSectorNumber*3/4-1)
	{
	  wheelQuarter= ModelSupport::getComposite(SMap,wheelIndex," -503 7 -8 ");
	  Out=ModelSupport::getComposite(SMap,SI3," -1 2");
	  Out1=ModelSupport::getComposite(SMap,SI4,SI3," 11M -2M (3M : -4M : 7 ");
	}
      else
	{
	  wheelQuarter= ModelSupport::getComposite(SMap,wheelIndex," 501 7 -8 ");
	  Out=ModelSupport::getComposite(SMap,SI3," 1 -2");
	  
	  if(i0==targetSectorNumber-1)
	    Out1=ModelSupport::getComposite(SMap,wheelIndex,SI3," -1M 2 (3M : -4M : 58 ");
	  else	
	    Out1=ModelSupport::getComposite(SMap,SI4,SI3," -1M 12M (3M : -4M : 7 ");
	}
      
      System.addCell(MonteCarlo::Qhull(cellIndex++, matNum[matTYPE[0]],0.0,
				       wheelQuarter+Out+TopBaseSteelFront));  
      
      System.addCell(MonteCarlo::Qhull(cellIndex++,matNum[matTYPE[0]],0.0,
				       wheelQuarter+Out+TopBaseSteelBack));  
      
      System.addCell(MonteCarlo::Qhull(cellIndex++,matNum[matTYPE[0]],0.0,
				       wheelQuarter+Out+TopBaseSteelBody));  
      
      System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,0.0,
				       wheelQuarter+Out1+TopBaseHeFront));
      
      System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,0.0,
				       wheelQuarter+Out1+TopBaseHeBack));
      
      System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,0.0,
				       wheelQuarter+Out1+TopBaseHeBody));
      
      SI3+=10;
      
    }
  
  // Metal cover
  Out=ModelSupport::getComposite(SMap,wheelIndex, 
				 "(-10 9 35 -36 (-15:16:8)):(-10 28 -38  45 -46 (-25:26)) "
				 " : (68 -28 45 -25):(68 -28 -46 26)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

  // Void surround
  Out=ModelSupport::getComposite(SMap,wheelIndex,"(38 -48 55 -56 (10:-35:36)) " 
				          ":(18 -38 55 -56 (-45:46))");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // outer world 
  //  Out=ModelSupport::getComposite(SMap,wheelIndex," (-48 55 -56 18) : "
  //                                            "(-18 68 45 -46): (7 -68 25 -26)");	
  Out=ModelSupport::getComposite(SMap,wheelIndex,"55 -56 -48");

  addOuterSurf("Wheel",Out);

  return; 
}

void
SegWheel::createLinks()
  /*!
    Creates a full attachment set
    Surfaces pointing outwards
  */
{
  ELog::RegMethod RegA("PressVessel","createLinks");
  // set Links :: Inner links:

  FixedComp::setConnect(0,Origin+Y*innerRadius,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(wheelIndex+7));

  FixedComp::setConnect(1,Origin+Y*voidRadius,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(wheelIndex+48));

  const double H1=(targetHeight/2.0)+voidThick;
  FixedComp::setConnect(2,Origin-Z*H1,-Z);
  FixedComp::setLinkSurf(2,-SMap.realSurf(wheelIndex+55));

  FixedComp::setConnect(3,Origin+Z*H1,Z);
  FixedComp::setLinkSurf(3,SMap.realSurf(wheelIndex+56));

  return;
}

void
SegWheel::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: LinkPt for origin / axis
   */
{
  ELog::RegMethod RegA("SegWheel","createAll");
  populate(System.getDataBase());

  WheelBase::createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);

  makeShaftSurfaces();
  makeShaftObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE instrumentSystem

