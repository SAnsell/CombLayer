/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/Box.cxx
 *
 * Copyright (c) 2004-2022 by Konstantin Batkov
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
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
#include "ExternalCut.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "Box.h"

namespace essSystem
{

Box::Box(const std::string& Key)  :
  attachSystem::FixedRotate(Key,9),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::LayerComp(0),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Box::Box(const Box& A) :
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  attachSystem::LayerComp(A),
  attachSystem::CellMap(A),
  nLayers(A.nLayers),
  length(A.length),width(A.width),height(A.height),
  depth(A.depth),
  mat(A.mat),
  temp(A.temp),
  sideRuleHR(A.sideRuleHR)
  /*!
    Copy constructor
    \param A :: Box to copy
  */
{}

Box&
Box::operator=(const Box& A)
  /*!
    Assignment operator
    \param A :: Box to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      nLayers=A.nLayers;
      length=A.length;
      width=A.width;
      height=A.height;
      depth=A.depth;
      mat=A.mat;
      temp=A.temp;
      sideRuleHR=A.sideRuleHR;
    }
  return *this;
}

Box*
Box::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new Box(*this);
}

Box::~Box()
  /*!
    Destructor
  */
{}

void
Box::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("Box","populate");

  FixedRotate::populate(Control);

  double L(0.0);
  double W(0.0);
  double H(0.0);
  double D(0.0);
  double T(0.0);
  int M;
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  for (size_t i=0; i<nLayers; i++)
    {
      const std::string NStr(std::to_string(i));
      L+=Control.EvalVar<double>(keyName+"Length"+NStr) * ((i==0) ? 1.0 : 2.0);
      W+=Control.EvalVar<double>(keyName+"Width"+NStr) * ((i==0) ? 1.0 : 2.0);
      H+=Control.EvalVar<double>(keyName+"Height"+NStr);
      D+=Control.EvalVar<double>(keyName+"Depth"+NStr);
      M=ModelSupport::EvalMat<int>(Control,keyName+"Mat"+NStr);
      const std::string TStr=keyName+"Temp"+NStr;
      T=(!M || !Control.hasVariable(TStr)) ? 0.0 : Control.EvalVar<double>(TStr);

      length.push_back(L);
      width.push_back(W);
      height.push_back(H);
      depth.push_back(D);
      mat.push_back(M);
      temp.push_back(T);
    }

  return;
}

void
Box::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Box","createSurfaces");

  int SI(buildIndex);
  for (size_t i=0;i<nLayers;i++)
    {
      const double l = length[i]/2.0;
      const double w = width[i]/2.0;

      ModelSupport::buildPlane(SMap,SI+1,Origin-Y*l,Y);
      ModelSupport::buildPlane(SMap,SI+2,Origin+Y*l,Y);
      ModelSupport::buildPlane(SMap,SI+3,Origin-X*w,X);
      ModelSupport::buildPlane(SMap,SI+4,Origin+X*w,X);
      ModelSupport::buildPlane(SMap,SI+5,Origin-Z*(depth[i]),Z);
      ModelSupport::buildPlane(SMap,SI+6,Origin+Z*(height[i]),Z);
      SI += 10;
    }

  return;
}

void
Box::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Box","createObjects");

  HeadRule HR,innerHR;

  int SI(buildIndex);
  for (size_t i=0; i<nLayers; i++)
    {
      HR=ModelSupport::getHeadRule(SMap,SI,"1 -2 3 -4 5 -6");

      if (!i)
	makeCell("Inner",System,cellIndex++,mat[i],temp[i],HR*innerHR);
      else
	makeCell("Layer"+std::to_string(i),
		 System,cellIndex++,mat[i],temp[i],HR*innerHR);

      innerHR=HR.complement();
      SI += 10;
    }
  

  addOuterSurf(HR);
  sideRuleHR=ModelSupport::getHeadRule(SMap,SI-10,"1 -2 3 -4");

  return;
}


void
Box::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("Box","createLinks");

  if (nLayers)
    {
      const int SI(buildIndex+static_cast<int>(nLayers-1)*10);


      const double l = length[nLayers-1]/2.0;
      const double w = width[nLayers-1]/2.0;
      
      FixedComp::setConnect(0,Origin-Y*l,-Y);
      FixedComp::setLinkSurf(0,-SMap.realSurf(SI+1));
      FixedComp::setConnect(1,Origin+Y*l,Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(SI+2));
      
      FixedComp::setConnect(2,Origin-X*w,-X);
      FixedComp::setLinkSurf(2,-SMap.realSurf(SI+3));
      FixedComp::setConnect(3,Origin+X*w,X); 
      FixedComp::setLinkSurf(3,SMap.realSurf(SI+4));
      
      FixedComp::setConnect(4,Origin-Z*(depth[nLayers-1]),-Z);
      FixedComp::setLinkSurf(4,-SMap.realSurf(SI+5));
      FixedComp::setConnect(5,Origin+Z*(height[nLayers-1]),Z);
      FixedComp::setLinkSurf(5,SMap.realSurf(SI+6));
      
      // inner link points for F5 collimators
      FixedComp::setConnect(7,Origin-Z*depth[0],Z);
      FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+5));
      
      FixedComp::setConnect(8,Origin+Z*height[0],-Z);
      FixedComp::setLinkSurf(8,-SMap.realSurf(buildIndex+6));
    }
  return;
}

int
Box::getLayerSurf(const size_t layerIndex,
		  const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [1-4]
    \return Surface string
  */
{
  ELog::RegMethod RegA("Box","getLayerSurf");

  if (layerIndex>=nLayers)
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layerIndex");

  const int uSIndex(std::abs(static_cast<int>(sideIndex)));
  const int signValue((sideIndex>0) ? 1 : -1);  
  const int SI(10*static_cast<int>(layerIndex)+buildIndex);
  

  if (uSIndex<1 || uSIndex>6)
    throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
  
  return signValue*SMap.realSurf(SI+uSIndex);
}


std::string
Box::getLayerString(const size_t layerIndex,
		    const long int sideIndex) const
  /*!
    Given a side and a layer calculate the layer string
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [1-6]
    \return Surface string
  */
{
  ELog::RegMethod RegA("Box","getLayerString");

  if (layerIndex>nLayers)
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  const int SI(10*static_cast<int>(layerIndex)+buildIndex);

  HeadRule HR;
  const long int uSIndex(std::abs(sideIndex));
  switch(uSIndex)
    {
    case 1:
      HR=ModelSupport::getHeadRule(SMap,SI,"-1");
      break;
    case 2:
      HR=ModelSupport::getHeadRule(SMap,SI,"2");
      break;
    case 3:
      HR=ModelSupport::getHeadRule(SMap,SI,"-3");
      break;
    case 4:
      HR=ModelSupport::getHeadRule(SMap,SI,"4");
      break;
    case 5:
      HR=ModelSupport::getHeadRule(SMap,SI,"-5");
      break;
    case 6:
      HR=ModelSupport::getHeadRule(SMap,SI,"6");
      break;
    default:
      throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
    }
  if (sideIndex<0)
    HR.makeComplement();

  return HR.display();
}

Geometry::Vec3D
Box::getSurfacePoint(const size_t layerIndex,
		     const long int sideIndex) const
  /*!
    Given a side and a layer calculate the surface point
    \param layerIndex :: layer, 0 is inner moderator
    \param sideIndex :: Side [0-6]
   \return Surface point
  */
{
  ELog::RegMethod RegA("Box","getSurfacePoint");

  if (layerIndex>nLayers)
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");
  if (!sideIndex) return Origin;
  const size_t SI((sideIndex>0) ?
                  static_cast<size_t>(sideIndex-1) :
                  static_cast<size_t>(-1-sideIndex));
  
  const double l = length[layerIndex]/2.0;
  const double w = width[layerIndex]/2.0;

  switch(SI)
    {
    case 0:
      return Origin-Y*l;
    case 1:
      return Origin+Y*l;
    case 2:
      return Origin-X*w;
    case 3:
      return Origin+X*w;
    case 4:
      return Origin-Z*depth[layerIndex];
    case 5:
      return Origin+Z*height[layerIndex];
    }
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex ");
}



void
Box::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("Box","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  const double D=(depth.empty()) ? 0.0 : depth.back();
  applyShift(0,0,D);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
