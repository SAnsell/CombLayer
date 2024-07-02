/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BlockAddition.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
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
#include "support.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "LayerComp.h"
#include "ContainedComp.h"
#include "ExternalCut.h"

#include "BlockAddition.h"

namespace essSystem
{

BlockAddition::BlockAddition(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::LayerComp(0),
  attachSystem::ExternalCut(),
  active(0),edgeSurf(0)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

BlockAddition::BlockAddition(const BlockAddition& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::LayerComp(A),
  attachSystem::ExternalCut(A),
  active(A.active),length(A.length),
  height(A.height),width(A.width),
  wallThick(A.wallThick),waterMat(A.waterMat),
  edgeSurf(A.edgeSurf)
  /*!
    Copy constructor
    \param A :: BlockAddition to copy
  */
{}

BlockAddition&
BlockAddition::operator=(const BlockAddition& A)
  /*!
    Assignment operator
    \param A :: BlockAddition to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      active=A.active;
      length=A.length;
      height=A.height;
      width=A.width;
      wallThick=A.wallThick;
      waterMat=A.waterMat;
      edgeSurf=A.edgeSurf;
    }
  return *this;
}

BlockAddition::~BlockAddition()
  /*!
    Destructor
  */
{}

void
BlockAddition::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("BlockAddition","populate");

  FixedRotate::populate(Control);

  // Extension
  length=Control.EvalVar<double>(keyName+"Length");   
  width=Control.EvalVar<double>(keyName+"Width");   
  height=Control.EvalVar<double>(keyName+"Height");   

  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");   
  
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");   
  double WT(0.0),T(0.0);
  int M(waterMat);
  
  wallThick.push_back(WT);
  wallTemp.push_back(T);
  wallMat.push_back(M);
  
  for(size_t i=1;i<nLayers;i++)
    {
      WT+=Control.EvalVar<double>
	(keyName+"WallThick"+std::to_string(i));   
      M=ModelSupport::EvalMat<int>
	(Control,keyName+"WallMat"+std::to_string(i));   

      T=Control.EvalDefVar<double>
	(keyName+"WallTemp"+std::to_string(i),0.0);   
            
      wallThick.push_back(WT);
      wallTemp.push_back(T);
      wallMat.push_back(M);
    }
  
  return;
}

void
BlockAddition::createUnitVector(const Geometry::Vec3D& O,
				const Geometry::Vec3D& YAxis,
				const Geometry::Vec3D& ZAxis)
  /*!
    Create the unit vectors
    \param O :: Origin [calc from edge point]
    \param YAxis :: Direction of Y Axis
    \param ZAxis :: Direction of Z Axis 
  */
{
  ELog::RegMethod RegA("BlockAddition","createUnitVector");
  const Geometry::Plane* PPtr=
    SMap.realPtr<Geometry::Plane>(edgeSurf);

  Origin=O;
  Z=ZAxis;
  Y=YAxis;
  X=Z*Y;
  xStep+=wallThick.back();
  applyOffset();
  if (PPtr && PPtr->getNormal().dotProd(X)<0.0)
    X*=-1.0;
  return;
}

void
BlockAddition::createSurfaces()
  /*!
    Create planes for box layers
  */
{
  ELog::RegMethod RegA("BlockAddition","createSurfaces");

  // Inner planes

  if (std::abs(zAngle)<Geometry::zeroTol)
    SMap.addMatch(buildIndex+1,edgeSurf);
  else
    {
      ModelSupport::buildRotatedPlane(SMap,buildIndex+1,
				      SMap.realPtr<Geometry::Plane>(edgeSurf),
				      zAngle,Z,rotCent);
    }

  int BI(buildIndex);
  for(size_t i=0;i<nLayers;i++)
    {
      ModelSupport::buildPlane(SMap,BI+2,
			       Origin+Y*(length+wallThick[i]),Y);
      ModelSupport::buildPlane(SMap,BI+3,
			       Origin-X*wallThick[i],X);
      ModelSupport::buildPlane(SMap,BI+4,
			       Origin+X*(width+wallThick[i]),X);
      ModelSupport::buildPlane(SMap,BI+5,
			       Origin-Z*(height/2.0+wallThick[i]),Z);
      ModelSupport::buildPlane(SMap,BI+6,
			       Origin+Z*(height/2.0+wallThick[i]),Z);
      BI+=10;
    }
		       

  return; 
}

HeadRule
BlockAddition::rotateItem(HeadRule LHR)
  /*!
    Given a string convert to an angle rotate form
    \param LString :: Link string
    \return string of rotated surface
  */
{
  ELog::RegMethod RegA("BlockAddtion","rotateItem");

  if (std::abs<double>(zAngle)<Geometry::zeroTol)
    return LHR;

  const Geometry::Quaternion QrotXY=
    Geometry::Quaternion::calcQRotDeg(zAngle,Z);

  int BI(buildIndex+1000);
  std::set<const Geometry::Surface*> hSurf=
    LHR.getSurfaces();
  
  for(const Geometry::Surface* SPtr : hSurf)
    {
      const Geometry::Plane* PPtr=
	dynamic_cast<const Geometry::Plane*>(SPtr);

      if (PPtr)
	{
	  const int PN=PPtr->getName();
	  BI++;
	  const Geometry::Plane* PNewPtr=
	    ModelSupport::buildRotatedPlane
	    (SMap,BI,PPtr,zAngle,Z,rotCent);
	  
	  LHR.substituteSurf(PN,BI,PNewPtr);
	}
    }
  return LHR;
}

void
BlockAddition::createObjects(Simulation& System,
			     const attachSystem::LayerComp& PMod,
			     const size_t layerIndex,
			     const long int sideIndex)
  /*!
    Create the block object
    \param System :: Simulation to add results
    \param PMod :: Moderator fixed unit
    \param layerIndex :: Layer number
    \param sideIndex :: surface number
  */
{
  ELog::RegMethod RegA("BlockAddition","createObjects");

  HeadRule HR;

  if (active)
    {
      HR=PMod.getLayerHR(layerIndex,sideIndex);
      preModInner=rotateItem(HR);
      HR=PMod.getLayerHR(layerIndex+2,sideIndex);
      preModOuter=rotateItem(HR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6 ");
      System.addCell(cellIndex++,waterMat,0.0,HR*preModInner);
  

      int SI(buildIndex);
      for(size_t i=1;i<nLayers;i++)
	{ 
	  HR=ModelSupport::getHeadRule
	    (SMap,SI,buildIndex,"1M -12 13 -14 15 -16 (2:-3:4:-5:6)");
	  if(i==1)
	    HR*=preModInner;
	  else
	    HR*=preModOuter;
	  System.addCell(cellIndex++,wallMat[i],0.0,HR);
	  SI+=10;
	}

      HR=ModelSupport::getHeadRule(SMap,SI,buildIndex,"1M -2 3 -4 5 -6");
      addOuterSurf(HR);
    }

  return; 
}

HeadRule
BlockAddition::createCut(const size_t layerIndex) const
  /*!
    Ugly function to create the cuts for the pre-moderator
    \param layerIndex :: layer for string
    \return HeadRule at layer
   */
{
  ELog::RegMethod RegA("BlockAddition","createCut");
  if (layerIndex>=nLayers)
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,keyName+":layerIndex");
  
  HeadRule HR;
  if (active)
    {
      const int SI(10*static_cast<int>(layerIndex)+buildIndex);
      HR=ModelSupport::getHeadRule
	(SMap,SI,buildIndex,"(-1M:-3:4:-5:6)");
    }
  return HR;
}

void
BlockAddition::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("BlockAddition","createLinks");
  
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  if (nLayers)
    {
      const int BI(buildIndex+10*static_cast<int>(nLayers-1));
      
      FixedComp::setConnect(1,Origin+Y*(length+wallThick.back()),Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(BI+2));
      
      FixedComp::setConnect(2,Origin-X*wallThick.back(),-X);
      FixedComp::setLinkSurf(2,SMap.realSurf(BI+3));
      
      FixedComp::setConnect(3,Origin+X*(width+wallThick.back()),X);
      FixedComp::setLinkSurf(3,SMap.realSurf(BI+4));

      FixedComp::setConnect(4,Origin-Z*(height/2.0+wallThick.back()),-Z);
      FixedComp::setLinkSurf(4,SMap.realSurf(BI+5));
      
      FixedComp::setConnect(5,Origin+Z*(height/2.0+wallThick.back()),Z);
      FixedComp::setLinkSurf(5,SMap.realSurf(BI+6));
    }
  else 
    ELog::EM<<"NO Layers in BlockAddition"<<ELog::endErr;
  
  return;
}

Geometry::Vec3D
BlockAddition::getSurfacePoint(const size_t layerIndex,
                               const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param layerIndex :: Layer, 0 is inner moderator 
    \param sideIndex  :: Side [0-6]   
    \return Surface point
  */
{
  ELog::RegMethod RegA("BlockAddition","getSurfacePoint");

  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"nLayer/layerIndex");
  if (!sideIndex) return Origin;
  
  const double outDist=wallThick[layerIndex];
  const size_t SI((sideIndex>0) ?
                  static_cast<size_t>(sideIndex-1) :
                  static_cast<size_t>(-1-sideIndex));

  switch(SI)
    {
    case 0:
      return Origin;
    case 1:
      return Origin+Y*(outDist+length);
    case 2:
      return Origin-X*outDist;
    case 3:
      return Origin+X*(outDist+width);
    case 4:
      return Origin-Z*(outDist+height/2.0);
    case 5:
      return Origin+Z*(outDist+height/2.0);
    }
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
}

HeadRule
BlockAddition::getLayerHR(const size_t layerIndex,
			  const long int sideIndex) const
  /*!
    Given a side and a layer calculate the layerSurface
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface HeadRule
  */
{
  ELog::RegMethod RegA("BlockAddition","getLayerHeadRule");

  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  if (sideIndex>6 || sideIndex<-6 || !sideIndex)
    throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");

  // NEED To get conditional surface [IFF sideIndex==0]

  const int SI(buildIndex+10*static_cast<int>(layerIndex));
  const int uSIndex(static_cast<int>(std::abs(sideIndex)));
  if (uSIndex>1)
    {
      int signValue((sideIndex<0) ? -1 : 1);
      signValue*=((sideIndex % 2) ? -1 : 1);
      return HeadRule(SMap,SI,signValue*uSIndex);
    }
  HeadRule HR=preModInner*HeadRule(SMap,buildIndex,-1);
  if (sideIndex<0)
    HR.makeComplement();

  return HR;
}

void
BlockAddition::createAll(Simulation& System,
			 const Geometry::Vec3D& O,
			 const attachSystem::LayerComp& CylPreMod,
			 const size_t layerIndex,
			 const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param O :: Origin (from track intersect)
    \param FC :: FixedComp for the angle
    \param CylPreMod :: Cylindrical moderator [3 surfaces]
    \param layerIndex :: Level to join to
    \param sideIndex :: Index object direction
   */
{
  ELog::RegMethod RegA("BlockAddition","createAll");

  const attachSystem::FixedComp* FC=
    dynamic_cast<const attachSystem::FixedComp*>(&CylPreMod);

  if (FC)
    {
      populate(System.getDataBase());
      createUnitVector(O,FC->getLinkAxis(sideIndex),FC->getZ());

      createSurfaces();
      createObjects(System,CylPreMod,layerIndex,sideIndex);
      createLinks();
      insertObjects(System);       
    }

  return;
}

}  // NAMESPACE essSystem
