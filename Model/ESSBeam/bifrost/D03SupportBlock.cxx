/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/D03SupportBlock.cxx
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

#include "D03SupportBlock.h"

namespace constructSystem
{

D03SupportBlock::D03SupportBlock(const std::string& Key) : 
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

D03SupportBlock::D03SupportBlock(const D03SupportBlock& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  //  buildIndex(A.buildIndex),cellIndex(A.cellIndex),
  activeFront(A.activeFront),activeBack(A.activeBack),
  frontSurf(A.frontSurf),frontCut(A.frontCut),
  backSurf(A.backSurf),backCut(A.backCut),
  //
  l1edge(A.l1edge),l1mid(A.l1mid),l2mid(A.l2mid),l2edge(A.l2edge),gap(A.gap),
    length1low(A.length1low),length2low(A.length2low),
    wid1F(A.wid1F), wid1B(A.wid1B),wid2(A.wid2),wid3(A.wid3),hup(A.hup),hlow(A.hlow),
  defMat(A.defMat), gapMat(A.gapMat),nSeg(A.nSeg),nLayers(A.nLayers),
  RTube(A.RTube), TubeThick(A.TubeThick), TubeOffsetL(A.TubeOffsetL),
  TubeOffsetV(A.TubeOffsetV), RBend(A.RBend), iBuildTube(A.iBuildTube),
  tubeMat(A.tubeMat) //, iLast(A.iLast),distLast(A.distLast)
  /*!
    Copy constructor
    \param A :: D03SupportBlock to copy
  */
{}

D03SupportBlock&
D03SupportBlock::operator=(const D03SupportBlock& A)
  /*!
    Assignment operator
    \param A :: D03SupportBlock to copy
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
    wid1B=A.wid1B; wid1F=A.wid1F;wid2=A.wid2;wid3=A.wid3;hup=A.hup;hlow=A.hlow;
    defMat=A.defMat; gapMat=A.gapMat;nSeg=A.nSeg;nLayers=A.nLayers;
  RTube=A.RTube;  TubeThick=A.TubeThick;  TubeOffsetL=A.TubeOffsetL; 
  TubeOffsetV=A.TubeOffsetV;  RBend=A.RBend;  iBuildTube=A.iBuildTube;
  tubeMat=A.tubeMat; // iLast=A.iLast; distLast=A.distLast;

    }
  return *this;
}

D03SupportBlock::~D03SupportBlock() 
  /*!
    Destructor
  */
{}

void
D03SupportBlock::removeFrontOverLap()
  /*!
    Remove segments that are completly covered by the
    active front.
  */
{
  ELog::RegMethod RegA("D03SupportBlock","removeFrontOverLap");
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
D03SupportBlock::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("D03SupportBlock","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  l1edge=Control.EvalDefVar<double>(keyName+"Length1Edge",100.5);
  l2edge=Control.EvalDefVar<double>(keyName+"Length2Edge",100.5);
  l1mid=Control.EvalDefVar<double>(keyName+"Length1Mid",115.5);
  //  l1mid=Control.EvalDefVar<double>(keyName+"Length1Mid",120);
  l2mid=Control.EvalDefVar<double>(keyName+"Length2mid",85.5);
  length1low=Control.EvalDefVar<double>(keyName+"Length1Low",40);
  length2low=Control.EvalDefVar<double>(keyName+"Length2Low",40);
  gap=Control.EvalDefVar<double>(keyName+"Gap",1.2);

  l2mid = l1edge+l2edge-l1mid;
    
  wid1F=Control.EvalDefVar<double>(keyName+"Width1F",152);
  wid1B=Control.EvalDefVar<double>(keyName+"Width1B",152);
  wid2=Control.EvalDefVar<double>(keyName+"Width2",100);
  wid3=Control.EvalDefVar<double>(keyName+"Width3",70);
  hup=Control.EvalDefVar<double>(keyName+"HeightUp",50);
  hlow=Control.EvalDefVar<double>(keyName+"HeightLow",50);
  
  defMat=ModelSupport::EvalDefMat<int>(Control,keyName+"DefMat",144);
 defMatF=ModelSupport::EvalDefMat<int>(Control,keyName+"DefMatF",defMat);
 defMatB=ModelSupport::EvalDefMat<int>(Control,keyName+"DefMatB",defMat);
  gapMat=ModelSupport::EvalDefMat<int>(Control,keyName+"GapMat",60);
  tubeMat=ModelSupport::EvalDefMat<int>(Control,keyName+"TubeMat",148); // 148=PVC
 
  //  int B4CMat;

  nSeg=Control.EvalDefVar<size_t>(keyName+"NSeg",3);

  nLayers=Control.EvalDefVar<size_t>(keyName+"NLayers",5);
  // Cable tube
  RTube=Control.EvalDefVar<double>(keyName+"RTube",10.);
  TubeThick=Control.EvalDefVar<double>(keyName+"TubeThick",0.5);
  TubeOffsetL=Control.EvalDefVar<double>(keyName+"TubeOffsetL",25);
  TubeOffsetV=Control.EvalDefVar<double>(keyName+"TubeOffsetV",25);
  RBend=Control.EvalDefVar<double>(keyName+"RBend",20); 
  iBuildTube=Control.EvalDefVar<size_t>(keyName+"BuildTube",1);
  TubeOffsetL=l1mid-length1low-TubeOffsetL;
  //  iLast=Control.EvalDefVar<size_t>(keyName+"LastSection",0);

  iSideShield=Control.EvalDefVar<int>(keyName+"SideShield",0);
  int iLayerFlag=Control.EvalDefVar<int>("LayerFlag",1);

  if(iLayerFlag==0){
  nSeg=1;
  nLayers=1;
  }
 
  return;
}

void
D03SupportBlock::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("D03SupportBlock","createUnitVector");


  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  double length=l1edge+l2edge;
  Origin+=Y*(length/2.0);
  return;
}

void
D03SupportBlock::createSurfaces()
  /*!
    Create the surfaces. Note that layers is not used
    because we want to break up the objects into sub components
  */
{
  ELog::RegMethod RegA("D03SupportBlock","createSurfaces");

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
 
  //  std::cout <<"FB" << std::endl;


  ModelSupport::buildPlane(SMap,buildIndex+4, Origin+X*(0.5*wid1F),X);
ModelSupport::buildPlane(SMap,buildIndex+499, Origin+X*(0.5*wid1F-12),X); 

  ModelSupport::buildPlane(SMap,buildIndex+49, Origin+X*(0.5*wid1B),X);
  ModelSupport::buildPlane(SMap,buildIndex+41, Origin+X*(0.5*wid2),X);
  ModelSupport::buildPlane(SMap,buildIndex+4199, Origin+X*(0.5*wid3+1),X);
  ModelSupport::buildPlane(SMap,buildIndex+69,Origin-Z*9.5,Z);

  ModelSupport::buildPlane(SMap,buildIndex+42, Origin+X*(0.5*wid3),X);

  ModelSupport::buildPlane(SMap,buildIndex+3, Origin-X*(0.5*wid1F),X);
  ModelSupport::buildPlane(SMap,buildIndex+399, Origin-X*(0.5*wid1F-12),X);

  ModelSupport::buildPlane(SMap,buildIndex+39, Origin-X*(0.5*wid1B),X);
  ModelSupport::buildPlane(SMap,buildIndex+31, Origin-X*(0.5*wid2),X);
  ModelSupport::buildPlane(SMap,buildIndex+32, Origin-X*(0.5*wid3),X);

    ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(0.5*wid3)-Y*(length/2.0-l1mid+0.5*gap),
			     Y*(0.5*(wid3-wid2))+X*(l1mid-l1edge));
    ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(0.5*wid3)-Y*(length/2.0-l1mid+0.5*gap),
			     -Y*(0.5*(wid3-wid2))+X*(l1mid-l1edge));

    ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(0.5*wid3)-Y*(length/2.0-0.5*gap-l1mid),
			     -Y*(0.5*(wid3-wid2))-X*(l1mid-l1edge));
    ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(0.5*wid3)-Y*(length/2.0-0.5*gap-l1mid),
			     Y*(0.5*(wid3-wid2))-X*(l1mid-l1edge));

    ModelSupport::buildPlane(SMap,buildIndex+6,Origin,Z);

      int LI(0);
    for (int i=0; i<nLayers; i++){
      LI++;
ModelSupport::buildPlane(SMap,buildIndex+10*LI+5,Origin-Z*((nLayers-i)*hup/nLayers),Z);
// std::cout << buildIndex+10*LI+5 << std::endl;
    };
    ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(hup+hlow),Z);

    if(iBuildTube){
    ModelSupport::buildCylinder(SMap,buildIndex+7,
		       Origin-Y*(length/2.0-l1mid+TubeOffsetL),
				Z,RTube);
    ModelSupport::buildCylinder(SMap,buildIndex+8,
		       Origin-Y*(length/2.0-l1mid+TubeOffsetL),
				Z,RTube+TubeThick);

    ModelSupport::buildCylinder(SMap,buildIndex+97,
				Origin-Z*(hup+TubeOffsetV),
				Y,RTube);
    ModelSupport::buildCylinder(SMap,buildIndex+98,
		       Origin-Z*(hup+TubeOffsetV),
				Y,RTube+TubeThick);

    ModelSupport::buildPlane(SMap,buildIndex+65,Origin-Z*(hup+TubeOffsetV-RBend),Z);

    ModelSupport::buildPlane(SMap,buildIndex+61,Origin-Y*(length/2.0-l1mid)-Y*(TubeOffsetL+RBend),Y);

     ModelSupport::buildCylinder(SMap,buildIndex+791,
     Origin-Z*(hup+TubeOffsetV-RBend)-Y*(length/2.0-l1mid)-Y*(TubeOffsetL+RBend),
				 X, 0.5*(RBend-RTube));
    int CI(1);
    double pipipi=4.0*atan(1.0);
    for (int i=0; i<nSeg; i++){
    
      double angle=pipipi/4.0/(nSeg+1) + (CI-1)*pipipi/2.0/(nSeg+1);
      
      ModelSupport::buildPlane(
      SMap,buildIndex+65+100*CI,
      Origin-Z*(hup+TubeOffsetV-RBend)-Y*(length/2.0-l1mid)-Y*(TubeOffsetL+RBend),
      Z*cos(angle)+Y*sin(angle));
      //      std::cout << buildIndex+65+100*CI << std::endl;

      angle=pipipi/2.0/(nSeg+1) + (CI-1)*pipipi/2.0/(nSeg+1);
      
      ModelSupport::buildCylinder(SMap,buildIndex+7+100*CI,
      Origin-Z*(hup+TubeOffsetV-RBend)-Y*(length/2.0-l1mid)-Y*(TubeOffsetL+RBend)
				  +Y*RBend*cos(angle) -Z*RBend*sin(angle),
				Z*RBend*cos(angle)+Y*RBend*sin(angle),RTube);
      ModelSupport::buildCylinder(SMap,buildIndex+8+100*CI,
      Origin-Z*(hup+TubeOffsetV-RBend)-Y*(length/2.0-l1mid)-Y*(TubeOffsetL+RBend)
				  +Y*RBend*cos(angle) -Z*RBend*sin(angle),
				Z*RBend*cos(angle)+Y*RBend*sin(angle),RTube+TubeThick);
      CI++;
    }
    double angle=pipipi/4.0/(nSeg+1) + (CI-1)*pipipi/2.0/(nSeg+1);
      ModelSupport::buildPlane(
      SMap,buildIndex+65+100*CI,
      Origin-Z*(hup+TubeOffsetV-RBend)-Y*(length/2.0-l1mid)-Y*(TubeOffsetL+RBend),
      Z*cos(angle)+Y*sin(angle));

    }    

    //    std::cout << "Surf done" << std::endl;  
  return;
}

void
D03SupportBlock::createObjects(Simulation& System)
  /*!
    Adds the block with the gap
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("D03SupportBlock","createObjects");
  // std::cout<< "Making objects" << std::endl;
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
    int LI(buildIndex+10);
    for (int i=0; i<nLayers-1; i++){
      if ((iSideShield>0)&&(i>1)){
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 3 -399 -111 -15M 5M ");
      makeCell("SideShieldL",System,cellIndex++,151,0.0,Out+frontStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 399 -31 -111 -15M 5M ");
      makeCell("BlockLL",System,cellIndex++,defMatF,0.0,Out+frontStr);
      }else {
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 3 -399 -111 -15M 5M ");
      makeCell("SideShieldL",System,cellIndex++,defMatF,0.0,Out+frontStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 399 -31 -111 -15M 5M ");
      makeCell("BlockLL",System,cellIndex++,defMatF,0.0,Out+frontStr);
      };
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 31 13 -32 -15M 5M ");
      makeCell("BlockL",System,cellIndex++,defMatF,0.0,Out+frontStr);

      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 32 -42 -12 -15M 5M ");
      makeCell("BlockC",System,cellIndex++,defMatF,0.0,Out+frontStr);

    if(iBuildTube){
      Out=ModelSupport::getComposite(SMap,buildIndex,LI,"  8 32 -42 12 -11 -15M 5M ");
      makeCell("BlockCC",System,cellIndex++,defMatF,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex,LI, " -8 7  -15M 5M ");
      makeCell("Tube",System,cellIndex++,tubeMat,0.0,Out);


    } else       {
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 32 -42 12 -11 -15M 5M ");
      makeCell("BlockCC",System,cellIndex++,defMatF,0.0,Out);
   
 }
    
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 42 -41 -14 -15M 5M ");
      makeCell("BlockR",System,cellIndex++,defMatF,0.0,Out+frontStr);

      if ((iSideShield>0)&&(i>1)){
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 41 -499 -111 -15M 5M ");
      makeCell("BlockRR",System,cellIndex++,defMatF,0.0,Out+frontStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 499 -4 -111 -15M 5M ");
      makeCell("SideShieldR",System,cellIndex++,151,0.0,Out+frontStr);
      }else{
      
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 499 -4 -111 -15M 5M ");
      makeCell("BlockRR",System,cellIndex++,defMatF,0.0,Out+frontStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 41 -499 -111 -15M 5M ");
      makeCell("SideShieldR",System,cellIndex++,defMatF,0.0,Out+frontStr);
      }

      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 399 -31 211 -15M 5M ");
      makeCell("BlockLL",System,cellIndex++,defMatB,0.0,Out+backStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 39 -399 211 -15M 5M ");
      makeCell("SideShieldL",System,cellIndex++,defMatB,0.0,Out+backStr);
      
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 31 23 -32 -15M 5M ");
      makeCell("BlockL",System,cellIndex++,defMatB,0.0,Out+backStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 32 -42 21 -15M 5M ");
      makeCell("BlockC",System,cellIndex++,defMatB,0.0,Out+backStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI, " 42 -41 -24 -15M 5M ");

      /* if(iSideShield>0) Out+=ModelSupport::getComposite(SMap,buildIndex,LI,
				     " (  4199 : 12 : -69 )");
      */
      makeCell("BlockR",System,cellIndex++,defMatB,0.0,Out+backStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 41 -499 211 -15M 5M ");
      makeCell("BlockRR",System,cellIndex++,defMatB,0.0,Out+backStr);

      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 499 -49 211 -15M 5M ");
   
   makeCell("SideShieldR",System,cellIndex++,defMatB,0.0,Out+backStr);

      LI+=10;
    };
   
    if (iSideShield>0){
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 3 -399 -111 -6 5M ");
      makeCell("SideShieldL",System,cellIndex++,151,0.0,Out+frontStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 399 -31 -111 -6 5M ");
      makeCell("BlockLL",System,cellIndex++,defMatF,0.0,Out+frontStr);
      /*
      if (iSideShield>0){
      std::cout << "Side Shield" << std::endl; 
      Out=ModelSupport::getComposite(SMap,buildIndex,LI,"42 -4199 -12 69 -6 ");
      makeCell("B4CBlockR",System,cellIndex++,147,0.0,Out+frontStr);
     }
      */
     }else{
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 3 -399 -111 -6 5M ");
      makeCell("SideShieldL",System,cellIndex++,defMatF,0.0,Out+frontStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 399 -31 -111 -6 5M ");
      makeCell("BlockLL",System,cellIndex++,defMatF,0.0,Out+frontStr);
      };
    
    Out=ModelSupport::getComposite(SMap,buildIndex,LI," 31 13 -32 -6 5M ");
      makeCell("BlockL",System,cellIndex++,defMatF,0.0,Out+frontStr);

      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 32 -42 -12 -6 5M ");
      makeCell("BlockC",System,cellIndex++,defMatF,0.0,Out+frontStr);


    if(iBuildTube){
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 8 32 -42 12 -11 -6 5M ");
      makeCell("BlockCC",System,cellIndex++,defMatF,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI, " -8 7  -6 5M ");
      makeCell("Tube",System,cellIndex++,tubeMat,0.0,Out);
    }else{
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 32 -42 12 -11 -6 5M ");
      makeCell("BlockCC",System,cellIndex++,defMatF,0.0,Out);
    }     
      Out=ModelSupport::getComposite(SMap,buildIndex,LI, " 42 -41 -14 -6 5M ");

      /*if(iSideShield>0) Out+=ModelSupport::getComposite(SMap,buildIndex,LI,
				     " (  4199 : 12 : -69 )");
      */
      makeCell("BlockR",System,cellIndex++,defMatF,0.0,Out+frontStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 41 -499 -111 -6 5M ");
           makeCell("BlockRR",System,cellIndex++,defMatF,0.0,Out+frontStr);
	   if((iSideShield>0)){
       Out=ModelSupport::getComposite(SMap,buildIndex,LI," 499 -4 -111 -6 5M ");
      makeCell("SideShieldR",System,cellIndex++,151,0.0,Out+frontStr);}else{
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 499 -4 -111 -6 5M ");
      makeCell("SideShieldR",System,cellIndex++,defMatF,0.0,Out+frontStr);}

      Out=ModelSupport::getComposite(SMap,buildIndex," 3 -31 111 -211 -6 5 ");
      makeCell("GapLL",System,cellIndex++,gapMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex," 31 -13 -23 -32 -6 5 ");
      makeCell("GapL",System,cellIndex++,gapMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex," 32 -42 11 -21 -6 5 ");
      makeCell("GapC",System,cellIndex++,gapMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 42 -41 14 24 -6 5 ");
      makeCell("GapR",System,cellIndex++,gapMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 41 -4 111 -211 -6 5 ");
      makeCell("GapRR",System,cellIndex++,gapMat,0.0,Out);

      //Back      
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 39 -399 211 -6 5M ");
      makeCell("SideShieldL",System,cellIndex++,defMatB,0.0,Out+backStr);

      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 399 -31 211 -6 5M ");
      makeCell("BlockLL",System,cellIndex++,defMatB,0.0,Out+backStr);

      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 31 23 -32 -6 5M ");
      makeCell("BlockL",System,cellIndex++,defMatB,0.0,Out+backStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 32 -42 21 -6 5M ");
      makeCell("BlockC",System,cellIndex++,defMatB,0.0,Out+backStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 42 -41 -24 -6 5M ");
      makeCell("BlockR",System,cellIndex++,defMatB,0.0,Out+backStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 499 -49 211 -6 5M ");
      makeCell("SideShieldR",System,cellIndex++,defMatB,0.0,Out+backStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI," 41 -499 211 -6 5M ");
      makeCell("BlockRR",System,cellIndex++,defMatB,0.0,Out+backStr);

      //Lower blocks
      //Front
      Out=ModelSupport::getComposite(SMap,buildIndex," 3 -31 12 -111 -15 5 ");
      makeCell("BlockLowLL",System,cellIndex++,defMatF,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex," 31 12 13 -32 -15 5 ");
      makeCell("BlockLowL",System,cellIndex++,defMatF,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -12 -15 5 ");
      makeCell("BlockLowCVoid",System,cellIndex++,gapMat,0.0,Out+frontStr);
      if(iBuildTube){
      Out=ModelSupport::getComposite(SMap,buildIndex," 32 -42 12 -11 -15 65 8 ");
      makeCell("BlockLowC",System,cellIndex++,defMatF,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex, " -8 7  65 -15 ");
      makeCell("Tube",System,cellIndex++,tubeMat,0.0,Out);
      
      Out=ModelSupport::getComposite(SMap,buildIndex," 32 -42 12 -61 5 -65 98 ");
      makeCell("BlockLowC",System,cellIndex++,defMatF,0.0,Out);


      //         Out=ModelSupport::getComposite(SMap,buildIndex," 61 32 -42 12 -11 5 -65 ");
      //  makeCell("BlockCC",System,cellIndex++,defMat,0.0,Out);

      //      Out=ModelSupport::getComposite(SMap,buildIndex, " -98 97  61 -11 ");
      //  makeCell("Tube",System,cellIndex++,tubeMat,0.0,Out);


	 Out=ModelSupport::getComposite(SMap,buildIndex," -8 7 -65 165 ");
       makeCell("Tube",System,cellIndex++,tubeMat,0.0,Out);
	 Out=ModelSupport::getComposite(SMap,buildIndex," -7 -65 165 ");
       makeCell("TubeVoid",System,cellIndex++,gapMat,0.0,Out);
       Out=ModelSupport::getComposite(SMap,buildIndex,"  32 -42 12 -11 5 -65 165 8 791");
        makeCell("BlockCC",System,cellIndex++,defMatF,0.0,Out);

	   Out=ModelSupport::getComposite(SMap,buildIndex,"  32 -42 12 -11 5 -65 61 -791");
        makeCell("BlockCC",System,cellIndex++,defMatF,0.0,Out);
    
      
	int  CI(buildIndex+100);
       for (int i=0; i<nSeg; i++){
	 Out=ModelSupport::getComposite(SMap,buildIndex,CI," -8M 7M -65M 165M ");
       makeCell("Tube",System,cellIndex++,tubeMat,0.0,Out);
	 Out=ModelSupport::getComposite(SMap,buildIndex,CI," -7M -65M 165M ");
       makeCell("TubeVoid",System,cellIndex++,gapMat,0.0,Out);
       Out=ModelSupport::getComposite(SMap,buildIndex,CI,"  32 -42 12 -11 5 -65M 165M 8M 791 ");
        makeCell("BlockCC",System,cellIndex++,defMatF,0.0,Out);
       CI+=100;
	  }
       
	 Out=ModelSupport::getComposite(SMap,buildIndex,CI," -98 97 -65M 61 ");
       makeCell("Tube",System,cellIndex++,tubeMat,0.0,Out);
	 Out=ModelSupport::getComposite(SMap,buildIndex,CI," -97 -65M 61 ");
       makeCell("TubeVoid",System,cellIndex++,gapMat,0.0,Out);
       Out=ModelSupport::getComposite(SMap,buildIndex,CI,"  32 -42 12 -11 5 -65M 61 98 791");
        makeCell("BlockCC",System,cellIndex++,defMatF,0.0,Out);
      


      }else{
      Out=ModelSupport::getComposite(SMap,buildIndex," 32 -42 12 -11 -15 5 ");
      makeCell("BlockLowC",System,cellIndex++,defMatF,0.0,Out);
      }
      Out=ModelSupport::getComposite(SMap,buildIndex," 42 -41 12 -14 -15 5 ");
      makeCell("BlockLowR",System,cellIndex++,defMatF,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex," 41 -4 -111 12  -15 5 ");
      makeCell("BlockLowR",System,cellIndex++,defMatF,0.0,Out);

      
      //Back      
      //Out=ModelSupport::getComposite(SMap,buildIndex," 3 -31 211 -22 -15 5 ");
      Out=ModelSupport::getComposite(SMap,buildIndex," 39 -31 211 -22 -15 5 ");
      makeCell("BlockLowLL",System,cellIndex++,defMatB,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex," 31 23 -32 -22 -15 5 ");
      makeCell("BlockLowL",System,cellIndex++,defMatB,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex," 32 -42 21 -22 -15 5 ");
      makeCell("BlockLowC",System,cellIndex++,defMatB,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex," 39 -49 22 -15 5 ");
      makeCell("BlockLowCVoid",System,cellIndex++,gapMat,0.0,Out+backStr);
      Out=ModelSupport::getComposite(SMap,buildIndex," 42 -41 -22 -24 -15 5 ");
      makeCell("BlockLowR",System,cellIndex++,defMatB,0.0,Out);
      //      Out=ModelSupport::getComposite(SMap,buildIndex," 41 -4 -22 211 -15 5 ");
            Out=ModelSupport::getComposite(SMap,buildIndex," 41 -49 -22 211 -15 5 ");
      makeCell("BlockLowR",System,cellIndex++,defMatB,0.0,Out);

    if(iBuildTube){
    Out=ModelSupport::getComposite(SMap,buildIndex, " -7  65 -6 ");
    makeCell("TubeVoid",System,cellIndex++,gapMat,0.0,Out);
        Out=ModelSupport::getComposite(SMap,buildIndex, " -97  12 -61 ");
	makeCell("TubeVoid",System,cellIndex++,gapMat,0.0,Out);
        Out=ModelSupport::getComposite(SMap,buildIndex, " -98 97  12 -61 ");
	makeCell("Tube",System,cellIndex++,tubeMat,0.0,Out);
    }
      
      
  // Outer
        if(wid1F>wid1B) {
	  //  std::cout << "1>2" << std::endl;

	  Out=ModelSupport::getComposite(SMap,buildIndex, " 211 49 -4 5 -6 ")+backStr;
	makeCell("VoidOut",System,cellIndex++,gapMat,0.0,Out);

	Out=ModelSupport::getComposite(SMap,buildIndex, " 211 3 -39 5 -6 ")+backStr;
	makeCell("VoidOut",System,cellIndex++,gapMat,0.0,Out);
      
          Out=ModelSupport::getComposite(SMap,buildIndex, " 3 -4 5 -6 ")+frontStr+backStr;
      addOuterSurf(Out+frontStr+backStr);}
    else if (wid1B>wid1F){
      //      std::cout <<"Shuttersupport" << std::endl;
       Out=ModelSupport::getComposite(SMap,buildIndex, " -211 39 -3 5 -6 ")+frontStr;
	makeCell("VoidOut",System,cellIndex++,gapMat,0.0,Out);

	      Out=ModelSupport::getComposite(SMap,buildIndex, " -211 4 -49 5 -6 ")+frontStr;
	makeCell("VoidOut",System,cellIndex++,gapMat,0.0,Out);

	Out=ModelSupport::getComposite(SMap,buildIndex, " 39 -49 5 -6 ")+frontStr+backStr;
      addOuterSurf(Out);
    } else{
      Out=ModelSupport::getComposite(SMap,buildIndex, " 3 -4 5 -6 ")+frontStr+backStr;
      addOuterSurf(Out+frontStr+backStr);}
     return;
}

void
D03SupportBlock::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("D03SupportBlock","createLinks");

 
  double wid1;
  if (wid1F>wid1B) wid1=wid1F; else wid1= wid1B;
    
  double length=l1edge+l2edge;

  FixedComp::setConnect(2,Origin-X*0.5*wid1,-X);
  FixedComp::setConnect(3,Origin+X*0.5*wid1,X);
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
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));



  return;
}
  
void
D03SupportBlock::setFront(const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Set front surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("D03SupportBlock","setFront");
  
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
D03SupportBlock::setBack(const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Set back surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("D03SupportBlock","setBack");
  
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
D03SupportBlock::getXSectionIn() const
  /*!
    Get the line shield inner void section
    \return HeadRule of cross-section
   */
{
  ELog::RegMethod RegA("D03SupportBlock","getXSectionIn");
  const std::string Out=
    ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 ");
  HeadRule HR(Out);
  HR.populateSurf();
  return HR;
}
  
    
void
D03SupportBlock::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("D03SupportBlock","createAll(FC)");

  populate(System.getDataBase());
  // std::cout << "Vars done" << std::endl;

 createUnitVector(FC,FIndex);
 //std::cout << "Uvec done" << std::endl;
 
  createSurfaces();    
  // std::cout << "Surf done" << std::endl;
  createObjects(System);
  // std::cout << "Obj done" << std::endl;
  
    createLinks();
    // std::cout << "Link done" << std::endl;
  
  insertObjects(System);   
  // std::cout << "Insert done done" << std::endl;
  
  return;
}
  
}  // NAMESPACE constructSystem
