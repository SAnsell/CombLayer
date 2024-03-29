/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/H2Wing.cxx 
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
#include <array>
#include <algorithm>
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
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
#include "ContainedComp.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "geomSupport.h"
#include "H2FlowGuide.h"
#include "H2Wing.h"

namespace essSystem
{

H2Wing::H2Wing(const std::string& baseKey,
	       const std::string& extraKey,
	       const double XYAngle) :
  attachSystem::FixedComp(baseKey+extraKey,16),
  attachSystem::ContainedComp(),
  attachSystem::LayerComp(0,0),
  attachSystem::CellMap(),
  baseName(baseKey),
  InnerComp(new H2FlowGuide(baseKey,extraKey,"FlowGuide")),
  xyOffset(XYAngle)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param baseKey :: Butterfly main key
    \param extraKey :: Specialized name
    \param XYAngle :: Offset of angle of Wing
  */
{

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  OR.addObject(InnerComp);
}

H2Wing::H2Wing(const H2Wing& A) : 
  attachSystem::FixedComp(A),
  attachSystem::ContainedComp(A),
  attachSystem::LayerComp(A),
  attachSystem::CellMap(A),
  baseName(A.baseName),
  InnerComp(A.InnerComp->clone()),
  Pts(A.Pts),radius(A.radius),height(A.height),
  modMat(A.modMat),modTemp(A.modTemp)
  /*!
    Copy constructor
    \param A :: H2Wing to copy
  */
{}

H2Wing&
H2Wing::operator=(const H2Wing& A)
  /*!
    Assignment operator
    \param A :: H2Wing to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      *InnerComp=*A.InnerComp;
      Pts=A.Pts;
      radius=A.radius;
      height=A.height;
      modMat=A.modMat;
      modTemp=A.modTemp;
    }
  return *this;
}

H2Wing*
H2Wing::clone() const
  /*!
    Virtual copy constructor
    \return new (this)
   */
{
  return new H2Wing(*this);
}
  
H2Wing::~H2Wing() 
  /*!
    Destructor
  */
{}

void
H2Wing::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("H2Wing","populate");

  engActive=Control.EvalTriple<int>(keyName,baseName,"","EngineeringActive");

  totalHeight=Control.EvalVar<double>(baseName+"TotalHeight");
  
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");

  for(size_t i=0;i<3;i++)
    {
      Pts[i]=Control.EvalVar<Geometry::Vec3D>
	(keyName+"Corner"+std::to_string(i+1));
      radius[i]=Control.EvalVar<double>
	(keyName+"Radius"+std::to_string(i+1));
    }

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");

  // Multi-layer
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  double T,mTemp;
  double VH, VD;
  int M;
  Thick.push_back(0.0);
  layerHeight.push_back(0.0);
  layerDepth.push_back(0.0);
  temp.push_back(modTemp);
  mat.push_back(modMat);
  double TH(0.0);
  for(size_t i=1;i<nLayers;i++)
    {
      const std::string Num=std::to_string(i);
      T=Control.EvalVar<double>(keyName+"Thick"+Num);
      VH=Control.EvalDefVar<double>(keyName+"Height"+Num,T);
      VD=Control.EvalDefVar<double>(keyName+"Depth"+Num,T);
      mTemp=Control.EvalDefVar<double>(keyName+"Temp"+Num,0.0);
      M=ModelSupport::EvalMat<int>(Control,keyName+"Mat"+Num);

      Thick.push_back(T);
      layerHeight.push_back(VH);
      layerDepth.push_back(VD);
      temp.push_back(mTemp);
      mat.push_back(M);
      TH+=VH+VD;
    }

  // calculated relative to 
  height=totalHeight-TH;
  if (height<Geometry::zeroTol)
    throw ColErr::NumericalAbort("Unable to calculate a negative height.\n"
				 "Thickness   == "+
				 std::to_string(height)+
				 "totalheight == "+
				 std::to_string(totalHeight));
  
  return;
}
  
void
H2Wing::createUnitVector(const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Points down the H2Wing direction
    - X Across the H2Wing
    - Z up (towards the target)
    \param FC :: fixed Comp [and link comp]
  */
{
  ELog::RegMethod RegA("H2Wing","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  const double dh = std::accumulate(layerDepth.begin(),layerDepth.end(),0.0) -
    std::accumulate(layerHeight.begin(),layerHeight.end(),0.0);

  applyShift(xStep,yStep, dh/2.0);
  applyAngleRotate(xyOffset,0.0);
  for(size_t i=0;i<3;i++)
    Pts[i]=realPt(Pts[i]);
  return;
}

void
H2Wing::createLinks()
  /*!
    Construct links for the triangle moderator
    The normal 1-3 and 4-5 are plane,
    7,8,9 are 
  */
{
  ELog::RegMethod RegA("H2Wing","createLinks");

  // Loop over corners that are bound by convex
  std::array<Geometry::Vec3D,3> CPts;
  std::array<Geometry::Vec3D,3> NPts;
  double PDepth(0.0);
  double VH(height/2.0);
  double VD(height/2.0);
  for(size_t i=1;i<nLayers;i++)
    {
      PDepth+=Thick[i];
      VH += layerHeight[i];
      VD += layerDepth[i];
    }
  cornerSet(PDepth,CPts,NPts);

  // mid plane points:
  const int triOffset(buildIndex+static_cast<int>(nLayers)*100);
  int ii(triOffset);
  for(size_t i=0;i<3;i++)
    {
      ii++;
      FixedComp::setConnect(i,(CPts[i]+CPts[(i+1)%3])/2.0,NPts[i]);
      FixedComp::setLinkSurf(i,SMap.realSurf(ii));
    }
  // Top/bottom

  FixedComp::setConnect(4,Origin-Z*VD,-Z);
  FixedComp::setConnect(5,Origin+Z*VH,Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(triOffset+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(triOffset+6));

  // OuterCorners and linkage
  FixedComp::setLinkSurf(6,getLayerHR(nLayers-1,-7));
  FixedComp::setLinkSurf(7,getLayerHR(nLayers-1,-8));
  FixedComp::setLinkSurf(8,getLayerHR(nLayers-1,-9));
  
  // INNER LINKS
  
  cornerSet(0.0,CPts,NPts);
  // mid plane points
  ii=buildIndex+100;
  for(size_t i=0;i<3;i++)
    {
      ii++;
      FixedComp::setConnect(i+9,(CPts[i]+CPts[(i+1)%3])/2.0,-NPts[i]);
      FixedComp::setLinkSurf(i+9,-SMap.realSurf(ii));
    }
  FixedComp::setConnect(12,Origin-Z*(height/2.0),Z);
  FixedComp::setConnect(13,Origin+Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(12,SMap.realSurf(buildIndex+105));
  FixedComp::setLinkSurf(13,-SMap.realSurf(buildIndex+106));


  
  return;
}
  

Geometry::Vec3D
H2Wing::realPt(const Geometry::Vec3D& CPt) const
  /*!
    Given a point put it into the origin frame
    \param CPt :: Point to convert
    \return Converted point
  */
{
  ELog::RegMethod RegA("H2Wing","realPt");

  Geometry::Vec3D APoint=
    Origin + X * CPt.X() + Y*CPt.Y();
  return APoint;
}

Geometry::Vec3D
H2Wing::realAxis(const Geometry::Vec3D& CPt) const
  /*!
    Given a point put it into the origin frame
    \param CPt :: Axis to convert
    \return Converted axis
  */
{
  ELog::RegMethod RegA("H2Wing","realAxis");

  Geometry::Vec3D APoint=
    X * CPt.X() + Y*CPt.Y();
  return APoint;
}

void
H2Wing::cornerSet(const double scale,
		  std::array<Geometry::Vec3D,3>& CPts,
		  std::array<Geometry::Vec3D,3>& NPts) const

  /*!
    Calculate the corners and the normals to the surfaces
    \param scale :: Distance out from the surface
    \param CPts :: Corner points
    \param NPts :: Normals 
  */
{
  ELog::RegMethod RegA("H2Wing","cornerSet");

  CPts=Pts;
  for(size_t i=0;i<3;i++)
    {
      const Geometry::Vec3D a=(Pts[(i+1)%3]-Pts[i]).unit();
      const Geometry::Vec3D b=(Pts[(i+2)%3]-Pts[i]).unit();
      const Geometry::Vec3D c=(Pts[(i+2)%3]-Pts[(i+1)%3]).unit();
      
      const double cosA=a.dotProd(b);
      const double sinA=sqrt(1.0-cosA*cosA);
      CPts[i]-=(a+b)*(scale/sinA);
      NPts[i]=(a*Z).unit();
      // Check -- direction -- highly inefficient
      if (NPts[i].dotProd(c)>0.0)
	NPts[i]*=-1.0;
    }
  return;
}

Geometry::Vec3D 
H2Wing::midNorm(const size_t index) const
  /*!
    Calcuate
    \param index :: Corner point
    \return Normal between two sides
  */
{
  ELog::RegMethod RegA("H2Wing","midNorm");

  const Geometry::Vec3D a=(Pts[(index+1)%3]-Pts[index]).unit();
  const Geometry::Vec3D b=(Pts[(index+2)%3]-Pts[index]).unit();
  return((a+b)/2.0);
}
  

void
H2Wing::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("H2Wing","createSurface");

  // Surfaces 1-6 are the FC container
  // Surfaces 11-16 are the outer blades etc

  //  const double PSteps[]={wallThick,flatClearance,0.0};  
  
  int triOffset(buildIndex+100);
  std::array<Geometry::Vec3D,3> CPts;
  std::array<Geometry::Vec3D,3> NPts;
  double PDepth(0.0);
  double VH(0.0);
  double VD(0.0);
  
  for(size_t j=0;j<nLayers;j++)
    {
      PDepth+=Thick[j];
      VH += layerHeight[j];
      VD += layerDepth[j];
      cornerSet(PDepth,CPts,NPts);
      for(size_t i=0;i<3;i++)
	{
	  const int ii(static_cast<int>(i)+1);
	  const Geometry::Vec3D MD= midNorm(i);
	  // Note :: Side norm faces out
	  ModelSupport::buildPlane(SMap,triOffset+ii,CPts[i],
				   NPts[i]);
	  
	  // corner centre
	  const std::tuple<Geometry::Vec3D,Geometry::Vec3D,Geometry::Vec3D>
	    RCircle=Geometry::findCornerCircle
	    (CPts[i],CPts[(i+1)%3],CPts[(i+2)%3],radius[i]+PDepth);

	  
	  // Positive inward
	  ModelSupport::buildPlane(SMap,triOffset+ii+20,
				   std::get<1>(RCircle),
				   std::get<2>(RCircle),
				   std::get<1>(RCircle)+Z,MD);
	  ModelSupport::buildCylinder(SMap,triOffset+ii+6,
				      std::get<0>(RCircle),
				      Z,radius[i]+PDepth);
	}
      
      ModelSupport::buildPlane(SMap,triOffset+5,Origin-
			       Z*(VD+height/2.0),Z);
      ModelSupport::buildPlane(SMap,triOffset+6,Origin+
			       Z*(VH+height/2.0),Z);
      triOffset+=100;
    }

  // MidPlane divider optimizatoin:
  // Get original points again
  cornerSet(0.0,CPts,NPts);
  const Geometry::Vec3D midPoint((CPts[0]+CPts[1]+CPts[2])/3.0);
  for(size_t i=0;i<3;i++)
    {
      const int ii(buildIndex+1001+static_cast<int>(i));
      const Geometry::Vec3D sidePt((CPts[(i+1)%3]+CPts[i])/2.0);
      
      ModelSupport::buildPlane(SMap,ii,midPoint,midPoint+Z,
			       sidePt,CPts[(i+1) % 3]-sidePt);
    }
  return;
}
 
void
H2Wing::createObjects(Simulation& System)
  /*!
    Adds the main components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("H2Wing","createObjects");

  HeadRule HR,HRa,HRb,HRc;

  int triOffset(buildIndex+100);
  HeadRule InnerA,InnerB,InnerC;
  
  const HeadRule CutA=
    ModelSupport::getHeadRule(SMap,buildIndex," -1001 1003 ");
  const HeadRule CutB=
    ModelSupport::getHeadRule(SMap,buildIndex," 1001 -1002 ");
  const HeadRule CutC=
    ModelSupport::getHeadRule(SMap,buildIndex," 1002 -1003 ");
  
  for(size_t i=0;i<nLayers;i++)
    {
      InnerA.makeComplement();
      InnerB.makeComplement();
      InnerC.makeComplement();

      HRa=ModelSupport::getHeadRule(SMap,triOffset,"-1 -3 5 -6 (21:-7)");
      HRb=ModelSupport::getHeadRule(SMap,triOffset,"-1 -2 5 -6 (22:-8)");
      HRc=ModelSupport::getHeadRule(SMap,triOffset,"-2 -3 5 -6 (23:-9)");

      if (!i && engActive)
	{
	  HR=ModelSupport::getHeadRule
	    (SMap,triOffset,"-1 -2 -3 5 -6 (21:-7) (22:-8) (23:-9)");
	  makeCell("Inner",System,cellIndex++,mat[i],temp[i],HR);
	}
      else
	{
	  System.addCell(cellIndex++,mat[i],temp[i],HRa*InnerA*CutA);
	  System.addCell(cellIndex++,mat[i],temp[i],HRb*InnerB*CutB);
	  System.addCell(cellIndex++,mat[i],temp[i],HRc*InnerC*CutC);
	}
      
      InnerA=HRa;
      InnerB=HRb;
      InnerC=HRc;
      triOffset+=100;
    }
  // Add last cell to cell map
  CellMap::setCell("Outer",cellIndex-1);

  triOffset-=100;
  HR=ModelSupport::getHeadRule
    (SMap,triOffset,"-1 -2 -3 5 -6 (21:-7) (22:-8) (23:-9)");
  addOuterSurf(HR);
  return;
}

Geometry::Vec3D
H2Wing::getSurfacePoint(const size_t layerIndex,
			const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \param sideIndex :: Side [0-3] // mid si-des   
    \return Surface point
  */
{
  ELog::RegMethod RegA("H2Wing","getSurfacePoint");
  if (layerIndex>=nLayers)
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layerIndex");
  
  // Loop over corners that are bound by convex
  std::array<Geometry::Vec3D,3> CPts;
  std::array<Geometry::Vec3D,3> NPts;
  double PDepth(0.0);
  double VHigh(height/2.0);
  double VDepth(height/2.0);

  for(size_t i=1;i<layerIndex;i++)
    {
      PDepth+=Thick[i];
      VHigh += layerHeight[i];
      VDepth += layerDepth[i];
    }
  cornerSet(PDepth,CPts,NPts);

  const size_t SI=static_cast<size_t>(std::abs(sideIndex));
  
  switch(SI)
    {
    case 0:
      return Origin;
    case 1:
    case 2:
    case 3:
      return (CPts[SI-1]+CPts[SI % 3])/2.0;
    case 5:
      return Origin-Z*VDepth;
    case 6:
      Origin+Z*VHigh;
    }

  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
}


HeadRule 
H2Wing::getLayerHR(const size_t layerIndex,
		   const long int sideIndex) const
  /*!
    Given a side and a layer calculate the surface bounding
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \param sideIndex :: Side [0-3]+mid sides   
    \return Surface HeadRule
  */
{
  ELog::RegMethod RegA("H2Wing","getLayerString");

  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layerIndex");

  const int triOffset(buildIndex+static_cast<int>((layerIndex+1)*100));
  HeadRule HR;
  const long int uSIndex(std::abs(sideIndex));
  switch(uSIndex)
    {
    case 1:
      HR=HeadRule(SMap,triOffset,1);
      break;
    case 2:
      HR=HeadRule(SMap,triOffset,2);
      break;
    case 3:
      HR=HeadRule(SMap,triOffset,3);
      break;
    case 5:
      HR=HeadRule(SMap,triOffset,-5);
      break;
    case 6:
      HR=HeadRule(SMap,triOffset,6);
      break;
    case 7:
      HR=ModelSupport::getHeadRule(SMap,triOffset,"-1 -3 (21:-7)");
      break;
    case 8:
      HR=ModelSupport::getHeadRule(SMap,triOffset,"-1 -2 (22:-8)");
      break;
    case 9:
      HR=ModelSupport::getHeadRule(SMap,triOffset,"-2 -3 (23:-9)");
      break;
    case 10:
      HR=ModelSupport::getHeadRule(SMap,triOffset,"(21:-7)");
      break;
    case 11:
      HR=ModelSupport::getHeadRule(SMap,triOffset,"(22:-8)");
      break;
    case 12:
      HR=ModelSupport::getHeadRule(SMap,triOffset,"(23:-9)");
      break;
    default:
      throw ColErr::IndexError<long int>(sideIndex,12,
					 "sideIndex + missing gap");
    }
  
  if (sideIndex<0)
    HR.makeComplement();

  return HR;
}


void
H2Wing::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed object just for origin/axis
    \param sideIndex :: side to link
  */
{
  ELog::RegMethod RegA("H2Wing","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);

  createLinks();
  insertObjects(System);

  if (engActive)
    InnerComp->createAll(System,*this,0);
  return;
}
  
}  // NAMESPACE moderatorSystem
