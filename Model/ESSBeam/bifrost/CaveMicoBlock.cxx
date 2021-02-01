/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/CaveMicoBlock.cxx
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

#include "CaveMicoBlock.h"

namespace constructSystem
{

CaveMicoBlock::CaveMicoBlock(const std::string& Key) : 
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

CaveMicoBlock::CaveMicoBlock(const CaveMicoBlock& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  //  buildIndex(A.buildIndex),cellIndex(A.cellIndex),
  activeFront(A.activeFront),activeBack(A.activeBack),
  frontSurf(A.frontSurf),frontCut(A.frontCut),
  backSurf(A.backSurf),backCut(A.backCut),
  //
  l1edge(A.l1edge),l1mid(A.l1mid),l2mid(A.l2mid),l2edge(A.l2edge),gap(A.gap),gapWave(A.gapWave),
    length1low(A.length1low),length2low(A.length2low),
    wid1F(A.wid1F), wid1B(A.wid1B),wid2(A.wid2),wid3(A.wid3),hup(A.hup),hlow(A.hlow),
  defMat(A.defMat), gapMat(A.gapMat),nSeg(A.nSeg),nLayers(A.nLayers),
  RTube(A.RTube), TubeThick(A.TubeThick), TubeOffsetL(A.TubeOffsetL),
  TubeOffsetV(A.TubeOffsetV), RBend(A.RBend), iBuildTube(A.iBuildTube),
  tubeMat(A.tubeMat) //, iLast(A.iLast),distLast(A.distLast)
  /*!
    Copy constructor
    \param A :: CaveMicoBlock to copy
  */
{}

CaveMicoBlock&
CaveMicoBlock::operator=(const CaveMicoBlock& A)
  /*!
    Assignment operator
    \param A :: CaveMicoBlock to copy
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

  l1edge=A.l1edge;l1mid=A.l1mid;l2mid=A.l2mid;l2edge=A.l2edge;gap=A.gap;gapWave=A.gapWave;
    length1low=A.length1low;length2low=A.length2low;
    wid1B=A.wid1B; wid1F=A.wid1F;wid2=A.wid2;wid3=A.wid3;hup=A.hup;hlow=A.hlow;
    defMat=A.defMat; gapMat=A.gapMat;nSeg=A.nSeg;nLayers=A.nLayers;
  RTube=A.RTube;  TubeThick=A.TubeThick;  TubeOffsetL=A.TubeOffsetL; 
  TubeOffsetV=A.TubeOffsetV;  RBend=A.RBend;  iBuildTube=A.iBuildTube;
  tubeMat=A.tubeMat; // iLast=A.iLast; distLast=A.distLast;

    }
  return *this;
}

CaveMicoBlock::~CaveMicoBlock() 
  /*!
    Destructor
  */
{}

void
CaveMicoBlock::removeFrontOverLap()
  /*!
    Remove segments that are completly covered by the
    active front.
  */
{
  ELog::RegMethod RegA("CaveMicoBlock","removeFrontOverLap");
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
CaveMicoBlock::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("CaveMicoBlock","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  //  Control.addVariable(keyname+"XYAngle",-90);
  l1edge=Control.EvalDefVar<double>(keyName+"Length1Edge",115.5);
  l2edge=Control.EvalDefVar<double>(keyName+"Length2Edge",4.5);
  l1mid=Control.EvalDefVar<double>(keyName+"Length1Mid",118.);
  l2mid=Control.EvalDefVar<double>(keyName+"Length2mid",2.);
  length1low=Control.EvalDefVar<double>(keyName+"Length1Low",111);
  length2low=Control.EvalDefVar<double>(keyName+"Length2Low",40);
  gap=Control.EvalDefVar<double>(keyName+"Gap",1.2);

  gapWave=Control.EvalDefVar<double>(keyName+"GapWave",0.2);
    
  l2mid = l1edge+l2edge-l1mid;
    
  wid1F=Control.EvalDefVar<double>(keyName+"Width1F",7.8);
  wid1B=Control.EvalDefVar<double>(keyName+"Width1B",7.8);
  wid2=Control.EvalDefVar<double>(keyName+"Width2",5.2);
  wid3=Control.EvalDefVar<double>(keyName+"Width3",2.6);
  hup=Control.EvalDefVar<double>(keyName+"HeightUp",49.5+10+53);
  hlow=Control.EvalDefVar<double>(keyName+"HeightLow",50.5+10-53);
  
  defMat=ModelSupport::EvalDefMat<int>(Control,keyName+"DefMat",144);
  gapMat=ModelSupport::EvalDefMat<int>(Control,keyName+"GapMat",60);
  tubeMat=ModelSupport::EvalDefMat<int>(Control,keyName+"TubeMat",148); // 148=PVC
  waxMat= ModelSupport::EvalDefMat<int>(Control,keyName+"WaxMat",48); // 48=poly
  //  int B4CMat;

  nSeg=Control.EvalDefVar<size_t>(keyName+"NSeg",3);

  nLayers=Control.EvalDefVar<size_t>(keyName+"NLayers",5);

  // Cable tube
  /*
  RTube=Control.EvalDefVar<double>(keyName+"RTube",10.);
  TubeThick=Control.EvalDefVar<double>(keyName+"TubeThick",0.5);
  TubeOffsetL=Control.EvalDefVar<double>(keyName+"TubeOffsetL",25);
  TubeOffsetV=Control.EvalDefVar<double>(keyName+"TubeOffsetV",25);
  RBend=Control.EvalDefVar<double>(keyName+"RBend",20); 
  iBuildTube=Control.EvalDefVar<size_t>(keyName+"BuildTube",0);
  iBuildTube=0;
  TubeOffsetL=l1mid-length1low-TubeOffsetL;
  //  iLast=Control.EvalDefVar<size_t>(keyName+"LastSection",0);
  */
  int iLayerFlag=Control.EvalDefVar<int>("LayerFlag",1);

  if(iLayerFlag==0){
  nSeg=1;
  nLayers=1;
  }
  nLayers=1;
 
  return;
}

void
CaveMicoBlock::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("CaveMicoBlock","createUnitVector");


  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  double length=l1edge+l2edge;
  Origin+=Y*(length/2.0);
  return;
}

void
CaveMicoBlock::createSurfaces()
  /*!
    Create the surfaces. Note that layers is not used
    because we want to break up the objects into sub components
  */
{
  ELog::RegMethod RegA("CaveMicoBlock","createSurfaces");

  double length=l1edge+l2edge;

  // Inner void
  if (!activeFront)
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
   if (!activeBack)
    ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  
      
    ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(length/2.0-l1mid+0.5*gap),Y);
    ModelSupport::buildPlane(SMap,buildIndex+111,Origin-Y*(length/2.0-l1edge+0.5*gap),Y);
    ModelSupport::buildPlane(SMap,buildIndex+1011,Origin-Y*(length/2.0-l1mid+0.5*gapWave),Y);
    ModelSupport::buildPlane(SMap,buildIndex+1111,Origin-Y*(length/2.0-l1edge+0.5*gapWave),Y);
    ModelSupport::buildPlane(SMap,buildIndex+12,Origin-Y*(length/2.0-length1low),Y);


  ModelSupport::buildPlane(SMap,buildIndex+21,Origin-Y*(length/2.0-l1mid-0.5*gap),Y);
  ModelSupport::buildPlane(SMap,buildIndex+211,Origin-Y*(length/2.0-l1edge-0.5*gap),Y);
  ModelSupport::buildPlane(SMap,buildIndex+1021,Origin-Y*(length/2.0-l1mid-0.5*gapWave),Y);
  ModelSupport::buildPlane(SMap,buildIndex+1211,Origin-Y*(length/2.0-l1edge-0.5*gapWave),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*(length/2.0-length2low),Y);
 
  //  std::cout <<"FB" << std::endl;

    int WI(0);
    for (int i=0; i<14; i++){
      double extra=0.0;
            if (i==13) extra=5.4;
  ModelSupport::buildPlane(SMap,buildIndex+4+1000*WI, Origin+X*(0.5*wid1F+WI*wid1F+
								extra-6.5*wid1F),X);
   ModelSupport::buildPlane(SMap,buildIndex+49+1000*WI, Origin+X*(0.5*wid1B+WI*wid1F+
								  extra-6.5*wid1F),X);
  ModelSupport::buildPlane(SMap,buildIndex+41+1000*WI, Origin+X*(0.5*wid2+WI*wid1F-
								 6.5*wid1F),X);
  ModelSupport::buildPlane(SMap,buildIndex+42+1000*WI, Origin+X*(0.5*wid3+WI*wid1F-
								 6.5*wid1F),X);
  if (i==0) extra=5.4; else extra=0.0;

  ModelSupport::buildPlane(SMap,buildIndex+3+1000*WI, Origin-X*(0.5*wid1F-WI*wid1F+extra+6.5*wid1F),X);
   ModelSupport::buildPlane(SMap,buildIndex+39+1000*WI, Origin-X*(0.5*wid1B-WI*wid1F+extra+6.5*wid1F),X);
  ModelSupport::buildPlane(SMap,buildIndex+31+1000*WI, Origin-X*(0.5*wid2-WI*wid1F+6.5*wid1F),X);
  ModelSupport::buildPlane(SMap,buildIndex+32+1000*WI, Origin-X*(0.5*wid3-WI*wid1F+6.5*wid1F),X);

  double sinsin=sin(atan(0.5*(wid2-wid3)/abs(l1edge-l1mid)));
  
  ModelSupport::buildPlane(SMap,buildIndex+1000*WI+13,
			   Origin-X*(0.5*wid3-WI*wid1F+6.5*wid1F)
			   -Y*(length/2.0-l1mid+0.5*gapWave/sinsin),
			     Y*(0.5*(wid3-wid2))+X*(l1mid-l1edge));

  ModelSupport::buildPlane(SMap,buildIndex+1000*WI+14,
			   Origin+X*(0.5*wid3+WI*wid1F-6.5*wid1F)
			   -Y*(length/2.0-l1mid+0.5*gapWave/sinsin),
			     -Y*(0.5*(wid3-wid2))+X*(l1mid-l1edge));

  ModelSupport::buildPlane(SMap,buildIndex+23+1000*WI,
			   Origin-X*(0.5*wid3-WI*wid1F+6.5*wid1F)
			   -Y*(length/2.0-0.5*gapWave/sinsin-l1mid),
			     -Y*(0.5*(wid3-wid2))-X*(l1mid-l1edge));
  ModelSupport::buildPlane(SMap,buildIndex+24+1000*WI,
			   Origin+X*(0.5*wid3+WI*wid1F-6.5*wid1F)
			   -Y*(length/2.0-0.5*gapWave/sinsin-l1mid),
			     Y*(0.5*(wid3-wid2))-X*(l1mid-l1edge));
      WI++;
    }
  
    ModelSupport::buildPlane(SMap,buildIndex+6,Origin,Z);

    /*  int LI(0);
    for (int i=0; i<nLayers; i++){
      LI++;
ModelSupport::buildPlane(SMap,buildIndex+10*LI+5,Origin-Z*((nLayers-i)*hup/nLayers),Z);
// std::cout << buildIndex+10*LI+5 << std::endl;
    };
    */
    ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(hup+hlow),Z);
    ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*hup-Z*2.0-Z*gapWave,Z);
    ModelSupport::buildPlane(SMap,buildIndex+25,Origin-Z*hup-Z*2.0,Z);
    ModelSupport::buildPlane(SMap,buildIndex+35,Origin-Z*hup-Z*1.0,Z);
    ModelSupport::buildPlane(SMap,buildIndex+45,Origin-Z*hup,Z);
    ModelSupport::buildPlane(SMap,buildIndex+55,Origin-Z*hup+Z*1.0,Z);

    //    std::cout << "Surf done" << std::endl;  
  return;
}

void
CaveMicoBlock::createObjects(Simulation& System)
  /*!
    Adds the block with the gap
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("CaveMicoBlock","createObjects");
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

  //Loop over waves
  int WI(buildIndex);
  int LI(buildIndex+20);
  for (int j=0; j<14; j++){
  // Loop over all layers:

  //    std::cout << "Layers done" << std::endl;

    if (WI==buildIndex){
    Out=ModelSupport::getComposite(SMap,buildIndex,LI+20,WI," 3N -31N -111 -6 5M ");
    makeCell("BlockLL",System,cellIndex++,defMat,0.0,Out+frontStr);
     Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 31N -4N -12 -6 5M ");
     makeCell("BlockCC",System,cellIndex++,defMat,0.0,Out+frontStr);}
    else   {  Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 12 3N -31N -1111 -6 5M ");
    makeCell("BlockLL",System,cellIndex++,defMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 3N -4N -12 -6 5M ");
 if (WI!=buildIndex+13000)
      makeCell("BlockCC",System,cellIndex++,defMat,0.0,Out+frontStr);
    }
    //    std::cout << WI << std::endl;
      Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 12 31N 13N -32N -6 5M ");
      makeCell("BlockL",System,cellIndex++,defMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 32N -42N 12 -1011 -6 5M ");
      makeCell("BlockC",System,cellIndex++,defMat,0.0,Out);


      /*
      Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 32N -42N -12 -6 5M ");
      makeCell("BlockC",System,cellIndex++,defMat,0.0,Out+frontStr);

      // std::cout << WI << std::endl;
      
    if(iBuildTube){
      Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 8 32 -42 12 -11 -6 5M ");
      makeCell("BlockCC",System,cellIndex++,defMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI, " -8 7  -6 5M ");
      makeCell("Tube",System,cellIndex++,tubeMat,0.0,Out);
    }else{
      Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 32N -42N 12 -11 -6 5M ");
      makeCell("BlockCC",System,cellIndex++,defMat,0.0,Out);
      }   */
      
    // std::cout << WI << std::endl;
      Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 12 42N -41N -14N -6 5M ");
      makeCell("BlockR",System,cellIndex++,defMat,0.0,Out);

      if (WI==buildIndex+13000)
	{	Out=ModelSupport::getComposite(SMap,buildIndex,LI+20,WI," 41N -4N -111 -6 5M ");
	  makeCell("BlockR",System,cellIndex++,defMat,0.0,Out+frontStr);
	  //  	Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 12 41N -4N -111 -6 5M ");
	  //	  makeCell("BlockR",System,cellIndex++,defMat,0.0,Out+frontStr);
     Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 3N -41N -12 -6 5M ");
      makeCell("BlockCC",System,cellIndex++,defMat,0.0,Out+frontStr);	} 
           else
	{Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 12 41N -4N -1111 -6 5M ");
	  makeCell("BlockR",System,cellIndex++,defMat,0.0,Out);}
    
if (WI==buildIndex)
  {         Out=ModelSupport::getComposite(SMap,buildIndex,LI+20,WI," 3N -31N 111 -211 -6 5M ");
      makeCell("GapLL",System,cellIndex++,gapMat,0.0,Out);
  }else{     Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 3N -31N 1111 -1211 -6 5M ");
      makeCell("GapLL",System,cellIndex++,waxMat,0.0,Out);
 }

 if (WI==buildIndex)
   {   Out=ModelSupport::getComposite(SMap,buildIndex,LI+10,WI," 3N -31N  -15M 5M ");
         makeCell("GapHH",System,cellIndex++,gapMat,0.0,Out+frontStr+backStr);
}


      
      Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 31N -13N -23N -32N -6 5M ");
      makeCell("GapL",System,cellIndex++,waxMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 32N -42N 1011 -1021 -6 5M ");
      makeCell("GapC",System,cellIndex++,waxMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 42N -41N 14N 24N -6 5M ");
      makeCell("GapR",System,cellIndex++,waxMat,0.0,Out);

      if(WI==buildIndex+13000)
	{ Out=ModelSupport::getComposite(SMap,buildIndex,LI+20,WI," 41N -4N 111 -211 -6 5M ");
	  makeCell("GapRR",System,cellIndex++,gapMat,0.0,Out);}
      else{
	 Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 41N -4N 1111 -1211 -6 5M ");
	 makeCell("GapRR",System,cellIndex++,waxMat,0.0,Out);}
      //    std::cout << WI << std::endl;

      if (WI==buildIndex+13000)
   {   Out=ModelSupport::getComposite(SMap,buildIndex,LI+10,WI," 41N -4N  -15M 5M ");
         makeCell("GapHH",System,cellIndex++,gapMat,0.0,Out+frontStr+backStr);
}

      //Back      
 if (WI==buildIndex)
      Out=ModelSupport::getComposite(SMap,buildIndex,LI+20,WI," 3N -31N 211 -6 5M ");
 else
   Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 3N -31N 1211 -6 5M ");
      makeCell("BlockLL",System,cellIndex++,defMat,0.0,Out+backStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 31N 23N -32N -6 5M ");
      makeCell("BlockL",System,cellIndex++,defMat,0.0,Out+backStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 32N -42N 1021 -6 5M ");
      makeCell("BlockC",System,cellIndex++,defMat,0.0,Out+backStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 42N -41N -24N -6 5M ");
      makeCell("BlockR",System,cellIndex++,defMat,0.0,Out+backStr);


	/*    Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 3N -31N -111 -6 5M ");
    makeCell("BlockLL",System,cellIndex++,defMat,0.0,Out+frontStr);
     Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 31N -4N -12 -6 5M ");
     makeCell("BlockCC",System,cellIndex++,defMat,0.0,Out+frontStr);}
	*/

      if(WI==buildIndex+13000)
            Out=ModelSupport::getComposite(SMap,buildIndex,LI+20,WI,"12 41N -49N 211 -6 5M ");
            else
          Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 41N -49N 1211 -6 5M ");
	  makeCell("BlockRR",System,cellIndex++,defMat,0.0,Out+backStr);
      // std::cout << WI << std::endl;
   WI+=1000;  }; // waves done
  //  std::cout << "waves done" << std::endl;

  //Lower part
   WI=buildIndex;
  LI= buildIndex+10;
  for (int j=0; j<14; j++){
  // Loop over all layers:

  //    std::cout << "Layers done" << std::endl;

    if (WI==buildIndex){
    Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 3N -31N  5 -25M ");
        makeCell("BlockLL",System,cellIndex++,defMat,0.0,Out+frontStr+backStr);
     Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 31N -4N 5 -5M ");
      makeCell("BlockCC",System,cellIndex++,defMat,0.0,Out+frontStr+backStr);
    Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 31N -4N 15 -15M ");
      makeCell("GapCC",System,cellIndex++,waxMat,0.0,Out+frontStr+backStr);
 
    }
    else   {  Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 3N -31N -1111 5 -5M ");
   //   makeCell("BlockLL",System,cellIndex++,defMat,0.0,Out+" "+std::to_string(buildIndex+12));
      Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 3N -4N  5 -5M ");
 if (WI!=buildIndex+13000)
      makeCell("BlockCC",System,cellIndex++,defMat,0.0,Out+frontStr+backStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 3N -4N  15 -15M ");
 if (WI!=buildIndex+13000)
      makeCell("GapCC",System,cellIndex++,waxMat,0.0,Out+frontStr+backStr);
    }


      if (WI==buildIndex+13000)
	{	Out=ModelSupport::getComposite(SMap,buildIndex,LI+20,WI," 41N -4N 5 -5M ");
	  makeCell("BlockR",System,cellIndex++,defMat,0.0,Out+frontStr+backStr);
	  //  	Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 12 41N -4N -111 -6 5M ");
	  //	  makeCell("BlockR",System,cellIndex++,defMat,0.0,Out+frontStr);
     Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 3N -41N  5 -5M ");
      makeCell("BlockCC",System,cellIndex++,defMat,0.0,Out+frontStr+backStr);	 
     Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI," 3N -41N  15 -15M ");
      makeCell("GapCC",System,cellIndex++,waxMat,0.0,Out+frontStr+backStr);	} 
     
      // Waxed gap
    
 WI+=1000;   }

  
  WI-=1000;
  //  std::cout << "Outer" <<std::endl;
  Out=ModelSupport::getComposite(SMap,buildIndex,LI,WI, " 3 -4N 5 -6 ")+frontStr+backStr;
      addOuterSurf(Out+frontStr+backStr);

  return;
  
}

void
CaveMicoBlock::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("CaveMicoBlock","createLinks");

 
  double wid1;
  if (wid1F>wid1B) wid1=wid1F; else wid1= wid1B;
    
  double length=l1edge+l2edge;

  FixedComp::setConnect(2,Origin-X*0.5*(14*wid1+10.8),-X);
  FixedComp::setConnect(3,Origin+X*0.5*(14*wid1+10.8),X);
  FixedComp::setConnect(4,Origin-Z*(hup+hlow),-Z);
  FixedComp::setConnect(5,Origin,Z);


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
CaveMicoBlock::setFront(const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Set front surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("CaveMicoBlock","setFront");
  
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
CaveMicoBlock::setBack(const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Set back surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("CaveMicoBlock","setBack");
  
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
CaveMicoBlock::getXSectionIn() const
  /*!
    Get the line shield inner void section
    \return HeadRule of cross-section
   */
{
  ELog::RegMethod RegA("CaveMicoBlock","getXSectionIn");
  const std::string Out=
    ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 ");
  HeadRule HR(Out);
  HR.populateSurf();
  return HR;
}
  
    
void
CaveMicoBlock::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("CaveMicoBlock","createAll(FC)");

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
