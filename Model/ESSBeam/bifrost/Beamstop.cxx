/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Model/caveBuild/Beamstop.cxx
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

#include "FixedGroup.h"
#include "FixedOffsetGroup.h"

#include "Beamstop.h"

namespace constructSystem
{

Beamstop::Beamstop(const std::string& Key) : 
  //  attachSystem::FixedOffset(Key,0),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::FixedOffsetGroup(Key,"Inner",6,"Outer",6),
//  buildIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  // cellIndex(buildIndex+1),
  activeFront(0),activeBack(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

Beamstop::Beamstop(const Beamstop& A) : 
  //  attachSystem::FixedOffset(A),
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffsetGroup(A),attachSystem::CellMap(A),
  //  buildIndex(A.buildIndex),cellIndex(A.cellIndex),
  activeFront(A.activeFront),activeBack(A.activeBack),
  frontSurf(A.frontSurf),frontCut(A.frontCut),
  backSurf(A.backSurf),backCut(A.backCut),
  length(A.length),left(A.left),voidLeft(A.voidLeft),
  right(A.right),voidRight(A.voidRight),
  height(A.height),voidHeight(A.voidHeight),
  depth(A.depth),voidDepth(A.voidDepth),
  defMat(A.defMat),
  nSeg(A.nSeg),nWallLayers(A.nWallLayers),
  iFrontLining(A.iFrontLining),
  iSplitVoid(A.iSplitVoid),
  wallFrac(A.wallFrac),wallMat(A.wallMat),
  nRoofLayers(A.nRoofLayers),roofFrac(A.roofFrac),
  roofMat(A.roofMat),nFloorLayers(A.nFloorLayers),
  floorFrac(A.floorFrac),floorMat(A.floorMat)
  /*!
    Copy constructor
    \param A :: Beamstop to copy
  */
{}

Beamstop&
Beamstop::operator=(const Beamstop& A)
  /*!
    Assignment operator
    \param A :: Beamstop to copy
    \return *this
  */
{
  if (this!=&A)
    {
      //  attachSystem::FixedOffset::operator=(A);    
      attachSystem::FixedOffsetGroup::operator=(A);
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
      nSeg=A.nSeg;
      nWallLayers=A.nWallLayers;
      wallFrac=A.wallFrac;
      wallMat=A.wallMat;
      nRoofLayers=A.nRoofLayers;
      roofFrac=A.roofFrac;
      roofMat=A.roofMat;
      nFloorLayers=A.nFloorLayers;
      floorFrac=A.floorFrac;
      floorMat=A.floorMat;
      iFrontLining=A.iFrontLining;
      iSplitVoid=A.iSplitVoid;
    }
  return *this;
}

Beamstop::~Beamstop() 
  /*!
    Destructor
  */
{}

void
Beamstop::removeFrontOverLap()
  /*!
    Remove segments that are completly covered by the
    active front.
  */
{
  ELog::RegMethod RegA("Beamstop","removeFrontOverLap");

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
Beamstop::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("Beamstop","populate");
  
  FixedOffsetGroup::populate(Control);

  // Void + Fe special:
  //  length=Control.EvalVar<double>(keyName+"Length");
  left=Control.EvalVar<double>(keyName+"Left");
  right=Control.EvalVar<double>(keyName+"Right");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  front=Control.EvalVar<double>(keyName+"Front");
  length = front;
  
  spacing =  Control.EvalVar<double>(keyName+"Spacing");
  
  voidLeft=Control.EvalVar<double>(keyName+"VoidLeft");
  voidRight=Control.EvalVar<double>(keyName+"VoidRight");
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidFront=Control.EvalVar<double>(keyName+"VoidFront");
  
  defMat=ModelSupport::EvalDefMat<int>(Control,keyName+"DefMat",0);
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  //  int B4CMat;

  nSeg=Control.EvalDefVar<size_t>(keyName+"NSeg",1);
  iFrontLining=Control.EvalDefVar<int>(keyName+"FrontLining",1);
  iSplitVoid=Control.EvalDefVar<int>(keyName+"SplitVoid",0);



  int iLayerFlag=Control.EvalDefVar<int>("LayerFlag",1);

  if(iLayerFlag)
    {  nSeg=Control.EvalDefVar<size_t>(keyName+"NSeg",1);
  nWallLayers=Control.EvalVar<size_t>(keyName+"NWallLayers");
  nFloorLayers=Control.EvalVar<size_t>(keyName+"NFloorLayers");
  nRoofLayers=Control.EvalVar<size_t>(keyName+"NRoofLayers");
    }else {nSeg=1; nWallLayers=2; nRoofLayers=2; nFloorLayers=2;}



  
  //   yStep=
  //    Control.EvalDefVar2<double>(keyName+"YStep",0.0);

  InnerB4CThick=
        Control.EvalPair<double>(keyName+"InnerB4CLayerThick","DefaultTileThick");

  OuterB4CThick=
        Control.EvalPair<double>(keyName+"OuterB4CLayerThick","DefaultTileThick");
   B4CMat=ModelSupport::EvalDefMat<int>(Control,keyName+"B4CTileMat",0);

  
  
  ModelSupport::populateDivide(Control,nWallLayers,keyName+"WallMat",
			       defMat,wallMat);
  ModelSupport::populateDivideLen(Control,nWallLayers,keyName+"WallLen",
				  std::min(left,right),wallFrac);
  wallFrac.push_back(1.0);
   
  
  nRoofLayers=Control.EvalVar<size_t>(keyName+"NRoofLayers");
  ModelSupport::populateDivide(Control,nRoofLayers,keyName+"RoofMat",
			       defMat,roofMat);
  ModelSupport::populateDivideLen(Control,nRoofLayers,keyName+"RoofLen",
				  height,roofFrac);
  roofFrac.push_back(1.0);
  
  if(OuterB4CThick>0.0){
    wallMat.push_back(B4CMat);roofMat.push_back(B4CMat);}
  
  nFloorLayers=Control.EvalVar<size_t>(keyName+"NFloorLayers");
  ModelSupport::populateDivide(Control,nFloorLayers,keyName+"FloorMat",
			       defMat,floorMat);
  ModelSupport::populateDivideLen(Control,nFloorLayers,keyName+"FloorLen",
				  depth,floorFrac);

  floorFrac.push_back(1.0);
  
  return;
}

void
Beamstop::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("Beamstop","createUnitVector");


  FixedComp::createUnitVector(FC,sideIndex);
  //   applyOffset();
  // std::cout << xStep << " "  << yStep <<" " <<zStep << std::endl;
  // after rotation
   Origin+=Y*(length/2.0)+Y*yStep+X*xStep+Z*zStep;
  return;
}

void
Beamstop::createSurfaces()
  /*!
    Create the surfaces. Note that layers is not used
    because we want to break up the objects into sub components
  */
{
  ELog::RegMethod RegA("Beamstop","createSurfaces");

  // Inner void
  if (!activeFront){
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
   if(InnerB4CThick>0.0) ModelSupport::buildPlane(SMap,
		  buildIndex+11,Origin+Y*InnerB4CThick,Y);
  }
    /*      if (!activeBack)
       ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(front),Y);
    */
    
  if (activeFront)
    {    removeFrontOverLap();
   if(InnerB4CThick>0.0) ModelSupport::buildPlane(SMap,
		  buildIndex+11,Origin+Y*InnerB4CThick,Y);
			   };
       
      const double segStep(voidFront/static_cast<double>(nSeg));
      double segLen(0);//(-(front-voidFront)/2.0);
   

  int SI(buildIndex);
  for(size_t i=0;i<nSeg;i++)
    {
      segLen+=segStep;
      ModelSupport::buildPlane(SMap,SI+2,Origin+Y*segLen,Y);
      SI+=10;
    }


    // Front planes  
    int FRI(SI);

     if(InnerB4CThick>0.0){
  ModelSupport::buildPlane(SMap,FRI+2,
   Origin+Y*
  (-InnerB4CThick+voidFront+(front-voidFront)*(wallFrac[0]-1./nWallLayers)/(1.-1./nWallLayers)),Y);
  FRI+=10;
     }
      for(size_t i=0;i<nWallLayers;i++)
    {
      ModelSupport::buildPlane(SMap,FRI+2,
   Origin+
       Y*(voidFront+(front-voidFront)*(wallFrac[i]-1./nWallLayers)/(1.-1./nWallLayers)),Y);
      //      std::cout << FRI+2 << std::endl;
      FRI+=10;
    }

  
  // Wall planes      

  int WI(buildIndex);
  if(InnerB4CThick>0.0){
  ModelSupport::buildPlane(SMap,WI+3,
   Origin-X*
	(-InnerB4CThick+voidLeft+(left-voidLeft)*(wallFrac[0]-1./nWallLayers)/(1.-1./nWallLayers)),X);
      ModelSupport::buildPlane(SMap,WI+4,
   Origin+X*
     (-InnerB4CThick+voidRight+(right-voidRight)*(wallFrac[0]-1./nWallLayers)/(1.-1./nWallLayers)),X);
      WI+=10;}
  
  for(size_t i=0;i<nWallLayers;i++)
    {
      ModelSupport::buildPlane(SMap,WI+3,
   Origin-X*
	(voidLeft+(left-voidLeft)*(wallFrac[i]-1./nWallLayers)/(1.-1./nWallLayers)),X);
      ModelSupport::buildPlane(SMap,WI+4,
   Origin+X*
     (voidRight+(right-voidRight)*(wallFrac[i]-1./nWallLayers)/(1.-1./nWallLayers)),X);
      WI+=10;
    }

  if(OuterB4CThick>0.0){
      ModelSupport::buildPlane(SMap,WI+3, Origin-X*(left+OuterB4CThick),X);
      ModelSupport::buildPlane(SMap,WI+4, Origin+X*(right+OuterB4CThick),X);
      WI+=10;
  }

  //Roof planes
  
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


   //Floor planes
   
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
Beamstop::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("Beamstop","createObjects");

  std::string Out;
  
  const std::string frontStr
    (activeFront ? frontSurf.display()+frontCut.display() : 
     ModelSupport::getComposite(SMap,buildIndex," 1 "));
  
  /*const std::string backStr
    (activeBack ? backSurf.display()+backCut.display() : 
     ModelSupport::getComposite(SMap,buildIndex," -2 "));
  */
  
  // const std::string frontStr =  ModelSupport::getComposite(SMap,buildIndex," 1 ");
  const std::string backStr =  ModelSupport::getComposite(SMap,buildIndex," -2 ");

  //  std::cout << frontStr << backStr << std::endl;

  
  // Inner void is a single segment

  // Loop over all segments:
  std::string FBStrVoid;
  std::string FBStrLining; 
  std::string FBStr;
  int SI(buildIndex);

  int WI(buildIndex),RI(buildIndex),FI(buildIndex);    

  for(size_t index=0;index<nSeg;index++)
    {

  
      if(InnerB4CThick>0){
	FBStrVoid=((index) ?
	     ModelSupport::getComposite(SMap,SI-10," 2 ") :
		   (iFrontLining)? ModelSupport::getComposite(SMap,SI," 11 "):
		       frontStr);
      //            std::cout << FBStr << std::endl;

	    FBStrVoid+= ((InnerB4CThick>0.0)&&(index==nSeg-1)) ?
	       ModelSupport::getComposite(SMap,SI," -12 ") :
	ModelSupport::getComposite(SMap,SI," -2 ");// );

      }else{
      FBStrVoid=((index) ?
	     ModelSupport::getComposite(SMap,SI-10," 2 ") :
	     frontStr);
      //            std::cout << FBStr << std::endl;

	    FBStrVoid+= ((InnerB4CThick>0.0)&&(index==nSeg-1)) ?
	       ModelSupport::getComposite(SMap,SI," -12 ") :
	ModelSupport::getComposite(SMap,SI," -2 ");// );
      }
      FBStr=((index) ?
	     ModelSupport::getComposite(SMap,SI-10," 2 ") :
	     frontStr);
      //            std::cout << FBStr << std::endl;

      FBStr+= //((InnerB4CThick>0.0)&&(index==nSeg-1)) ?
	//  ModelSupport::getComposite(SMap,SI," -12 ") :
	ModelSupport::getComposite(SMap,SI," -2 ");// );

	    SI+=10; 
      //    std::cout << FBStr << std::endl;
        Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6 5 ");
	if(iSplitVoid==0){
	Out+=frontStr; Out+=	ModelSupport::getComposite(SMap,SI+10*(nSeg-1)," -2 ");
	if(index==0) 	makeCell("Void",System,cellIndex++,voidMat,0.0,Out);
	}else{	makeCell("Void",System,cellIndex++,voidMat,0.0,Out+FBStrVoid);}

	if((index==0)&&(InnerB4CThick>0)){
        Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6 5 -11");
	if(iFrontLining)
	  makeCell("FrontLining",System,cellIndex++,B4CMat,0.0,Out+frontStr);
	 }

       
      // Inner is a single component
      // Walls are contained:
      WI=buildIndex;
      // Innermost layer is B4C concrete.

      // A separate calculation.
      if(InnerB4CThick>0.0){
          Out=ModelSupport::getComposite(SMap,WI,buildIndex,
			  " 13 -3 -6M 5M");
	  //	  System.addCell(MonteCarlo::Object
	  //		 (cellIndex++,B4CMat,0.0,Out+FBStr));
       makeCell("WallLining",System,cellIndex++,B4CMat,0.0,Out+FBStr);
		 
	  Out=ModelSupport::getComposite(SMap,WI,buildIndex,
			 " 4 -14 -6M 5M");
	  /*	  Out+=ModelSupport::getComposite(SMap,SI," -12 ");
	  Out+=((index) ?
	     ModelSupport::getComposite(SMap,SI-10," 2 ") :
	     frontStr);*/
	  //	  System.addCell(MonteCarlo::Object
	  //		 (cellIndex++,B4CMat,0.0,Out+FBStr));
       makeCell("WallLining",System,cellIndex++,B4CMat,0.0,Out+FBStr);
	  WI+=10;}

      size_t imax=OuterB4CThick>0.0 ? nWallLayers+1 : nWallLayers;
 
      for(size_t i=1;i<imax;i++)
	{
	  if (InnerB4CThick>0.0) Out=ModelSupport::getComposite(SMap,WI,buildIndex,
					 " 13 -3 -16M 5M"); else
	  Out=ModelSupport::getComposite(SMap,WI,buildIndex,
					 " 13 -3 -6M 5M");
	  //need to add 10 to build index for upper border of the rectangle
	  //to go above B4Clayer height if it is present
	  //	  System.addCell(MonteCarlo::Object
	  //		 (cellIndex++,wallMat[i],0.0,Out+FBStr));
       makeCell("Wall",System,cellIndex++,wallMat[i],0.0,Out+FBStr);
	  if(InnerB4CThick>0.0)  Out=ModelSupport::getComposite(SMap,WI,buildIndex,
			 " 4 -14 -16M 5M"); else
	    Out=ModelSupport::getComposite(SMap,WI,buildIndex,
			 " 4 -14 -6M 5M");
	  //	  System.addCell(MonteCarlo::Object
	  //		 (cellIndex++,wallMat[i],0.0,Out+FBStr));
	         makeCell("Wall",System,cellIndex++,wallMat[i],0.0,Out+FBStr);
	  WI+=10;
   	}

      // Roof on top of walls are contained:
      RI=buildIndex;
     int WII(buildIndex);
      if(InnerB4CThick>0.0){
          Out=ModelSupport::getComposite(SMap,RI,buildIndex," 3M -4M -16 6 ");
	  //	  System.addCell(MonteCarlo::Object
	  //		 (cellIndex++,B4CMat,0.0,Out+FBStr));
       makeCell("RoofLining",System,cellIndex++,B4CMat,0.0,Out+FBStr);
	  RI+=10; WII+=10;}

      imax=OuterB4CThick>0.0 ? nRoofLayers+1 : nRoofLayers;
 
      for(size_t i=1;i<imax;i++)
	{
	  Out=ModelSupport::getComposite(SMap,RI,WII," 3M -4M -16 6 ");
	  //	  System.addCell(MonteCarlo::Object
	  //		 (cellIndex++,roofMat[i],0.0,Out+FBStr));
         makeCell("Roof",System,cellIndex++,roofMat[i],0.0,Out+FBStr);
		 RI+=10;

	}


      
      // Floor complete:
      FI=buildIndex;
      if (spacing>0.0){
      Out=ModelSupport::getComposite(SMap,FI,WI,
		      " 3M -4M -5 15  (-3 : 4)");
       System.addCell(MonteCarlo::Object
			 (cellIndex++,0,0.0,Out+FBStr));

       Out=ModelSupport::getComposite(SMap,FI,WI,
		      " -5 15  3  -4 ");
       if (InnerB4CThick>0.0)
	 {// System.addCell(MonteCarlo::Object(cellIndex++,B4CMat,0.0,Out+FBStr));
       makeCell("FloorLining",System,cellIndex++,B4CMat,0.0,Out+FBStr);
	 }
       else
	 { 	 System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out+FBStr));
	   // makeCell("FloorLining",System,cellIndex++,0,0.0,Out+FBStr);
	 }
	 }
      FI+=10;
      for(size_t i=1;i<nFloorLayers;i++)
	{
	  Out=ModelSupport::getComposite(SMap,FI,WI," 3M -4M -5 15 ");
	
	  //	  System.addCell(MonteCarlo::Object
	  //		 (cellIndex++,floorMat[i],0.0,Out+FBStr));
	    makeCell("Floor",System,cellIndex++,floorMat[i],0.0,Out+FBStr);
	    FI+=10;
	}

      
      
      // Left corner
      WI=buildIndex; RI=buildIndex;

      if (InnerB4CThick>0.0){
       Out=ModelSupport::getComposite(SMap,WI,RI," -3 13 6M -16M ");
	System.addCell(MonteCarlo::Object(cellIndex++,B4CMat,0.0,Out+FBStr));
	      
        Out=ModelSupport::getComposite(SMap,WI,RI," 4 -14 6M -16M ");
	System.addCell(MonteCarlo::Object(cellIndex++,B4CMat,0.0,Out+FBStr));

	      RI=buildIndex+10;
      }

      imax=OuterB4CThick>0.0 ? nRoofLayers+1 : nRoofLayers;
            size_t jmax=OuterB4CThick>0.0 ? nWallLayers+1 : nWallLayers;
 
      for(size_t i=1;i<imax;i++)
	{
	  WI=buildIndex; if(InnerB4CThick>0.0)WI+=10;
	  for(size_t j=1;j<jmax;j++)
	    {
	      const int mat((i>j) ? roofMat[i] : wallMat[j]);
	      Out=ModelSupport::getComposite(SMap,WI,RI," -3 13 6M -16M ");
	      System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out+FBStr));
	      
	      Out=ModelSupport::getComposite(SMap,WI,RI," 4 -14 6M -16M ");
	      System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out+FBStr));
	      WI+=10;
	    }
	  RI+=10;
	}

      /// Walls of the labyrinth constructed.




}

      
  int FRI(SI);

  //      FI+=10;
       if (InnerB4CThick>0.0)
	 {
 Out=ModelSupport::getComposite(SMap,FRI," 2 -12 ");
 // std::cout << Out << std::endl;
 Out+=ModelSupport::getComposite(SMap,buildIndex," -6 ");
	  	  Out+=ModelSupport::getComposite(SMap,buildIndex," 5 ");
	  	  Out+=ModelSupport::getComposite(SMap,buildIndex," 3 -4 ");	       
		  //  System.addCell(MonteCarlo::Object
		  //	 (cellIndex++,B4CMat,0.0,Out));
  makeCell("BeamstopLining",System,cellIndex++,B4CMat,0.0,Out);
	  FRI+=10;
	 }

  for(size_t i=1;i<nWallLayers;i++)
         {
	  Out=ModelSupport::getComposite(SMap,FRI," 2 -12 ");
	  //	  std::cout <<i <<" "<< Out << std::endl;
	  	  Out+=ModelSupport::getComposite(SMap,RI," -6 ");
	  	  Out+=ModelSupport::getComposite(SMap,FI," 5 ");
	  	  Out+=ModelSupport::getComposite(SMap,WI," 3 -4 ");		
		  //	  System.addCell(MonteCarlo::Object
		  //	 (cellIndex++,wallMat[i],0.0,Out));
  makeCell("Beamstop",System,cellIndex++,wallMat[i],0.0,Out);

	  FRI+=10;

	}

      
      
  // Outer
      Out=ModelSupport::getComposite(SMap,WI,FI,RI," 3 -4 5M -6N ");
      Out+=ModelSupport::getComposite(SMap,FRI," -2 ");
  addOuterSurf(Out+frontStr);

  return;
}

void
Beamstop::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("Beamstop","createLinks");

  attachSystem::FixedComp& innerFC=FixedGroup::getKey("Inner");
  attachSystem::FixedComp& outerFC=FixedGroup::getKey("Outer");


  outerFC.setConnect(2,Origin-X*left,-X);
  outerFC.setConnect(3,Origin+X*right,X);
  outerFC.setConnect(4,Origin-Z*depth,-Z);
  outerFC.setConnect(5,Origin+Z*height,Z);

  const int WI(buildIndex+(static_cast<int>(nWallLayers)-1)*10);
  const int RI(buildIndex+(static_cast<int>(nRoofLayers)-1)*10);
  const int FI(buildIndex+(static_cast<int>(nFloorLayers)-1)*10);

  if (!activeFront)
    {
      outerFC.setConnect(0,Origin-Y*(length/2.0),-Y);
      outerFC.setLinkSurf(0,-SMap.realSurf(buildIndex+1));      
    }
  else
    {
      outerFC.setLinkSurf(0,frontSurf,1,frontCut,0);      
      outerFC.setConnect
        (0,SurInter::getLinePoint(Origin,Y,frontSurf,frontCut),-Y);
    }
  
  if (!activeBack)
    {
      //      outerFC.setConnect(1,Origin+Y*(length/2.0),Y);
            outerFC.setConnect(1,Origin+Y*front,Y);
	    int iLining= (InnerB4CThick>0)?1:0;
	    outerFC.setLinkSurf(1,SMap.realSurf(buildIndex+2+10*iLining+
						(static_cast<int>(nWallLayers)+
						 static_cast<int>(nSeg)-1)*10));
    }
  else
    {
      outerFC.setLinkSurf(1,backSurf,1,backCut,0);      
      outerFC.setConnect
        (1,SurInter::getLinePoint(Origin,Y,backSurf,backCut),Y);
    }
  outerFC.setLinkSurf(2,-SMap.realSurf(WI+3));
  outerFC.setLinkSurf(3,SMap.realSurf(WI+4));
  outerFC.setLinkSurf(4,-SMap.realSurf(FI+5));
  outerFC.setLinkSurf(5,SMap.realSurf(RI+6));


  //Inner surfaces
  
  innerFC.setConnect(2,Origin-X*voidLeft,-X);
  innerFC.setConnect(3,Origin+X*voidRight,X);
  innerFC.setConnect(4,Origin-Z*voidDepth,-Z);
  innerFC.setConnect(5,Origin+Z*voidHeight,Z);

  if (!activeFront)
    {
      innerFC.setConnect(0,Origin-Y*(length/2.0),-Y);
      innerFC.setLinkSurf(0,-SMap.realSurf(buildIndex+1));      
    }
  else
    {
      innerFC.setLinkSurf(0,frontSurf,1,frontCut,0);      
      innerFC.setConnect
        (0,SurInter::getLinePoint(Origin,Y,frontSurf,frontCut),-Y);
    }
  
  /*  if (!activeBack)
      {*/
      //      innerFC.setConnect(1,Origin+Y*(length/2.0),Y);
            innerFC.setConnect(1,Origin+Y*voidFront,Y);
	    // innerFC.setLinkSurf(1,SMap.realSurf(buildIndex+2+
	    //				      static_cast<int>(nWallLayers)*10));
	    if (InnerB4CThick>0)innerFC.setLinkSurf(1,-SMap.realSurf(buildIndex
								     +10*nSeg+2));
	    else innerFC.setLinkSurf(1,-SMap.realSurf(buildIndex+2+10*(nSeg-1)));
	    /* }
  else
    {
      innerFC.setLinkSurf(1,backSurf,1,backCut,0);      
      innerFC.setConnect
        (1,SurInter::getLinePoint(Origin,Y,backSurf,backCut),Y);
	}*/
  innerFC.setLinkSurf(2,-SMap.realSurf(3));
  innerFC.setLinkSurf(3,SMap.realSurf(4));
  innerFC.setLinkSurf(4,-SMap.realSurf(5));
  innerFC.setLinkSurf(5,SMap.realSurf(6));



  

  return;
}
  
void
Beamstop::setFront(const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Set front surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("Beamstop","setFront");
  
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
Beamstop::setBack(const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Set back surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("Beamstop","setBack");
  
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
Beamstop::getXSectionIn() const
  /*!
    Get the line shield inner void section
    \return HeadRule of cross-section
   */
{
  ELog::RegMethod RegA("Beamstop","getXSectionIn");
  const std::string Out=
    ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 ");
  HeadRule HR(Out);
  HR.populateSurf();
  return HR;
}
  
    
void
Beamstop::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("Beamstop","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
