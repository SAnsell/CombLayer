/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/DiskPreMod.cxx
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
#include "FixedOffset.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "CylFlowGuide.h"
#include "OnionCooling.h"
#include "DiskPreMod.h"


namespace essSystem
{

DiskPreMod::DiskPreMod(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::LayerComp(0),
  attachSystem::FixedOffset(Key,9),
  attachSystem::CellMap(),attachSystem::SurfMap(),  
  modIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(modIndex+1),NWidth(0),
  InnerComp(new CylFlowGuide(Key+"FlowGuide")),
  onion(new OnionCooling(Key+"OnionCooling")),
  sideRule("")
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
  attachSystem::LayerComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  modIndex(A.modIndex),cellIndex(A.cellIndex),radius(A.radius),
  height(A.height),depth(A.depth),width(A.width),
  mat(A.mat),temp(A.temp),
  flowGuideType(A.flowGuideType),
  InnerComp(A.InnerComp->clone()),
  onion(A.onion->clone()),
  sideRule(A.sideRule)
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
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      cellIndex=A.cellIndex;
      radius=A.radius;
      height=A.height;
      depth=A.depth;
      width=A.width;
      mat=A.mat;
      temp=A.temp;
      flowGuideType=A.flowGuideType;
      *InnerComp=*A.InnerComp;
      *onion=*A.onion;
      sideRule=A.sideRule;
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
		     const double& outRadius)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
    \param outRadius :: Outer radius of reflector [for void fill]
  */
{
  ELog::RegMethod RegA("DiskPreMod","populate");

  FixedOffset::populate(Control);

  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");
  flowGuideType=Control.EvalVar<std::string>(keyName+"FlowGuideType");

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
      if (Control.hasVariable(keyName+"Radius"+NStr))
	R=Control.EvalVar<double>(keyName+"Radius"+NStr);
      else
	R+=Control.EvalVar<double>(keyName+"Thick"+NStr);
      W+=Control.EvalDefVar<double>(keyName+"Width"+NStr,0.0);
      M=ModelSupport::EvalMat<int>(Control,keyName+"Mat"+NStr);   
      const std::string TStr=keyName+"Temp"+NStr;
      T=(!M || !Control.hasVariable(TStr)) ?
	0.0 : Control.EvalVar<double>(TStr); 

      if (R>Geometry::zeroTol)
	radius.push_back(R);
      else
	radius.push_back(outerRadius+R);
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
  Origin=refCentre.getLinkPt(sideIndex);
  if (zRotate)
    {
      X*=-1;
      Z*=-1;
    }
  const double D=(depth.empty()) ? 0.0 : depth.back();

  zStep += D;
  applyOffset();

  return;
}


void
DiskPreMod::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("DiskPreMod","createSurfaces");

  // Divide plane
  ModelSupport::buildPlane(SMap,modIndex+1,Origin,X);  
  ModelSupport::buildPlane(SMap,modIndex+2,Origin,Y);  


  int SI(modIndex);
  for(size_t i=0;i<nLayers;i++)
    {
      ModelSupport::buildCylinder(SMap,SI+7,Origin,Z,radius[i]);

      ModelSupport::buildPlane(SMap,SI+5,Origin-Z*depth[i],Z);  
      ModelSupport::buildPlane(SMap,SI+6,Origin+Z*height[i],Z);
      SurfMap::setSurf("LayerBase"+std::to_string(i),SMap.realSurf(SI+5));
      SurfMap::setSurf("LayerTop"+std::to_string(i),SMap.realSurf(SI+6));
      
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
DiskPreMod::createObjects(Simulation& System)
  /*!
    Create the disc component
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("DiskPreMod","createObjects");

  std::string Out;

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
      Out=ModelSupport::getComposite(SMap,SI," -7 5 -6 ");

	
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],
				       Out+widthUnit+
				       Inner.display()+Width.display()));
      if (!i)
	CellMap::setCell("Inner", cellIndex-1);

      SI+=10;
      Inner.procString(Out);
      Inner.makeComplement();
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

  sideRule=ModelSupport::getComposite(SMap,SI," -7 ");

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
                            const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \param sideIndex :: Side [0-6] 
   \return Surface point
  */
{
  ELog::RegMethod RegA("DiskPreMod","getSurfacePoint");

  if (layerIndex>nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");
  if (!sideIndex) return Origin;
  const size_t SI((sideIndex>0) ?
                  static_cast<size_t>(sideIndex-1) :
                  static_cast<size_t>(-1-sideIndex));

  switch(SI)
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
      return Origin-Z*(depth[layerIndex]);
    case 5:
      return Origin+Z*(height[layerIndex]);
    }
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex ");
}


int
DiskPreMod::getLayerSurf(const size_t layerIndex,
			const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [1-4]
    \return Surface string
  */
{
  ELog::RegMethod RegA("DiskPreMod","getLayerSurf");

  if (layerIndex>nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  const int SI(10*static_cast<int>(layerIndex)+modIndex);
  const long int uSIndex(std::abs(sideIndex));
  const int signValue((sideIndex>0) ? 1 : -1);
	       
  switch(uSIndex)
    {
    case 1:
    case 2:    
    case 3:
    case 4:
      return signValue*SMap.realSurf(SI+7);
    case 5:
      return -signValue*SMap.realSurf(SI+5);
    case 6:
      return signValue*SMap.realSurf(SI+6);
    }
  throw ColErr::IndexError<long int>(sideIndex,7,"sideIndex");
}

std::string
DiskPreMod::getLayerString(const size_t layerIndex,
			   const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [0-3]
    \return Surface string
  */
{
  ELog::RegMethod RegA("DiskPreMod","getLayerString");

  if (layerIndex>nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  const int SI(10*static_cast<int>(layerIndex)+modIndex);

  std::string Out;
  const long int uSIndex(std::abs(sideIndex));
  switch(uSIndex)
    {
    case 1:
      Out=ModelSupport::getComposite(SMap,SI,modIndex," 7 -2M ");
      break;
    case 2:
      Out=ModelSupport::getComposite(SMap,SI,modIndex," 7 2M ");
      break;
    case 3:
      Out=ModelSupport::getComposite(SMap,SI,modIndex," 7 -1M ");
      break;
    case 4:
      Out=ModelSupport::getComposite(SMap,SI,modIndex," 7 1M ");
      break;
    case 5:
      Out=ModelSupport::getComposite(SMap,SI," -5 ");
      break;
    case 6:
      Out=ModelSupport::getComposite(SMap,SI," 6 ");
      break;
    default:
      throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
    }
  if (sideIndex<0)
    {
      HeadRule HR(Out);
      HR.makeComplement();
      return HR.display();
    }
  return Out;
}


void
DiskPreMod::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex,
		      const bool zRotate,
		      const double& ORad)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point	       
    \param sideIndex :: side of object
    \param zRotate :: Rotate to -ve Z
    \param ORad :: Outer radius of zone
   */
{
  ELog::RegMethod RegA("DiskPreMod","createAll");

  populate(System.getDataBase(),ORad);
  createUnitVector(FC,sideIndex,zRotate);

  createSurfaces();
  createObjects(System);
  createLinks();

  insertObjects(System);

  if (engActive)
    {
      if (flowGuideType.find("Onion")!=std::string::npos)
          onion->createAll(System,*this);
      else
        InnerComp->createAll(System,*this,7);
    }

  return;
}

}  // NAMESPACE essSystem 
