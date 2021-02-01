/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Model/caveBuild/Corner.cxx
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

#include "Corner.h"

namespace constructSystem
{

Corner::Corner(const std::string& Key) : 
  attachSystem::FixedOffset(Key,7),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  //  buildIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  // cellIndex(buildIndex+1),
  activeFront(0),activeBack(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

Corner::Corner(const Corner& A) : 
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
  defMat(A.defMat), voidMat(A.voidMat),
  nSeg(A.nSeg),nWallLayers(A.nWallLayers),
  wallFrac(A.wallFrac),wallMat(A.wallMat),
  nRoofLayers(A.nRoofLayers),roofFrac(A.roofFrac),
  roofMat(A.roofMat),nFloorLayers(A.nFloorLayers),
  floorFrac(A.floorFrac),floorMat(A.floorMat), iFrontLining(A.iFrontLining)
  /*!
    Copy constructor
    \param A :: Corner to copy
  */
{}

Corner&
Corner::operator=(const Corner& A)
  /*!
    Assignment operator
    \param A :: Corner to copy
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
    }
  return *this;
}

Corner::~Corner() 
  /*!
    Destructor
  */
{}

void
Corner::removeFrontOverLap()
  /*!
    Remove segments that are completly covered by the
    active front.
  */
{
  ELog::RegMethod RegA("Corner","removeFrontOverLap");

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
Corner::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("Corner","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  //  length=Control.EvalVar<double>(keyName+"Length");
  left=Control.EvalVar<double>(keyName+"Left");
  right=Control.EvalVar<double>(keyName+"Right");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  front=Control.EvalVar<double>(keyName+"Front");

  spacing =  Control.EvalVar<double>(keyName+"Spacing");
  
  voidLeft=Control.EvalVar<double>(keyName+"VoidLeft");
  voidRight=Control.EvalVar<double>(keyName+"VoidRight");
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidFront=Control.EvalVar<double>(keyName+"VoidFront");
  
  defMat=ModelSupport::EvalDefMat<int>(Control,keyName+"DefMat",0);
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  spacingMat=ModelSupport::EvalDefMat<int>(Control,keyName+"SpacingMat",0);
  //  int B4CMat;


  int iLayerFlag=Control.EvalDefVar<int>("LayerFlag",1);

  iFrontLining=Control.EvalDefVar<int>(keyName+"FrontLining",1);

  if(iLayerFlag)
    {  nSeg=Control.EvalDefVar<size_t>(keyName+"NSeg",1);
  nWallLayers=Control.EvalVar<size_t>(keyName+"NWallLayers");
  nRoofLayers=Control.EvalVar<size_t>(keyName+"NRoofLayers");
  nFloorLayers=Control.EvalVar<size_t>(keyName+"NFloorLayers");
  nFrontLayers=
   Control.EvalPair<size_t>(keyName+"NFrontLayers",keyName+"NWallLayers");
  }else{
    nSeg=1; nWallLayers=2; nRoofLayers=2; nFloorLayers=2;  nFloorLayers=2;
    nFrontLayers=2;}

  


  InnerB4CThick=
        Control.EvalPair<double>(keyName+"InnerB4CLayerThick","DefaultTileThick");

  OuterB4CThick=
        Control.EvalPair<double>(keyName+"OuterB4CLayerThick","DefaultTileThick");
   B4CMat=ModelSupport::EvalDefMat<int>(Control,keyName+"B4CTileMat",0);


  
 
  ModelSupport::populateDivide(Control,nFrontLayers,keyName+"FrontMat",
			       defMat,frontMat);
  ModelSupport::populateDivideLen(Control,nFrontLayers,keyName+"FrontLen",
				  front,frontFrac);
  frontFrac.push_back(1.0);
 
  
  
  ModelSupport::populateDivide(Control,nWallLayers,keyName+"WallMat",
			       defMat,wallMat);
  ModelSupport::populateDivideLen(Control,nWallLayers,keyName+"WallLen",
				  (left>0.0)? left:right,wallFrac);
  wallFrac.push_back(1.0);
   
  
  ModelSupport::populateDivide(Control,nRoofLayers,keyName+"RoofMat",
			       defMat,roofMat);
  ModelSupport::populateDivideLen(Control,nRoofLayers,keyName+"RoofLen",
				  height,roofFrac);
  roofFrac.push_back(1.0);
  
  if(OuterB4CThick>0.0){
    wallMat.push_back(B4CMat);roofMat.push_back(B4CMat);}
  
  ModelSupport::populateDivide(Control,nFloorLayers,keyName+"FloorMat",
			       defMat,floorMat);
  ModelSupport::populateDivideLen(Control,nFloorLayers,keyName+"FloorLen",
				  depth,floorFrac);

  floorFrac.push_back(1.0);
  
  return;
}

void
Corner::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("Corner","createUnitVector");


  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  Origin+=Y*(length/2.0);
  return;
}

void
Corner::createSurfaces()
  /*!
    Create the surfaces. Note that layers is not used
    because we want to break up the objects into sub components
  */
{
  ELog::RegMethod RegA("Corner","createSurfaces");

  // Inner void
  //  if (!activeFront)
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
     if(InnerB4CThick>0.0)
    ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*InnerB4CThick,Y);
      
    //  if (!activeBack)
    //    ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(front),Y);
    /*
  if (activeFront)
    removeFrontOverLap();
    */
    /*    
  const double segStep(length/static_cast<double>(nSeg));
  double segLen(-length/2.0);
    */

    // Front planes  
    int FRI(buildIndex);

     if(InnerB4CThick>0.0){
  ModelSupport::buildPlane(SMap,FRI+2,
   Origin+Y*
  (-InnerB4CThick+voidFront+(front-voidFront)*(frontFrac[0]-1./nFrontLayers)/(1.-1./nFrontLayers)),Y);
  FRI+=10;
     }
      for(size_t i=0;i<nFrontLayers;i++)
    {
         ModelSupport::buildPlane(SMap,FRI+2,
   Origin+
       Y*(voidFront+(front-voidFront)*(frontFrac[i]-1./nFrontLayers)/(1.-1./nFrontLayers)),Y);
      FRI+=10;
    }

      // Wall planes      

  int WI(buildIndex);
  if(InnerB4CThick>0.0){
    if (left>0){
    ModelSupport::buildPlane(SMap,WI+3,Origin-X*
 (-InnerB4CThick+voidLeft+(left-voidLeft)*(wallFrac[0]-1./nWallLayers)/(1.-1./nWallLayers)),X);

    ModelSupport::buildPlane(SMap,WI+4,Origin,X);

    } else {

   ModelSupport::buildPlane(SMap,WI+3, Origin,X);
  ModelSupport::buildPlane(SMap,WI+4, Origin+X*
 (-InnerB4CThick+voidRight+(right-voidRight)*(wallFrac[0]-1./nWallLayers)/(1.-1./nWallLayers)),X);
    }    
    WI+=10;} else {
    if (left>0)
    ModelSupport::buildPlane(SMap,WI+4,Origin,X);
     else  ModelSupport::buildPlane(SMap,WI+3, Origin,X);
    
  }
  
  for(size_t i=0;i<nWallLayers;i++)
    {
      if (left>0){
      ModelSupport::buildPlane(SMap,WI+3,
     Origin-X*
    (voidLeft+(left-voidLeft)*(wallFrac[i]-1./nWallLayers)/(1.-1./nWallLayers)),X);
      } else {   ModelSupport::buildPlane(SMap,WI+4,
   Origin+X*
  (voidRight+(right-voidRight)*(wallFrac[i]-1./nWallLayers)/(1.-1./nWallLayers)),X);


      }

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
Corner::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("Corner","createObjects");

  std::string Out;
  /*  
  const std::string frontStr
    (activeFront ? frontSurf.display()+frontCut.display() : 
     ModelSupport::getComposite(SMap,buildIndex," 1 "));
  const std::string backStr
    (activeBack ? backSurf.display()+backCut.display() : 
     ModelSupport::getComposite(SMap,buildIndex," -2 "));
  */
  const std::string frontStr =  ModelSupport::getComposite(SMap,buildIndex," 1 ");
  const std::string backStr =  ModelSupport::getComposite(SMap,buildIndex," -2 ");
  // Inner void is a single segment

  if (InnerB4CThick>0.0){
 Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6 5 -2 11 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6 5 1 -11 ");

  if (iFrontLining) makeCell("FrontLining",System,cellIndex++,B4CMat,0.0,Out);
  else makeCell("FrontLining",System,cellIndex++,voidMat,0.0,Out);
 }else{
    Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6 5 1 -2 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);
  }
  //  return;
  // Loop over all segments:
  std::string FBStr;


  int WI(buildIndex),RI(buildIndex),FI(buildIndex);    



      FBStr= frontStr;
      FBStr+= backStr;
      

      // Inner is a single component
      // Walls are contained:
      WI=buildIndex;
      // Innermost layer is B4C concrete.

      // A separate calculation.

      //Walls

      if(InnerB4CThick>0.0){
	if (left>0){
	  Out=ModelSupport::getComposite(SMap,WI,buildIndex,
			  " 13 -3 -6M 5M 1M -2M");
	  System.addCell(MonteCarlo::Object
			 (cellIndex++,B4CMat,0.0,Out));
	} else {	 
	  Out=ModelSupport::getComposite(SMap,WI,buildIndex,
			 " 4 -14 -6M 5M 1M -2M");
	  System.addCell(MonteCarlo::Object
			 (cellIndex++,B4CMat,0.0,Out));
	};
	WI+=10;}
      
      
      size_t imax=OuterB4CThick>0.0 ? nWallLayers+1 : nWallLayers;
 
      for(size_t i=1;i<imax;i++)
	{
	  if(left>0){
	    if (InnerB4CThick>0.0) Out=ModelSupport::getComposite(SMap,WI,buildIndex,
				     " 13 -3 -16M 5M 1M -12M "); else
	  Out=ModelSupport::getComposite(SMap,WI,buildIndex,
					 " 13 -3 -6M 5M 1M -2M ");
	  //need to add 10 to build index for upper border of the rectangle
	  //to go above B4Clayer height if it is present
	  System.addCell(MonteCarlo::Object
			 (cellIndex++,wallMat[i],0.0,Out));
	  }else {
	    //	    std::cout <<"bbb"<<std::endl;
	  if(InnerB4CThick>0.0)  Out=ModelSupport::getComposite(SMap,WI,buildIndex,
			 " 4 -14 -16M 5M 1M -12M "); else
	    Out=ModelSupport::getComposite(SMap,WI,buildIndex,
			 " 4 -14 -6M 5M 1M -2M ");
	  System.addCell(MonteCarlo::Object
			 (cellIndex++,wallMat[i],0.0,Out));
	  };
	  WI+=10;
   	}
    
      
      
      // Roof on top of walls are contained:
    std::string LRString;
      if(left>0) LRString=" 3M -4N "; else LRString=" 3N -4M ";
  
      RI=buildIndex;
     int WII(buildIndex);
      if(InnerB4CThick>0.0){
	Out=ModelSupport::getComposite(SMap,RI,buildIndex,buildIndex,
				       LRString+" -16 6 ");
	  System.addCell(MonteCarlo::Object
			 (cellIndex++,B4CMat,0.0,Out+FBStr));
	  RI+=10; WII+=10;}
      
      imax=OuterB4CThick>0.0 ? nRoofLayers+1 : nRoofLayers;
 
      for(size_t i=1;i<imax;i++)
	{
	  if(InnerB4CThick>0.0){
	  Out=ModelSupport::getComposite(SMap,RI,WII,buildIndex,
					 LRString +" -16 6 1N -12N");
	  } else {
	    	  Out=ModelSupport::getComposite(SMap,RI,WII,buildIndex,
					 LRString +" -16 6 1N -2N");
	  };

makeCell("Roof",System,cellIndex++,roofMat[i],0.0,Out);
//	  System.addCell(MonteCarlo::Object
//			 (cellIndex++,roofMat[i],0.0,Out));
	  RI+=10;

	}

      
     
      //     return;
      
  int FRI(buildIndex);

    if(InnerB4CThick>0.0){
	Out=ModelSupport::getComposite(SMap,FRI," 5 -6 2 -12 3 -4 ");
	  System.addCell(MonteCarlo::Object
			 (cellIndex++,B4CMat,0.0,Out));
	  FRI+=10;}
      
     
  //      FI+=10;
      for(size_t i=1;i<nFrontLayers;i++)
	{
    if(InnerB4CThick>0.0){
      if(left>0){
	  Out=ModelSupport::getComposite(SMap,buildIndex,FRI,
					 " 5 -16 13 -4  2M -12M ");}
      else{ Out=ModelSupport::getComposite(SMap,buildIndex,FRI,
					 " 5 -16 3 -14  2M -12M ");}
    }else { Out=ModelSupport::getComposite(SMap,buildIndex,FRI,
						      " 5 -6 3 -4  2M -12M ");}
	  /*	  	  Out+=ModelSupport::getComposite(SMap,RI," -6 ");
	  	  Out+=ModelSupport::getComposite(SMap,FI," 5 ");
		  //  	  Out+=ModelSupport::getComposite(SMap,WI," 3 -4 ");
	  Out+=ModelSupport::getComposite(SMap,buildIndex," 3 -4 ");		
	  */
	  System.addCell(MonteCarlo::Object
			 (cellIndex++,wallMat[i],0.0,Out));
	  FRI+=10;

	}
     
     
      // Floor complete:
      FI=buildIndex;
          if(left>0) LRString=" 3M -4N "; else LRString=" 3N -4M ";
  
      if (spacing>0.0){
	Out=ModelSupport::getComposite(SMap,FI,WI,buildIndex,
		      LRString+ " -5 15  (-3 : 4)");
       System.addCell(MonteCarlo::Object
			 (cellIndex++,spacingMat,0.0,Out+FBStr));

       Out=ModelSupport::getComposite(SMap,FI,WI,
		      " -5 15  3  -4 ");
       if (InnerB4CThick>0.0)
	 System.addCell(MonteCarlo::Object(cellIndex++,B4CMat,0.0,Out+FBStr));
       else
 	 System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out+FBStr));
      }// end if spacing...
      FI+=10;
      for(size_t i=1;i<nFloorLayers;i++)
	{
	  Out=ModelSupport::getComposite(SMap,FI,WI,buildIndex, LRString + " -5 15 1N  ");
	  Out+=ModelSupport::getComposite(SMap,FRI," -2 ");
	  System.addCell(MonteCarlo::Object
			 (cellIndex++,floorMat[i],0.0,Out));
	  FI+=10;

	}

      
      // Left corner wall/front
      WI=buildIndex; FRI=buildIndex;
     
      if (InnerB4CThick>0.0){
	if (left>0){  Out=ModelSupport::getComposite(SMap,WI,FRI,buildIndex, " -3 13 2M -12M 5N -16N ");
	System.addCell(MonteCarlo::Object(cellIndex++,B4CMat,0.0,Out));
	}else{
	  Out=ModelSupport::getComposite(SMap,WI,FRI,buildIndex," 4 -14 2M -12M 5 -16 ");
	System.addCell(MonteCarlo::Object(cellIndex++,B4CMat,0.0,Out));
	}
	      FRI=buildIndex+10;
      }
      
      imax=OuterB4CThick>0.0 ? nFrontLayers+1 : nFrontLayers;
            size_t jmax=OuterB4CThick>0.0 ? nWallLayers+1 : nWallLayers;
 
      for(size_t i=1;i<imax;i++)
	{
	  WI=buildIndex; if(InnerB4CThick>0.0)WI+=10;
	  for(size_t j=1;j<jmax;j++)
	    {
	      const int mat((i>j) ? roofMat[i] : wallMat[j]);
	      if(left>0){
		if(InnerB4CThick>0.0)
		  Out=ModelSupport::getComposite(SMap,WI,FRI,buildIndex,
					       " -3 13 2M -12M 5N -16N");
		else
		  Out=ModelSupport::getComposite(SMap,WI,FRI,buildIndex,
					       " -3 13 2M -12M 5N -6N");									   
	      System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
	      }else{
		if(InnerB4CThick>0.0)
		  Out=ModelSupport::getComposite(SMap,WI,FRI,buildIndex,
					       " 4 -14 2M -12M 5N -16N");
		else
		  Out=ModelSupport::getComposite(SMap,WI,FRI,buildIndex,
					       " 4 -14 2M -12M 5N -6N");
	      System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out ));
	      }
	      WI+=10;
	    }
	  FRI+=10;
	}

     
      // Left corner wall/roof
      WI=buildIndex; RI=buildIndex;
     
      if (InnerB4CThick>0.0){
	if (left>0){  Out=ModelSupport::getComposite(SMap,WI,RI," -3 13 6M -16M ");
	System.addCell(MonteCarlo::Object(cellIndex++,B4CMat,0.0,Out+FBStr));
	}else{
        Out=ModelSupport::getComposite(SMap,WI,RI," 4 -14 6M -16M ");
	System.addCell(MonteCarlo::Object(cellIndex++,B4CMat,0.0,Out+FBStr));
	}
	      RI=buildIndex+10;
      }
      
      imax=OuterB4CThick>0.0 ? nRoofLayers+1 : nRoofLayers;
             jmax=OuterB4CThick>0.0 ? nWallLayers+1 : nWallLayers;
	     FRI =(InnerB4CThick>0.0)? buildIndex+10 : buildIndex;
      for(size_t i=1;i<imax;i++)
	{
	  WI=buildIndex; if(InnerB4CThick>0.0)WI+=10;
	  for(size_t j=1;j<jmax;j++)
	    {
	      const int mat((i>j) ? roofMat[i] : wallMat[j]);
	      if(left>0){
		Out=ModelSupport::getComposite(SMap,WI,RI,buildIndex," -3 13 6M -16M 1N ");
		Out+=ModelSupport::getComposite(SMap,FRI," -2 ");
	      System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
	      }else{
		Out=ModelSupport::getComposite(SMap,WI,RI,buildIndex," 4 -14 6M -16M 1N");
		Out+=ModelSupport::getComposite(SMap,FRI," -2 ");
	      System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
	      }
	      WI+=10;
	    }
	  RI+=10;
	}


      // return;
      
      //Front Corner front/roof
      FRI=buildIndex; RI=buildIndex;
     
      if (InnerB4CThick>0.0){
    Out=ModelSupport::getComposite(SMap,FRI,RI,buildIndex," 2 -12 6M -16M 3N -4N ");
	System.addCell(MonteCarlo::Object(cellIndex++,B4CMat,0.0,Out));
	      RI=buildIndex+10;
      }

      imax=OuterB4CThick>0.0 ? nRoofLayers+1 : nRoofLayers;
            jmax=OuterB4CThick>0.0 ? nFrontLayers+1 : nFrontLayers;
 
      for(size_t i=1;i<imax;i++)
	{
	  FRI=buildIndex; if(InnerB4CThick>0.0)FRI+=10;
	  for(size_t j=1;j<jmax;j++)
	    {
	      const int mat((i>j) ? roofMat[i] : wallMat[j]);
	      if(InnerB4CThick>0.0)
		{
		if(left>0.0)
	        Out=
	   ModelSupport::getComposite(SMap,FRI,RI,buildIndex," 2 -12 6M -16M 13N -4N ");
		else
	        Out=
     	ModelSupport::getComposite(SMap,FRI,RI,buildIndex," 2 -12 6M -16M 3N -14N ");
		}
	      else {
		if(left>0.0)
		Out=
	 ModelSupport::getComposite(SMap,FRI,RI,buildIndex," 2 -12 6M -16M 3N -4N "); 
	        else
		Out=
	 ModelSupport::getComposite(SMap,FRI,RI,buildIndex," 2 -12 6M -16M 3N -4N "); 
	      };
	      System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
	      FRI+=10;
	    }
	  RI+=10;
	}
     
      //Triple corner

      if (InnerB4CThick>0.0){ RI=buildIndex+10; }
      else { RI=buildIndex; }

      /*      if (InnerB4CThick>0.0){
    Out=ModelSupport::getComposite(SMap,FRI,RI,buildIndex," 2 -12 6M -16M 3N -4N ");
	System.addCell(MonteCarlo::Object(cellIndex++,B4CMat,0.0,Out));
	      RI=buildIndex+10;
      }
      */
      imax=OuterB4CThick>0.0 ? nRoofLayers+1 : nRoofLayers;
         jmax=OuterB4CThick>0.0 ? nFrontLayers+1 : nFrontLayers;
int  kmax=OuterB4CThick>0.0 ? nWallLayers+1 : nWallLayers;
      for(size_t i=1;i<imax;i++)
	{
	  FRI=buildIndex; if(InnerB4CThick>0.0)FRI+=10;
	  for(size_t j=1;j<jmax;j++)
	    {
	      const int mat((i>j) ? roofMat[i] : wallMat[j]);
   	  WI=buildIndex; if(InnerB4CThick>0.0)WI+=10;
	  for(size_t k=1;k<kmax;k++)
	    {	    
   	      if (left>0){
	      Out=ModelSupport::getComposite(SMap,FRI,RI,WI," 2 -12 6M -16M -3N 13N ");
	      }else{
	    Out=ModelSupport::getComposite(SMap,FRI,RI,WI," 2 -12 6M -16M 4N -14N ");
	      }
	    System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
	    WI+=10;
	      };
	      FRI+=10;
	    }
	  RI+=10;
	}
      
      
      /// Walls of the labyrinth constructed.
         
  // Outer
      if(left>0){
      Out=ModelSupport::getComposite(SMap,WI,FI,RI," 3 5M -6N ");
      Out+=ModelSupport::getComposite(SMap,FRI,buildIndex," -2 -4M 1M");
      }else{
      Out=ModelSupport::getComposite(SMap,WI,FI,RI," -4 5M -6N ");
      Out+=ModelSupport::getComposite(SMap,FRI,buildIndex," -2 3M 1M");
      };

      addOuterSurf(Out);

  return;
}

void
Corner::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("Corner","createLinks");

  if(left>0){ FixedComp::setConnect(2,Origin-X*left,-X);
    FixedComp::setConnect(3,Origin,X);}else{
  FixedComp::setConnect(2,Origin,-X);
  FixedComp::setConnect(3,Origin+X*right,X);}
  
  FixedComp::setConnect(4,Origin-Z*depth,-Z);
  FixedComp::setConnect(5,Origin+Z*height,Z);

  const int WI(buildIndex+(static_cast<int>(nWallLayers)-1)*10);
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
  
  if (left<0)
    {FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
      FixedComp::setLinkSurf(3,SMap.realSurf(WI+4));} else {
       FixedComp::setLinkSurf(2,-SMap.realSurf(WI+3));
       FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));};
  
  FixedComp::setLinkSurf(4,-SMap.realSurf(FI+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(RI+6));
  //Include inner surface of the B4C lining at the entrance 
  if(InnerB4CThick>0) {
    FixedComp::setConnect(6,Origin-Y*(length/2.0-InnerB4CThick),-Y);
    FixedComp::setLinkSurf(6,-SMap.realSurf(buildIndex+11));}



  return;
}
  
void
Corner::setFront(const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Set front surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("Corner","setFront");
  
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
Corner::setBack(const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Set back surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("Corner","setBack");
  
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
Corner::getXSectionIn() const
  /*!
    Get the line shield inner void section
    \return HeadRule of cross-section
   */
{
  ELog::RegMethod RegA("Corner","getXSectionIn");
  const std::string Out=
    ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 ");
  HeadRule HR(Out);
  HR.populateSurf();
  return HR;
}
  
    
void
Corner::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("Corner","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
