/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuild/DiskPreSimple.cxx
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
#include "ContainedComp.h"
#include "DiskPreMod.h"

namespace essSystem
{

DiskPreMod::DiskPreMod(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::LayerComp(0),
  attachSystem::FixedComp(Key,6),
  modIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(modIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

DiskPreMod::DiskPreMod(const DiskPreMod& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::LayerComp(A),attachSystem::FixedComp(A),
  modIndex(A.modIndex),cellIndex(A.cellIndex),radius(A.radius),
  height(A.height),depth(A.depth),mat(A.mat),temp(A.temp)
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
      cellIndex=A.cellIndex;
      radius=A.radius;
      height=A.height;
      depth=A.depth;
      mat=A.mat;
      temp=A.temp;
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
DiskPreMod::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("DiskPreMod","populate");

  // clear stuff 
  double R(0.0);
  double H(0.0);
  double D(0.0);
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
      M=ModelSupport::EvalMat<int>(Control,keyName+"Mat"+NStr);   
      const std::string TStr=keyName+"Temp"+NStr;
      T=(!M || !Control.hasVariable(TStr)) ?
	0.0 : Control.EvalVar<double>(TStr); 
      
      radius.push_back(R);
      height.push_back(H);
      depth.push_back(D);
      mat.push_back(M);
      temp.push_back(T);
    }

  return;
}

void
DiskPreMod::createUnitVector(const attachSystem::FixedComp& refCentre,
			     const attachSystem::FixedComp& targFC,
			     const long int linkIndex)
  /*!
    Create the unit vectors
    \param refCentre :: Centre for object
    \param targFC :: Target Unit [Z distance]
    \param linkIndex :: Link index for target
  */
{
  ELog::RegMethod RegA("DiskPreMod","createUnitVector");
  attachSystem::FixedComp::createUnitVector(refCentre);

  const Geometry::Vec3D ZAxis=
    targFC.getSignedLinkAxis(linkIndex);
  const Geometry::Vec3D ZTargPt=
    targFC.getSignedLinkPt(linkIndex);

  ELog::EM<<"OR == "<<Origin<<ELog::endDiag;
  Origin+=ZAxis*ZTargPt.dotProd(ZAxis);
  ELog::EM<<"Z == "<<ZAxis<<ELog::endDiag;
  // move away from connection
  ELog::EM<<"OR == "<<Origin<<ELog::endDiag;
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
      SI+=10;
    }
  return; 
}

void
DiskPreMod::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("DiskPreMod","createObjects");

  std::string Out;

  int SI(modIndex);

  // Process even number of surfaces:
  HeadRule Inner;
  for(size_t i=0;i<nLayers;i++)
    {
      Out=ModelSupport::getComposite(SMap,SI," -7 5 -6 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out+Inner.display()));
      SI+=10;
      Inner.procString(Out);
      Inner.makeComplement();
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
  
  FixedComp::setConnect(4,Origin-Z*(height[nLayers-1]/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(SI+5));
  
  FixedComp::setConnect(5,Origin+Z*(height[nLayers-1]/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(SI+6));

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
      return Origin-X*(radius[layerIndex]);
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
		      const attachSystem::FixedComp& targFC,
		      const long int linkIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point
    \param linkIndex :: link index 
   */
{
  ELog::RegMethod RegA("DiskPreMod","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,targFC,linkIndex);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  return;
}

}  // NAMESPACE essSystem
