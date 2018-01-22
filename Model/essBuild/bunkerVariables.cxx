/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/bunkerVariables.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell/Konstantin Batkov
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
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "variableSetup.h"
#include "essVariables.h"

namespace setVariable
{

void
setBunkerVar(FuncDataBase&,const std::string&,const std::string&);



void
setBunkerVar(FuncDataBase& Control,const std::string& AKey,
	     const std::string& BKey)
/*!
    Create all the bunker variables
    \param Control :: DataBase
    \param AKey :: Key for A/C bunker
    \param BKey :: Key for B/D bunker
*/
{
  ELog::RegMethod RegA("bunkerVariables[F]","setBunkerVar");

  Control.addVariable(AKey+"BunkerLeftPhase",-65.0);
  Control.addVariable(AKey+"BunkerRightPhase",-12.0);
  Control.addVariable(AKey+"BunkerLeftAngle",0.0);
  Control.addVariable(AKey+"BunkerRightAngle",0.0);
  Control.addVariable(AKey+"BunkerNSectors",10);
  Control.addVariable(AKey+"BunkerNVert",1);
  Control.addVariable(AKey+"BunkerNLayers",1);
  Control.addVariable(AKey+"BunkerNSegment",5);
  Control.addVariable(AKey+"BunkerRoofActive",0);
  Control.addVariable(AKey+"BunkerWallRadius",2450.0);
  Control.addVariable(BKey+"BunkerWallRadius",1150.0);
  
  for(const std::string& KItem : {AKey,BKey})
    {
      Control.addVariable(KItem+"BunkerWallActive",0);

      Control.addVariable(KItem+"BunkerWallNVert",8);
      Control.addVariable(KItem+"BunkerWallNMedial",8);
      Control.addVariable(KItem+"BunkerWallNRadial",0);

      Control.addVariable(KItem+"BunkerRoofNVert",20);
      Control.addVariable(KItem+"BunkerRoofNMedial",1);
      Control.addVariable(KItem+"BunkerRoofNRadial",12);
      
      Control.addVariable(KItem+"BunkerNSide",5);
      Control.addVariable(KItem+"BunkerNSideVert",5);
      Control.addVariable(KItem+"BunkerNSideThick",5);

      Control.addVariable(KItem+"BunkerFloorDepth",120.0);
      Control.addVariable(KItem+"BunkerRoofHeight",150.0);
      Control.addVariable(KItem+"BunkerWallHeight",170.0); 
      
      Control.addVariable(KItem+"BunkerWallThick",350.0);
      Control.addVariable(KItem+"BunkerSideThick",80.0);
      Control.addVariable(KItem+"BunkerRoofThick",175.0);
      Control.addVariable(KItem+"BunkerFloorThick",100.0);
      
      Control.addVariable(KItem+"BunkerVoidMat","Void");
      Control.addVariable(KItem+"BunkerWallMat","Steel71");
      Control.addVariable(KItem+"BunkerRoofMat","Aluminium");


      // WALL LAYERED
      Control.addVariable(KItem+"BunkerWallNBasic",15);      

      // ROOF LAYERED
      Control.addVariable(KItem+"BunkerRoofNBasicVert",8);
      Control.addVariable(KItem+"BunkerRoofVert1",20.0);   // Void [below]
      Control.addVariable(KItem+"BunkerRoofVert2",10.0);   // B poly
      Control.addVariable(KItem+"BunkerRoofVert3",5.0);    // steel
      Control.addVariable(KItem+"BunkerRoofVert4",20.0);
      Control.addVariable(KItem+"BunkerRoofVert5",45.0);   // steel
      Control.addVariable(KItem+"BunkerRoofVert6",40.0);  
      Control.addVariable(KItem+"BunkerRoofVert7",25.0);   // steel
  
      
      Control.addVariable(KItem+"BunkerRoofMat0","Void");
      Control.addVariable(KItem+"BunkerRoofMat1","Poly");
      Control.addVariable(KItem+"BunkerRoofMat2","ChipIRSteel");
      Control.addVariable(KItem+"BunkerRoofMat3","Poly");
      Control.addVariable(KItem+"BunkerRoofMat4","ChipIRSteel");
      Control.addVariable(KItem+"BunkerRoofMat5","Poly");
      Control.addVariable(KItem+"BunkerRoofMat6","ChipIRSteel");
      Control.addVariable(KItem+"BunkerRoofMat7","Poly");


      Control.addVariable(KItem+"BunkerWallMat0","Poly");
      Control.addVariable(KItem+"BunkerWallMat1","ChipIRSteel");
      Control.addVariable(KItem+"BunkerWallMat2","Poly");
      Control.addVariable(KItem+"BunkerWallMat3","ChipIRSteel");
      Control.addVariable(KItem+"BunkerWallMat4","Poly");
      Control.addVariable(KItem+"BunkerWallMat5","ChipIRSteel");
      Control.addVariable(KItem+"BunkerWallMat6","Poly");
      Control.addVariable(KItem+"BunkerWallMat7","ChipIRSteel");
      Control.addVariable(KItem+"BunkerWallMat8","Poly");
      Control.addVariable(KItem+"BunkerWallMat9","ChipIRSteel");
      Control.addVariable(KItem+"BunkerWallMat10","Poly");
      Control.addVariable(KItem+"BunkerWallMat11","ChipIRSteel");
      Control.addVariable(KItem+"BunkerWallMat12","Poly");
      Control.addVariable(KItem+"BunkerWallMat13","ChipIRSteel");
      Control.addVariable(KItem+"BunkerWallMat14","Poly");
      
      Control.addVariable(KItem+"BunkerWallLen1",45.0);   // p
      Control.addVariable(KItem+"BunkerWallLen2",15.0);   // s 1
      Control.addVariable(KItem+"BunkerWallLen3",15.0);   // p
      Control.addVariable(KItem+"BunkerWallLen4",30.0);   // s 3 
      Control.addVariable(KItem+"BunkerWallLen5",15.0);   // p
      Control.addVariable(KItem+"BunkerWallLen6",20.0);   // s 5
      Control.addVariable(KItem+"BunkerWallLen7",15.0);   // p
      Control.addVariable(KItem+"BunkerWallLen8",20.0);   // s 6
      Control.addVariable(KItem+"BunkerWallLen9",30.0);   // p 
      Control.addVariable(KItem+"BunkerWallLen10",15.0);  // s   
      Control.addVariable(KItem+"BunkerWallLen11",31.0);  // p
      Control.addVariable(KItem+"BunkerWallLen12",15.5);  // s
      Control.addVariable(KItem+"BunkerWallLen13",25.5);  // p
      Control.addVariable(KItem+"BunkerWallLen14",15.5);  // s
    }
  
  Control.addVariable(AKey+"BunkerLoadFile","ABunkerDef.xml");
  Control.addVariable(AKey+"BunkerOutFile","ABunker.xml");

  // PILLARS:
  
  Control.addVariable(AKey+"BunkerPillarsWidth",18.0);
  Control.addVariable(AKey+"BunkerPillarsDepth",18.0);
  Control.addVariable(AKey+"BunkerPillarsThick",1.0);
  Control.addVariable(AKey+"BunkerPillarsMat","Stainless304");
  Control.addVariable(AKey+"BunkerPillarsInnerMat","Poly%Void%50.0");
  Control.addVariable(AKey+"BunkerPillarsNRadius",7);
  Control.addVariable(AKey+"BunkerPillarsNSector",9); // default

  Control.addVariable(AKey+"BunkerPillarsTopFootWidth",35.0);
  Control.addVariable(AKey+"BunkerPillarsTopFootDepth",35.0);
  Control.addVariable(AKey+"BunkerPillarsTopFootHeight",29.9);  // vertical
  Control.addVariable(AKey+"BunkerPillarsTopFootThick",11.0);  // vertical
  Control.addVariable(AKey+"BunkerPillarsTopFootGap",3.0);

  Control.addVariable(AKey+"BunkerPillarsBeamWidth",12.0);
  Control.addVariable(AKey+"BunkerPillarsBeamWallThick",2.0);
  Control.addVariable(AKey+"BunkerPillarsBeamRoofThick",10.0);
  Control.addVariable(AKey+"BunkerPillarsBeamWallGap",5.0);
  

  Control.addVariable(AKey+"BunkerPillarsR0",600.0);
  Control.addVariable(AKey+"BunkerPillarsR1",981.0);
  Control.addVariable(AKey+"BunkerPillarsR2",1309.0);
  Control.addVariable(AKey+"BunkerPillarsR3",1581.0);
  Control.addVariable(AKey+"BunkerPillarsR4",1854.0);
  Control.addVariable(AKey+"BunkerPillarsR5",2127.0);
  Control.addVariable(AKey+"BunkerPillarsR6",2400.0);
  
  Control.addVariable(AKey+"BunkerPillarsRS_0",-15.0);
  Control.addVariable(AKey+"BunkerPillarsRS_1",-21.0);
  Control.addVariable(AKey+"BunkerPillarsRS_2",-27.0);
  Control.addVariable(AKey+"BunkerPillarsRS_3",-34.0);
  Control.addVariable(AKey+"BunkerPillarsRS_4",-39.0);
  Control.addVariable(AKey+"BunkerPillarsRS_5",-46.0);
  Control.addVariable(AKey+"BunkerPillarsRS_6",-52.0);
  Control.addVariable(AKey+"BunkerPillarsRS_7",-57.0);
  Control.addVariable(AKey+"BunkerPillarsRS_8",-63.0);

  //  Control.addVariable(AKey+"BunkerPillarsR_0S_1Active",0);
  //  Control.addVariable(AKey+"BunkerPillarsR_0S_2Active",0);
  Control.addVariable(AKey+"BunkerPillarsR_0S_5Active",0);
  Control.addVariable(AKey+"BunkerPillarsR_0S_6Active",0);
  //  Control.addVariable(AKey+"BunkerPillarsR_1S_2Active",0);


  Control.addVariable(AKey+"BunkerPillarsNXBeam",-8);
  Control.addVariable(AKey+"BunkerPillarsXBeam0A","R_1S_0");
  Control.addVariable(AKey+"BunkerPillarsXBeam1A","R_1S_1");
  Control.addVariable(AKey+"BunkerPillarsXBeam2A","R_1S_2");
  Control.addVariable(AKey+"BunkerPillarsXBeam3A","R_2S_3");
  Control.addVariable(AKey+"BunkerPillarsXBeam4A","R_2S_4");
  Control.addVariable(AKey+"BunkerPillarsXBeam5A","R_3S_5");
  Control.addVariable(AKey+"BunkerPillarsXBeam6A","R_4S_6");
  Control.addVariable(AKey+"BunkerPillarsXBeam7A","R_5S_7");


  Control.addVariable(AKey+"BunkerPillarsNLBeam",-15);

  // R0 BLOCK
  Control.addVariable(AKey+"BunkerPillarsLBeam0A","R_0S_0");
  Control.addVariable(AKey+"BunkerPillarsLBeam1A","R_0S_1");
  Control.addVariable(AKey+"BunkerPillarsLBeam2A","R_0S_2");
  Control.addVariable(AKey+"BunkerPillarsLBeam3A","R_1S_2");
  Control.addVariable(AKey+"BunkerPillarsLBeam4A","R_1S_3");
  Control.addVariable(AKey+"BunkerPillarsLBeam5A","R_1S_4");
  Control.addVariable(AKey+"BunkerPillarsLBeam6A","R_2S_5");
  Control.addVariable(AKey+"BunkerPillarsLBeam7A","R_3S_6");
  Control.addVariable(AKey+"BunkerPillarsLBeam8A","R_4S_7");

  Control.addVariable(AKey+"BunkerPillarsLBeam9A","R_0S_8");
  Control.addVariable(AKey+"BunkerPillarsLBeam10A","R_1S_8");
  Control.addVariable(AKey+"BunkerPillarsLBeam11A","R_2S_8");
  Control.addVariable(AKey+"BunkerPillarsLBeam12A","R_3S_8");
  Control.addVariable(AKey+"BunkerPillarsLBeam13A","R_4S_8");
  Control.addVariable(AKey+"BunkerPillarsLBeam14A","R_5S_8");
    
  
  //
  // RIGHT BUNKER : B PART
  //
  Control.addVariable(BKey+"BunkerLeftPhase",-12.0);
  Control.addVariable(BKey+"BunkerRightPhase",65.0);
  Control.addVariable(BKey+"BunkerLeftAngle",0.0);
  Control.addVariable(BKey+"BunkerRightAngle",0.0);
  Control.addVariable(BKey+"BunkerNSectors",9);
  Control.addVariable(BKey+"BunkerNVert",1);
  Control.addVariable(BKey+"BunkerNLayers",1);

  
  Control.addVariable(BKey+"BunkerActiveSegment",0);
  Control.addVariable(BKey+"BunkerActiveRoof",0);

  Control.addVariable(BKey+"BunkerLoadFile","BBunkerDef.xml");
  Control.addVariable(BKey+"BunkerOutFile","BBunker.xml");


  Control.addVariable(BKey+"BunkerPillarsWidth",18.0);
  Control.addVariable(BKey+"BunkerPillarsDepth",18.0);
  Control.addVariable(BKey+"BunkerPillarsThick",1.0);
  Control.addVariable(BKey+"BunkerPillarsMat","Stainless304");
  Control.addVariable(BKey+"BunkerPillarsInnerMat","Poly%Void%50.0");
  Control.addVariable(BKey+"BunkerPillarsNRadius",2);
  Control.addVariable(BKey+"BunkerPillarsNSector",9); // default

  Control.addVariable(BKey+"BunkerPillarsTopFootWidth",35.0);
  Control.addVariable(BKey+"BunkerPillarsTopFootDepth",35.0);
  Control.addVariable(BKey+"BunkerPillarsTopFootHeight",29.9);  // vertical
  Control.addVariable(BKey+"BunkerPillarsTopFootThick",11.0);  // vertical
  Control.addVariable(BKey+"BunkerPillarsTopFootGap",3.0);

  Control.addVariable(BKey+"BunkerPillarsBeamWidth",12.0);
  Control.addVariable(BKey+"BunkerPillarsBeamWallThick",2.0);
  Control.addVariable(BKey+"BunkerPillarsBeamRoofThick",10.0);
  Control.addVariable(BKey+"BunkerPillarsBeamWallGap",5.0);
  

  Control.addVariable(BKey+"BunkerPillarsR0",600.0);
  Control.addVariable(BKey+"BunkerPillarsR1",900.0);
  
  Control.addVariable(BKey+"BunkerPillarsRS_0",3.0);
  Control.addVariable(BKey+"BunkerPillarsRS_1",9.0);
  Control.addVariable(BKey+"BunkerPillarsRS_2",15.0);
  Control.addVariable(BKey+"BunkerPillarsRS_3",21.0);
  Control.addVariable(BKey+"BunkerPillarsRS_4",27.0);
  Control.addVariable(BKey+"BunkerPillarsRS_5",34.0);
  Control.addVariable(BKey+"BunkerPillarsRS_6",39.0);
  Control.addVariable(BKey+"BunkerPillarsRS_7",46.0);
  Control.addVariable(BKey+"BunkerPillarsRS_8",52.0);
  Control.addVariable(BKey+"BunkerPillarsRS_9",57.0);
  Control.addVariable(BKey+"BunkerPillarsRS_10",63.0);

  //  Control.addVariable(AKey+"BunkerPillarsR_0S_1Active",0);
  //  Control.addVariable(AKey+"BunkerPillarsR_0S_2Active",0);
  //  Control.addVariable(AKey+"BunkerPillarsR_0S_5Active",0);
  //  Control.addVariable(AKey+"BunkerPillarsR_0S_6Active",0);
  //  Control.addVariable(AKey+"BunkerPillarsR_1S_2Active",0);


  Control.addVariable(BKey+"BunkerPillarsNXBeam",0);
  Control.addVariable(BKey+"BunkerPillarsXBeam0A","R_1S_0");
  Control.addVariable(BKey+"BunkerPillarsXBeam1A","R_1S_1");
  Control.addVariable(BKey+"BunkerPillarsXBeam2A","R_1S_2");
  Control.addVariable(BKey+"BunkerPillarsXBeam3A","R_2S_3");
  Control.addVariable(BKey+"BunkerPillarsXBeam4A","R_2S_4");
  Control.addVariable(BKey+"BunkerPillarsXBeam5A","R_3S_5");
  Control.addVariable(BKey+"BunkerPillarsXBeam6A","R_4S_6");
  Control.addVariable(BKey+"BunkerPillarsXBeam7A","R_5S_7");


  Control.addVariable(BKey+"BunkerPillarsNLBeam",0);

  // R0 BLOCK
  Control.addVariable(BKey+"BunkerPillarsLBeam0A","R_0S_0");
  Control.addVariable(BKey+"BunkerPillarsLBeam1A","R_0S_1");
  Control.addVariable(BKey+"BunkerPillarsLBeam2A","R_0S_2");
  Control.addVariable(BKey+"BunkerPillarsLBeam3A","R_1S_2");
  Control.addVariable(BKey+"BunkerPillarsLBeam4A","R_1S_3");
  Control.addVariable(BKey+"BunkerPillarsLBeam5A","R_1S_4");
  Control.addVariable(BKey+"BunkerPillarsLBeam6A","R_2S_5");
  Control.addVariable(BKey+"BunkerPillarsLBeam7A","R_3S_6");
  Control.addVariable(BKey+"BunkerPillarsLBeam8A","R_4S_7");

  Control.addVariable(BKey+"BunkerPillarsLBeam9A","R_0S_8");
  Control.addVariable(BKey+"BunkerPillarsLBeam10A","R_1S_8");
  Control.addVariable(BKey+"BunkerPillarsLBeam11A","R_2S_8");
  Control.addVariable(BKey+"BunkerPillarsLBeam12A","R_3S_8");
  Control.addVariable(BKey+"BunkerPillarsLBeam13A","R_4S_8");
  Control.addVariable(BKey+"BunkerPillarsLBeam14A","R_5S_8");

  
  return;
}

void
EssBunkerVariables(FuncDataBase& Control)
  /*!
    Create all the bunker variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("essVariables[F]","EssBunkerVariables");
  setBunkerVar(Control,"A","B");
  setBunkerVar(Control,"C","D");

  // BUNKER QUAKE:
  Control.addVariable("ABunkerQuake0XGap",6.5);
  Control.addVariable("ABunkerQuake0ZGap",100.0);
  
  Control.addVariable("ABunkerQuakeNPath",24);

  Control.addVariable("ABunkerQuake0NPoint",11);
  Control.addVariable("ABunkerQuake0XGap",6.5);
  Control.addVariable("ABunkerQuake0ZGap",50.0);
  Control.addVariable("ABunkerQuake0XStep",0.0);
  Control.addVariable("ABunkerQuake0YStep",0.0);
  Control.addVariable("ABunkerQuake0ZStep",20.0);

  // top dilitation
  Control.addVariable("ABunkerQuake0PtA0",Geometry::Vec3D(785,103,0));
  Control.addVariable("ABunkerQuake0PtA1",Geometry::Vec3D(595,79,0));
  Control.addVariable("ABunkerQuake0PtA2",Geometry::Vec3D(491,332,0));
  Control.addVariable("ABunkerQuake0PtA3",Geometry::Vec3D(819,545,0));
  Control.addVariable("ABunkerQuake0PtA4",Geometry::Vec3D(673,703,0));
  Control.addVariable("ABunkerQuake0PtA5",Geometry::Vec3D(894,940,0));
  Control.addVariable("ABunkerQuake0PtA6",Geometry::Vec3D(809,1036,0));
  Control.addVariable("ABunkerQuake0PtA7",Geometry::Vec3D(977,1244,0));
  Control.addVariable("ABunkerQuake0PtA8",Geometry::Vec3D(859,1324,0));
  Control.addVariable("ABunkerQuake0PtA9",Geometry::Vec3D(1003,1553,0));
  Control.addVariable("ABunkerQuake0PtA10",Geometry::Vec3D(841,1652,0));

  // Midlayer dilitation
  Control.addVariable("ABunkerQuake1NPoint",9);
  Control.addVariable("ABunkerQuake1XGap",6.5);
  Control.addVariable("ABunkerQuake1ZGap",60.0);
  Control.addVariable("ABunkerQuake1XStep",0.0);
  Control.addVariable("ABunkerQuake1YStep",0.0);
  Control.addVariable("ABunkerQuake1ZStep",70.0);

  Control.addVariable("ABunkerQuake1PtA0",Geometry::Vec3D(1111.3,253.0,0));
  Control.addVariable("ABunkerQuake1PtA1",Geometry::Vec3D(646,151.0,0));
  Control.addVariable("ABunkerQuake1PtA2",Geometry::Vec3D(575,349,0)); //curve
  Control.addVariable("ABunkerQuake1PtA3",Geometry::Vec3D(1068,683,0));
  Control.addVariable("ABunkerQuake1PtA4",Geometry::Vec3D(905,882,0));
  Control.addVariable("ABunkerQuake1PtA5",Geometry::Vec3D(1264,1255,0));
  Control.addVariable("ABunkerQuake1PtA6",Geometry::Vec3D(984,1451,0));
  Control.addVariable("ABunkerQuake1PtA7",Geometry::Vec3D(1258,1876,0));
  Control.addVariable("ABunkerQuake1PtA8",Geometry::Vec3D(927,2043,0));

  Control.addVariable("ABunkerQuake2NPoint",8);
  Control.addVariable("ABunkerQuake2XGap",6.5);
  Control.addVariable("ABunkerQuake2ZGap",45.0);
  Control.addVariable("ABunkerQuake2XStep",0.0);
  Control.addVariable("ABunkerQuake2YStep",0.0);
  Control.addVariable("ABunkerQuake2ZStep",130.0);

  // top dilitation
  Control.addVariable("ABunkerQuake2PtA0",Geometry::Vec3D(1347.0,317.0,0));
  Control.addVariable("ABunkerQuake2PtA1",Geometry::Vec3D(1317,424,0));
  Control.addVariable("ABunkerQuake2PtA2",Geometry::Vec3D(749,241,0));
  Control.addVariable("ABunkerQuake2PtA3",Geometry::Vec3D(689,396,0));
  Control.addVariable("ABunkerQuake2PtA4",Geometry::Vec3D(1192,711,0));
  Control.addVariable("ABunkerQuake2PtA5",Geometry::Vec3D(1017,923,0));
  Control.addVariable("ABunkerQuake2PtA6",Geometry::Vec3D(1490,1344,0));
  Control.addVariable("ABunkerQuake2PtA7",Geometry::Vec3D(917,1784,0));

  // Top Gaps
  Control.addVariable("ABunkerQuake3NPoint",2);
  Control.addVariable("ABunkerQuake3XGap",2.5);
  Control.addVariable("ABunkerQuake3ZGap",45.0);
  Control.addVariable("ABunkerQuake3XStep",0.0);
  Control.addVariable("ABunkerQuake3YStep",0.0);
  Control.addVariable("ABunkerQuake3ZStep",130.0);

  Control.addVariable("ABunkerQuake3PtA0",Geometry::Vec3D(600,0.0,0.0));
  Control.addVariable("ABunkerQuake3PtA1",Geometry::Vec3D(1350,0.0,0.0));

  std::vector<int> index({0,7,14});
  std::vector<double> ZStep({20.0,70.0,130.0});
  std::vector<double> ZGap({50,60,45.0});
  std::vector<double> AOffset({30.0,33.75,30.0});
  for(size_t i=0;i<3;i++)
    {
      const std::vector<double> AVec({AOffset[i]-60.0/16.0,AOffset[i]+60.0/16.0});
      std::string QP="ABunkerQuake"+std::to_string(index[i]+4);
      Control.addVariable(QP+"NPoint",2);
      Control.addVariable(QP+"XGap",2.5);
      Control.addVariable(QP+"ZGap",ZGap[i]);
      Control.addVariable(QP+"ZStep",ZStep[i]);
      Geometry::Vec3D AAxis(cos(M_PI*AVec[0]/180.0),sin(M_PI*AVec[0]/180.0),0);
      Control.addVariable(QP+"PtA0",AAxis*600.0);
      Control.addVariable(QP+"PtA1",AAxis*2750.0);

      QP="ABunkerQuake"+std::to_string(index[i]+5);
      Control.addVariable(QP+"NPoint",2);
      Control.addVariable(QP+"XGap",2.5);
      Control.addVariable(QP+"ZGap",ZGap[i]);
      Control.addVariable(QP+"ZStep",ZStep[i]);
      
      Geometry::Vec3D BAxis(cos(M_PI*AVec[1]/180.0),sin(M_PI*AVec[1]/180.0),0);
      Control.addVariable(QP+"PtA0",BAxis*600.0);
      Control.addVariable(QP+"PtA1",BAxis*2750.0);
      
      QP="ABunkerQuake6";
      QP="ABunkerQuake"+std::to_string(index[i]+6);
      Control.addVariable(QP+"NPoint",2);
      Control.addVariable(QP+"XGap",2.5);
      Control.addVariable(QP+"ZGap",ZGap[i]);
      Control.addVariable(QP+"ZStep",ZStep[i]);
      Control.addVariable(QP+"PtA0",AAxis*900.0);
      Control.addVariable(QP+"PtA1",BAxis*900.0);
      
      QP="ABunkerQuake7";
      QP="ABunkerQuake"+std::to_string(index[i]+7);
      Control.addVariable(QP+"NPoint",2);
      Control.addVariable(QP+"XGap",2.5);
      Control.addVariable(QP+"ZGap",ZGap[i]);
      Control.addVariable(QP+"ZStep",ZStep[i]);
      Control.addVariable(QP+"PtA0",AAxis*1200.0);
      Control.addVariable(QP+"PtA1",BAxis*1200.0);
      
      QP="ABunkerQuake8";
      QP="ABunkerQuake"+std::to_string(index[i]+8);
      Control.addVariable(QP+"NPoint",2);
      Control.addVariable(QP+"XGap",2.5);
      Control.addVariable(QP+"ZGap",ZGap[i]);
      Control.addVariable(QP+"ZStep",ZStep[i]);
      Control.addVariable(QP+"PtA0",AAxis*1500.0);
      Control.addVariable(QP+"PtA1",BAxis*1500.0);
      
      QP="ABunkerQuake9";
      QP="ABunkerQuake"+std::to_string(index[i]+9);
      Control.addVariable(QP+"NPoint",2);
      Control.addVariable(QP+"XGap",2.5);
      Control.addVariable(QP+"ZGap",ZGap[i]);
      Control.addVariable(QP+"ZStep",ZStep[i]);
      Control.addVariable(QP+"PtA0",AAxis*1800.0);
      Control.addVariable(QP+"PtA1",BAxis*1800.0);
      
      QP="ABunkerQuake10";
      QP="ABunkerQuake"+std::to_string(index[i]+10);
      Control.addVariable(QP+"NPoint",2);
      Control.addVariable(QP+"XGap",2.5);
      Control.addVariable(QP+"ZGap",ZGap[i]);
      Control.addVariable(QP+"ZStep",ZStep[i]);
      Control.addVariable(QP+"PtA0",AAxis*2100.0);
      Control.addVariable(QP+"PtA1",BAxis*2100.0);
    }
  

  // BBUNKER : 
  Control.addVariable("BBunkerQuakeNPath",1);
  Control.addVariable("BBunkerQuake0NPoint",2);
  Control.addVariable("BBunkerQuake0XGap",5.0);
  Control.addVariable("BBunkerQuake0ZGap",22.0);
  Control.addVariable("BBunkerQuake0PtA0",Geometry::Vec3D(620,0,0));
  Control.addVariable("BBunkerQuake0PtA1",
		      Geometry::Vec3D(620.0/sqrt(2),-620.0/sqrt(2.0),0.0));
  Control.addVariable("BBunkerQuake0Radius0",620.0);
  Control.addVariable("BBunkerQuake0YFlag0",1);


  Control.addVariable("CurtainLeftPhase",-65.0);
  Control.addVariable("CurtainRightPhase",65.0);
  Control.addVariable("CurtainWallThick",30.0);
  Control.addVariable("CurtainInnerStep",30.0);
  Control.addVariable("CurtainBaseGap",6.0);
  Control.addVariable("CurtainOuterGap",5.0);
  Control.addVariable("CurtainTopRaise",60.0);
  Control.addVariable("CurtainHeight",450.0);
  Control.addVariable("CurtainDepth",186.0);     // fixed by Ben.
  Control.addVariable("CurtainWallMat","Concrete");
  Control.addVariable("CurtainNTopLayers",1);   // 7 
  Control.addVariable("CurtainNMidLayers",1);   // 2 
  Control.addVariable("CurtainNBaseLayers",3);  // 7

  Control.addVariable("CurtainBaseLen1",70.0);
  Control.addVariable("CurtainBaseLen2",6.0);
  Control.addVariable("CurtainBaseMat0","Stainless304");
  Control.addVariable("CurtainBaseMat1","Void");
  Control.addVariable("CurtainBaseMat2","Concrete"); 

  Control.addVariable("BunkerFeedOffset",Geometry::Vec3D(0,0,0));
  Control.addVariable("BunkerFeedHeight",6.0);
  Control.addVariable("BunkerFeedWidth",6.0);
  Control.addVariable("BunkerFeedTrack0",Geometry::Vec3D(0,0,0));
  Control.addVariable("BunkerFeedTrack1",Geometry::Vec3D(0,0,-10));
  Control.addVariable("BunkerFeedTrack2",Geometry::Vec3D(0,100,-10));

  Control.addVariable("BunkerChicane0NBlocks", 1);
  Control.addVariable("BunkerChicane0ZStep",200.0);
  Control.addVariable("BunkerChicane0Length", 200.0);
  Control.addVariable("BunkerChicane0Width",  20.0);
  Control.addVariable("BunkerChicane0Height", 30.0);
  Control.addVariable("BunkerChicane0Mat", "Void");


  Control.addVariable("ABHighBayLength",450.0);
  Control.addVariable("ABHighBayHeight",330.0);
  Control.addVariable("ABHighBayThick",200.0);
  Control.addVariable("ABHighBayWallMat","Concrete");
  Control.addVariable("ABHighBayRoofMat","Concrete"); 
  
  Control.addVariable("CDHighBayLength",450.0);
  Control.addVariable("CDHighBayHeight",330.0);
  Control.addVariable("CDHighBayThick",200.0);
  Control.addVariable("CDHighBayWallMat","Concrete");
  Control.addVariable("CDHighBayRoofMat","Concrete"); 
return;
}
  
}  // NAMESPACE setVariable
