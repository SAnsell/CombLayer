/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BlockAddition.cxx
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
#include "Plane.h"
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
#include "FixedOffset.h"
#include "LayerComp.h"
#include "ContainedComp.h"
#include "BlockAddition.h"

namespace essSystem
{

BlockAddition::BlockAddition(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::LayerComp(0),
  attachSystem::FixedOffset(Key,6),
  blockIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(blockIndex+1),active(0),nLayers(0),
  edgeSurf(0)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

BlockAddition::BlockAddition(const BlockAddition& A) : 
  attachSystem::ContainedComp(A),attachSystem::LayerComp(A),
  attachSystem::FixedOffset(A),
  blockIndex(A.blockIndex),cellIndex(A.cellIndex),
  active(A.active),length(A.length),
  height(A.height),width(A.width),nLayers(A.nLayers),
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
      attachSystem::ContainedComp::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      active=A.active;
      length=A.length;
      height=A.height;
      width=A.width;
      nLayers=A.nLayers;
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

  FixedOffset::populate(Control);

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
	(StrFunc::makeString(keyName+"WallThick",i));   
      M=ModelSupport::EvalMat<int>
	(Control,StrFunc::makeString(keyName+"WallMat",i));   

      T=Control.EvalDefVar<double>
	(StrFunc::makeString(keyName+"WallTemp",i),0.0);   
            
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

  if (fabs(xyAngle)<Geometry::zeroTol)
    SMap.addMatch(blockIndex+1,edgeSurf);
  else
    {
      ModelSupport::buildRotatedPlane(SMap,blockIndex+1,
				      SMap.realPtr<Geometry::Plane>(edgeSurf),
				      xyAngle,Z,rotCent);
    }

  int BI(blockIndex);
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

std::string
BlockAddition::rotateItem(std::string LString)
  /*!
    Given a string convert to an angle rotate form
    \param LString :: Link string
    \return string of rotated surface
  */
{
  ELog::RegMethod RegA("BlockAddtion","rotateItem");

  if (fabs(xyAngle)<Geometry::zeroTol)
    return LString;

  std::ostringstream cx;
  std::string S;

  const Geometry::Quaternion QrotXY=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);

  int BI(blockIndex+1000);
  while(StrFunc::section(LString,S))
    {
      int N;
      if (StrFunc::convert(S,N))
	{
	  const Geometry::Plane* PN=
	    dynamic_cast<const Geometry::Plane*>
	    (SMap.realSurfPtr(abs(N)));
	  if (PN)
	    {
	      BI++;
	      ModelSupport::buildRotatedPlane(SMap,BI,PN,
					      xyAngle,Z,rotCent);
	      cx<<" "<<((N>0) ? BI : -BI);
	    }
	  else
	    cx<<" "<<S;
	}
      else
	cx<<" "<<S;
    }
  return cx.str();
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

  std::string Out;

  if (active)
    {
      Out=PMod.getLayerString(layerIndex,sideIndex);
      preModInner=rotateItem(Out);
      Out=PMod.getLayerString(layerIndex+2,sideIndex);
      preModOuter=rotateItem(Out);

      Out=ModelSupport::getComposite(SMap,blockIndex,"1 -2 3 -4 5 -6 ");
      Out+=preModInner;
      System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  

      int SI(blockIndex);
      for(size_t i=1;i<nLayers;i++)
	{ 
	  Out=ModelSupport::getComposite(SMap,SI,blockIndex,
					 "1M -12 13 -14 15 -16 (2:-3:4:-5:6)");
	  if(i==1)
	    Out+=preModInner;
	  else
	    Out+=preModOuter;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat[i],0.0,Out));
	  SI+=10;
	}

      Out=ModelSupport::getComposite(SMap,SI,blockIndex,"1M -2 3 -4 5 -6 ");
      addOuterSurf(Out);
    }

  return; 
}

std::string 
BlockAddition::createCut(const size_t layerIndex) const
  /*!
    Ugly function to create the cuts for the pre-moderator
    \param layerIndex :: layer for string
    \return string at layer
   */
{
  ELog::RegMethod RegA("BlockAddition","createCut");
  if (layerIndex>=nLayers)
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,keyName+":layerIndex");
  
  std::string Out;
  if (active)
    {
      const int SI(10*static_cast<int>(layerIndex)+blockIndex);
      Out=ModelSupport::getComposite(SMap,SI,blockIndex,
				   " (-1M:-3:4:-5:6) ");
    }
  return Out;
}

void
BlockAddition::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("BlockAddition","createLinks");
  
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(blockIndex+1));

  if (nLayers)
    {
      const int BI(blockIndex+10*static_cast<int>(nLayers-1));
      
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

int
BlockAddition::getLayerSurf(const size_t layerIndex,
			    const long int sideIndex) const
  /*!
    Given a side and a layer calculate the layerSurface
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner
    \return Surface number [signed]
  */
{
  ELog::RegMethod RegA("BlockAddition","getLayerSurf");

  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer/layerIndex");

  if (sideIndex>6 || sideIndex<-6 || !sideIndex)
    throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");

  const int SI(blockIndex+10*static_cast<int>(layerIndex));
  const int dirValue=(sideIndex<0) ? -1 : 1;
  const int uSIndex(static_cast<int>(std::abs(sideIndex)));

  if (sideIndex>1)
    return (sideIndex % 2) ? -dirValue*SMap.realSurf(SI+uSIndex) :
      dirValue*SMap.realSurf(SI+uSIndex);
  
  return -dirValue*SMap.realSurf(blockIndex+1);
}

std::string
BlockAddition::getLayerString(const size_t layerIndex,
			      const long int sideIndex) const
  /*!
    Given a side and a layer calculate the layerSurface
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface string
  */
{
  ELog::RegMethod RegA("BlockAddition","getLayerString");

  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  if (sideIndex>6 || sideIndex<-6 || !sideIndex)
    throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");

  // NEED To get conditional surface [IFF sideIndex==0]

  const int SI(blockIndex+10*static_cast<int>(layerIndex));
  const int uSIndex(static_cast<int>(std::abs(sideIndex)));
  if (uSIndex>1)
    {
      int signValue((sideIndex<0) ? -1 : 1);
      signValue*=((sideIndex % 2) ? -1 : 1);
      const int SurfN= signValue*SMap.realSurf(SI+uSIndex);
      return " "+StrFunc::makeString(SurfN)+" ";
    }
  const std::string Out=preModInner+" "+
    StrFunc::makeString(-SMap.realSurf(blockIndex+1));
  if (sideIndex<0)
    {
      HeadRule HR(Out);
      HR.makeComplement();
      return HR.display();
    }
  return Out;
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

}  // NAMESPACE instrumentSystem
