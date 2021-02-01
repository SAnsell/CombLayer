/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   caveBuild/brickShape.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <math.h>
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
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
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "SurInter.h"
#include "surfDIter.h"


#include "support.h"


#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "LayerComp.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "surfDivide.h"
//#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "surfDBase.h"
#include "mergeTemplate.h"
/*

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
#include "surfDivide.h"
#include "surfDIter.h"
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
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "surfDBase.h"
#include "mergeTemplate.h"
*/
#include "brickShape.h"

namespace simpleCaveSystem
{

      
brickShape::brickShape(const std::string& Key) :
  attachSystem::FixedOffset(Key,6),attachSystem::ContainedComp(),
  attachSystem::CellMap(),attachSystem::SurfMap()
  // layerIndex(ModelSupport::objectRegister::Instance().cell(Key)), 
  //  cellIndex(layerIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

  

brickShape::brickShape(const brickShape& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),attachSystem::CellMap(A),
  //  layerIndex(A.layerIndex),cellIndex(A.cellIndex),
  brickHeight(A.brickHeight),brickWidth(A.brickWidth),
  brickThick(A.brickThick),brickGapWidthD(A.brickGapWidthD),
  brickGapWidthH(A.brickGapWidthH),brickGapWidthV(A.brickGapWidthV),
  brickMat(A.brickMat), surroundMat(A.surroundMat),
  nBrickLayers(A.nBrickLayers), brickFracList(A.brickFracList),
  brickMatList(A.brickMatList)
{}

brickShape&
brickShape::operator=(const brickShape& A)
  /*!
    Assignment operator
    \param A :: brickShape to copy
    \return *this
  */
  {
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      brickHeight=A.brickHeight;
      brickWidth=A.brickWidth;
      brickThick=A.brickThick;
      brickGapWidthD=A.brickGapWidthD;
      brickGapWidthH=A.brickGapWidthH;
      brickGapWidthV=A.brickGapWidthV;
      brickMat=A.brickMat;
      surroundMat=A.surroundMat;
      nBrickLayers=A.nBrickLayers;
      brickFracList=A.brickFracList;
      brickMatList=A.brickMatList;
    }
  return *this;
}
  

  
  
brickShape::~brickShape()
  /*!
    Destructor
  */
{}


void
brickShape::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("brickShape","populate");

  FixedOffset::populate(Control);

  ELog::EM << keyName << ELog::endDiag;

      height=Control.EvalVar<double>(keyName+"Height");
      width=Control.EvalVar<double>(keyName+"Width");
      offsetV=Control.EvalVar<double>(keyName+"OffsetVertical");
      offsetH=Control.EvalVar<double>(keyName+"OffsetHorizontal");
      brickHeight=Control.EvalVar<double>(keyName+"BrickHeight");
      brickWidth=Control.EvalVar<double>(keyName+"BrickWidth");
      brickThick=Control.EvalVar<double>(keyName+"BrickThick");
      brickGapWidthD=Control.EvalVar<double>(keyName+"GapWidthDepth");
      brickGapWidthH=Control.EvalVar<double>(keyName+"GapWidthHorizontal");
      brickGapWidthV=Control.EvalVar<double>(keyName+"GapWidthVertical");
      brickMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
      surroundMat=ModelSupport::EvalMat<int>(Control,keyName+"SurroundMat");
      nBrickLayers=Control.EvalVar<size_t>(keyName+"Layers");

     ModelSupport::populateDivide(Control,nBrickLayers,
			       keyName+"LayerMat",
			       brickMat,brickMatList);
     ModelSupport::populateDivideLen(Control,nBrickLayers,
				  keyName+"LayerThick",
				  brickThick,
  				  brickFracList);

     
  return;
}

void
brickShape::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("brickShape","createUnitVector");

  const attachSystem::LinkUnit& LU=FC.getLU(sideIndex);
      attachSystem::FixedComp::createUnitVector(FC,sideIndex);
      applyOffset();
  /* if ((sideIndex!=0)&&(sideIndex<=4)) yStep+=holderOuterRadius;
     applyOffset();*/
  
 
  return;
}

void
brickShape::createSurfaces()
  /*!
    Create cylinders and planes for the sample holder and the sample
  */
{
  ELog::RegMethod RegA("brickShape","createSurfaces");
  int brickIndexV=int(height/brickHeight)+2,
      brickIndexH=int(width/brickWidth)+2, iH, iV;
  //top and bottom of the wall
  ModelSupport::buildPlane(SMap,buildIndex+26,
			     Origin+Z*(0.5*height),Z);
  ModelSupport::buildPlane(SMap,buildIndex+25,
			     Origin-Z*(0.5*height),Z);
  //left and right of the wall
  ModelSupport::buildPlane(SMap,buildIndex+23,
			     Origin-X*(0.5*width),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,
			     Origin+X*(0.5*width),X);
  
  for(iH=0;iH<brickIndexH;iH++){
  for(iV=0;iV<brickIndexV;iV++){
  //Brick top and bottom

    ModelSupport::buildPlane(SMap,buildIndex+6+100*iH+1000*iV,
			     Origin-Z*(0.5*height+offsetV)+
			     Z*((1.0*iV+1)*brickHeight),Z);
    ModelSupport::buildPlane(SMap,buildIndex+16+100*iH+1000*iV,
			     Origin-Z*(0.5*height+offsetV)+
			     Z*((1.0*iV+1)*brickHeight-brickGapWidthV),Z);
    ModelSupport::buildPlane(SMap,buildIndex+5+100*iH+1000*iV,
			     Origin-Z*(0.5*height+offsetV)+
			     Z*(1.0*iV*brickHeight),Z);
 
 
    ModelSupport::buildPlane(SMap,buildIndex+3+100*iH+1000*iV,
     Origin
    -X*(0.5*width+offsetH+floor(-offsetH/brickWidth+0.3*iV+1)*brickWidth)
			     +X*((1.0*iH+0.3*iV)*brickWidth),X);
    ModelSupport::buildPlane(SMap,buildIndex+13+100*iH+1000*iV,
    Origin-X*(0.5*width+offsetH+floor(-offsetH/brickWidth+0.3*iV+1)*brickWidth)+
			     X*((1.0*iH+0.3*iV)*brickWidth+brickGapWidthH),X);
    ModelSupport::buildPlane(SMap,buildIndex+4+100*iH+1000*iV,
    Origin-X*(0.5*width+offsetH+floor(-offsetH/brickWidth+0.3*iV+1)*brickWidth)+
			     X*((1.0*iH+1+0.3*iV)*brickWidth),X);
  }
 }

  double brickLayerThick = (brickThick-brickGapWidthD)/nBrickLayers; 
 ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(0.5*brickThick),Y);
 for (int i=0; i<=nBrickLayers; i++){
 ModelSupport::buildPlane(SMap,buildIndex+10*i+11,
		 Origin-Y*(0.5*brickThick-brickGapWidthD-brickLayerThick*i),Y);
 
   //   std::cout<<buildIndex+10*i+11 << std::endl;
 }
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(0.5*brickThick),Y);  
  
  return; 
}

void
brickShape::createObjects(Simulation& System)
  /*!
    Create sample holder and sample cells
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("brickShape","createObjects");

  std::string Out,OuterSurf,OutBrickVoid;
 int brickIndexV=int(height/brickHeight)+2,
      brickIndexH=int(width/brickWidth)+2, iH, iV;

   OuterSurf=ModelSupport::getComposite(SMap,buildIndex,
    " 1 -2 23 -24 25 -26");
   addOuterSurf(OuterSurf); // external border is a box


   if(brickGapWidthD>0.0){
      OutBrickVoid= ModelSupport::getComposite(SMap,buildIndex,
       " 1 -11 23 -24 25 -26");
    System.addCell(MonteCarlo::Object(cellIndex++,surroundMat,0.0,OutBrickVoid));
    CellMap::setCell("BrickVoid",cellIndex-1);
  }
   
   for (int i=0; i<nBrickLayers; i++){
     Out="";OutBrickVoid="";
  int flag=0;
  for(iH=0;iH<brickIndexH;iH++){
  for(iV=0;iV<brickIndexV;iV++){
    int surf6=6+100*iH+1000*iV;
    if(-offsetV+(1.0*iV+1)*brickHeight<height) surf6=6+100*iH+1000*iV; else surf6=26;
    int surf5;    
    if(-offsetV+(1.0*iV)*brickHeight>0.0) surf5=5+100*iH+1000*iV; else surf5=25;
    int surf4;
    if(-offsetH-floor(-offsetH/brickWidth+0.3*iV+1)*brickWidth+
	   (1.0*iH+1+0.3*iV)*brickWidth<width)
                    surf4=4+100*iH+1000*iV; else surf4=24;

    int surf3;
if(-offsetH-floor(-offsetH/brickWidth+0.3*iV+1)*brickWidth+
   (1.0*iH+0.3*iV)*brickWidth>0.0)  surf3=3+100*iH+1000*iV; else surf3=23;

    int surf3out=3+100*(brickIndexH-1)+1000*0;
    int surf4out=4+100*0+1000*(brickIndexV-1);

    int surf13;
    if(-offsetH-floor(-offsetH/brickWidth+0.3*iV+1)*brickWidth+
	   (1.0*iH+0.3*iV)*brickWidth>0.0)
                    surf13=13+100*iH+1000*iV; else surf13=23;
    
    int surf16;
    if(-offsetV+(1.0*iV+1)*brickHeight-brickGapWidthV<height) surf16=16+100*iH+1000*iV; else surf16=26;

			     
   if(-offsetV+(1.0*iV+1)*brickHeight<height+brickHeight &&
      -offsetH-floor(-offsetH/brickWidth+0.3*iV+1)*brickWidth+
	   (1.0*iH+1+0.3*iV)*brickWidth<width+brickWidth)
     {  //if brick fits in the outer frame
     if (flag>0) {Out+=" : ";OutBrickVoid+=" : ";} 
    flag++;
  Out+= ModelSupport::getComposite(SMap,buildIndex,
	std::to_string(10*i+11)+" -"+std::to_string(10*i+21)+" "+
	     std::to_string(surf13)+" "+std::to_string(-surf4)+" "+
				   std::to_string(surf5)+" "+std::to_string(-surf16));
      //if brick fits in outer frame
  
   OutBrickVoid+= ModelSupport::getComposite(SMap,buildIndex,
     	std::to_string(10*i+11)+" -"+std::to_string(10*i+21)+" "
	       +std::to_string(surf3)+" "+std::to_string(-surf4)+" "+
		 std::to_string(surf5)+" "+std::to_string(-surf6)
	    + "( "+std::to_string(-surf13)+" : "+
			   std::to_string(surf16)+")");
     }
    }
   }

    System.addCell(MonteCarlo::Object(cellIndex++,brickMat,0.0,Out));
    CellMap::setCell("Brick",cellIndex-1);

    System.addCell(MonteCarlo::Object(cellIndex++,surroundMat,0.0,OutBrickVoid));
    CellMap::setCell("BrickVoid",cellIndex-1);

 } // for int i=0 .... layers in brick structure
    

  return; 
}

void
brickShape::createLinks()
  /*!
    Creates a full attachment set
   Link points and link surfaces
 */
{  
  ELog::RegMethod RegA("brickShape","createLinks");
  //Link point at the sample position
  //Along the beam
  
  FixedComp::setConnect(0,Origin-Y*(0.5*brickThick),-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+1));
  FixedComp::setConnect(1,Origin+Y*(0.5*brickThick),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  //Perp to beam
  FixedComp::setConnect(2,Origin-X*(0.5*brickWidth),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+3));
  FixedComp::setConnect(3,Origin+X*(0.5*brickWidth),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
  //Vertical dimensions
  FixedComp::setConnect(4,Origin+Z*(0.5*brickHeight),Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+6));
  FixedComp::setConnect(5,Origin-Z*(0.5*brickHeight),-Z);
  FixedComp::setLinkSurf(5,-SMap.realSurf(buildIndex+5));

  
  return;
}


  
void
brickShape::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: surface offset
   */
{
  ELog::RegMethod RegA("brickShape","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE constructSystem
