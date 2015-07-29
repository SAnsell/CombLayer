/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/CylContainer.cxx
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
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "CylContainer.h"

namespace photonSystem
{

CylContainer::CylContainer(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::LayerComp(0,0),
  attachSystem::FixedComp(Key,6),
  cylIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(cylIndex+1),mainCell(0)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

CylContainer::CylContainer(const CylContainer& A) : 
  attachSystem::ContainedComp(A),attachSystem::LayerComp(A),
  attachSystem::FixedComp(A),
  cylIndex(A.cylIndex),cellIndex(A.cellIndex),xStep(A.xStep),
  yStep(A.yStep),zStep(A.zStep),xyAngle(A.xyAngle),
  zAngle(A.zAngle),radius(A.radius),height(A.height),
  mat(A.mat),temp(A.temp),mainCell(A.mainCell)
  /*!
    Copy constructor
    \param A :: CylContainer to copy
  */
{}

CylContainer&
CylContainer::operator=(const CylContainer& A)
  /*!
    Assignment operator
    \param A :: CylContainer to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      radius=A.radius;
      height=A.height;
      mat=A.mat;
      temp=A.temp;
      mainCell=A.mainCell;
    }
  return *this;
}


CylContainer::~CylContainer()
  /*!
    Destructor
  */
{}

CylContainer*
CylContainer::clone() const
  /*!
    Clone copy constructor
    \return copy of this
  */
{
  return new CylContainer(*this);
}

void
CylContainer::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("CylContainer","populate");

    // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  double R,H,T;
  int M;
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  if (nLayers==0) nLayers=1;  // Layers include middle

  for(size_t i=0;i<nLayers;i++)
    {
      const std::string kN=keyName+StrFunc::makeString(i);
      H=Control.EvalPair<double>(kN,keyName,"Height");   
      R=Control.EvalPair<double>(kN,keyName,"Radius");   
      M=ModelSupport::EvalMat<int>(Control,kN+"Mat",keyName+"Mat");
      
      T=Control.EvalDefVar<double>(keyName+"Temp",0.0);
      T=Control.EvalDefVar<double>(kN+"Temp",T);   

      radius.push_back(R);
      height.push_back(H);
      mat.push_back(M);
      temp.push_back(T);
    }
  return;
}

void
CylContainer::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
{
  ELog::RegMethod RegA("CylContainer","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

  return;
}

void
CylContainer::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("CylContainer","createSurfaces");

  // Divide plane
  ModelSupport::buildPlane(SMap,cylIndex+103,Origin,X);  
  ModelSupport::buildPlane(SMap,cylIndex+105,Origin,Z);  

  int SI(cylIndex);
  for(size_t i=0;i<nLayers;i++)
    {
      ModelSupport::buildCylinder(SMap,SI+7,Origin,Y,radius[i]);  
      ModelSupport::buildPlane(SMap,SI+1,Origin-Y*height[i]/2.0,Y);  
      ModelSupport::buildPlane(SMap,SI+2,Origin+Y*height[i]/2.0,Y);  
      SI+=10;
    }

  return; 
}

void
CylContainer::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("CylContainer","createObjects");

  std::string Out;
  mainCell=cellIndex;
  int SI(cylIndex);
  for(size_t i=0;i<nLayers;i++)
    {
     Out=ModelSupport::getComposite(SMap,SI," -7 1 -2 ");

      if ((i+1)==nLayers) addOuterSurf(Out);
      if (i)
	Out+=ModelSupport::getComposite(SMap,SI-10," (7:-1:2) ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
      SI+=10;
    }
  return; 
}

void
CylContainer::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("CylContainer","createLinks");

  if (!nLayers) return;
  const size_t NL(nLayers-1);
  const int SI(cylIndex+static_cast<int>(NL)*10);

  FixedComp::setConnect(0,Origin-Y*(height[NL]/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(SI+1));
  
  FixedComp::setConnect(1,Origin+Y*(height[NL]/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(SI+2));

  FixedComp::setConnect(2,Origin-X*radius[NL],-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(SI+7));
  FixedComp::setBridgeSurf(2,-SMap.realSurf(cylIndex+103));
  
  FixedComp::setConnect(3,Origin+X*radius[NL],X);
  FixedComp::setLinkSurf(3,SMap.realSurf(SI+7));
  FixedComp::setBridgeSurf(3,SMap.realSurf(cylIndex+103));

  FixedComp::setConnect(4,Origin-Z*radius[NL],-Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(SI+7));
  FixedComp::setBridgeSurf(4,-SMap.realSurf(cylIndex+105));
  
  FixedComp::setConnect(5,Origin+Z*radius[NL],Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(SI+7));
  FixedComp::setBridgeSurf(5,SMap.realSurf(cylIndex+105));
      

  return;
}


Geometry::Vec3D
CylContainer::getSurfacePoint(const size_t layerIndex,
			const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \return Surface point
  */
{
  ELog::RegMethod RegA("CylContainer","getSurfacePoint");

  if (sideIndex>5) 
    throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex ");
  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  // Modification map:
  switch(sideIndex)
    {
    case 0:
      return Origin-Y*(height[layerIndex]/2.0);
    case 1:
      return Origin+Y*(height[layerIndex]/2.0);
    case 2:
      return Origin-X*radius[layerIndex];
    case 3:
      return Origin+X*radius[layerIndex];
    case 4:
      return Origin-Z*radius[layerIndex];
    case 5:
      return Origin+Z*radius[layerIndex];
    }
  throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex ");
}

int
CylContainer::getCommonSurf(const size_t sideIndex) const
  /*!
    Given a side calculate the boundary surface
    \param sideIndex :: Side [0-5]
    \return Common dividing surface [outward pointing]
  */
{
  ELog::RegMethod RegA("CylContainererator","getCommonSurf");

  switch(sideIndex)
    {
    case 0:
    case 1:
      return 0;
    case 2:
      return -SMap.realSurf(cylIndex+103);
    case 3:
      return SMap.realSurf(cylIndex+103);
    case 4:
      return -SMap.realSurf(cylIndex+105);
    case 5:
      return SMap.realSurf(cylIndex+105);
    }
  throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex ");
}

std::string
CylContainer::getLayerString(const size_t layerIndex,
		       const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [0-5]
    \return Surface string
  */
{
  ELog::RegMethod RegA("CylContainer","getLayerString");

  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  const int NL(static_cast<int>(layerIndex));
  const int SI(cylIndex+NL*10);
  std::ostringstream cx;
  switch(sideIndex)
    {
    case 0:
      cx<<" "<<-SMap.realSurf(SI+1)<<" ";
      return cx.str();
    case 1:
      cx<<" "<<SMap.realSurf(SI+2)<<" ";
      return cx.str();
    case 2:
    case 3:
    case 4:
    case 5:
      cx<<" "<<SMap.realSurf(SI+7)<<" ";
      return cx.str();
    }
  throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex");
}

int
CylContainer::getLayerSurf(const size_t layerIndex,
			   const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link surf. Surf points out
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface number [outgoing]
  */
{
  ELog::RegMethod RegA("CylContainer","getLayerSurf");

  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layerIndex");
  
  const int SI(cylIndex+static_cast<int>(layerIndex)*10);
  switch(sideIndex)
    {
    case 0:
      return -SMap.realSurf(SI+1);
    case 1:
      return SMap.realSurf(SI+2);
    case 2:
    case 3:
    case 4:
    case 5:
      return SMap.realSurf(SI+7);
    }
  throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex ");
}


void
CylContainer::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
   */
{
  ELog::RegMethod RegA("CylContainer","createAll");
  populate(System.getDataBase());

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE constructSystem
