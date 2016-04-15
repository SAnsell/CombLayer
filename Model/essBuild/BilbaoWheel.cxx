/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BilbaoWheel.cxx
 *
 * Copyright (c) 2015 by Konstantin Batkov
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
  WheelBase(Key), attachSystem::CellMap(),
  InnerComp(new BilbaoWheelInnerStructure(Key + "InnerStructure"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

BilbaoWheel::BilbaoWheel(const BilbaoWheel& A) : 
  WheelBase(A),attachSystem::CellMap(A),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),zAngle(A.zAngle),engActive(A.engActive),
  targetHeight(A.targetHeight),
  voidTungstenThick(A.voidTungstenThick),
  steelTungstenThick(A.steelTungstenThick),
  caseThickIn(A.caseThickIn),coolantThick(A.coolantThick),
  caseThick(A.caseThick),voidThick(A.voidThick),
  innerRadius(A.innerRadius),
  coolantRadiusIn(A.coolantRadiusIn),
  coolantRadiusOut(A.coolantRadiusOut),
  caseRadius(A.caseRadius),voidRadius(A.voidRadius),
  aspectRatio(A.aspectRatio),
  mainTemp(A.mainTemp),nSectors(A.nSectors),
  nLayers(A.nLayers),radius(A.radius),
  matTYPE(A.matTYPE),shaftHeight(A.shaftHeight),
  nShaftLayers(A.nShaftLayers),shaftRadius(A.shaftRadius),
  shaftMat(A.shaftMat),wMat(A.wMat),heMat(A.heMat),
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
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      engActive=A.engActive;
      targetHeight=A.targetHeight;
      voidTungstenThick=A.voidTungstenThick;
      steelTungstenThick=A.steelTungstenThick;
      caseThickIn=A.caseThickIn;
      coolantThick=A.coolantThick;
      caseThick=A.caseThick;
      voidThick=A.voidThick;
      innerRadius=A.innerRadius;
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

  // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

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
  coolantRadiusIn=Control.EvalVar<double>(keyName+"CoolantRadiusIn");  
  coolantRadiusOut=Control.EvalVar<double>(keyName+"CoolantRadiusOut");  
  caseRadius=Control.EvalVar<double>(keyName+"CaseRadius");  
  voidRadius=Control.EvalVar<double>(keyName+"VoidRadius");
  aspectRatio=Control.EvalVar<double>(keyName+"AspectRatio");

  mainTemp=Control.EvalVar<double>(keyName+"Temperature");
  targetHeight=Control.EvalVar<double>(keyName+"TargetHeight");
  voidTungstenThick=Control.EvalVar<double>(keyName+"VoidTungstenThick");
  steelTungstenThick=Control.EvalVar<double>(keyName+"SteelTungstenThick");
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
  Out=ModelSupport::getComposite(SMap,wheelIndex,"-7 5 -6");	  
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,mainTemp,Out));

  // steel : inner shroud above/below
  Out=ModelSupport::getComposite
    (SMap,wheelIndex," -1027 45 -46 (-35 : 36 2017)" ); 
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  // void
  Out=ModelSupport::getComposite
    (SMap,wheelIndex, " -1027 55 -56 (-45 : 46 2027)" );	
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // shaft
  int SI(wheelIndex);
  for (size_t i=0; i<nShaftLayers; i++)
    {
      if (i==0)
	{
	  Out=ModelSupport::getComposite
	    (SMap,SI,wheelIndex," -2007 6M -2006M ");
	  System.addCell(MonteCarlo::Qhull
			 (cellIndex++,shaftMat[i],mainTemp,Out));
	}
      else
	{
	  Out=ModelSupport::getComposite
	    (SMap,SI,SI-10,wheelIndex," -2007 2007M 26 -2006N ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,shaftMat[i],mainTemp,Out));
	}
      if (i==nShaftLayers-1)
	{
	  Out=ModelSupport::getComposite(SMap,SI,wheelIndex," -2007 56M -2006M ");
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
  std::string surf = "sq " + StrFunc::makeString(1./pow(R,2)) + " " +
    StrFunc::makeString(1./pow(R,2)) + " " +
    StrFunc::makeString(e) + " 0 0 0 -1 " +
    StrFunc::makeString(Origin[0]) + " " + // X
    StrFunc::makeString(Origin[1]) + " " + // Y 
    StrFunc::makeString(Origin[2]);        // Z

  return surf;
}

void
BilbaoWheel::createRadialSurfaces()
/*!
  Create planes for the inner structure iside BilbaoWheel
*/
{
  ELog::RegMethod RegA("BilbaoWheel","createRadialSurfaces");

  if (nSectors<2) return;

  int SI(wheelIndex+3000);
  double theta(0.0);
  const double dTheta = 360.0/nSectors;

  for (size_t j=0; j<nSectors; j++)
    {
      theta = j*dTheta;
      ModelSupport::buildPlaneRotAxis(SMap, SI+1, Origin, X, Z, theta);
      SI += 10;
    }
  // add 1st surface again with reversed normal - to simplify building cells
  SMap.addMatch(SI+1,SMap.realSurf(wheelIndex+3001));
  return; 
}


void
BilbaoWheel::divideRadial(Simulation& System, std::string& sides, int mat)
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

  int SJ(wheelIndex+3000);
  std::string Out;
  for(size_t j=0;j<nSectors;j++)
    {
      //      Out=ModelSupport::getComposite(SMap,wheelIndex,SI,SJ," 7M -17M 5 -6 1N -11N");
      Out=ModelSupport::getComposite(SMap, SJ, " 1 -11 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,mainTemp,Out+sides));  
      SJ+=10;
    }
  
}

void
BilbaoWheel::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
{
  ELog::RegMethod RegA("BilbaoWheel","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

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

  H+=caseThickIn;
  ModelSupport::buildPlane(SMap,wheelIndex+45,Origin-Z*H,Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+46,Origin+Z*H,Z);  

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
  for(size_t i=0;i<nLayers;i++)
    {
      Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 7M -17M 5 -6 ");

      if ((matTYPE[i]==3) && (engActive))
	{
	  System.addCell(MonteCarlo::Qhull(cellIndex++,matNum[matTYPE[i]],mainTemp,Out));  
	  CellMap::setCell("Inner",cellIndex-1);
	  nInner++;
	  if (nInner>1)
	    ELog::EM << "More than one spallation layer" << ELog::endErr;
	} else
	  divideRadial(System, Out, matNum[matTYPE[i]]);

      SI+=10;
    }

  // front coolant:
  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 7M -517 35 -36");
  divideRadial(System, Out, heMat);

  // Void above/below W
  Out=ModelSupport::getComposite(SMap,wheelIndex,SI,
				 " -7M 15 -16 (-5 : 6 2007)" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  // Steel above/below W
  Out=ModelSupport::getComposite(SMap,wheelIndex,SI,
				 " -7M 25 -26 (-15 : 16 2007)" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));


  // Coolant
  Out=ModelSupport::getComposite(SMap,wheelIndex,SI,
				 " -7M 35 -36 (-25 : 26 2007)" );	// He above/below W
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
				 "1027 55 -56 -537 (-125:126:527)");
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

  FixedComp::setConnect(0,Origin-Y*voidRadius,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(wheelIndex+537));
  FixedComp::setBridgeSurf(0,-SMap.realSurf(wheelIndex+1));

  FixedComp::setConnect(1,Origin+Y*voidRadius,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(wheelIndex+537));
  FixedComp::setBridgeSurf(1,SMap.realSurf(wheelIndex+1));

  //<<<<<<< HEAD
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
		     const attachSystem::FixedComp& FC)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
   */
{
  ELog::RegMethod RegA("BilbaoWheel","createAll");
  populate(System.getDataBase());

  createUnitVector(FC);
  createSurfaces();
  makeShaftSurfaces();

  createObjects(System);
  makeShaftObjects(System);

  createLinks();
  insertObjects(System);  

  if (engActive)
    InnerComp->createAll(System, *this);

  return;
}

}  // NAMESPACE instrumentSystem
