/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/DiskPreMod.cxx
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
#include "SurInter.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LayerComp.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "CylFlowGuide.h"
#include "DiskPreMod.h"
#include "Cone.h"
#include "Plane.h"
#include "Cylinder.h"

namespace essSystem
{

DiskPreMod::DiskPreMod(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::LayerComp(0),
  attachSystem::FixedComp(Key,9),
  attachSystem::CellMap(),  
  modIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(modIndex+1),NWidth(0),
  InnerComp(new CylFlowGuide(Key+"FlowGuide"))

  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  OR.addObject(InnerComp);
}

DiskPreMod::DiskPreMod(const DiskPreMod& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::LayerComp(A),attachSystem::FixedComp(A),
  attachSystem::CellMap(A),
  modIndex(A.modIndex),cellIndex(A.cellIndex),radius(A.radius),
  height(A.height),depth(A.depth),width(A.width),
  mat(A.mat),temp(A.temp),
  InnerComp(A.InnerComp->clone())
  /*!
    Copy constructor
    \param A :: DiskPreMod to copy
  */
{}

DiskPreMod&
DiskPreMod::operator=(const DiskPreMod& A)
  /*!
    Assignment operator
    \param A :: DiskPreMod to copy
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
      radius=A.radius;
      height=A.height;
      depth=A.depth;
      width=A.width;
      mat=A.mat;
      temp=A.temp;
      *InnerComp=*A.InnerComp;
   }
  return *this;
}

DiskPreMod*
DiskPreMod::clone() const
  /*!
    Clone self 
    \return new (this)
   */
{
  return new DiskPreMod(*this);
}

DiskPreMod::~DiskPreMod()
  /*!
    Destructor
  */
{}
  

void
DiskPreMod::populate(const FuncDataBase& Control,
		     const double zShift,
		     const double outRadius)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
    \param zShift :: Default offset height a
    \param outRadius :: Outer radius of reflector [for void fill]
  */
{
  ELog::RegMethod RegA("DiskPreMod","populate");

  ///< \todo Make this part of IParam NOT a variable
  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");

  zStep=Control.EvalDefVar<double>(keyName+"ZStep",zShift);
  outerRadius=outRadius;

  // clear stuff 
  double R(0.0);
  double H(0.0);
  double D(0.0);
  double W(0.0);
  double T;
  int M;
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");   
  for(size_t i=0;i<nLayers;i++)
    {
      const std::string NStr(StrFunc::makeString(i));
      H+=Control.EvalVar<double>(keyName+"Height"+NStr);
      D+=Control.EvalVar<double>(keyName+"Depth"+NStr);
      R+=Control.EvalPair<double>(keyName+"Radius"+NStr,
				  keyName+"Thick"+NStr);
      W+=Control.EvalDefVar<double>(keyName+"Width"+NStr,0.0);
      M=ModelSupport::EvalMat<int>(Control,keyName+"Mat"+NStr);   
      const std::string TStr=keyName+"Temp"+NStr;
      T=(!M || !Control.hasVariable(TStr)) ?
	0.0 : Control.EvalVar<double>(TStr); 
      
      radius.push_back(R);
      height.push_back(H);
      depth.push_back(D);
      width.push_back(W);
      mat.push_back(M);
      temp.push_back(T);
    }

  // Find first Width that has not increase from last:
  W=0.0;
  NWidth=0;
  while(NWidth<width.size() &&
	(width[NWidth]-W)>Geometry::zeroTol)
    {
      W+=width[NWidth];
      NWidth++;
    } 

  tiltAngle = Control.EvalDefVar<double>(keyName+"TiltAngle", 0.0); // must correspond to ZBase of the corresponding flight line !!! remove default or set it to zero
  tiltRadius = Control.EvalDefVar<double>(keyName+"TiltRadius", 20.0); // must correspond to ZBase of the corresponding flight line !!! remove default or set it to zero

  return;
}

void
DiskPreMod::createUnitVector(const attachSystem::FixedComp& refCentre,
			     const long int sideIndex,const bool zRotate)
  /*!
    Create the unit vectors
    \param refCentre :: Centre for object
    \param sideIndex :: index for link
    \param zRotate :: rotate Zaxis
  */
{
  ELog::RegMethod RegA("DiskPreMod","createUnitVector");
  attachSystem::FixedComp::createUnitVector(refCentre);
  Origin=refCentre.getSignedLinkPt(sideIndex);
  if (zRotate)
    {
      X*=-1;
      Z*=-1;
    }
  const double D=(depth.empty()) ? 0.0 : depth.back();
  applyShift(0,0,zStep+D);

  return;
}


void
DiskPreMod::createSurfaces(const bool tiltSide)
  /*!
    Create planes for the silicon and Polyethene layers
    \param tiltSide :: top/bottom side to tilt
  */
{
  ELog::RegMethod RegA("DiskPreMod","createSurfaces");

  // Divide plane
  ModelSupport::buildPlane(SMap,modIndex+1,Origin,X);  
  ModelSupport::buildPlane(SMap,modIndex+2,Origin,Y);  

  const double h = tiltRadius * tan(tiltAngle*M_PI/180.0); // cone must be shifted for the tilting to start at Y=tiltRadius

  int SI(modIndex);
  for(size_t i=0;i<nLayers;i++)
    {
      ModelSupport::buildCylinder(SMap,SI+7,Origin,Z,radius[i]);  
      // tilting:
      ModelSupport::buildCylinder(SMap,SI+8,Origin,Z,tiltRadius);  
      if (tiltSide)
	ModelSupport::buildCone(SMap, SI+9, Origin+Z*(depth[i]+h), Z, 90-tiltAngle, -1);
      else
	ModelSupport::buildCone(SMap, SI+9, Origin-Z*(depth[i]+h), Z, 90-tiltAngle, 1);

      ModelSupport::buildPlane(SMap,SI+5,Origin-Z*depth[i],Z);  
      ModelSupport::buildPlane(SMap,SI+6,Origin+Z*height[i],Z);
      if (i<NWidth)
	{
	  ModelSupport::buildPlane(SMap,SI+3,Origin-X*(width[i]/2.0),X);
	  ModelSupport::buildPlane(SMap,SI+4,Origin+X*(width[i]/2.0),X);
	}
      SI+=10;
    }
  if (radius.empty() || radius.back()<outerRadius-Geometry::zeroTol)
    ModelSupport::buildCylinder(SMap,SI+7,Origin,Z,outerRadius);
  return; 
}

void
DiskPreMod::createObjects(Simulation& System, const bool tiltSide)
  /*!
    Create the disc component
    \param System :: Simulation to add results
    \param tiltSide :: top/bottom side to tilt
  */
{
  ELog::RegMethod RegA("DiskPreMod","createObjects");

  std::string Out, Out1;

  int SI(modIndex);
  // Process even number of surfaces:
  HeadRule Inner;
  HeadRule Width;
  std::string widthUnit;
  for(size_t i=0;i<nLayers;i++)
    {
      if (i<NWidth)
	{
	  // previous width:
	  Width.procString(widthUnit);
	  Width.makeComplement();
	  widthUnit=ModelSupport::getComposite(SMap,SI," -3 4 ");
	}

      if (tiltAngle>Geometry::zeroTol)
	{
	  Out = ModelSupport::getComposite(SMap, SI, " ((-8 5 -6) : (8 -7 5 -9 -6)) ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i], Out+widthUnit+Inner.display()+Width.display()));
	  if (i==nLayers-1)
	    {
	      if (tiltSide)
		Out1 = ModelSupport::getComposite(SMap, SI, " -7 -6 9 ");
	      else
		Out1 = ModelSupport::getComposite(SMap, SI, " -7  5 9 ");
	      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0, Out1+widthUnit+Width.display()));
	      }
	    }
      else
	{
	  Out=ModelSupport::getComposite(SMap,SI," -7 5 -6 ");
      
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i], Out+widthUnit+Inner.display()+Width.display()));
	}

      if (!i)
	CellMap::setCell("Inner", cellIndex-1);

      Inner.procString(Out);
      Inner.makeComplement();

      SI+=10;
    }

  SI-=10;


  // Outer extra void
  if (radius.empty() || radius.back()<outerRadius-Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,SI," -17 5 -6 7");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      // For exit surface
      Out=ModelSupport::getComposite(SMap,SI," -17 5 -6 ");
    }

  addOuterSurf(Out);
  return; 
}

void
DiskPreMod::createLinks()
  /*!
    Creates a full attachment set
    First two are in the -/+Y direction and have a divider
    Last two are in the -/+X direction and have a divider
    The mid two are -/+Z direction
  */
{  
  ELog::RegMethod RegA("DiskPreMod","createLinks");

  const int SI(modIndex+static_cast<int>(nLayers-1)*10);
  FixedComp::setConnect(0,Origin-Y*radius[nLayers-1],-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(SI+7));
  FixedComp::setBridgeSurf(0,-SMap.realSurf(modIndex+2));

  FixedComp::setConnect(1,Origin+Y*radius[nLayers-1],Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(SI+7));
  FixedComp::setBridgeSurf(1,SMap.realSurf(modIndex+2));
  
  
  FixedComp::setConnect(2,Origin-X*radius[nLayers-1],-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(SI+7));
  FixedComp::addLinkSurf(2,-SMap.realSurf(modIndex+1));
  
  FixedComp::setConnect(3,Origin+X*radius[nLayers-1],X);
  FixedComp::setLinkSurf(3,SMap.realSurf(SI+7));
  FixedComp::addLinkSurf(3,SMap.realSurf(modIndex+1));
  
  FixedComp::setConnect(4,Origin-Z*depth[nLayers-1],-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(SI+5));

  FixedComp::setConnect(5,Origin+Z*height[nLayers-1],Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(SI+6));

  // inner links point inwards
  FixedComp::setConnect(6,Origin+Y*radius[0],-Y);
  FixedComp::setLinkSurf(6,-SMap.realSurf(modIndex+7));

  FixedComp::setConnect(7,Origin-Z*depth[0],Z);
  FixedComp::setLinkSurf(7,SMap.realSurf(modIndex+5));

  FixedComp::setConnect(8,Origin+Z*height[0],-Z);
  FixedComp::setLinkSurf(8,-SMap.realSurf(modIndex+6));

  return;
}

Geometry::Vec3D
DiskPreMod::getSurfacePoint(const size_t layerIndex,
			   const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \return Surface point
  */
{
  ELog::RegMethod RegA("DiskPreMod","getSurfacePoint");

  if (layerIndex>nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  switch(sideIndex)
    {
    case 0:
      return Origin-Y*(radius[layerIndex]);
    case 1:
      return Origin+Y*(radius[layerIndex]);
    case 2:
      return (layerIndex<NWidth) ? 
	Origin-X*(width[layerIndex]/2.0) :
	Origin-X*radius[layerIndex];
    
    case 3:
      return Origin+X*(radius[layerIndex]);
    case 4:
      return Origin-Z*(height[layerIndex]);
    case 5:
      return Origin+Z*(height[layerIndex]);
    }
  throw ColErr::IndexError<size_t>(sideIndex,6,"sideIndex ");
}


int
DiskPreMod::getLayerSurf(const size_t layerIndex,
			const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-3]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface string
  */
{
  ELog::RegMethod RegA("H2Moderator","getLinkSurf");

  if (layerIndex>nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  const int SI(10*static_cast<int>(layerIndex)+modIndex);
	       
  switch(sideIndex)
    {
    case 0:
    case 1:    
    case 2:
    case 3:
      return SMap.realSurf(SI+7);
    case 4:
      return -SMap.realSurf(SI+5);
    case 5:
      return SMap.realSurf(SI+6);
    }
  throw ColErr::IndexError<size_t>(sideIndex,6,"sideIndex ");
}

std::string
DiskPreMod::getLayerString(const size_t layerIndex,
			 const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [0-3]
    \return Surface string
  */
{
  ELog::RegMethod RegA("DiskPreMod","getLinkString");

  if (layerIndex>nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  const int SI(10*static_cast<int>(layerIndex)+modIndex);

  std::ostringstream cx;
  switch(sideIndex)
    {
    case 0:
      cx<<" "<<SMap.realSurf(SI+7)<<" "
	<< -SMap.realSurf(modIndex+2)<<" ";
      return cx.str();
    case 1:
      cx<<" "<<SMap.realSurf(SI+7)<<" "
	<< SMap.realSurf(modIndex+2)<<" ";
      return cx.str();
    case 2:
      cx<<" "<<SMap.realSurf(SI+7)<<" "
	<< -SMap.realSurf(modIndex+1)<<" ";
      return cx.str();
    case 3:
      cx<<" "<<SMap.realSurf(SI+7)<<" "
	<< SMap.realSurf(modIndex+1)<<" ";
      return cx.str();
    case 4:
      cx<<" "<<-SMap.realSurf(SI+5)<<" ";
      return cx.str();
    case 5:
      cx<<" "<<SMap.realSurf(SI+6)<<" ";
      return cx.str();
    }
  throw ColErr::IndexError<size_t>(sideIndex,4,"sideIndex ");
}


void
DiskPreMod::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex,
		      const bool zRotate,
		      const double VOffset,
		      const double ORad,
		      const bool tiltSide)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point	       
    \param sideIndex :: side of object
    \param zRotate :: Rotate to -ve Z
    \param VOffset :: Vertical offset from target
    \param ORad :: Outer radius of zone
    \param tiltSide :: top/bottom side to be tilted
   */
{
  ELog::RegMethod RegA("DiskPreMod","createAll");

  populate(System.getDataBase(),VOffset,ORad);
  createUnitVector(FC,sideIndex,zRotate);

  createSurfaces(tiltSide);
  createObjects(System, tiltSide);
  createLinks();

  insertObjects(System);

  if (engActive) 
    InnerComp->createAll(System,*this,7);
  

  return;
}

  const double DiskPreMod::getZFlightLine(const bool tiltSide) const
  /*!
    Return z-coordinate of intersection with flight line
    To be used for flight line height calculation, e.g.
    height = TopCapMod->getZFlightLine()-TopPreMod->getZFlightLine();

    \todo
    We use tan() instead of probably a more clever way to do it with surface intersection (SurInter::getPoint),
    but for some reason intersection with cone gives a wrong result.
    It seems the method SurInter::getPoint does not take into account that a cone in MCNP is made of two surfaces.
    
    \param tiltSide :: defines which side is tilted (top or bottom)
   */
  {
    const double z = tiltSide ? (Origin+Z*height[nLayers-1])[2] : (Origin-Z*depth[nLayers-1])[2];
    const double R = radius[nLayers-1]-tiltRadius;
    const double x = R * tan(tiltAngle*M_PI/180.0);
    return tiltSide ? z-x : z+x;

    // // this is how the same could have been done with SurInter::gePoint:
    // const int SI(modIndex+static_cast<int>(nLayers-1)*10);
    // const Geometry::Cone *tiltCone = SMap.realPtr<Geometry::Cone>(SI+9);
    // const Geometry::Plane *yPlane =  SMap.realPtr<Geometry::Plane>(modIndex+2);
    // const Geometry::Cylinder *outerCyl = SMap.realPtr<Geometry::Cylinder>(SI+7);

    // Geometry::Vec3D nearPt(radius[nLayers-1], 0.0, z);
    // Geometry::Vec3D p = SurInter::getPoint(yPlane, outerCyl, tiltCone, nearPt);
    // return p[3];
  }

}  // NAMESPACE essSystem 
