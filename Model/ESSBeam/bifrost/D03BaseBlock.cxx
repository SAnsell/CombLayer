/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/D03BaseBlock.cxx
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

#include "D03BaseBlock.h"

namespace constructSystem
{

D03BaseBlock::D03BaseBlock(const std::string& Key) : 
  attachSystem::FixedOffset(Key,8),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  //  buildIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  // cellIndex(buildIndex+1),
  activeFront(0),activeBack(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

D03BaseBlock::D03BaseBlock(const D03BaseBlock& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  //  buildIndex(A.buildIndex),cellIndex(A.cellIndex),
  activeFront(A.activeFront),activeBack(A.activeBack),
  frontSurf(A.frontSurf),frontCut(A.frontCut),
  backSurf(A.backSurf),backCut(A.backCut),
  //
  l1edge(A.l1edge),l1mid(A.l1mid),l2mid(A.l2mid),l2edge(A.l2edge),gap(A.gap),
    length1low(A.length1low),length2low(A.length2low),
    wid1b(A.wid1b), wid1f(A.wid1f),wid2(A.wid2),wid3f(A.wid3f),
  wid3b(A.wid3b),hup(A.hup),hlow(A.hlow),
  defMatb(A.defMatb), gapV(A.gapV),
  defMatf(A.defMatf), gapMat(A.gapMat),nSeg(A.nSeg),nLayers(A.nLayers),
  B4CMat(A.B4CMat),B4CMatF(A.B4CMatF),B4CMatB(A.B4CMatB)
  /*!
    Copy constructor
    \param A :: D03BaseBlock to copy
  */
{}

D03BaseBlock&
D03BaseBlock::operator=(const D03BaseBlock& A)
  /*!
    Assignment operator
    \param A :: D03BaseBlock to copy
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

  l1edge=A.l1edge;l1mid=A.l1mid;l2mid=A.l2mid;l2edge=A.l2edge;gap=A.gap;
    length1low=A.length1low;length2low=A.length2low;
    wid1b=A.wid1b; wid1f=A.wid1f;wid2=A.wid2;wid3f=A.wid3f;wid3b=A.wid3b;
    hup=A.hup;hlow=A.hlow; gapV=A.gapV;
    defMatf=A.defMatf;
    defMatb=A.defMatb; gapMat=A.gapMat;nSeg=A.nSeg;nLayers=A.nLayers;
    B4CMat=A.B4CMat; B4CMatF=A.B4CMatF; B4CMatB=A.B4CMatB;
    }
  return *this;
}

D03BaseBlock::~D03BaseBlock() 
  /*!
    Destructor
  */
{}

void
D03BaseBlock::removeFrontOverLap()
  /*!
    Remove segments that are completly covered by the
    active front.
  */
{
  ELog::RegMethod RegA("D03BaseBlock","removeFrontOverLap");
  /*
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
  */
  return;
}
  
void
D03BaseBlock::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("D03BaseBlock","populate");
  
  FixedOffset::populate(Control);

  //Total block length on sides: 4150mm and 3850mm
  // Gap in the drawings 10 mm, make 20 mm
  // Void + Fe special:
  l1edge=Control.EvalDefVar<double>(keyName+"Length1Edge",192.5);
  l2edge=Control.EvalDefVar<double>(keyName+"Length2Edge",207.5);
  l1mid=Control.EvalDefVar<double>(keyName+"Length1Mid",207.5);
  l2mid=Control.EvalDefVar<double>(keyName+"Length2mid",192.5);
  length1low=Control.EvalDefVar<double>(keyName+"Length1Low",40);
  length2low=Control.EvalDefVar<double>(keyName+"Length2Low",40);
  gap=Control.EvalDefVar<double>(keyName+"Gap",1.2);
  gapV=Control.EvalDefVar<double>(keyName+"GapV",0.2);

  l2mid = l1edge+l2edge-l1mid;

  wid1f=Control.EvalDefVar<double>(keyName+"Width1Front",150);
  wid1b=Control.EvalDefVar<double>(keyName+"Width1Back",150);
  wid2=Control.EvalDefVar<double>(keyName+"Width2",110);
  wid3f=Control.EvalDefVar<double>(keyName+"Width3Front",70);
  wid3b=Control.EvalDefVar<double>(keyName+"Width3Back",70);
  hup=Control.EvalDefVar<double>(keyName+"HeightUp",100);
  hlow=Control.EvalDefVar<double>(keyName+"HeightLow",50);
  
  defMatf=ModelSupport::EvalDefMat<int>(Control,keyName+"DefMatF",144);
  defMatb=ModelSupport::EvalDefMat<int>(Control,keyName+"DefMatB",144);
  gapMat=ModelSupport::EvalDefMat<int>(Control,keyName+"GapMat",60);
  B4CMat=ModelSupport::EvalDefMat<int>(Control,keyName+"B4CTileMat",147);
  B4CMatF=ModelSupport::EvalDefMat<int>(Control,keyName+"B4CTileMatF",B4CMat);
  B4CMatB=ModelSupport::EvalDefMat<int>(Control,keyName+"B4CTileMatB",B4CMat);


  InnerB4CThick=
        Control.EvalDefVar<double>(keyName+"InnerB4CLayerThick",0);

  wid3bSteel=Control.EvalDefVar<double>(keyName+"Width3BackSteel",0);

  
  //  int B4CMat;

  nSeg=Control.EvalDefVar<size_t>(keyName+"NSeg",3);

  nLayers=Control.EvalDefVar<size_t>(keyName+"NLayers",4);

  int iLayerFlag=Control.EvalDefVar<int>("LayerFlag",1);


  if (wid3f>(wid2+gap)){
    std::cout << "Warning from BaseBlock populate:"
	      << std::endl << "too small inner width forward part" <<
              std::endl<< "--> making the gap flush with it"
	      <<std::endl;
    //    wid3f=(wid2+gap);
    wid2=(wid3f-gap);
    }
  if (wid3b>(wid2+gap)){
    std::cout << "Warning form BaseBlock populate:"
	      << std::endl << "too small inner width rear part" <<
              std::endl<< "--> making the gap flush"
	      <<std::endl;
    //  wid3b=(wid2+gap);
      wid2=(wid3b-gap);
  }
 

  
  if(iLayerFlag==0){
  nSeg=1;
  nLayers=1;
  }
  // Cable tube
  return;
}

void
D03BaseBlock::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("D03BaseBlock","createUnitVector");


  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  double length=l1edge+l2edge;
  Origin+=Y*(length/2.0);
  return;
}

void
D03BaseBlock::createSurfaces()
  /*!
    Create the surfaces. Note that layers is not used
    because we want to break up the objects into sub components
  */
{
  ELog::RegMethod RegA("D03BaseBlock","createSurfaces");

  double length=l1edge+l2edge;

  // Inner void
  if (!activeFront)
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
    
  if (!activeBack)
    ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);


      
    ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(length/2.0-l1mid+0.5*gap),Y);
    ModelSupport::buildPlane(SMap,buildIndex+111,Origin-Y*(length/2.0-l1edge+0.5*gap),Y);
    ModelSupport::buildPlane(SMap,buildIndex+12,Origin-Y*(length/2.0-length1low),Y);


  ModelSupport::buildPlane(SMap,buildIndex+21,Origin-Y*(length/2.0-l1mid-0.5*gap),Y);
  ModelSupport::buildPlane(SMap,buildIndex+211,Origin-Y*(length/2.0-l1edge-0.5*gap),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*(length/2.0-length2low),Y);
 
  if(InnerB4CThick>0.0){
  ModelSupport::buildPlane(SMap,buildIndex+371,
   Origin-X*
	(-InnerB4CThick+0.5*wid3f),X);
  ModelSupport::buildPlane(SMap,buildIndex+372,
   Origin-X*
	(-InnerB4CThick+0.5*wid3b),X);
    ModelSupport::buildPlane(SMap,buildIndex+471,
   Origin+X*
	(-InnerB4CThick+0.5*wid3f),X);
  ModelSupport::buildPlane(SMap,buildIndex+472,
   Origin+X*
	(-InnerB4CThick+0.5*wid3b),X);
  };


  ModelSupport::buildPlane(SMap,buildIndex+4, Origin+X*(0.5*wid1f),X);
  ModelSupport::buildPlane(SMap,buildIndex+42, Origin+X*(0.5*wid1b),X);
  ModelSupport::buildPlane(SMap,buildIndex+41, Origin+X*(0.5*wid2+0.5*gap),X);
  ModelSupport::buildPlane(SMap,buildIndex+401, Origin+X*(0.5*wid2-0.5*gap),X);
  // ModelSupport::buildPlane(SMap,buildIndex+42, Origin+X*(0.5*wid3f),X);
  // ModelSupport::buildPlane(SMap,buildIndex+421, Origin+X*(0.5*wid3b),X);

  ModelSupport::buildPlane(SMap,buildIndex+3, Origin-X*(0.5*wid1f),X);
  ModelSupport::buildPlane(SMap,buildIndex+32, Origin-X*(0.5*wid1b),X);
  ModelSupport::buildPlane(SMap,buildIndex+31, Origin-X*(0.5*wid2+0.5*gap),X);
  ModelSupport::buildPlane(SMap,buildIndex+301, Origin-X*(0.5*wid2-0.5*gap),X);
  //  ModelSupport::buildPlane(SMap,buildIndex+32, Origin-X*(0.5*wid3f),X);
  //  ModelSupport::buildPlane(SMap,buildIndex+321, Origin-X*(0.5*wid3b),X);


  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*hup,Z);

    ModelSupport::buildPlane(SMap,buildIndex+5,Origin,Z);
    ModelSupport::buildPlane(SMap,buildIndex+15,Origin+Z*gapV,Z);
    int LIF(0),LIB(0);
    for (int i=0; i<nLayers; i++){
      LIF+=10;
      LIB+=100;
      ModelSupport::buildPlane(SMap,buildIndex+LIF+4,
		       Origin+X*(0.5*wid1f)-X*((i+1)*0.5*(wid1f-wid3f)/nLayers),X);
      ModelSupport::buildPlane(SMap,buildIndex+LIF+3,
		       Origin-X*(0.5*wid1f)+X*((i+1)*0.5*(wid1f-wid3f)/nLayers),X);
      ModelSupport::buildPlane(SMap,buildIndex+LIB+42,
		       Origin+X*(0.5*wid1b)-X*((i+1)*0.5*(wid1b-wid3b)/nLayers),X);
      ModelSupport::buildPlane(SMap,buildIndex+LIB+32,
		       Origin-X*(0.5*wid1b)+X*((i+1)*0.5*(wid1b-wid3b)/nLayers),X);
      //  std::cout << buildIndex+10*LI+4 << std::endl;
      //  std::cout << buildIndex+10*LI+3 << std::endl;
    };



    
    //    ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(hup+hlow),Z);

    if (wid3bSteel>0){

      ModelSupport::buildPlane(SMap,buildIndex+4299,
		       Origin+X*(0.5*wid3bSteel),X);
      ModelSupport::buildPlane(SMap,buildIndex+3299,
		       Origin-X*(0.5*wid3bSteel),X);

    }
  
  return;
}

void
D03BaseBlock::createObjects(Simulation& System)
  /*!
    Adds the block with the gap
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("D03BaseBlock","createObjects");

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
  // Loop over all layers:


  int LIF(buildIndex),LIB(buildIndex);
    for (int i=0; i<nLayers; i++){
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,"-11 ( -111 : 301 )  -6 15 3M -13M   ");
      //     std::cout << "Objexts" << i << Out +frontStr<<std::endl;
      makeCell("WallFrontL",System,cellIndex++,defMatf,0.0,Out+frontStr);
      

      Out=ModelSupport::getComposite(SMap,buildIndex,LIF," -11 ( -111 : -401 ) -6 15  -4M 14M");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallFrontR",System,cellIndex++,defMatf,0.0,Out+frontStr);


if ((wid3bSteel>0)&&(wid1b-i*(wid1b-wid3b)/nLayers > wid3bSteel)&&(wid1b-(i+1)*(wid1b-wid3b)/nLayers < wid3bSteel)){
 Out=ModelSupport::getComposite(SMap,buildIndex,LIB,"211 ( 21 : -31 )  -6 15 32M -132M -3299");
      //     std::cout << "Objexts" << i << Out +frontStr<<std::endl;
      makeCell("WallBackL",System,cellIndex++,defMatb,0.0,Out+backStr);
      

 Out=ModelSupport::getComposite(SMap,buildIndex,LIB,
				" 211 (21 : 41 )  -6 15  -42M 142M 4299");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallBackR",System,cellIndex++,defMatb,0.0,Out+backStr);



Out=ModelSupport::getComposite(SMap,buildIndex,LIB,"211 ( 21 : -31 )  -6 15 32M -132M 3299");
      //     std::cout << "Objexts" << i << Out +frontStr<<std::endl;
      makeCell("WallBackL",System,cellIndex++,151,0.0,Out+backStr);
      

 Out=ModelSupport::getComposite(SMap,buildIndex,LIB,
				" 211 (21 : 41 )  -6 15  -42M 142M -4299");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallBackR",System,cellIndex++,151,0.0,Out+backStr);

      
      } else       if ((wid3bSteel>0)&&(wid1b-(i+1)*(wid1b-wid3b)/nLayers < wid3bSteel)){
       
 Out=ModelSupport::getComposite(SMap,buildIndex,LIB,"211 ( 21 : -31 )  -6 15 32M -132M");
      //     std::cout << "Objexts" << i << Out +frontStr<<std::endl;
      makeCell("WallBackL",System,cellIndex++,151,0.0,Out+backStr);
      

 Out=ModelSupport::getComposite(SMap,buildIndex,LIB,
				" 211 (21 : 41 )  -6 15  -42M 142M");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallBackR",System,cellIndex++,151,0.0,Out+backStr);
      }
      else {
       Out=ModelSupport::getComposite(SMap,buildIndex,LIB,"211 ( 21 : -31 )  -6 15 32M -132M ");
      //     std::cout << "Objexts" << i << Out +frontStr<<std::endl;
      makeCell("WallBackL",System,cellIndex++,defMatb,0.0,Out+backStr);
      

 Out=ModelSupport::getComposite(SMap,buildIndex,LIB,
				" 211 (21 : 41 )  -6 15  -42M 142M");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallBackR",System,cellIndex++,defMatb,0.0,Out+backStr);
}

      LIF+=10; LIB+=100;
    };
    //Voids

   int isurf3, isurf4;
  
  if (wid1f>wid1b){ isurf3=3; isurf4=4;}
  else{ isurf3=32; isurf4=42;};

  //gap below
Out=ModelSupport::getComposite(SMap,buildIndex,
  std::to_string(isurf3)+" -"+std::to_string(isurf4)+" 5 -15 ")+frontStr+backStr;
      makeCell("VoidLow",System,cellIndex++,gapMat,0.0,Out+backStr);
  //Void front block
      if (InnerB4CThick>0){
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "-11   -6 15 371 -471   ");
      makeCell("VoidF",System,cellIndex++,gapMat,0.0,Out+frontStr);

      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "-11   -6 15 3M -371   ");
      makeCell("B4CF",System,cellIndex++,B4CMatF,0.0,Out+frontStr);

      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "-11   -6 15 471 -4M   ");
      makeCell("B4CF",System,cellIndex++,B4CMatF,0.0,Out+frontStr);

      Out=ModelSupport::getComposite(SMap,buildIndex,LIB,
	      " 21 -6 15  -472 372");
      makeCell("VoidB",System,cellIndex++,gapMat,0.0,Out+backStr);

      Out=ModelSupport::getComposite(SMap,buildIndex,LIB,
	      " 21 -6 15  -372 32M");
      makeCell("B4CB",System,cellIndex++,B4CMatB,0.0,Out+backStr);

      Out=ModelSupport::getComposite(SMap,buildIndex,LIB,
	      " 21 -6 15  -42M 472");
      makeCell("B4CB",System,cellIndex++,B4CMatB,0.0,Out+backStr);

      }else{
    Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "-11   -6 15 3M -4M   ");
       makeCell("VoidF",System,cellIndex++,gapMat,0.0,Out+frontStr);

      Out=ModelSupport::getComposite(SMap,buildIndex,LIB,
	      " 21 -6 15  -42M 32M");
      makeCell("VoidB",System,cellIndex++,gapMat,0.0,Out+backStr);
      };

       Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "11 -21   -6 15 3M -4M   ");
       if(gap>0)      makeCell("VoidGapC",System,cellIndex++,gapMat,0.0,Out);
    

      //Gaps between blocks
      //innermost
      if(gap>0){
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "11 -21  -6 15 -3M 31   ");
      makeCell("VoidGap",System,cellIndex++,gapMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "11 -21  -6 15 -41 4M   ");
      makeCell("VoidGap",System,cellIndex++,gapMat,0.0,Out);

      //mid
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "211 -11  -6 15 -301 31   ");
      makeCell("VoidGap",System,cellIndex++,gapMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "211 -11  -6 15 -41 401   ");
      makeCell("VoidGap",System,cellIndex++,gapMat,0.0,Out);

     //outermost
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
				     "111 -211  -6 15  -301 "+std::to_string(isurf3));
      makeCell("VoidGap",System,cellIndex++,gapMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "111 -211  -6 15 401 -"+std::to_string(isurf4));
      makeCell("VoidGap",System,cellIndex++,gapMat,0.0,Out);
      }
  // Outer
      // voids  

  if (wid1f>wid1b){
     Out=ModelSupport::getComposite(SMap,buildIndex,
	      " 211 -6 15 4 -42");
      makeCell("VoidOuter",System,cellIndex++,gapMat,0.0,Out+backStr);

      Out=ModelSupport::getComposite(SMap,buildIndex,
	      " 211 -6 15 32 -3");
      makeCell("VoidOuter",System,cellIndex++,gapMat,0.0,Out+backStr);
      }
  else if (wid1f<wid1b) {     Out=ModelSupport::getComposite(SMap,buildIndex,
	      " -111 -6 15 4 -42");
      makeCell("VoidOuter",System,cellIndex++,gapMat,0.0,Out+frontStr);

      Out=ModelSupport::getComposite(SMap,buildIndex,
	      " -111 -6 15 32 -3");
      makeCell("VoidOuter",System,cellIndex++,gapMat,0.0,Out+frontStr);
 };

      Out=ModelSupport::getComposite(SMap,buildIndex,
  std::to_string(isurf3)+" -"+std::to_string(isurf4)+" 5 -6 ")+frontStr+backStr;
  addOuterSurf(Out);

  return;
}

void
D03BaseBlock::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("D03BaseBlock","createLinks");
  double length=l1edge+l2edge;

  FixedComp::setConnect(4,Origin,-Z);
  FixedComp::setConnect(5,Origin+Z*hup,Z);


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

  //  ModelSupport::buildPlane(SMap,buildIndex+21,Origin-Y*(length/2.0-l1mid-0.5*gap),Y);
      FixedComp::setConnect(6,Origin-Y*(length/2.0-l1mid-0.5*gap),-Y);
      FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+21));      

      FixedComp::setConnect(7,Origin-Y*(length/2.0-l1mid+0.5*gap),Y);
      FixedComp::setLinkSurf(7,-SMap.realSurf(buildIndex+11));

  
  double wid1; int isurf3, isurf4;
  
  if (wid1f>wid1b){wid1=wid1f; isurf3=3; isurf4=4;}
  else{wid1=wid1b; isurf3=32; isurf4=42;};
    FixedComp::setConnect(2,Origin-X*0.5*wid1,-X);
    FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+isurf3));

   FixedComp::setConnect(3,Origin+X*0.5*wid1,X);
   FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+isurf4));

  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));



  return;
}
  
void
D03BaseBlock::setFront(const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Set front surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("D03BaseBlock","setFront");
  
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
D03BaseBlock::setBack(const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Set back surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("D03BaseBlock","setBack");
  
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
D03BaseBlock::getXSectionIn() const
  /*!
    Get the line shield inner void section
    \return HeadRule of cross-section
   */
{
  ELog::RegMethod RegA("D03BaseBlock","getXSectionIn");
  const std::string Out=
    ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 ");
  HeadRule HR(Out);
  HR.populateSurf();
  return HR;
}
  
    
void
D03BaseBlock::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("D03BaseBlock","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);  
    createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
