/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/PlateTarget.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <numeric>
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
#include "FixedUnit.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "boxValues.h"
#include "boxUnit.h"
#include "BoxLine.h"
#include "PlateTarget.h"

namespace ts1System
{

PlateTarget::PlateTarget(const std::string& Key)  :
  attachSystem::FixedComp(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PlateTarget::PlateTarget(const PlateTarget& A) : 
  attachSystem::FixedComp(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  height(A.height),width(A.width),nBlock(A.nBlock),
  tBlock(A.tBlock),taThick(A.taThick),
  waterThick(A.waterThick),waterHeight(A.waterHeight),
  WWidth(A.WWidth),WHeight(A.WHeight),taMat(A.taMat),
  wMat(A.wMat),waterMat(A.waterMat)
  /*!
    Copy constructor
    \param A :: PlateTarget to copy
  */
{}

PlateTarget&
PlateTarget::operator=(const PlateTarget& A)
  /*!
    Assignment operator
    \param A :: PlateTarget to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      height=A.height;
      width=A.width;
      nBlock=A.nBlock;
      tBlock=A.tBlock;
      taThick=A.taThick;
      waterThick=A.waterThick;
      waterHeight=A.waterHeight;
      WWidth=A.WWidth;
      WHeight=A.WHeight;
      taMat=A.taMat;
      wMat=A.wMat;
      waterMat=A.waterMat;
    }
  return *this;
}


PlateTarget::~PlateTarget() 
 /*!
   Destructor
 */
{}

void
PlateTarget::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("PlateTarget","populate");

  // Global values:
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  // Blocks:
  nBlock=Control.EvalVar<size_t>(keyName+"NBlocks");

  for(size_t index=0;index<nBlock;index++)
    {
      tBlock.push_back(Control.EvalVar<double>
		       (keyName+"Thick"+std::to_string(index)));
    }
	  
  // Master Ta dimensions:
  taThick=Control.EvalVar<double>(keyName+"TaThick");
  waterThick=Control.EvalVar<double>(keyName+"WaterThick");
  waterHeight=Control.EvalVar<double>(keyName+"WaterHeight");

  // Master W Dimensions:
  WWidth=Control.EvalVar<double>(keyName+"WWidth");
  WHeight=Control.EvalVar<double>(keyName+"WHeight");

  // Back plate:
  backPlateThick=Control.EvalVar<double>(keyName+"BPlateThick");
  IThick=Control.EvalVar<double>(keyName+"BPlateIThick");
  IWidth=Control.EvalVar<double>(keyName+"BPlateIWidth");
  IOffset=Control.EvalVar<double>(keyName+"BPlateIOffset");
  pinRadius=Control.EvalVar<double>(keyName+"BPlatePinRadius");
  
  // Ta Plate feedthrough
  feedXOffset=Control.EvalVar<double>(keyName+"FeedXOffset");
  feedHeight=Control.EvalVar<double>(keyName+"FeedHeight");
  feedWidth=Control.EvalVar<double>(keyName+"FeedWidth");

  blockType.resize(nBlock);
  for(size_t i=0;i<nBlock;i++)
    blockType[i]=Control.EvalDefVar<int>
      (keyName+"BlockType"+std::to_string(i),1);
  

  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
  taMat=ModelSupport::EvalMat<int>(Control,keyName+"TaMat");
  wMat=ModelSupport::EvalMat<int>(Control,keyName+"WMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");

  return;
}
  
void
PlateTarget::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed compontent [front of target void vessel]
    - Y Down the beamline
    \param sideIndex :: link pint
  */
{
  ELog::RegMethod RegA("PlateTarget","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  Origin=FC.getLinkPt(1);
  return;
}

void
PlateTarget::createSurfaces(const attachSystem::FixedComp& FC)
  /*!
    Create All the surfaces
    \param FC :: Fixed component for surface
  */
{
  ELog::RegMethod RegA("PlateTarget","createSurface");

  // First layer [Bulk]
  ModelSupport::buildPlane(SMap,buildIndex+3,
			   Origin-X*width,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,
			   Origin+X*width,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,
			   Origin-Z*height,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,
			   Origin+Z*height,Z);

  // Water Edge
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*waterHeight,Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*waterHeight,Z);
  // W block
  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*WWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*WWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+25,Origin-Z*WHeight,Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*WHeight,Z);

  SMap.addMatch(buildIndex+1004,FC.getLinkSurf(1));

  Geometry::Vec3D FPt(Origin);
  for(size_t i=0;i<nBlock;i++)
    {
      int surfNum(buildIndex+1011+10*static_cast<int>(i));
      // water layer
      FPt+=Y*waterThick;
      ModelSupport::buildPlane(SMap,surfNum++,FPt,Y);
      FPt+=Y*taThick;
      ModelSupport::buildPlane(SMap,surfNum++,FPt,Y);
      FPt+=Y*tBlock[i];
      ModelSupport::buildPlane(SMap,surfNum++,FPt,Y);
      FPt+=Y*taThick;
      ModelSupport::buildPlane(SMap,surfNum++,FPt,Y);
    }
  tBlock.push_back(backPlateThick);  // For use in connection later
  // BackPlate at 50:
  int surfNum(buildIndex+1011+10*static_cast<int>(nBlock));
  FPt+=Y*waterThick;
  ModelSupport::buildPlane(SMap,surfNum++,FPt,Y);

  ModelSupport::buildPlane(SMap,buildIndex+51,FPt+Y*IThick,Y);  
  ModelSupport::buildPlane(SMap,buildIndex+52,FPt+Y*backPlateThick,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+57,
			      Origin,Y,pinRadius);

  // FixedComp::setConnect(1,FPt+Y*backPlateThick,Y);

  // Edge voids
  ModelSupport::buildPlane(SMap,buildIndex+53,Origin-X*(IOffset+IWidth),X);
  ModelSupport::buildPlane(SMap,buildIndex+54,Origin-X*(IOffset-IWidth),X);
  ModelSupport::buildPlane(SMap,buildIndex+63,Origin+X*(IOffset-IWidth),X);
  ModelSupport::buildPlane(SMap,buildIndex+64,Origin+X*(IOffset+IWidth),X);
 
  // Back of target surface
  FixedComp::setConnect(1,FPt,Y);  
  return;
}

void
PlateTarget::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PlateTarget","createObjects");

  HeadRule HR;

  const HeadRule WEdge=
    ModelSupport::getHeadRule(SMap,buildIndex,"23 -24 25 -26");
  const HeadRule H2OEdge=
    ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 15 -16");
  const HeadRule TaEdge=
    ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6");

  int surfNum;
  HeadRule prevWater=ModelSupport::getHeadRule(SMap,buildIndex,buildIndex,"1004");
  // Make plates:
  for(size_t i=0;i<nBlock;i++)
    {
      surfNum=buildIndex+1000+10*static_cast<int>(i);
      const std::string numStr(std::to_string(i));
      if (blockType[i])
	{
	  // W CELL Full:
	  HR=ModelSupport::getHeadRule(SMap,surfNum,"12 -13");
	  makeCell("WCell"+numStr,System,cellIndex++,wMat,0.0,HR*WEdge);

	  HR=ModelSupport::getHeadRule(SMap,surfNum,buildIndex,
				       "11 -14 (-12:13:-23M:24M:-25M:26M)");
	  makeCell("TaCell"+numStr,System,cellIndex++,taMat,0.0,
		   HR*TaEdge);

	  // WATER CELL
	  HR=ModelSupport::getHeadRule(SMap,surfNum,"-11");
	  makeCell("WaterChannel"+numStr,System,
	   	   cellIndex++,waterMat,0.0,HR*TaEdge*prevWater);
	  prevWater=ModelSupport::getHeadRule(SMap,surfNum,"14");
	}
      else   // VOID BLOCK
	{
	  HR=ModelSupport::getHeadRule(SMap,surfNum,"4 -14");
	  makeCell("VoidBlock",System,cellIndex++,0,0.0,HR*TaEdge);
	}
    }
  // Add backplate
  surfNum=buildIndex+1000+10*static_cast<int>(nBlock);

  // TA BackPlate Full:
  std::ostringstream cx;
  HR=ModelSupport::getHeadRule(SMap,surfNum,"4");
  HR*=ModelSupport::getHeadRule(SMap,buildIndex,
				"(-51:-53:54) (-51:-63:64) -52 (-51:57)");
  System.addCell(MonteCarlo::Object(cellIndex++,taMat,0.0,HR*TaEdge));

  // Void:
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 51 -52  53 -54 5 -6");
  makeCell("WaterMain",System,cellIndex++,waterMat,0.0,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 51 -52  63 -64 5 -6");
  makeCell("WaterMain",System,cellIndex++,waterMat,0.0,HR);
  // Steel pin
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 51 -52 -57");
  makeCell("SteelPin",System,cellIndex++,feMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1004 3 -4 5 -6 -52");
  addOuterSurf(HR);


  return;
}

Geometry::Vec3D
PlateTarget::plateEdge(const size_t plateN,double& W,double& L) const
  /*!
    Function to obtain the plate centre point and width and length
    \param plateN :: Index number of plate [0 to 11]
    \param W :: output for the plate width
    \param L :: output for the plate length [total inc layers]
    \return plate centre point
  */
{
  ELog::RegMethod RegA("PlateTarget","plateEdge");
  if (plateN>nBlock)
    throw ColErr::IndexError<size_t>(plateN,nBlock,"plate number+1");
  
  W=width;
  Geometry::Vec3D FPt(Origin);
  FPt += Y*(static_cast<double>(plateN+1) * waterThick+
	    static_cast<double>(2*plateN+1) * taThick);
  for(size_t i=1; i<= plateN;i++)
    FPt+=Y*tBlock[i-1];
  
  FPt+=Y*(tBlock[plateN]/2.0);
  L=tBlock[plateN]+2*taThick;

  return FPt;
}

void
PlateTarget::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("PlateTarget","createLinks");


  FixedComp::setConnect(0,Origin,-Y);

  FixedComp::setConnect(2,Origin-X*width,-X);
  FixedComp::setConnect(3,Origin+X*width,X);
  FixedComp::setConnect(4,Origin-Z*height,-Z);
  FixedComp::setConnect(5,Origin+Z*height,Z);

  // Set Connect surfaces:
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+1004));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+52));
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(3,-SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
PlateTarget::buildFeedThrough(Simulation& System)
  /*!
    Generate the feed though water channels [should only be 
    in the target]
    \param System :: Simulation system to use
   */
{
  ELog::RegMethod RegA("PlateTarget","buildFeedThrough");
  return;
  for(int i=0;i<4;i++)
    {
      ModelSupport::BoxLine 
	WaterChannel("waterChannel"+std::to_string(i+1));   

//     ELog::EM<<"Start of feedThrough"<<ELog::endDebug;
      const double sX((i % 2) ? -1 : 1);
      const double sZ((i / 2) ? -1 : 1);
      const Geometry::Vec3D PStart=
	X*sX*feedXOffset+Z*sZ*(height-feedHeight);
      const Geometry::Vec3D PEnd=
	PStart+getLinkPt(2)+Y*backPlateThick;

      WaterChannel.addPoint(PStart+Origin);
      WaterChannel.addPoint(PEnd);
      WaterChannel.addSection(feedWidth,feedHeight,waterMat,0.0);
      WaterChannel.setInitZAxis(Z);
      WaterChannel.build(System);
    }

  return;
}

double
PlateTarget::getTargetLength() const
  /*!
    Calculate the target length
    \return target length [including water]
  */
{
  const double TL=
    std::accumulate(tBlock.begin(),tBlock.end(),0.0,std::plus<double>());
  
  return TL+2.0*static_cast<double>(nBlock)*(taThick+waterThick);
}


void
PlateTarget::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("PlateTarget","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces(FC);
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
