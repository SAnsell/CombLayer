/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/D03SteelInsert.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
 *************************************************************************/
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
#include <array>

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
#include "SurInter.h"
#include "surfDIter.h"

#include "D03SteelInsert.h"

namespace constructSystem
{

D03SteelInsert::D03SteelInsert(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  //  buildIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  // cellIndex(buildIndex+1),
  activeFront(0),activeBack(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

D03SteelInsert::D03SteelInsert(const D03SteelInsert& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  //  buildIndex(A.buildIndex),cellIndex(A.cellIndex),
  activeFront(A.activeFront),activeBack(A.activeBack),
  frontSurf(A.frontSurf),frontCut(A.frontCut),
  backSurf(A.backSurf),backCut(A.backCut),
  length(A.length),left(A.left),voidLeft(A.voidLeft),
  right(A.right),voidRight(A.voidRight),
  height(A.height),voidHeight(A.voidHeight),
  depth(A.depth),voidDepth(A.voidDepth),
  defMat(A.defMat),voidMat(A.voidMat),
  nSeg(A.nSeg),nWallLayersL(A.nWallLayersL),nWallLayersR(A.nWallLayersR),
  wallFracR(A.wallFracR), wallFracL(A.wallFracL),wallMat(A.wallMat),
  nRoofLayers(A.nRoofLayers),roofFrac(A.roofFrac),
  roofMat(A.roofMat),nFloorLayers(A.nFloorLayers),
  floorFrac(A.floorFrac),floorMat(A.floorMat),
  iSplitVoid(A.iSplitVoid)
  /*!
    Copy constructor
    \param A :: D03SteelInsert to copy
  */
{}

D03SteelInsert&
D03SteelInsert::operator=(const D03SteelInsert& A)
  /*!
    Assignment operator
    \param A :: D03SteelInsert to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      //    cellIndex=A.cellIndex;
      activeFront=A.activeFront;
      activeBack=A.activeBack;
      frontSurf=A.frontSurf;
      frontCut=A.frontCut;
      backSurf=A.backSurf;
      backCut=A.backCut;
      length=A.length;
      left=A.left;
      voidLeft=A.voidLeft;
      right=A.right;
      voidRight=A.voidRight;
      height=A.height;
      voidHeight=A.voidHeight;
      depth=A.depth;
      voidDepth=A.voidDepth;
      defMat=A.defMat;
      voidMat=A.voidMat;
      nSeg=A.nSeg;
      nWallLayersR=A.nWallLayersR;
      nWallLayersL=A.nWallLayersL;
      wallFracR=A.wallFracR;
      wallFracL=A.wallFracL;
      wallMat=A.wallMat;
      nRoofLayers=A.nRoofLayers;
      roofFrac=A.roofFrac;
      roofMat=A.roofMat;
      nFloorLayers=A.nFloorLayers;
      floorFrac=A.floorFrac;
      floorMat=A.floorMat;
      iSplitVoid=A.iSplitVoid;
    }
  return *this;
}

D03SteelInsert::~D03SteelInsert() 
  /*!
    Destructor
  */
{}

void
D03SteelInsert::removeFrontOverLap()
  /*!
    Remove segments that are completly covered by the
    active front.
  */
{
  ELog::RegMethod RegA("D03SteelInsert","removeFrontOverLap");

  if (activeFront)
    {
      size_t index(1);
      const double segStep(length/static_cast<double>(nSeg));
      // note : starts one step ahead of front.
      Geometry::Vec3D SP(Origin-Y*(length/2.0-segStep));
      frontSurf.populateSurf();
       while(index<nSeg && !frontSurf.isValid(SP))
        {
          SP+=Y*segStep;
          index++;
        }
      index--;
      if (index>0)
        {
	  const double LUnit=segStep*static_cast<double>(index);
          length-=LUnit;
          nSeg-=index;
          Origin+=Y*(LUnit/2.0);
          ELog::EM<<"Removal["<<keyName<<"] of Active segment == "
                  <<index<<ELog::endDiag;
	}
    }
  
  return;
}
  
void
D03SteelInsert::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("D03SteelInsert","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  int iLayerFlag=Control.EvalDefVar<int>("LayerFlag",1);
  length=Control.EvalDefVar<double>(keyName+"Length",50);
  left=Control.EvalDefVar<double>(keyName+"Left",34);
  right=Control.EvalDefVar<double>(keyName+"Right",34);
  height=Control.EvalDefVar<double>(keyName+"Height",46.5);
  depth=Control.EvalDefVar<double>(keyName+"Depth",8.5);
  spacing =  Control.EvalDefVar<double>(keyName+"Spacing",0);
  
  voidLeft=Control.EvalDefVar<double>(keyName+"VoidLeft",24);
  voidRight=Control.EvalDefVar<double>(keyName+"VoidRight",24);
  voidHeight=Control.EvalDefVar<double>(keyName+"VoidHeight",36.5);
  voidDepth=Control.EvalDefVar<double>(keyName+"VoidDepth",8.5);
  
  //  defMat=ModelSupport::EvalDefMat<int>(Control,keyName+"DefMat",3);
  //M3=Stainless304, M151=SteelS355
  defMat=ModelSupport::EvalDefMat<int>(Control,keyName+"DefMat",151);
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",60);
  spacingMat=ModelSupport::EvalDefMat<int>(Control,keyName+"SpacingMat",60);
  //  int B4CMat;

  nSeg=Control.EvalDefVar<size_t>(keyName+"NSeg",1);

  nWallLayersR=Control.EvalDefVar<size_t>(keyName+"NWallLayersR",3);
  nWallLayersL=Control.EvalDefVar<size_t>(keyName+"NWallLayersL",3);
  
  InnerB4CThick=
        Control.EvalDefVar<double>(keyName+"InnerB4CLayerThick",0);
  
  OuterB4CThick=
        Control.EvalDefVar<double>(keyName+"OuterB4CLayerThick",0);
  /*
  nWallLayersR=Control.EvalPair<size_t>(keyName+"NWallLayersR",keyName+"NWallLayers");
  nWallLayersL=Control.EvalPair<size_t>(keyName+"NWallLayersL",keyName+"NWallLayers");
  
  InnerB4CThick=
        Control.EvalPair<double>(keyName+"InnerB4CLayerThick","DefaultTileThick");
  
  OuterB4CThick=
        Control.EvalPair<double>(keyName+"OuterB4CLayerThick","DefaultTileThick");
  */
  B4CMat=ModelSupport::EvalDefMat<int>(Control,keyName+"B4CTileMat",0);

  iSplitVoid=Control.EvalDefVar<int>(keyName+"SplitVoid",0);
  
  nRoofLayers=Control.EvalDefVar<size_t>(keyName+"NRoofLayers",3);

  if(iLayerFlag==0){
   nSeg=1;

  nWallLayersR=2;
  nWallLayersL=2;
  nRoofLayers=2;
  }


  size_t nWallLayers=std::max(nWallLayersR,nWallLayersL);
  ModelSupport::populateDivide(Control,nWallLayers,keyName+"WallMat",
			       defMat,wallMat);
  ModelSupport::populateDivideLen(Control,nWallLayersL,keyName+"WallLen",
				  left,wallFracL);
  ModelSupport::populateDivideLen(Control,nWallLayersR,keyName+"WallLen",
				  right,wallFracR);
  wallFracL.push_back(1.0);
  wallFracR.push_back(1.0);
   
  
  ModelSupport::populateDivide(Control,nRoofLayers,keyName+"RoofMat",
			       defMat,roofMat);
  ModelSupport::populateDivideLen(Control,nRoofLayers,keyName+"RoofLen",
				  height,roofFrac);
  roofFrac.push_back(1.0);
  
  if(OuterB4CThick>0.0){
    wallMat.push_back(B4CMat);roofMat.push_back(B4CMat);}
  
  nFloorLayers=Control.EvalDefVar<size_t>(keyName+"NFloorLayers",1);
  if (depth==voidDepth) nFloorLayers=1;

  int fMat=ModelSupport::EvalDefMat<int>(Control,keyName+"FloorMat",3);
  
    ModelSupport::populateDivide(Control,nFloorLayers,keyName+"FloorMat",
				 fMat,floorMat);
  ModelSupport::populateDivideLen(Control,nFloorLayers,keyName+"FloorLen",
				  depth,floorFrac);

  floorFrac.push_back(1.0);
  
  return;
}

void
D03SteelInsert::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("D03SteelInsert","createUnitVector");


  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  Origin+=Y*(length/2.0);
  return;
}

void
D03SteelInsert::createSurfaces()
  /*!
    Create the surfaces. Note that layers is not used
    because we want to break up the objects into sub components
  */
{
  ELog::RegMethod RegA("D03SteelInsert","createSurfaces");

  // Inner void
  if (!activeFront)
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      
  if (!activeBack)
    ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);

 
  if (activeFront)
    removeFrontOverLap();
  
  
  const double segStep(length/static_cast<double>(nSeg));
  double segLen(-length/2.0);
  int SI(buildIndex+10);
  for(size_t i=1;i<nSeg;i++)
    {
      segLen+=segStep;
      ModelSupport::buildPlane(SMap,SI+2,Origin+Y*segLen,Y);
      SI+=10;
    }

  int WIL(buildIndex);
  if(InnerB4CThick>0.0){
  ModelSupport::buildPlane(SMap,WIL+3,
   Origin-X*
	(-InnerB4CThick+voidLeft+(left-voidLeft)*(wallFracL[0]-1./nWallLayersL)/(1.-1./nWallLayersL)),X);
  /*      ModelSupport::buildPlane(SMap,WI+4,
   Origin+X*
   (-InnerB4CThick+voidRight+(right-voidRight)*(wallFrac[0]-1./nWallLayers)/(1.-1./nWallLayers)),X);*/
      WIL+=10;}
  
  for(size_t i=0;i<nWallLayersL;i++)
    {
      ModelSupport::buildPlane(SMap,WIL+3,
   Origin-X*
   (voidLeft+(left-voidLeft)*(wallFracL[i]-1./nWallLayersL)/(1.-1./nWallLayersL)),X);
      /*  ModelSupport::buildPlane(SMap,WI+4,
   Origin+X*
     (voidRight+(right-voidRight)*(wallFrac[i]-1./nWallLayers)/(1.-1./nWallLayers)),X);
      */ WIL+=10;
    }

  if(OuterB4CThick>0.0){
      ModelSupport::buildPlane(SMap,WIL+3, Origin-X*(left+OuterB4CThick),X);
      //  ModelSupport::buildPlane(SMap,WI+4, Origin+X*(right+OuterB4CThick),X);
      WIL+=10;
  }





    int WIR(buildIndex);
  if(InnerB4CThick>0.0){
    /*  ModelSupport::buildPlane(SMap,WI+3,
   Origin-X*
   (-InnerB4CThick+voidLeft+(left-voidLeft)*(wallFrac[0]-1./nWallLayers)/(1.-1./nWallLayers)),X);*/
      ModelSupport::buildPlane(SMap,WIR+4,
   Origin+X*
     (-InnerB4CThick+voidRight+(right-voidRight)*(wallFracR[0]-1./nWallLayersR)/(1.-1./nWallLayersR)),X);
      WIR+=10;}
  
  for(size_t i=0;i<nWallLayersR;i++)
    {
      /*      ModelSupport::buildPlane(SMap,WI+3,
   Origin-X*
	(voidLeft+(left-voidLeft)*(wallFrac[i]-1./nWallLayers)/(1.-1./nWallLayers)),X);
      */
      ModelSupport::buildPlane(SMap,WIR+4,
   Origin+X*
    (voidRight+(right-voidRight)*(wallFracR[i]-1./nWallLayersR)/(1.-1./nWallLayersR)),X);
      WIR+=10;
    }

  if(OuterB4CThick>0.0){
    // ModelSupport::buildPlane(SMap,WI+3, Origin-X*(left+OuterB4CThick),X);
      ModelSupport::buildPlane(SMap,WIR+4, Origin+X*(right+OuterB4CThick),X);
      WIR+=10;
  }





  
  int RI(buildIndex);
  if(InnerB4CThick>0.0){
      ModelSupport::buildPlane(SMap,RI+6,
      Origin+Z*(-InnerB4CThick+voidHeight
	 +(height-voidHeight)*(roofFrac[0]-1./nRoofLayers)/(1.-1./nRoofLayers)),Z);
      RI+=10;
  };
  
  for(size_t i=0;i<nRoofLayers;i++)
    {
      ModelSupport::buildPlane(SMap,RI+6,
      Origin+Z*(voidHeight
	 +(height-voidHeight)*(roofFrac[i]-1./nRoofLayers)/(1.-1./nRoofLayers)),Z);
      RI+=10;
    }

   if(OuterB4CThick>0.0){
      ModelSupport::buildPlane(SMap,RI+6,
      Origin+Z*(height+OuterB4CThick),Z);
      RI+=10;
  };
  
  int FI(buildIndex);
  ModelSupport::buildPlane(SMap,FI+5,Origin-Z*(voidDepth),Z);
       	   FI+=10;
	   if(nFloorLayers<=1){
      ModelSupport::buildPlane(SMap,FI+5,
			       Origin-Z*(voidDepth+spacing), Z);
      FI+=10;	   } else{
   for(size_t i=0;i<nFloorLayers;i++)
    {
      ModelSupport::buildPlane(SMap,FI+5,
     Origin-Z*(voidDepth+spacing
           +(depth-voidDepth)*(floorFrac[i]-1./nFloorLayers)/(1.-1./nFloorLayers)),Z);
      FI+=10;
    }

  };
  //Last plane, to have spacing between U-shape and bottom plate.
  
  return;
}

void
D03SteelInsert::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("D03SteelInsert","createObjects");

  std::string Out;
  
  const std::string frontStr
    (activeFront ? frontSurf.display()+frontCut.display() : 
     ModelSupport::getComposite(SMap,buildIndex," 1 "));
  const std::string backStr
    (activeBack ? backSurf.display()+backCut.display() : 
     ModelSupport::getComposite(SMap,buildIndex," -2 "));

  // Inner void is a single segment
  /*  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6 5 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr+backStr);
  */
  // Loop over all segments:
  std::string FBStr;
  int SI(buildIndex);
  int WIR(buildIndex),WIL(buildIndex),RI(buildIndex),FI(buildIndex);    
  for(size_t index=0;index<nSeg;index++)
    {
      FBStr=((index) ?
	     ModelSupport::getComposite(SMap,SI," 2 ") :
	     frontStr);
      FBStr+= ((index+1!=nSeg) ?
	       ModelSupport::getComposite(SMap,SI," -12 ") :
	       backStr);
      SI+=10;
      //      std::cout <<"aa"<<std::endl;      
      if (spacing>0) Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6 15 ");
          else Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6 5 ");
	if(iSplitVoid==0){
	Out+=frontStr+backStr;
	if(index==0)   makeCell("Void",System,cellIndex++,voidMat,0.0,Out);
	}else{	makeCell("Void",System,cellIndex++,voidMat,0.0,Out+FBStr);}

      // Inner is a single component
      // Walls are contained:
	WIR=buildIndex;
	WIL=buildIndex;
      // Innermost layer is B4C concrete.
      // A separate calculation.
      if(InnerB4CThick>0.0){
          Out=ModelSupport::getComposite(SMap,WIL,buildIndex,
			  " 13 -3 -6M 5M");
	  makeCell("WallLining",System,cellIndex++,B4CMat,0.0,Out+FBStr);
		 
	  Out=ModelSupport::getComposite(SMap,WIR,buildIndex,
			 " 4 -14 -6M 5M");
          makeCell("WallLining",System,cellIndex++,B4CMat,0.0,Out+FBStr);

	 WIL+=10; WIR+=10;}

      size_t imaxL=OuterB4CThick>0.0 ? nWallLayersL+1 : nWallLayersL;
 
      for(size_t i=1;i<imaxL;i++)
	{
	  if (InnerB4CThick>0.0) Out=ModelSupport::getComposite(SMap,WIL,buildIndex,
					 " 13 -3 -16M 5M"); else
	  Out=ModelSupport::getComposite(SMap,WIL,buildIndex,
					 " 13 -3 -6M 5M");
	  //need to add 10 to build index for upper border of the rectangle
	  //to go above B4Clayer height if it is present
	           makeCell("Wall",System,cellIndex++,wallMat[i],0.0,Out+FBStr);
	/*if(InnerB4CThick>0.0)  Out=ModelSupport::getComposite(SMap,WI,buildIndex,
			 " 4 -14 -16M 5M"); else
	    Out=ModelSupport::getComposite(SMap,WI,buildIndex,
 			 " 4 -14 -6M 5M");
           makeCell("Wall",System,cellIndex++,wallMat[i],0.0,Out+FBStr);
	  */ WIL+=10;
   	}



      size_t imaxR=OuterB4CThick>0.0 ? nWallLayersR+1 : nWallLayersR;
 
      for(size_t i=1;i<imaxR;i++)
	{
	/* if (InnerB4CThick>0.0) Out=ModelSupport::getComposite(SMap,WI,buildIndex,
					 " 13 -3 -16M 5M"); else
	  Out=ModelSupport::getComposite(SMap,WI,buildIndex,
					 " 13 -3 -6M 5M");
	  */
	  //need to add 10 to build index for upper border of the rectangle
	  //to go above B4Clayer height if it is present
	  //         makeCell("Wall",System,cellIndex++,wallMat[i],0.0,Out+FBStr);
	  if(InnerB4CThick>0.0)  Out=ModelSupport::getComposite(SMap,WIR,buildIndex,
			 " 4 -14 -16M 5M"); else
	    Out=ModelSupport::getComposite(SMap,WIR,buildIndex,
 			 " 4 -14 -6M 5M");
           makeCell("Wall",System,cellIndex++,wallMat[i],0.0,Out+FBStr);
	   WIR+=10;
   	}

      

      
      // Roof on top of walls are contained:
      RI=buildIndex;
     int WII(buildIndex);
      if(InnerB4CThick>0.0){
          Out=ModelSupport::getComposite(SMap,RI,buildIndex," 3M -4M -16 6 ");
	  //  System.addCell(MonteCarlo::Object
	  //		 (cellIndex++,B4CMat,0.0,Out+FBStr));
          makeCell("RoofLining",System,cellIndex++,B4CMat,0.0,Out+FBStr);
	  RI+=10; WII+=10;}

     size_t imax=OuterB4CThick>0.0 ? nRoofLayers+1 : nRoofLayers;
 
      for(size_t i=1;i<imax;i++)
	{
	  Out=ModelSupport::getComposite(SMap,RI,WII," 3M -4M -16 6 ");
	  //System.addCell(MonteCarlo::Object
	  //		 (cellIndex++,roofMat[i],0.0,Out+FBStr));
	 
	  makeCell("Roof",System,cellIndex++,roofMat[i],0.0,Out+FBStr);
	  RI+=10;

	}
    
      // Floor complete:
      FI=buildIndex;
      if (spacing>0.0){
		Out=ModelSupport::getComposite(SMap,FI,WIL,WIR,
		      " 3M -4N -5 15  (-3 : 4)");
      //       System.addCell(MonteCarlo::Object
      //		 (cellIndex++,spacingMat,0.0,Out+FBStr));

       makeCell("Floor",System,cellIndex++,spacingMat,0.0,Out+FBStr);
	
       //       Out=ModelSupport::getComposite(SMap,FI,WI," -5 15  3  -4 ");
          Out=ModelSupport::getComposite(SMap,FI," -5 15  3  -4 ");
	  if (nFloorLayers>1) {
	    if(InnerB4CThick>0.0)
	 { //System.addCell(MonteCarlo::Object(cellIndex++,B4CMat,0.0,Out+FBStr));
           makeCell("Floor",System,cellIndex++,B4CMat,0.0,Out+FBStr);
	 }else
	 // 	 System.addCell(MonteCarlo::Object(cellIndex++,B4CMat,0.0,Out+FBStr));
	      makeCell("Floor",System,cellIndex++,B4CMat,0.0,Out+FBStr); }
      }
      FI+=10;

      int FItmp(FI);     
 	  WIR=buildIndex; if(InnerB4CThick>0.0)WIR+=10;
	  WIL=buildIndex; if(InnerB4CThick>0.0)WIL+=10;

      for(size_t i=1;i<nFloorLayers;i++)
	{
	  Out=ModelSupport::getComposite(SMap,FI,WIL,WIR," 3M -4N -5 15 ");
	  //System.addCell(MonteCarlo::Object
	  //		 (cellIndex++,floorMat[i],0.0,Out+FBStr));
	            makeCell("Floor",System,cellIndex++,floorMat[i],0.0,Out+FBStr);
		    FI+=10;
	}



      /*****************/
   // Corners floor-roof
      WIL=buildIndex; FI=FItmp;
      WIR=buildIndex;
      /*
      if (InnerB4CThick>0.0){
       Out=ModelSupport::getComposite(SMap,WIL,FI," -3 13 -5M 15M ");
       //	System.addCell(MonteCarlo::Object(cellIndex++,B4CMat,0.0,Out+FBStr));
        makeCell("Corner",System,cellIndex++,B4CMat,0.0,Out+FBStr);    
        Out=ModelSupport::getComposite(SMap,WIR,FI," 4 -14 -5M 15M ");
	//System.addCell(MonteCarlo::Object(cellIndex++,B4CMat,0.0,Out+FBStr));
         makeCell("Corner",System,cellIndex++,B4CMat,0.0,Out+FBStr);
     
	      RI=buildIndex+10;
      }
      FI=FItmp;
      */
      //      imax=OuterB4CThick>0.0 ? nRoofLayers+1 : nRoofLayers;


      size_t jmaxR=OuterB4CThick>0.0 ? nWallLayersR+1 : nWallLayersR;
      size_t jmaxL=OuterB4CThick>0.0 ? nWallLayersL+1 : nWallLayersL;
 
      for(size_t i=1;i<nFloorLayers;i++)
	{
	  WIR=buildIndex; if(InnerB4CThick>0.0)WIR+=10;
	  for(size_t j=1;j<jmaxR;j++)
	    {
	      //	      const int mat((i>j) ? floorMat[i] : wallMat[j]);
	      	      const int mat(floorMat[i]);
	      Out=ModelSupport::getComposite(SMap,WIR,FI," 4 -14 -5M 15M ");
	      //  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out+FBStr));
	      makeCell("Corner",System,cellIndex++,mat,0.0,Out+FBStr);
               WIR+=10;
	    }
	  FI+=10;
	}

      FI=FItmp;

      for(size_t i=1;i<nFloorLayers;i++)
	{
	  WIL=buildIndex; if(InnerB4CThick>0.0)WIL+=10;
	  for(size_t j=1;j<jmaxL;j++)
	    {
	      //	      const int mat((i>j) ? floorMat[i] : wallMat[j]);
	      	      const int mat(floorMat[i]);
	      Out=ModelSupport::getComposite(SMap,WIL,FI," -3 13 -5M 15M ");
	      // System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out+FBStr));
  	      makeCell("Corner",System,cellIndex++,mat,0.0,Out+FBStr);
	      WIL+=10;
	    }
	  FI+=10;
	}
   


/**************/
      

      
      
      // Corners wall-roof
      WIL=buildIndex; RI=buildIndex;
      WIR=buildIndex;

      if (InnerB4CThick>0.0){
       Out=ModelSupport::getComposite(SMap,WIL,RI," -3 13 6M -16M ");
       //	System.addCell(MonteCarlo::Object(cellIndex++,B4CMat,0.0,Out+FBStr));
        makeCell("B4CCorner",System,cellIndex++,B4CMat,0.0,Out+FBStr);    
        Out=ModelSupport::getComposite(SMap,WIR,RI," 4 -14 6M -16M ");
	//System.addCell(MonteCarlo::Object(cellIndex++,B4CMat,0.0,Out+FBStr));
         makeCell("B4CCorner",System,cellIndex++,B4CMat,0.0,Out+FBStr);
     
	      RI=buildIndex+10;
      }
      size_t RItmp(RI);
      imax=OuterB4CThick>0.0 ? nRoofLayers+1 : nRoofLayers;


       jmaxR=OuterB4CThick>0.0 ? nWallLayersR+1 : nWallLayersR;
       jmaxL=OuterB4CThick>0.0 ? nWallLayersL+1 : nWallLayersL;
 
      for(size_t i=1;i<imax;i++)
	{
	  WIR=buildIndex; if(InnerB4CThick>0.0)WIR+=10;
	  for(size_t j=1;j<jmaxR;j++)
	    {
	      const int mat((i>j) ? roofMat[i] : wallMat[j]);
	      /*  Out=ModelSupport::getComposite(SMap,WI,RI," -3 13 6M -16M ");
	      // System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out+FBStr));
  	      makeCell("Corner",System,cellIndex++,mat,0.0,Out+FBStr);
	      */
	      Out=ModelSupport::getComposite(SMap,WIR,RI," 4 -14 6M -16M ");
	      //  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out+FBStr));
	      makeCell("Corner",System,cellIndex++,mat,0.0,Out+FBStr);
               WIR+=10;
	    }
	  RI+=10;
	}

      RI=RItmp;

      for(size_t i=1;i<imax;i++)
	{
	  WIL=buildIndex; if(InnerB4CThick>0.0)WIL+=10;
	  for(size_t j=1;j<jmaxL;j++)
	    {
	      const int mat((i>j) ? roofMat[i] : wallMat[j]);
	      Out=ModelSupport::getComposite(SMap,WIL,RI," -3 13 6M -16M ");
	      // System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out+FBStr));
  	      makeCell("Corner",System,cellIndex++,mat,0.0,Out+FBStr);
         
	      /*   Out=ModelSupport::getComposite(SMap,WI,RI," 4 -14 6M -16M ");
	      //  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out+FBStr));
	      makeCell("Corner",System,cellIndex++,mat,0.0,Out+FBStr);
              */
	      WIL+=10;
	    }
	  RI+=10;
	}
 

    }
  // Outer
  Out=ModelSupport::getComposite(SMap,WIL,FI,RI," 3 5M -6N ")+
     ModelSupport::getComposite(SMap,WIR," -4 ");
  addOuterSurf(Out+frontStr+backStr);

  return;
}

void
D03SteelInsert::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("D03SteelInsert","createLinks");

  FixedComp::setConnect(2,Origin-X*left,-X);
  FixedComp::setConnect(3,Origin+X*right,X);
  FixedComp::setConnect(4,Origin-Z*depth,-Z);
  FixedComp::setConnect(5,Origin+Z*height,Z);

  const int WIL(buildIndex+(static_cast<int>(nWallLayersL)-1)*10);
  const int WIR(buildIndex+(static_cast<int>(nWallLayersR)-1)*10);
  const int RI(buildIndex+(static_cast<int>(nRoofLayers)-1)*10);
  const int FI(buildIndex+(static_cast<int>(nFloorLayers)-1)*10);

  if (!activeFront)
    {
      FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
      FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));      
    }
  else
    {
      FixedComp::setLinkSurf(0,frontSurf,1,frontCut,0);      
      FixedComp::setConnect
        (0,SurInter::getLinePoint(Origin,Y,frontSurf,frontCut),-Y);
    }
  
  if (!activeBack)
    {
      FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
    }
  else
    {
      FixedComp::setLinkSurf(1,backSurf,1,backCut,0);      
      FixedComp::setConnect
        (1,SurInter::getLinePoint(Origin,Y,backSurf,backCut),Y);
    }
  FixedComp::setLinkSurf(2,-SMap.realSurf(WIL+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(WIR+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(FI+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(RI+6));



  return;
}
  
void
D03SteelInsert::setFront(const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Set front surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("D03SteelInsert","setFront");
  
  if (sideIndex==0)
    throw ColErr::EmptyValue<long int>("SideIndex cant be zero");

  activeFront=1;
  frontSurf=FC.getMainRule(sideIndex);
  frontCut=FC.getCommonRule(sideIndex);
  frontSurf.populateSurf();
  frontCut.populateSurf();
  
  return;
}

void
D03SteelInsert::setBack(const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Set back surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("D03SteelInsert","setBack");
  
  if (sideIndex==0)
    throw ColErr::EmptyValue<long int>("SideIndex cant be zero");

  activeBack=1;
  backSurf=FC.getMainRule(sideIndex);
  backCut=FC.getCommonRule(sideIndex);
  backSurf.populateSurf();
  backCut.populateSurf();
  
  return;
}

HeadRule
D03SteelInsert::getXSectionIn() const
  /*!
    Get the line shield inner void section
    \return HeadRule of cross-section
   */
{
  ELog::RegMethod RegA("D03SteelInsert","getXSectionIn");
  const std::string Out=
    ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 ");
  HeadRule HR(Out);
  HR.populateSurf();
  return HR;
}
  
    
void
D03SteelInsert::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("D03SteelInsert","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
