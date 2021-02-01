/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/D03UpperBlock.cxx
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

#include "D03UpperBlock.h"

namespace constructSystem
{

D03UpperBlock::D03UpperBlock(const std::string& Key) : 
  attachSystem::FixedOffset(Key,10),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  //  buildIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  // cellIndex(buildIndex+1),
  activeFront(0),activeBack(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

D03UpperBlock::D03UpperBlock(const D03UpperBlock& A) : 
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
  wid3b(A.wid3b),hupB(A.hupB),hlowB(A.hlowB),hmid(A.hmid),
  hupF(A.hupF),hlowF(A.hlowF),
  defMatb(A.defMatb), gapV(A.gapV),
  defMatf(A.defMatf), gapMat(A.gapMat),nSeg(A.nSeg),
  nSegF(A.nSegF),  nSegB(A.nSegB),nLayers(A.nLayers)
  /*!
    Copy constructor
    \param A :: D03UpperBlock to copy
  */
{}

D03UpperBlock&
D03UpperBlock::operator=(const D03UpperBlock& A)
  /*!
    Assignment operator
    \param A :: D03UpperBlock to copy
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
    hupB=A.hupB;hlowB=A.hlowB; hmid=A.hmid;
    hupF=A.hupF;hlowF=A.hlowF; gapV=A.gapV;
    defMatf=A.defMatf;  defMatb=A.defMatb; gapMat=A.gapMat;nSeg=A.nSeg;
      nSegF=A.nSegF;nSegB=A.nSegB;
    nLayers=A.nLayers;
    }
  return *this;
}

D03UpperBlock::~D03UpperBlock() 
  /*!
    Destructor
  */
{}

void
D03UpperBlock::removeFrontOverLap()
  /*!
    Remove segments that are completly covered by the
    active front.
  */
{
  ELog::RegMethod RegA("D03UpperBlock","removeFrontOverLap");
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
D03UpperBlock::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("D03UpperBlock","populate");
  
  FixedOffset::populate(Control);

  //Total block length on sides: 4150mm and 3850mm
  // Gap in the drawings 10 mm, make 20 mm
  // Void + Fe special:
  l1edge=Control.EvalDefVar<double>(keyName+"Length1Edge",42.5);
  l2edge=Control.EvalDefVar<double>(keyName+"Length2Edge",57.5);
  l1mid=Control.EvalDefVar<double>(keyName+"Length1Mid",57.5);
  l2mid=Control.EvalDefVar<double>(keyName+"Length2Mid",42.5);
  length1low=Control.EvalDefVar<double>(keyName+"Length1Low",40);
  length2low=Control.EvalDefVar<double>(keyName+"Length2Low",40);
  gap=Control.EvalDefVar<double>(keyName+"Gap",1.2);
  gapV=Control.EvalDefVar<double>(keyName+"GapV",0.5);

  l2mid = l1edge+l2edge-l1mid;
    
  wid1f=Control.EvalDefVar<double>(keyName+"Width1Front",148);
  wid1b=Control.EvalDefVar<double>(keyName+"Width1Back",148);
  wid2=Control.EvalDefVar<double>(keyName+"Width2",94.5);
  wid3f=Control.EvalDefVar<double>(keyName+"Width3Front",68);
  wid3b=Control.EvalDefVar<double>(keyName+"Width3Back",68);
  //Checking innermost widths for consistency
  if (wid3f>(wid2+gap)){
    std::cout << "Warning from UpperBlock populate:"
	      << std::endl << "too small inner width forward part" <<
              std::endl<< "--> making the gap flush with it"
	      <<std::endl;
    //    wid3f=(wid2+gap);
    wid2=(wid3f-gap);
    }
  if (wid3b>(wid2+gap)){
    std::cout << "Warning form UpperBlock populate:"
	      << std::endl << "too small inner width rear part" <<
              std::endl<< "--> making the gap flush"
	      <<std::endl;
    //  wid3b=(wid2+gap);
      wid2=(wid3b-gap);
  }
 
  hupF=Control.EvalDefVar<double>(keyName+"HeightUpFront",86.5);
  hlowF=Control.EvalDefVar<double>(keyName+"HeightLowFront",46.5);
  hmid=Control.EvalDefVar<double>(keyName+"HeightMid",60.6);
  hupB=Control.EvalDefVar<double>(keyName+"HeightUpBack",86.5);
  hlowB=Control.EvalDefVar<double>(keyName+"HeightLowBack",46.5);


  if (hlowF>(hmid+0.5*gap)){
    std::cout << "Warning form UpperBlock populate:"
	      << std::endl << "too small inner height forward part" <<
              std::endl<< "--> making the gap flush"
	      <<std::endl;
    hmid=(hlowF-0.5*gap);}
  if (hlowB>(hmid+0.5*gap)){
    std::cout << "Warning form UpperBlock populate:"
	      << std::endl << "too small inner height rear part" <<
              std::endl<< "--> making the gap flush"
	      <<std::endl;
    hmid=(hlowB-0.5*gap);}

  
  defMatf=ModelSupport::EvalDefMat<int>(Control,keyName+"DefMatF",144);
  defMatb=ModelSupport::EvalDefMat<int>(Control,keyName+"DefMatB",144);
  gapMat=ModelSupport::EvalDefMat<int>(Control,keyName+"GapMat",60);
 
  //  int B4CMat;

  nSeg=Control.EvalDefVar<size_t>(keyName+"NSeg",1);
  nSegF=Control.EvalDefVar<size_t>(keyName+"NSegF",nSeg);
  nSegB=Control.EvalDefVar<size_t>(keyName+"NSegB",nSeg);

  nLayers=Control.EvalDefVar<size_t>(keyName+"NLayers",4);

  int iLayerFlag=Control.EvalDefVar<int>("LayerFlag",1);

  if(iLayerFlag==0){
  nSeg=1;
    nSegB=1;
  nSegF=1;
  nLayers=1;
  }
 
  // Cable tube
  return;
}

void
D03UpperBlock::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("D03UpperBlock","createUnitVector");


  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  double length=l1edge+l2edge;
  Origin+=Y*(length/2.0);
  return;
}

void
D03UpperBlock::createSurfaces()
  /*!
    Create the surfaces. Note that layers is not used
    because we want to break up the objects into sub components
  */
{
  ELog::RegMethod RegA("D03UpperBlock","createSurfaces");

  double length=l1edge+l2edge;

  // Inner void
  if (!activeFront)
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
    
  if (!activeBack)
    ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);


      
    ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(length/2.0-l1mid+0.5*gap),Y);
    //Front block index
    int SIF(113);
    for (int i=1; i<nSegF; i++){
      ModelSupport::buildPlane(SMap,buildIndex+1+SIF,
			       Origin-Y*(length/2.0)+Y*(l1edge*i/nSegF),Y);
      //      std::cout << buildIndex+1+SIF << std::endl;
      SIF+=113;
    }
  ModelSupport::buildPlane(SMap,buildIndex+111,Origin-Y*(length/2.0-l1edge+0.5*gap),Y);
   //ModelSupport::buildPlane(SMap,buildIndex+12,Origin-Y*(length/2.0-length1low),Y);


  ModelSupport::buildPlane(SMap,buildIndex+21,Origin-Y*(length/2.0-l1mid-0.5*gap),Y);
  //Back block index
    int SIB(113);
    for (int i=1; i<nSegB; i++){
      ModelSupport::buildPlane(SMap,buildIndex+2+SIB,
			       Origin+Y*(length/2.0)-Y*(l2mid*i/nSegB),Y);
      SIB+=113;
    }
  ModelSupport::buildPlane(SMap,buildIndex+211,Origin-Y*(length/2.0-l1edge-0.5*gap),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*(length/2.0-length2low),Y);
 


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


    ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*hupF,Z);
    ModelSupport::buildPlane(SMap,buildIndex+62,Origin+Z*hupB,Z);
    ModelSupport::buildPlane(SMap,buildIndex+61,Origin+Z*(hmid+0.5*gap),Z);
    ModelSupport::buildPlane(SMap,buildIndex+601,Origin+Z*(hmid-0.5*gap),Z);

    ModelSupport::buildPlane(SMap,buildIndex+5,Origin,Z);
    //  ModelSupport::buildPlane(SMap,buildIndex+50,Origin-Z*10,Z);
    ModelSupport::buildPlane(SMap,buildIndex+15,Origin+Z*gapV,Z);
    int LIF(0),LIB(0),LIT(0),LITB(0);
    for (int i=0; i<nLayers; i++){
      LIF+=10;
      LIT+=10;
      LIB+=100;
      LITB+=100;
      ModelSupport::buildPlane(SMap,buildIndex+LIT+6,
			       Origin+Z*hupF-Z*((i+1)*(hupF-hlowF)/nLayers),Z);
      ModelSupport::buildPlane(SMap,buildIndex+LITB+62,
			       Origin+Z*hupB-Z*((i+1)*(hupB-hlowB)/nLayers),Z);
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


  
  return;
}

void
D03UpperBlock::createObjects(Simulation& System)
  /*!
    Adds the block with the gap
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("D03UpperBlock","createObjects");

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


  int LIF(buildIndex),LIB(buildIndex),LIT(buildIndex),LITB(buildIndex);
    for (int i=0; i<nLayers; i++){

      int SIF(buildIndex);
      int SIB(buildIndex);

      if((!activeFront)&&(!activeBack)){
    
	for (int i=1; i<nSegF; i++){
	  //      std::cout << ModelSupport::getComposite(SMap,SIF," 1 -114 ") << std::endl;
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,LIT,
	     " -6N 15 3M -13M ")+
      ModelSupport::getComposite(SMap,SIF," 1 -114 ");
      //     std::cout << "Objexts" << i << Out +frontStr<<std::endl;

      makeCell("WallFrontL",System,cellIndex++,defMatf,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
	     "  -6N 15  -4M 14M")+
		ModelSupport::getComposite(SMap,SIF," 1 -114  ");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallFrontR",System,cellIndex++,defMatf,0.0,Out);


      
      Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
	     " -6N 16N  -14M 13M ")+
		ModelSupport::getComposite(SMap,SIF," 1  -114 ");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallFrontT",System,cellIndex++,defMatf,0.0,Out);

      
      SIF+=113;
	}

      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,LIT,
	     "-11 ( -111 : 301 -601 )  -6N 15 3M -13M ")+
	ModelSupport::getComposite(SMap,SIF," 1  ");
      //     std::cout << "Objexts" << i << Out +frontStr<<std::endl;
      makeCell("WallFrontL",System,cellIndex++,defMatf,0.0,Out);


      
      Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
	     " -11 ( -111 : -401 -601 ) -6N 15  -4M 14M")+
		ModelSupport::getComposite(SMap,SIF," 1  ");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallFrontR",System,cellIndex++,defMatf,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
	     " -11 ( -111 : -601 -401 301 ) -6N 16N  -14M 13M ")+
		ModelSupport::getComposite(SMap,SIF," 1  ");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallFrontT",System,cellIndex++,defMatf,0.0,Out);



	for (int i=1; i<nSegB; i++){
      Out=ModelSupport::getComposite(SMap,buildIndex,LIB,LITB,
				  "  -62N 15 32M -132M ")+
			ModelSupport::getComposite(SMap,SIB," -2  115 ");
      //     std::cout << "Objexts" << i << Out +frontStr<<std::endl;
      makeCell("WallBackL",System,cellIndex++,defMatb,0.0,Out+backStr);
      

      Out=ModelSupport::getComposite(SMap,buildIndex,LIB,LITB,
				" -62N 15  -42M 142M")+
			ModelSupport::getComposite(SMap,SIB," -2  115 ");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallBackR",System,cellIndex++,defMatb,0.0,Out+backStr);

      Out=ModelSupport::getComposite(SMap,buildIndex,LIB,LITB,
				"  -62N 162N  132M -142M")+
			ModelSupport::getComposite(SMap,SIB," -2  115 ");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallBackT",System,cellIndex++,defMatb,0.0,Out+backStr);

        SIB+=113;
	}

      Out=ModelSupport::getComposite(SMap,buildIndex,LIB,LITB,
				  "211 ( 21 : -31 : 61 )  -62N 15 32M -132M ")+
				ModelSupport::getComposite(SMap,SIB," -2 ");
      //     std::cout << "Objexts" << i << Out +frontStr<<std::endl;
      makeCell("WallBackL",System,cellIndex++,defMatb,0.0,Out);
      

      Out=ModelSupport::getComposite(SMap,buildIndex,LIB,LITB,
				" 211 (21 : 41 : 61 )  -62N 15  -42M 142M")+
				ModelSupport::getComposite(SMap,SIB," -2  ");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallBackR",System,cellIndex++,defMatb,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex,LIB,LITB,
				" 211 (21 : 61 : -31 : 41 )  -62N 162N  132M -142M")+
				ModelSupport::getComposite(SMap,SIB," -2 ");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallBackT",System,cellIndex++,defMatb,0.0,Out);



      }else{


      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,LIT,
	     "-11 ( -111 : 301 -601 )  -6N 15 3M -13M ");
      //     std::cout << "Objexts" << i << Out +frontStr<<std::endl;
      makeCell("WallFrontL",System,cellIndex++,defMatf,0.0,Out+frontStr);
      

      Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
	     " -11 ( -111 : -401 -601 ) -6N 15  -4M 14M");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallFrontR",System,cellIndex++,defMatf,0.0,Out+frontStr);

      Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
	     " -11 ( -111 : -601 -401 301 ) -6N 16N  -14M 13M ");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallFrontT",System,cellIndex++,defMatf,0.0,Out+frontStr);

      

      Out=ModelSupport::getComposite(SMap,buildIndex,LIB,LITB,
				  "211 ( 21 : -31 : 61 )  -62N 15 32M -132M ");
      //     std::cout << "Objexts" << i << Out +frontStr<<std::endl;
      makeCell("WallBackL",System,cellIndex++,defMatb,0.0,Out+backStr);
      

      Out=ModelSupport::getComposite(SMap,buildIndex,LIB,LITB,
				" 211 (21 : 41 : 61 )  -62N 15  -42M 142M");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallBackR",System,cellIndex++,defMatb,0.0,Out+backStr);

      Out=ModelSupport::getComposite(SMap,buildIndex,LIB,LITB,
				" 211 (21 : 61 : -31 : 41 )  -62N 162N  132M -142M");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallBackT",System,cellIndex++,defMatb,0.0,Out+backStr);

	}
	

      LIF+=10; LIB+=100; LIT+=10; LITB+=100;
    };
    //Voids

    int isurf3, isurf4, isurf6;
  
  if (wid1f>wid1b){ isurf3=3; isurf4=4;}
  else{ isurf3=32; isurf4=42;};
  if (hupF>hupB){ isurf6=6; }
  else{ isurf6=62;};

  //gap below
  Out=ModelSupport::getComposite(SMap,buildIndex,LIF,
 std::to_string(isurf3)+" -"+std::to_string(isurf4)+" 5 -15 (-3M : 4M) -11 ")+frontStr;
      makeCell("VoidLowF",System,cellIndex++,gapMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex,LIB,
 std::to_string(isurf3)+" -"+std::to_string(isurf4)+" 5 -15 (-32M : 42M) 21 ")+backStr;
      makeCell("VoidLowB",System,cellIndex++,gapMat,0.0,Out);

       Out=ModelSupport::getComposite(SMap,buildIndex,LIF,
  std::to_string(isurf3)+" -"+std::to_string(isurf4)+" 5 -15 -21 11 (-3M : 4M) ");
      makeCell("VoidLowG",System,cellIndex++,gapMat,0.0,Out);

  //Void front block
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,LIT,
	      "-11 -6N 5 3M -4M   ");
       makeCell("VoidF",System,cellIndex++,gapMat,0.0,Out+frontStr);

       Out=ModelSupport::getComposite(SMap,buildIndex, LIF,LIT,
	      "11 -21   -6N 5 3M -4M   ");
       makeCell("VoidGapC",System,cellIndex++,gapMat,0.0,Out);
    
       Out=ModelSupport::getComposite(SMap,buildIndex,LIB,LITB,
	      " 21 -62N 5  -42M 32M");
      makeCell("VoidB",System,cellIndex++,gapMat,0.0,Out+backStr);
      
      //Gaps between blocks
      //innermost
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "11 -21  -61 15 -3M 31   ");
      makeCell("VoidGapIF",System,cellIndex++,gapMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "11 -21  -61 15 -41 4M   ");
      makeCell("VoidGapIF",System,cellIndex++,gapMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,LIT,
	      "11 -21  -61 6N 15 3M -4M ");
      makeCell("VoidGapIF",System,cellIndex++,gapMat,0.0,Out);
      
      //mid
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "211 -11  -61 15 -301 31   ");
      makeCell("VoidGap",System,cellIndex++,gapMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "211 -11  -61 15 -41 401   ");
      makeCell("VoidGap",System,cellIndex++,gapMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "211 -11  601 -61 -401 301   ");
      makeCell("VoidGap",System,cellIndex++,gapMat,0.0,Out);

     //outermost
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "111 -211  15 3 -301  -"+std::to_string(isurf6));
      makeCell("VoidGap",System,cellIndex++,gapMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "111 -211 15 401 -4  -"+std::to_string(isurf6));
      makeCell("VoidGap",System,cellIndex++,gapMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      "111 -211  601 -401 301  -"+std::to_string(isurf6));
      makeCell("VoidGap",System,cellIndex++,gapMat,0.0,Out);
      
  // Outer

  if (wid1f>wid1b){
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      " 111   15 3 -32 -6 ")+backStr;
      makeCell("VoidOuter",System,cellIndex++,gapMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      " 111   -6 15 42 -4  ")+backStr;
      makeCell("VoidOuter",System,cellIndex++,gapMat,0.0,Out);}
  else if (wid1f<wid1b){      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      " -211 15 -3 32 -62 ")+frontStr;
      makeCell("VoidOuter",System,cellIndex++,gapMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      " -211 -62 15 -42 4  ")+frontStr;
      makeCell("VoidOuter",System,cellIndex++,gapMat,0.0,Out);
      };
  if (hupF>hupB){  Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      " 211 -6 -42 32 62 ")+backStr;
       makeCell("VoidOuter",System,cellIndex++,gapMat,0.0,Out);
  }
  else if (hupF<hupB){  Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      " -111 6 -4 3 -62 ")+frontStr;
      makeCell("VoidOuter",System,cellIndex++,gapMat,0.0,Out);
 };

      
  Out=ModelSupport::getComposite(SMap,buildIndex,
		  std::to_string(isurf3)+
		  " -"+std::to_string(isurf4)+
				 " 5 -"+std::to_string(isurf6)+" ")+frontStr+backStr;
  /* " : "+ModelSupport::getComposite(SMap,buildIndex,
		  std::to_string(isurf3)+
		  " -"+std::to_string(isurf4)+
				 " 5 -"+std::to_string(isurf6)+" ")+frontStr+backStr;
  */
  addOuterSurf(Out);

  return;
}

void
D03UpperBlock::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("D03UpperBlock","createLinks");
  double length=l1edge+l2edge;

  FixedComp::setConnect(4,Origin,-Z);


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
  //Definition of surface 21
  //  ModelSupport::buildPlane(SMap,buildIndex+21,Origin-Y*(length/2.0-l1mid-0.5*gap),Y);

     FixedComp::setConnect(6,Origin-Y*(length/2.0-l1mid-0.5*gap),-Y);
      FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+21));      

      FixedComp::setConnect(7,Origin-Y*(length/2.0-l1mid+0.5*gap),Y);
      FixedComp::setLinkSurf(7,-SMap.realSurf(buildIndex+11));


     FixedComp::setConnect(8,Origin-Y*(length/2.0-l1edge-0.5*gap),-Y);
      FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+211));      

      FixedComp::setConnect(9,Origin-Y*(length/2.0-l1edge+0.5*gap),Y);
      FixedComp::setLinkSurf(9,-SMap.realSurf(buildIndex+111));

      
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

  double wid1,hup; int isurf3, isurf4,isurf6;
  if (hupF>hupB){ isurf6=6; hup=hupF; }
  else{ isurf6=62; hup=hupB;};
  
  if (wid1f>wid1b){wid1=wid1f; isurf3=3; isurf4=4;}
  else{wid1=wid1b; isurf3=32; isurf4=42;};
    FixedComp::setConnect(2,Origin-X*0.5*wid1,-X);
    FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+isurf3));

   FixedComp::setConnect(3,Origin+X*0.5*wid1,X);
   FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+isurf4));

  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*hup,Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+isurf6));



  return;
}
  
void
D03UpperBlock::setFront(const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Set front surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("D03UpperBlock","setFront");
  
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
D03UpperBlock::setBack(const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Set back surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("D03UpperBlock","setBack");
  
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
D03UpperBlock::getXSectionIn() const
  /*!
    Get the line shield inner void section
    \return HeadRule of cross-section
   */
{
  ELog::RegMethod RegA("D03UpperBlock","getXSectionIn");
  const std::string Out=
    ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 ");
  HeadRule HR(Out);
  HR.populateSurf();
  return HR;
}
  
    
void
D03UpperBlock::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("D03UpperBlock","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);  
    createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
