/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/H2Wing.cxx 
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "RuleSupport.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "Vert2D.h"
#include "Convex2D.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "CellMap.h"
#include "AttachSupport.h"
#include "geomSupport.h"
#include "ModBase.h"
#include "H2FlowGuide.h"
#include "H2Wing.h"

namespace essSystem
{

H2Wing::H2Wing(const std::string& baseKey,
	       const std::string& extraKey,
	       const double XYAngle) :
  attachSystem::ContainedComp(),
  attachSystem::LayerComp(0,0),
  attachSystem::FixedComp(baseKey+extraKey,16),
  attachSystem::CellMap(),
  baseName(baseKey),
  wingIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(wingIndex+1),
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
  attachSystem::ContainedComp(A),attachSystem::LayerComp(A),
  attachSystem::FixedComp(A),attachSystem::CellMap(A),
  baseName(A.baseName),
  wingIndex(A.wingIndex),cellIndex(A.cellIndex),
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
      attachSystem::ContainedComp::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
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
	(keyName+"Corner"+StrFunc::makeString(i+1));
      radius[i]=Control.EvalVar<double>
	(keyName+"Radius"+StrFunc::makeString(i+1));
    }

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");

  // Multi-layer
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  double T,mTemp;
  double VH, VD;
  int M;
  Thick.push_back(0.0);
  Height.push_back(0.0); Depth.push_back(0.0);
  temp.push_back(modTemp);
  mat.push_back(modMat);
  double TH(0.0);
  for(size_t i=1;i<nLayers;i++)
    {
      const std::string Num=StrFunc::makeString(i);
      T=Control.EvalVar<double>(keyName+"Thick"+Num);
      VH=Control.EvalDefVar<double>(keyName+"Height"+Num,T);
      VD=Control.EvalDefVar<double>(keyName+"Depth"+Num,T);
      mTemp=Control.EvalDefVar<double>(keyName+"Temp"+Num,0.0);
      M=ModelSupport::EvalMat<int>(Control,keyName+"Mat"+Num);

      Thick.push_back(T);
      Height.push_back(VH);
      Depth.push_back(VD);
      temp.push_back(mTemp);
      mat.push_back(M);
      TH+=VH+VD;
    }

  // calculated relative to 
  height=totalHeight-TH;
  if (height<Geometry::zeroTol)
    throw ColErr::NumericalAbort("Unable to calculate a negative height.\n"
				 "Thickness   == "+
				 StrFunc::makeString(height)+
				 "totalheight == "+
				 StrFunc::makeString(totalHeight));
  
  return;
}
  
void
H2Wing::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Points down the H2Wing direction
    - X Across the H2Wing
    - Z up (towards the target)
    \param FC :: fixed Comp [and link comp]
  */
{
  ELog::RegMethod RegA("H2Wing","createUnitVector");

  FixedComp::createUnitVector(FC);
  const double dh = std::accumulate(Depth.begin(), Depth.end(), 0.0) - std::accumulate(Height.begin(), Height.end(), 0.0); // difference between total depth and total height
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
  double VH(0.0), VD(0.0);
  for(size_t i=1;i<nLayers;i++)
    {
      PDepth+=Thick[i];
      VH += Height[i];
      VD += Depth[i];
    }
  cornerSet(PDepth,CPts,NPts);

  // mid plane points:
  const int triOffset(wingIndex+static_cast<int>(nLayers)*100);
  int ii(triOffset);
  for(size_t i=0;i<3;i++)
    {
      ii++;
      FixedComp::setConnect(i,(CPts[i]+CPts[(i+1)%3])/2.0,NPts[i]);
      FixedComp::setLinkSurf(i,SMap.realSurf(ii));
    }
  // Top/bottom

  FixedComp::setConnect(4,Origin-Z*(VD+height/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*(VH+height/2.0),Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(triOffset+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(triOffset+6));

  // INNER LINKS
  
  cornerSet(0.0,CPts,NPts);
  // mid plane points
  ii=wingIndex+100;
  for(size_t i=0;i<3;i++)
    {
      ii++;
      FixedComp::setConnect(i+8,(CPts[i]+CPts[(i+1)%3])/2.0,-NPts[i]);
      FixedComp::setLinkSurf(i+8,-SMap.realSurf(ii));
    }
  FixedComp::setConnect(12,Origin-Z*(height/2.0),Z);
  FixedComp::setConnect(13,Origin+Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(12,SMap.realSurf(wingIndex+105));
  FixedComp::setLinkSurf(13,-SMap.realSurf(wingIndex+106));

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
  
  int triOffset(wingIndex+100);
  std::array<Geometry::Vec3D,3> CPts;
  std::array<Geometry::Vec3D,3> NPts;
  double PDepth(0.0);
  double VH(0.0);
  double VD(0.0);
  
  for(size_t j=0;j<nLayers;j++)
    {
      PDepth+=Thick[j];
      VH += Height[j];
      VD += Depth[j];
      cornerSet(PDepth,CPts,NPts);
      for(size_t i=0;i<3;i++)
	{
	  const int ii(static_cast<int>(i)+1);
	  const Geometry::Vec3D MD= midNorm(i);
	  // Note :: Side norm faces out
	  ModelSupport::buildPlane(SMap,triOffset+ii,CPts[i],
				   NPts[i]);
	  
	  // corner centre
	  const Geometry::Vec3D RCent=
	    Geometry::cornerCircleTouch
	    (CPts[i],CPts[(i+1)%3],CPts[(i+2)%3],radius[i]+PDepth);
	  // Mid norm point INWARD!

	  std::pair<Geometry::Vec3D,Geometry::Vec3D> CutPair=
	    Geometry::cornerCircle
	    (CPts[i],CPts[(i+1)%3],CPts[(i+2)%3],radius[i]+PDepth);

	  // Positive inward
	  ModelSupport::buildPlane(SMap,triOffset+ii+20,
				   CutPair.first,CutPair.second,
				   CutPair.first+Z,MD);

	  
	  ModelSupport::buildCylinder(SMap,triOffset+ii+6,
				      RCent,Z,radius[i]+PDepth);
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
      const int ii(wingIndex+1001+static_cast<int>(i));
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

  std::string Out,OutA,OutB,OutC;

  int triOffset(wingIndex+100);
  HeadRule InnerA,InnerB,InnerC;
  
  const std::string CutA=
    ModelSupport::getComposite(SMap,wingIndex," -1001 1003 ");
  const std::string CutB=
    ModelSupport::getComposite(SMap,wingIndex," 1001 -1002 ");
  const std::string CutC=
    ModelSupport::getComposite(SMap,wingIndex," 1002 -1003 ");
  
  for(size_t i=0;i<nLayers;i++)
    {
      InnerA.makeComplement();
      InnerB.makeComplement();
      InnerC.makeComplement();

      OutA=ModelSupport::getComposite(SMap,triOffset,"-1 -3 5 -6 (21:-7)");
      OutB=ModelSupport::getComposite(SMap,triOffset,"-1 -2 5 -6 (22:-8)");
      OutC=ModelSupport::getComposite(SMap,triOffset,"-2 -3 5 -6 (23:-9) ");

      if (!i && engActive)
	{
	  Out=ModelSupport::getComposite
	    (SMap,triOffset,"-1 -2 -3 5 -6 (21:-7) (22:-8) (23:-9)");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
	  CellMap::setCell("Inner",cellIndex-1);
	}
      else
	{
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],
				       OutA+InnerA.display()+CutA));
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],
					   OutB+InnerB.display()+CutB));
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],
					   OutC+InnerC.display()+CutC));
	}
      
      InnerA.procString(OutA);
      InnerB.procString(OutB);
      InnerC.procString(OutC);
      triOffset+=100;
    }
  // Add last cell to cell map
  CellMap::setCell("Outer",cellIndex-1);

  triOffset-=100;
  OutA=ModelSupport::getComposite(SMap,triOffset,
				     "-1 -2 -3 5 -6 (21:-7) (22:-8) (23:-9)");
  addOuterSurf(OutA);

  return;
}

Geometry::Vec3D
H2Wing::getSurfacePoint(const size_t,
			const size_t) const
  /*!
    Given a side and a layer calculate the link point
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \param sideIndex :: Side [0-3] // mid sides   
    \return Surface point
  */
{
  ELog::RegMethod RegA("H2Wing","getSurfacePoint");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

int
H2Wing::getLayerSurf(const size_t layerIndex,
		     const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param layerIndex :: layer, 0 is inner moderator [0-3]
    \param sideIndex :: Side [0-3] // mid sides   
    \return Surface point
  */
{
  ELog::RegMethod RegA("H2Wing","getLayerSurf");
  throw ColErr::AbsObjMethod("Not implemented yet");

  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layerIndex");

  const int triIndex(wingIndex+static_cast<int>((layerIndex+1)*100));
  switch(sideIndex)
    {
    case 0:
      return SMap.realSurf(triIndex+1);
    case 1:
      return SMap.realSurf(triIndex+2);
    case 2:
      return SMap.realSurf(triIndex+3);
    case 5:
      return -SMap.realSurf(triIndex+5);
    case 6:
      return SMap.realSurf(triIndex+6);
    case 7:
      return SMap.realSurf(triIndex+7);
    case 8:
      return SMap.realSurf(triIndex+8);
    case 9:
      return SMap.realSurf(triIndex+9);
    }
  throw ColErr::IndexError<size_t>(sideIndex,9,"sideIndex");
}

std::string
H2Wing::getLayerString(const size_t layerIndex,
		       const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \param sideIndex :: Side [0-3] // mid sides   
    \return Surface point
  */
{
  ELog::RegMethod RegA("H2Wing","getLayerString");

  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layerIndex");

  const int triOffset(wingIndex+static_cast<int>((layerIndex+1)*100));
  std::string Out;

  switch(sideIndex)
    {
    case 0:
      return StrFunc::makeString(SMap.realSurf(triOffset+1));
    case 1:
      return StrFunc::makeString(SMap.realSurf(triOffset+2));
    case 2:
      return StrFunc::makeString(SMap.realSurf(triOffset+3));
    case 4:
      return StrFunc::makeString(-SMap.realSurf(triOffset+5));
    case 5:
      return StrFunc::makeString(SMap.realSurf(triOffset+6));
    case 6:
      return ModelSupport::getComposite(SMap,triOffset,"-1 -3 (21:-7) ");
    case 7:
      return ModelSupport::getComposite(SMap,triOffset,"-1 -2 (22:-8) ");
    case 8:
      return ModelSupport::getComposite(SMap,triOffset,"-2 -3 (23:-9) ");
    case 9:
      return ModelSupport::getComposite(SMap,triOffset," (21:-7) ");
    case 10:
      return ModelSupport::getComposite(SMap,triOffset," (22:-8) ");
    case 11:
      return ModelSupport::getComposite(SMap,triOffset," (23:-9) ");
    }
  
  throw ColErr::IndexError<size_t>(sideIndex,12,"sideIndex");
}


void
H2Wing::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed object just for origin/axis
  */
{
  ELog::RegMethod RegA("H2Wing","createAll");

  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createObjects(System);

  createLinks();
  insertObjects(System);

  if (engActive)
    InnerComp->createAll(System,*this);
  return;
}
  
}  // NAMESPACE moderatorSystem
