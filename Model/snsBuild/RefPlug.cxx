/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   snsBuild/RefPlug.cxx
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
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "Vec3D.h"
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "RefPlug.h"

namespace snsSystem
{

RefPlug::RefPlug(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::LayerComp(0)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

RefPlug::RefPlug(const RefPlug& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::LayerComp(A),
  height(A.height),depth(A.depth),
  radius(A.radius),temp(A.temp),mat(A.mat)
  /*!
    Copy constructor
    \param A :: RefPlug to copy
  */
{}

RefPlug&
RefPlug::operator=(const RefPlug& A)
  /*!
    Assignment operator
    \param A :: RefPlug to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::LayerComp::operator=(A);
      height=A.height;
      depth=A.depth;
      radius=A.radius;
      temp=A.temp;
      mat=A.mat;
    }
  return *this;
}

RefPlug::~RefPlug()
  /*!
    Destructor
  */
{}

RefPlug*
RefPlug::clone() const
  /*!
    Clone constructor
    \return new(this)
  */
{
  return new RefPlug(*this);
}

void
RefPlug::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("RefPlug","populate");
  FixedRotate::populate(Control);
    // Master values
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");

  double R,T(0.0);
  int M;
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");   
  for(size_t i=0;i<=nLayers;i++)
    {
      if (i)
	{
	  R+=Control.EvalVar<double>
	    (keyName+"RadGap"+std::to_string(i));   
	  M=ModelSupport::EvalMat<int>
	    (Control,keyName+"Material"+std::to_string(i));   

	  T=(!M) ? 0.0 : 
	    Control.EvalDefVar<double>
	    (keyName+"Temp"+std::to_string(i),T);   
	}
      else
	{
	  R=Control.EvalVar<double>(keyName+"Radius");   
	  M=ModelSupport::EvalMat<int>(Control,keyName+"Mat");   
	  T=Control.EvalVar<double>(keyName+"Temp");   
	}
      radius.push_back(R);
      mat.push_back(M);
      temp.push_back(T);
    }

  return;
}

void
RefPlug::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("RefPlug","createSurfaces");

  // Divide plane
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,X);  
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin,Y);  

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);  
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);  

  int SI(buildIndex);
  for(size_t i=0;i<=nLayers;i++)
    {
      ModelSupport::buildCylinder(SMap,SI+7,Origin,Z,radius[i]);  
      SI+=10;
    }

  // Across sides
  return; 
}

void
RefPlug::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("RefPlug","createObjects");

  HeadRule HR;
  int SI(buildIndex);
  for(size_t i=0;i<=nLayers;i++)
    {
      HR=ModelSupport::getHeadRule(SMap,SI,buildIndex,"-7 5M -6M");
      if (i==nLayers) addOuterSurf(HR);
      if (i)
	HR*=HeadRule(SMap,SI-10,7);
      System.addCell(cellIndex++,mat[i],temp[i],HR);
      SI+=10;
    }
  return; 
}

void
RefPlug::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("RefPlug","createLinks");

  if (nLayers)
    {
      const int SI(buildIndex+static_cast<int>(nLayers)*10);

      FixedComp::setConnect(0,Origin-Y*radius[nLayers],-Y);
      FixedComp::setLinkSurf(0,SMap.realSurf(SI+7));
      FixedComp::addLinkSurf(0,-SMap.realSurf(buildIndex+2));

      FixedComp::setConnect(1,Origin+Y*radius[nLayers],Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(SI+7));
      FixedComp::addLinkSurf(1,SMap.realSurf(buildIndex+2));

      FixedComp::setConnect(2,Origin-X*radius[nLayers],-X);
      FixedComp::setLinkSurf(2,SMap.realSurf(SI+7));
      FixedComp::addLinkSurf(2,-SMap.realSurf(buildIndex+1));

      FixedComp::setConnect(3,Origin+X*radius[nLayers],X);
      FixedComp::setLinkSurf(3,SMap.realSurf(SI+7));
      FixedComp::addLinkSurf(3,SMap.realSurf(buildIndex+1));
      
      FixedComp::setConnect(4,Origin-Z*depth,-Z);
      FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

      FixedComp::setConnect(5,Origin+Z*height,Z);
      FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));
    }
  else 
    ELog::EM<<"NO Layers in RefPlug"<<ELog::endErr;
  return;
}



Geometry::Vec3D
RefPlug::getSurfacePoint(const size_t layerIndex,
                         const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \param sideIndex :: Side [0-6]
    \return Surface point
  */
{
  ELog::RegMethod RegA("RefPlug","getSurfacePoint");

  if (!sideIndex) return Origin;
  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  const size_t SI((sideIndex>0) ?
                  static_cast<size_t>(sideIndex-1) :
                  static_cast<size_t>(-1-sideIndex));

  // Modification map:
  switch(SI)
    {
    case 0:
      return Origin-Y*radius[layerIndex];
    case 1:
      return Origin+Y*radius[layerIndex];
    case 2:
      return Origin-X*radius[layerIndex];
    case 3:
      return Origin+X*radius[layerIndex];
    case 4:
      return Origin-Z*depth;
    case 5:
      return Origin+Z*height;
    }
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
}

HeadRule
RefPlug::getLayerHR(const size_t layerIndex,
		    const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [1-6]
    \return Surface Rule
  */
{
  ELog::RegMethod RegA("RefPlug","getLayerHR");

  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  const int SI(buildIndex+static_cast<int>(layerIndex)*10);
  const long int uSIndex(std::abs(sideIndex));
  HeadRule HR;
  switch(uSIndex)
    {
    case 1:
      HR=ModelSupport::getHeadRule(SMap,SI,buildIndex,"7 -2M");
      break;
    case 2:
      HR=ModelSupport::getHeadRule(SMap,SI,buildIndex,"7 2M");
      break;
    case 3:
      HR=ModelSupport::getHeadRule(SMap,SI,buildIndex,"7 -1M");
      break;
    case 4:
      HR=ModelSupport::getHeadRule(SMap,SI,buildIndex,"7 -1M");
      break;
    case 5:
      HR=HeadRule(SMap,SI,-5);
      break;
    case 6:
      HR=HeadRule(SMap,SI,6);
      break;
    default:
      throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
    }
  
  if (sideIndex<0)
    HR.makeComplement();
  return HR;
  
}

HeadRule
RefPlug::getCommonSurf(const long int sideIndex) const
  /*!
    Given a side calculate the boundary surface
    \param sideIndex :: Side [1-6]
    \return Common dividing surface [outward pointing]
  */
{
  ELog::RegMethod RegA("RefPlug","getCommonSurf");

  switch(std::abs(sideIndex))
    {
    case 1:
      return HeadRule(SMap,buildIndex,-2);
    case 2:
      return HeadRule(SMap,buildIndex,2);
    case 3:
      return HeadRule(SMap,buildIndex,-1);
    case 4:
      return HeadRule(SMap,buildIndex,1);
    case 5:
    case 6:
      return HeadRule();
    }
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex ");
}

void
RefPlug::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
                   const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp for origin
    \param sideIndex :: link point
   */
{
  ELog::RegMethod RegA("RefPlug","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE snsSystem
