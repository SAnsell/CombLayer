/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/PlateTarget.cxx
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
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
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
#include "SimProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "boxValues.h"
#include "boxUnit.h"
#include "BoxLine.h"
#include "PlateTarget.h"

namespace ts1System
{

PlateTarget::PlateTarget(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  ptIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(ptIndex+1),populated(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PlateTarget::PlateTarget(const PlateTarget& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  ptIndex(A.ptIndex),
  cellIndex(A.cellIndex),populated(A.populated),
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
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
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
PlateTarget::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("PlateTarget","populate");

  const FuncDataBase& Control=System.getDataBase();

  // Global values:
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  // Blocks:
  nBlock=Control.EvalVar<size_t>(keyName+"NBlocks");
  tBlock=SimProcess::getVarVec<double>(Control,keyName+"Thick");
  if (tBlock.size()!=nBlock)
    throw ColErr::MisMatch<size_t>(nBlock,tBlock.size(),
				"Incorrect thicknesses");
  
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
      (StrFunc::makeString(keyName+"BlockType",i),1);
  

  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
  taMat=ModelSupport::EvalMat<int>(Control,keyName+"TaMat");
  wMat=ModelSupport::EvalMat<int>(Control,keyName+"WMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");

  populated |= 1;
  return;
}
  
void
PlateTarget::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed compontent [front of target void vessel]
    - Y Down the beamline
  */
{
  ELog::RegMethod RegA("PlateTarget","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);
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
  ModelSupport::buildPlane(SMap,ptIndex+3,
			   Origin-X*width,X);
  ModelSupport::buildPlane(SMap,ptIndex+4,
			   Origin+X*width,X);
  ModelSupport::buildPlane(SMap,ptIndex+5,
			   Origin-Z*height,Z);
  ModelSupport::buildPlane(SMap,ptIndex+6,
			   Origin+Z*height,Z);

  // Water Edge
  ModelSupport::buildPlane(SMap,ptIndex+15,Origin-Z*waterHeight,Z);
  ModelSupport::buildPlane(SMap,ptIndex+16,Origin+Z*waterHeight,Z);
  // W block
  ModelSupport::buildPlane(SMap,ptIndex+23,Origin-X*WWidth,X);
  ModelSupport::buildPlane(SMap,ptIndex+24,Origin+X*WWidth,X);
  ModelSupport::buildPlane(SMap,ptIndex+25,Origin-Z*WHeight,Z);
  ModelSupport::buildPlane(SMap,ptIndex+26,Origin+Z*WHeight,Z);

  SMap.addMatch(ptIndex+1004,FC.getLinkSurf(1));

  Geometry::Vec3D FPt(Origin);
  for(size_t i=0;i<nBlock;i++)
    {
      int surfNum(ptIndex+1011+10*static_cast<int>(i));
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
  int surfNum(ptIndex+1011+10*static_cast<int>(nBlock));
  FPt+=Y*waterThick;
  ModelSupport::buildPlane(SMap,surfNum++,FPt,Y);

  ModelSupport::buildPlane(SMap,ptIndex+51,FPt+Y*IThick,Y);  
  ModelSupport::buildPlane(SMap,ptIndex+52,FPt+Y*backPlateThick,Y);
  ModelSupport::buildCylinder(SMap,ptIndex+57,
			      Origin,Y,pinRadius);

  // FixedComp::setConnect(1,FPt+Y*backPlateThick,Y);

  // Edge voids
  ModelSupport::buildPlane(SMap,ptIndex+53,Origin-X*(IOffset+IWidth),X);
  ModelSupport::buildPlane(SMap,ptIndex+54,Origin-X*(IOffset-IWidth),X);
  ModelSupport::buildPlane(SMap,ptIndex+63,Origin+X*(IOffset-IWidth),X);
  ModelSupport::buildPlane(SMap,ptIndex+64,Origin+X*(IOffset+IWidth),X);
 
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
  
  std::string Out;
  const std::string WEdge=
    ModelSupport::getComposite(SMap,ptIndex,"23 -24 25 -26");
  const std::string H2OEdge=
    ModelSupport::getComposite(SMap,ptIndex,"3 -4 15 -16");
  const std::string TaEdge=
    ModelSupport::getComposite(SMap,ptIndex,"3 -4 5 -6");

  int surfNum;
  // Make plates:
  for(size_t i=0;i<nBlock;i++)
    {
      surfNum=ptIndex+1000+10*static_cast<int>(i);
      if (blockType[i])
	{
	  // W CELL Full:
	  Out=ModelSupport::getComposite(SMap,surfNum,"12 -13 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,0.0,Out+WEdge));
	  
	  // WATER CELL
	  Out=ModelSupport::getComposite(SMap,surfNum,"4 -11 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,
					   0.0,Out+H2OEdge));
	  std::ostringstream cx;
	  // TA CELL Full:
	  Out=ModelSupport::getComposite(SMap,surfNum,"4 -14 ");
	  cx<<" #"<<cellIndex-2<<" #"<<cellIndex-1<<" ";
	  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,
					   Out+cx.str()+TaEdge));
	}
      else   // VOID BLOCK
	{
	  Out=ModelSupport::getComposite(SMap,surfNum,"4 -14 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+TaEdge));
	}
    }
  // Add backplate
  surfNum=ptIndex+1000+10*static_cast<int>(nBlock);
  // WATER CELL
  Out=ModelSupport::getComposite(SMap,surfNum,"4 -11 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out+H2OEdge));
  // TA BackPlate Full:
  std::ostringstream cx;
  Out=ModelSupport::getComposite(SMap,surfNum,"4 ");
  Out+=ModelSupport::getComposite(SMap,ptIndex," (-51:-53:54) (-51:-63:64) -52 (-51:57)");
  cx<<" #"<<cellIndex-1<<" ";
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out+cx.str()+TaEdge));
  
  // Void:
  Out=ModelSupport::getComposite(SMap,ptIndex," 51 -52  53 -54 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0, Out));
  
  Out=ModelSupport::getComposite(SMap,ptIndex," 51 -52  63 -64 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0, Out));
  // Steel pin
  Out=ModelSupport::getComposite(SMap,ptIndex," 51 -52 -57");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,ptIndex,"1004 3 -4 5 -6 -52");
  //  Out+=ModelSupport::getComposite(SMap,surfNum," -52");
  addOuterSurf(Out);


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
  FixedComp::setLinkSurf(0,SMap.realSurf(ptIndex+1004));
  FixedComp::setLinkSurf(1,SMap.realSurf(ptIndex+52));
  FixedComp::setLinkSurf(2,SMap.realSurf(ptIndex+3));
  FixedComp::setLinkSurf(3,-SMap.realSurf(ptIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(ptIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(ptIndex+6));

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

  for(int i=0;i<4;i++)
    {
      ModelSupport::BoxLine 
	WaterChannel(StrFunc::makeString("waterChannel",i+1));   

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
      WaterChannel.createAll(System);
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
PlateTarget::createAll(Simulation& System,const attachSystem::FixedComp& FC)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("PlateTarget","createAll");
  populate(System);

  createUnitVector(FC);
  createSurfaces(FC);
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
