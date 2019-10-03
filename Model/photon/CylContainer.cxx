/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/CylContainer.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "CylContainer.h"

namespace photonSystem
{

CylContainer::CylContainer(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::LayerComp(0,0),
  attachSystem::FixedOffset(Key,6),
  mainCell(0)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

CylContainer::CylContainer(const CylContainer& A) : 
  attachSystem::ContainedComp(A),attachSystem::LayerComp(A),
  attachSystem::FixedOffset(A),
  radius(A.radius),height(A.height),
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
      attachSystem::FixedOffset::operator=(A);
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

  FixedOffset::populate(Control);

  double R,H,T;
  int M;
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  if (nLayers==0) nLayers=1;  // Layers include middle

  for(size_t i=0;i<nLayers;i++)
    {
      const std::string kN=keyName+std::to_string(i);
      H=Control.EvalTail<double>(kN,keyName,"Height");   
      R=Control.EvalTail<double>(kN,keyName,"Radius");   
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
CylContainer::createUnitVector(const attachSystem::FixedComp& FC,
			       const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \parma sideIndex :: link index
  */
{
  ELog::RegMethod RegA("CylContainer","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

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
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin,X);  
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin,Z);  

  int SI(buildIndex);
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
  int SI(buildIndex);
  for(size_t i=0;i<nLayers;i++)
    {
     Out=ModelSupport::getComposite(SMap,SI," -7 1 -2 ");

      if ((i+1)==nLayers) addOuterSurf(Out);
      if (i)
	Out+=ModelSupport::getComposite(SMap,SI-10," (7:-1:2) ");
      System.addCell(MonteCarlo::Object(cellIndex++,mat[i],temp[i],Out));
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
  const int SI(buildIndex+static_cast<int>(NL)*10);

  FixedComp::setConnect(0,Origin-Y*(height[NL]/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(SI+1));
  
  FixedComp::setConnect(1,Origin+Y*(height[NL]/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(SI+2));

  FixedComp::setConnect(2,Origin-X*radius[NL],-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(SI+7));
  FixedComp::setBridgeSurf(2,-SMap.realSurf(buildIndex+103));
  
  FixedComp::setConnect(3,Origin+X*radius[NL],X);
  FixedComp::setLinkSurf(3,SMap.realSurf(SI+7));
  FixedComp::setBridgeSurf(3,SMap.realSurf(buildIndex+103));

  FixedComp::setConnect(4,Origin-Z*radius[NL],-Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(SI+7));
  FixedComp::setBridgeSurf(4,-SMap.realSurf(buildIndex+105));
  
  FixedComp::setConnect(5,Origin+Z*radius[NL],Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(SI+7));
  FixedComp::setBridgeSurf(5,SMap.realSurf(buildIndex+105));
      

  return;
}


Geometry::Vec3D
CylContainer::getSurfacePoint(const size_t layerIndex,
                              const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \return Surface point
  */
{
  ELog::RegMethod RegA("CylContainer","getSurfacePoint");

  if (!sideIndex) return Origin;
  const size_t SI((sideIndex>0) ?
                  static_cast<size_t>(sideIndex-1) :
                  static_cast<size_t>(-1-sideIndex));

  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  // Modification map:
  switch(SI)
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
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex ");
}

int
CylContainer::getCommonSurf(const long int sideIndex) const
  /*!
    Given a side calculate the boundary surface
    \param sideIndex :: Side [1-6]
    \return Common dividing surface [outward pointing]
  */
{
  ELog::RegMethod RegA("CylContainererator","getCommonSurf");

  switch(std::abs(sideIndex))
    {
    case 1:
    case 2:
      return 0;
    case 3:
      return -SMap.realSurf(buildIndex+103);
    case 4:
      return SMap.realSurf(buildIndex+103);
    case 5:
      return -SMap.realSurf(buildIndex+105);
    case 6:
      return SMap.realSurf(buildIndex+105);
    }
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
}

std::string
CylContainer::getLayerString(const size_t layerIndex,
			     const long int sideIndex) const
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
  const int SI(buildIndex+NL*10);

  const long int uSIndex(std::abs(sideIndex));
  std::string Out;
  switch(uSIndex)
    {
    case 1:
      Out=ModelSupport::getComposite(SMap,SI," -1 ");
      break;
    case 2:
      Out=ModelSupport::getComposite(SMap,SI," 2 ");
      break;
    case 3:
    case 4:
    case 5:
    case 6:
      Out=ModelSupport::getComposite(SMap,SI," 7 ");
      break;
    default:
      throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
    }
  HeadRule HR(Out);
  if (sideIndex<0)
    HR.makeComplement();
  return HR.display();
}

int
CylContainer::getLayerSurf(const size_t layerIndex,
			   const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf. Surf points out
    \param sideIndex :: Side +/-[1-6] 
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface number [outgoing]
  */
{
  ELog::RegMethod RegA("CylContainer","getLayerSurf");

  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layerIndex");
  
  const int SI(buildIndex+static_cast<int>(layerIndex)*10);
  const long int uSIndex(std::abs(sideIndex));
  const int signValue((sideIndex>0) ? 1 : -1);

  switch(uSIndex)
    {
    case 1:
      return -signValue*SMap.realSurf(SI+1);
    case 2:
      return signValue*SMap.realSurf(SI+2);
    case 3:
    case 4:
    case 5:
    case 6:
      return signValue*SMap.realSurf(SI+7);
    }
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
}


void
CylContainer::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: Link poitn
   */
{
  ELog::RegMethod RegA("CylContainer","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE constructSystem
