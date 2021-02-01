/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Model/ESSBeam/bifrost/E02PillarCover.cxx
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

#include "E02PillarCover.h"

namespace constructSystem
{

E02PillarCover::E02PillarCover(const std::string& Key) : 
  attachSystem::FixedOffset(Key,8),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  //  buildIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  // cellIndex(buildIndex+1),
  activeFront(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

E02PillarCover::E02PillarCover(const E02PillarCover& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  //  buildIndex(A.buildIndex),cellIndex(A.cellIndex),
  activeFront(A.activeFront),
  frontSurf(A.frontSurf),frontCut(A.frontCut),
  backSurf(A.backSurf),backCut(A.backCut),
  //
  length(A.length),voidLength(A.voidLength),l2mid(A.l2mid),l2edge(A.l2edge),gap(A.gap),
    length1low(A.length1low),length2low(A.length2low),
    wid1b(A.wid1b), wid1f(A.wid1f),wid2(A.wid2),wid3f(A.wid3f),
  wid3b(A.wid3b),hupB(A.hupB),hlowB(A.hlowB),hmid(A.hmid),
  height(A.height),voidHeight(A.voidHeight),
  defMatb(A.defMatb), gapV(A.gapV),
  defMatf(A.defMatf), gapMat(A.gapMat),voidMat(A.voidMat),
  nSeg(A.nSeg),nLayers(A.nLayers)
  /*!
    Copy constructor
    \param A :: E02PillarCover to copy
  */
{}

E02PillarCover&
E02PillarCover::operator=(const E02PillarCover& A)
  /*!
    Assignment operator
    \param A :: E02PillarCover to copy
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
      frontSurf=A.frontSurf;
      frontCut=A.frontCut;
      backSurf=A.backSurf;
      backCut=A.backCut;

  length=A.length;voidLength=A.voidLength;l2mid=A.l2mid;l2edge=A.l2edge;gap=A.gap;
    length1low=A.length1low;length2low=A.length2low;
    wid1b=A.wid1b; wid1f=A.wid1f;wid2=A.wid2;wid3f=A.wid3f;wid3b=A.wid3b;
    hupB=A.hupB;hlowB=A.hlowB; hmid=A.hmid;
    height=A.height;voidHeight=A.voidHeight; gapV=A.gapV;
    defMatf=A.defMatf;  defMatb=A.defMatb;
    voidMat=A.voidMat; gapMat=A.gapMat;nSeg=A.nSeg;nLayers=A.nLayers;
    }
  return *this;
}

E02PillarCover::~E02PillarCover() 
  /*!
    Destructor
  */
{}

void
E02PillarCover::removeFrontOverLap()
  /*!
    Remove segments that are completly covered by the
    active front.
  */
{
  ELog::RegMethod RegA("E02PillarCover","removeFrontOverLap");
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
E02PillarCover::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("E02PillarCover","populate");
  
  FixedOffset::populate(Control);

  //Total block length on sides: 4150mm and 3850mm
  // Gap in the drawings 10 mm, make 20 mm
  // Void + Fe special:
  length=Control.EvalDefVar<double>(keyName+"Length",57);
  voidLength=Control.EvalDefVar<double>(keyName+"VoidLength",22);
  height=Control.EvalDefVar<double>(keyName+"Height",107);
  voidHeight=Control.EvalDefVar<double>(keyName+"VoidHeight",70);

  wid1f=Control.EvalDefVar<double>(keyName+"Width",118);
  wid3f=Control.EvalDefVar<double>(keyName+"VoidWidth",56);

  left=Control.EvalDefVar<double>(keyName+"Left",59);
  voidLeft=Control.EvalDefVar<double>(keyName+"VoidLeft",28);


  right=Control.EvalDefVar<double>(keyName+"Right",59);
  voidRight=Control.EvalDefVar<double>(keyName+"VoidRight",28);
  
  

  gap=Control.EvalDefVar<double>(keyName+"Gap",1.2);
  gapV=Control.EvalDefVar<double>(keyName+"GapV",0.2);

  
  defMatf=ModelSupport::EvalDefMat<int>(Control,keyName+"DefMatF",144);
  defMatb=ModelSupport::EvalDefMat<int>(Control,keyName+"DefMatB",144);
  gapMat=ModelSupport::EvalDefMat<int>(Control,keyName+"GapMat",60);
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",60);
 
  //  int B4CMat;

  nSeg=Control.EvalDefVar<size_t>(keyName+"NSeg",2);

  nLayers=Control.EvalDefVar<size_t>(keyName+"NLayers",3);

  int iLayerFlag=Control.EvalDefVar<int>("LayerFlag",1);

  if(iLayerFlag==0){
  nSeg=1;
  nLayers=1;
  }

  iTubes=Control.EvalDefVar<size_t>(keyName+"ITubes",0);
  iChicane=Control.EvalDefVar<size_t>(keyName+"IChicane",0);
  if (iChicane) left=59.0+25.0;
  
  // Cable tube
  return;
}

void
E02PillarCover::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("E02PillarCover","createUnitVector");


  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  //  double length=length+l2edge;
  Origin+=Y*(length/2.0);
  return;
}

void
E02PillarCover::createSurfaces()
  /*!
    Create the surfaces. Note that layers is not used
    because we want to break up the objects into sub components
  */
{
  ELog::RegMethod RegA("E02PillarCover","createSurfaces");

  //  double length=length+l2edge;

  // Inner void
  if (!activeFront)
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0-gap),Y);

    ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(length/2.0),Y);

    ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);

    ModelSupport::buildPlane(SMap,buildIndex+1002,Origin+Y*(length/2.0-15),Y);

    ModelSupport::buildPlane(SMap,buildIndex+2002,Origin+Y*(length/2.0+15)
			     -Y*(length-voidLength),Y);


      
    //Front block index
    int SIF(113);
    for (int i=1; i<nSeg; i++){
      ModelSupport::buildPlane(SMap,buildIndex+1+SIF,
			       Origin-Y*(length/2.0-gap)+Y*(voidLength*i/nSeg),Y);
      //      std::cout << buildIndex+1+SIF << std::endl;
      SIF+=113;
    }
    /*
  ModelSupport::buildPlane(SMap,buildIndex+111,Origin-Y*(length/2.0-length+0.5*gap),Y);
   //ModelSupport::buildPlane(SMap,buildIndex+12,Origin-Y*(length/2.0-length1low),Y);


  ModelSupport::buildPlane(SMap,buildIndex+21,Origin-Y*(length/2.0-voidLength-0.5*gap),Y);
  //Back block index
    int SIB(113);
    for (int i=1; i<nSeg; i++){
      ModelSupport::buildPlane(SMap,buildIndex+2+SIB,
			       Origin+Y*(length/2.0)-Y*(l2mid*i/nSeg),Y);
      SIB+=113;
    }
  ModelSupport::buildPlane(SMap,buildIndex+211,Origin-Y*(length/2.0-length-0.5*gap),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*(length/2.0-length2low),Y);
    */


  ModelSupport::buildPlane(SMap,buildIndex+4, Origin+X*left,X);
  //  ModelSupport::buildPlane(SMap,buildIndex+42, Origin+X*(0.5*wid1b),X);
  // ModelSupport::buildPlane(SMap,buildIndex+41, Origin+X*(0.5*wid2+0.5*gap),X);
  //  ModelSupport::buildPlane(SMap,buildIndex+401, Origin+X*(0.5*wid2-0.5*gap),X);
 
  ModelSupport::buildPlane(SMap,buildIndex+3, Origin-X*right,X);

  ModelSupport::buildPlane(SMap,buildIndex+1003, Origin-X*(right-15),X);
  ModelSupport::buildPlane(SMap,buildIndex+2003, Origin-X*(right+15)+X*(right-voidRight),X);

  //ModelSupport::buildPlane(SMap,buildIndex+32, Origin-X*(0.5*wid1b),X);
  // ModelSupport::buildPlane(SMap,buildIndex+31, Origin-X*(0.5*wid2+0.5*gap),X);
  // ModelSupport::buildPlane(SMap,buildIndex+301, Origin-X*(0.5*wid2-0.5*gap),X);
  

    ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);
    //   ModelSupport::buildPlane(SMap,buildIndex+62,Origin+Z*hupB,Z);
    //   ModelSupport::buildPlane(SMap,buildIndex+61,Origin+Z*(hmid+0.5*gap),Z);
    //    ModelSupport::buildPlane(SMap,buildIndex+601,Origin+Z*(hmid-0.5*gap),Z);

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
		       Origin+Z*height-Z*((i+1)*(height-voidHeight)/nLayers),Z);
      ModelSupport::buildPlane(SMap,buildIndex+LIF+4,
		       Origin+X*left-X*((i+1)*(left-voidLeft)/nLayers),X);
      ModelSupport::buildPlane(SMap,buildIndex+LIF+3,
		       Origin-X*right+X*((i+1)*(right-voidRight)/nLayers),X);

      ModelSupport::buildPlane(SMap,buildIndex+LIF+2,
	       Origin+Y*(0.5*length)-Y*((i+1)*(length-voidLength)/nLayers),Y);

      //ModelSupport::buildPlane(SMap,buildIndex+LIB+42,
      //      Origin+X*(0.5*wid1b)-X*((i+1)*0.5*(wid1b-wid3b)/nLayers),X);
      //    ModelSupport::buildPlane(SMap,buildIndex+LIB+32,
      //      Origin-X*(0.5*wid1b)+X*((i+1)*0.5*(wid1b-wid3b)/nLayers),X);
      //  std::cout << buildIndex+10*LI+4 << std::endl;
      //  std::cout << buildIndex+10*LI+3 << std::endl;
    };

    if (iChicane){
    ModelSupport::buildPlane(SMap,buildIndex+9,Origin+Z*45,Z);
    //  ModelSupport::buildPlane(SMap,buildIndex+50,Origin-Z*10,Z);
    ModelSupport::buildPlane(SMap,buildIndex+19,Origin+Z*70,Z);
    }

    if (iTubes>0){
    ModelSupport::buildCylinder(SMap,buildIndex+7,
		       Origin-X*140+Z*20+Y*0.5*length,Y,10);

        ModelSupport::buildCylinder(SMap,buildIndex+79,
				    Origin-X*140+Z*20+Y*0.5*length,Y,9.8);

    double pipipi=4.0*atan(1.0);
    double angle0=-pipipi/6.0;
    double RBend=10;  
      ModelSupport::buildPlane(
      SMap,buildIndex+27,
      Origin-X*140+Z*20+Y*0.5*length-Y*15,
      Y*cos(angle0)+X*sin(angle0));
      //      std::cout << buildIndex+65+100*CI << std::endl;

      double angle=-pipipi/3.0;
      
      ModelSupport::buildCylinder(SMap,buildIndex+77,
      Origin-X*140+Z*20+Y*0.5*length-Y*15,
				Y*cos(angle)+X*sin(angle),10);

    
      ModelSupport::buildPlane(
      SMap,buildIndex+37,
      Origin-X*140+Z*20+Y*0.5*length-Y*15-Y*cos(angle)*72-X*sin(angle)*72,
      Y*cos(angle0)+X*sin(angle0));

      ModelSupport::buildCylinder(SMap,buildIndex+777,
      Origin-X*140+Z*20+Y*0.5*length-Y*15-Y*cos(angle)*72-X*sin(angle)*72,Y,10);

      ModelSupport::buildCylinder(SMap,buildIndex+7779,
      Origin-X*140+Z*20+Y*0.5*length-Y*15-Y*cos(angle)*72-X*sin(angle)*72,Y,9.8);

      //Chicane #2

      ModelSupport::buildCylinder(SMap,buildIndex+8,
		       Origin+Z*20+Y*21+X*left,X,10);

      ModelSupport::buildCylinder(SMap,buildIndex+89,
		       Origin+Z*20+Y*21+X*left,X,9.8);

      ModelSupport::buildPlane(
      SMap,buildIndex+28,
      Origin+Z*20+Y*21+X*right-X*20,
      X*cos(angle0)-Y*sin(angle0));
      //      std::cout << buildIndex+65+100*CI << std::endl;
      ModelSupport::buildCylinder(SMap,buildIndex+78,
      Origin+Z*20+Y*21+X*right-X*20,
			X*cos(angle)-Y*sin(angle),10);
    
      
      ModelSupport::buildPlane(
      SMap,buildIndex+38,
      Origin+Z*20+Y*21+X*right-X*20-X*cos(angle)*72+Y*sin(angle)*72,
      X*cos(angle0)-Y*sin(angle0));

      ModelSupport::buildCylinder(SMap,buildIndex+778,
      Origin+Z*20+Y*21+X*right-X*20-X*cos(angle)*72+Y*sin(angle)*72,
X,10);
      ModelSupport::buildCylinder(SMap,buildIndex+7789,
      Origin+Z*20+Y*21+X*right-X*20-X*cos(angle)*72+Y*sin(angle)*72,
X,9.8);

      
      
    }


    if (iTubes<0){
    ModelSupport::buildCylinder(SMap,buildIndex+7,
		       Origin+X*140+Z*20+Y*0.5*length,Y,10);

    double pipipi=4.0*atan(1.0);
    double angle0=-pipipi/6.0;
    double RBend=10;  
      ModelSupport::buildPlane(
      SMap,buildIndex+27,
      Origin+X*140+Z*20+Y*0.5*length-Y*20,
      Y*cos(angle0)-X*sin(angle0));
      //      std::cout << buildIndex+65+100*CI << std::endl;

      double angle=-pipipi/3.0;
      
      ModelSupport::buildCylinder(SMap,buildIndex+77,
      Origin+X*140+Z*20+Y*0.5*length-Y*20,
				Y*cos(angle)-X*sin(angle),10);
    
      ModelSupport::buildPlane(
      SMap,buildIndex+37,
      Origin+X*140+Z*20+Y*0.5*length-Y*20-Y*cos(angle)*72+X*sin(angle)*72,
      Y*cos(angle0)-X*sin(angle0));

      ModelSupport::buildCylinder(SMap,buildIndex+777,
      Origin+X*140+Z*20+Y*0.5*length-Y*20-Y*cos(angle)*72+X*sin(angle)*72,Y,10);
      //Chicane #2
      
      ModelSupport::buildCylinder(SMap,buildIndex+8,
		       Origin+Z*20+Y*21-X*right,X,10);

      ModelSupport::buildPlane(
      SMap,buildIndex+28,
      Origin+Z*20+Y*21-X*right+X*20,
      -X*cos(angle0)-Y*sin(angle0));
      //      std::cout << buildIndex+65+100*CI << std::endl;
      ModelSupport::buildCylinder(SMap,buildIndex+78,
      Origin+Z*20+Y*21-X*right+X*20,
			-X*cos(angle)-Y*sin(angle),10);
    
      
      ModelSupport::buildPlane(
      SMap,buildIndex+38,
      Origin+Z*20+Y*21-X*right+X*20+X*cos(angle)*72+Y*sin(angle)*72,
     - X*cos(angle0)-Y*sin(angle0));

      ModelSupport::buildCylinder(SMap,buildIndex+778,
      Origin+Z*20+Y*21-X*right+X*20+X*cos(angle)*72+Y*sin(angle)*72,
X,10);



    }

    
    //    ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(hup+hlow),Z);


  
  return;
}

void
E02PillarCover::createObjects(Simulation& System)
  /*!
    Adds the block with the gap
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("E02PillarCover","createObjects");

  std::string Out;
  
  const std::string frontStr
    (activeFront ? frontSurf.display()+frontCut.display() : 
     ModelSupport::getComposite(SMap,buildIndex," 11 "));
  const std::string backStr=
     ModelSupport::getComposite(SMap,buildIndex," -2 ");

  // Inner void is a single segment
  /*  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6 5 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr+backStr);
  */
  // Loop over all layers:


  int LIF(buildIndex),LIB(buildIndex),LIT(buildIndex),LITB(buildIndex);
    for (int i=0; i<nLayers; i++){

      int SIF(buildIndex);
      int SIB(buildIndex);

      if((!activeFront)){
	//	std::cout << "building default" << std::endl;
	for (int i=1; i<nSeg; i++){
//      std::cout << ModelSupport::getComposite(SMap,SIF," 1 -114 ") << std::endl;
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,LIT,
	     " -6N 15 3M -13M ")+
      ModelSupport::getComposite(SMap,SIF," 1 -114 ");
      //           std::cout << Out <<std::endl;
      if (iTubes<0) //   Out+=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,  "  ( 78 : -38 : 28)  ");
    Out+=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,  "  ( 78 : -38 : 28)  (778 : 38) ");
     
      makeCell("WallL",System,cellIndex++,defMatf,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
	     "  -6N 15  -4M 14M ")+
		ModelSupport::getComposite(SMap,SIF," 1 -114  ");
      if (iChicane>0) Out+=ModelSupport::getComposite(SMap,buildIndex," ( -9 : 19 ) ");
      if (iTubes>0) Out+=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
						     "  ( 78 : -38 : 28)  (778 : 38) ");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallR",System,cellIndex++,defMatf,0.0,Out);

      //  std::cout << Out <<std::endl;

      
      Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
	     " -6N 16N  -14M 13M ")+
		ModelSupport::getComposite(SMap,SIF," 1  -114 ");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallT",System,cellIndex++,defMatf,0.0,Out);
      //   std::cout << Out <<std::endl;

      
      SIF+=113;
	}
	//	std::cout << LIF <<" " << LIT << " " <<SIF << std::endl;
	
	if (iTubes<0)  {    Out=ModelSupport::getComposite(SMap,buildIndex, LIF,LIT,
	     " -12M -6N 15 3M -13M ")+
	    ModelSupport::getComposite(SMap,SIF," 1  ");
	  //for open channels
            if (nLayers>1) Out+=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
		" ( 78 : -38 : 28)  (778 : 38)  ( 8: -28) "); else
	    Out+=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
		" ( 78 : -38 : 28)  (778 : 38)  ");
	  /* // for plugs
	  if (nLayers>1) Out+=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
		" ( 78 : -38 : 28)  "); else
	    Out+=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
		" ( 78 : -38 : 28)    ");
	  */
	}
	else
	        Out=ModelSupport::getComposite(SMap,buildIndex, LIF,LIT,
	     " -12M -6N 15 3M -13M ")+
	ModelSupport::getComposite(SMap,SIF," 1  ");
     //std::cout << Out <<std::endl;
		 
	   makeCell("WallL",System,cellIndex++,defMatf,0.0,Out);

	   
      	if (iTubes>0)
	  if (nLayers>1) {Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
	     " ( 78 : -38 : 28)  (778 : 38)  ( 8: -28)  -12M -6N 15  -4M 14M")+
	      ModelSupport::getComposite(SMap,SIF," 1  ");} else {
	    Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
	     " ( 78 : -38 : 28)  (778 : 38)   -12M -6N 15  -4M 14M")+
	      ModelSupport::getComposite(SMap,SIF," 1  ");}
	  else
       Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
	     " -12M -6N 15  -4M 14M")+
		ModelSupport::getComposite(SMap,SIF," 1  ");
      //         std::cout << Out <<std::endl;
	if (iChicane>0) Out+=ModelSupport::getComposite(SMap,buildIndex,
				" ( "+std::to_string(+2+10*nLayers)+": -9 : 19 )");

		 makeCell("WallR",System,cellIndex++,defMatf,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
	     " -12M -6N 16N  -14M 13M ")+
		ModelSupport::getComposite(SMap,SIF," 1  ");
      //         std::cout << Out <<std::endl;

		 makeCell("WallT",System,cellIndex++,defMatf,0.0,Out);


		 if (iTubes!=0)
		   {Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
     " (7: -27) (27: 77 : -37 ) (37 : 777 ) -2M 12M 15 -6N -4M 3M ") + // ( 8: -28)  (78 : 28)
		ModelSupport::getComposite(SMap,SIF," 1  ");
		     if (nLayers<2)
		          Out+=ModelSupport::getComposite(SMap,buildIndex,
					" ( 8: -28)  (78 : 28) ");
		   }
		 else
  		   Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
	     " -2M 12M 15 -6N -4M 3M ")+
		ModelSupport::getComposite(SMap,SIF," 1  ");

		  //         std::cout << Out <<std::endl;

		 makeCell("WallBack",System,cellIndex++,defMatf,0.0,Out);
	   
		 
	   

      /*
      for (int i=1; i<nSeg; i++){
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

      */

      }else{


      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,LIT,
	     "-11 ( -111 : 301 -601 )  -6N 15 3M -13M ");
      //     std::cout << "Objexts" << i << Out +frontStr<<std::endl;
      makeCell("WallL",System,cellIndex++,defMatf,0.0,Out+frontStr);
      

      Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
	     " -11 ( -111 : -401 -601 ) -6N 15  -4M 14M");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallR",System,cellIndex++,defMatf,0.0,Out+frontStr);

      Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
	     " -11 ( -111 : -601 -401 301 ) -6N 16N  -14M 13M ");
      // std::cout << "Objexts" << i << Out+frontStr <<std::endl;
      makeCell("WallT",System,cellIndex++,defMatf,0.0,Out+frontStr);

      
      /*
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
      */
	}
	

      LIF+=10; LIB+=100; LIT+=10; LITB+=100;
    };
    //Voids

    if(iChicane>0)
      {
Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
	     " 1  -2M 9 -19  -4 4M ");
  makeCell("Chicane",System,cellIndex++,voidMat,0.0,Out);
      }
	      

	   if (iTubes>0) {
	     	   Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
				  " -79 27 -2 : -27 -77 37 2M : -37 -7779 2M ");
	   makeCell("VoidCable",System,cellIndex++,voidMat,0.0,Out);


	   Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
				  " -7 79 27 -2 ");
	   makeCell("Mirrobor",System,cellIndex++,150,0.0,Out);

	   Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
				  "  -37 -777 7779 2M ");
	   makeCell("Mirrobor",System,cellIndex++,150,0.0,Out);

	   
	   Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
				  " -89 28 -4 : -28 -78 38  : -38 -7789 4M ");
	     	   makeCell("VoidCable",System,cellIndex++,voidMat,0.0,Out);
	   Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
				  " -8 89 28 -4  ");
	     	   makeCell("Mirrobor",System,cellIndex++,150,0.0,Out);
	   Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
				  "  -38 -778 7789 4M ");
	     	   makeCell("Mirrobor",System,cellIndex++,150,0.0,Out);

	   }else if (iTubes<0)
	     {
  	   Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
				  " -37 -777 2002  : -27 -77 37 2M :  -7 27 -1002");
	   makeCell("VoidCable",System,cellIndex++,voidMat,0.0,Out);

  	   Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
				  " -7 27 -2 1002 ");
	   makeCell("Plug",System,cellIndex++,141,0.0,Out);
  	   Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
				  "  -37 -777 -2002 2M ");
	   makeCell("Plug",System,cellIndex++,141,0.0,Out);


	   Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
				  "  -8 28 1003  : -28 -78 38 : -38 -778 -2003 ");
	   makeCell("VoidCable",System,cellIndex++,voidMat,0.0,Out);

           Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
				  " -8 28 3 -1003 ");
	   makeCell("Plug",System,cellIndex++,141,0.0,Out);

	   Out=ModelSupport::getComposite(SMap,buildIndex,LIF,LIT,
				  " -38 -778 2003 -3M ");
	   makeCell("Plug",System,cellIndex++,141,0.0,Out);

	     }
		 
    
    int isurf3=3, isurf4=4, isurf6=6;
  
    /*  if (wid1f>wid1b){ isurf3=3; isurf4=4;}
  else{ isurf3=32; isurf4=42;};
  if (height>hupB){ isurf6=6; }
  else{ isurf6=62;};
    */
  //gap below
  Out=ModelSupport::getComposite(SMap,buildIndex,LIF,
     " 3 -4 5 -15 -2 ")+frontStr;
      makeCell("VoidLow",System,cellIndex++,gapMat,0.0,Out);
      /*
      Out=ModelSupport::getComposite(SMap,buildIndex,LIB,
 std::to_string(isurf3)+" -"+std::to_string(isurf4)+" 5 -15 (-32M : 42M) 21 ")+backStr;
      makeCell("VoidLowB",System,cellIndex++,gapMat,0.0,Out);

       Out=ModelSupport::getComposite(SMap,buildIndex,LIF,
  std::to_string(isurf3)+" -"+std::to_string(isurf4)+" 5 -15 -21 11 (-3M : 4M) ");
      makeCell("VoidLowG",System,cellIndex++,gapMat,0.0,Out);
      */
  //Void front block
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,LIT,
	      "-2M -6N 15 3M -4M   ");
             makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr);
      Out=ModelSupport::getComposite(SMap,buildIndex, LIF,LIT,
	      "-2M 15 ( 6N : -3M : 4M ) -1 -6 -4 3  ");
       makeCell("VoidGap",System,cellIndex++,gapMat,0.0,Out+frontStr);
       /*
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
  if (height>hupB){  Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      " 211 -6 -42 32 62 ")+backStr;
       makeCell("VoidOuter",System,cellIndex++,gapMat,0.0,Out);
  }
  else if (height<hupB){  Out=ModelSupport::getComposite(SMap,buildIndex, LIF,
	      " -111 6 -42 32 -62 ")+frontStr;
      makeCell("VoidOuter",System,cellIndex++,gapMat,0.0,Out);
 };

   
  Out=ModelSupport::getComposite(SMap,buildIndex,
		  std::to_string(isurf3)+
		  " -"+std::to_string(isurf4)+
				 " 5 -"+std::to_string(isurf6)+" ")+frontStr+backStr;
    */
    /* " : "+ModelSupport::getComposite(SMap,buildIndex,
		  std::to_string(isurf3)+
		  " -"+std::to_string(isurf4)+
				 " 5 -"+std::to_string(isurf6)+" ")+frontStr+backStr;
  */
Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 +5 -6 ")+frontStr+backStr;

  addOuterSurf(Out);

  return;
}

void
E02PillarCover::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("E02PillarCover","createLinks");
  //  double length=length+l2edge;



  if (!activeFront)
    {
      FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
      FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+11));      
    }
  else
    {
      FixedComp::setLinkSurf(0,frontSurf,1,frontCut,0);      
      FixedComp::setConnect
        (0,SurInter::getLinePoint(Origin,Y,frontSurf,frontCut),-Y);
    }
  //Definition of surface 21
  //  ModelSupport::buildPlane(SMap,buildIndex+21,Origin-Y*(length/2.0-voidLength-0.5*gap),Y);
  /*
     FixedComp::setConnect(6,Origin-Y*(length/2.0-voidLength-0.5*gap),-Y);
      FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+21));      

      FixedComp::setConnect(7,Origin-Y*(length/2.0-voidLength+0.5*gap),Y);
      FixedComp::setLinkSurf(7,-SMap.realSurf(buildIndex+11));
  */
      FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  
    FixedComp::setConnect(2,Origin-X*left,-X);
    FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

   FixedComp::setConnect(3,Origin+X*right,X);
   FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin,-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*height,Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

   FixedComp::setConnect(6,Origin-X*left,-X);
   FixedComp::setLinkSurf(6,-SMap.realSurf(buildIndex+3+10*nLayers));

   FixedComp::setConnect(7,Origin+X*right,X);
   FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+4+10*nLayers));


  return;
}
  
void
E02PillarCover::setFront(const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Set front surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("E02PillarCover","setFront");
  
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
E02PillarCover::setBack(const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Set back surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("E02PillarCover","setBack");
  
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
E02PillarCover::getXSectionIn() const
  /*!
    Get the line shield inner void section
    \return HeadRule of cross-section
   */
{
  ELog::RegMethod RegA("E02PillarCover","getXSectionIn");
  const std::string Out=
    ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 ");
  HeadRule HR(Out);
  HR.populateSurf();
  return HR;
}
  
    
void
E02PillarCover::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("E02PillarCover","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);  
    createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
