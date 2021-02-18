/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essLinac/linacVariables.cxx
 *
 * Copyright (c) 2017-2021 by Konstantin Batkov
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
#include "PipeGenerator.h"
#include "variableSetup.h"
#include "essLinacVariableSetup.h"


// Scales are used to calculate non-marked dimensions:
// Scale of the plots in SPLTDISH0001:  1024.0/70
//const double SCALE1(14.6286);
// Scale of the plots in SPLTDISH0052 (top-left corner and central part):  620.0/106
const double SCALE52tl(5.84906);
// Scale of the plots in SPLTDISH0052 (top-right corner):  267.52/92
const double SCALE52tr(2.90783);
// Scale of the plots in SPLTDISH0052 (bottom-left corner):  14.5/10
const double SCALE52bl(1.45);
// Scale of the plots in Fc_design.pdf received from LT 13 March 2017
const double SCALE3(32.0/34.0);

namespace setVariable
{

void
EssLinacVariables(FuncDataBase& Control)
  /*!
    Create all the linac variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("essVariables[F]","EssLinacVariables");

  Control.addVariable("EngineeringActive",0);

  Control.addVariable("LinacEngineeringActive",1);
  const double dtl1Start(1113.1972); // Revised from lattice files from Mamad 2020
  Control.addVariable("LinacLengthBack",-dtl1Start);
  Control.addVariable("LinacLengthFront",51044.4); // MARS
  Control.addVariable("LinacWidthLeft",600./2.0+15.0); // K01-20---6-G01---011  + email from Pontus 26 Sep 2018
  Control.addVariable("LinacWidthRight",600./2.0-15.0); // K01-20---6-G01---011 + email from Pontus 26 Sep 2018
  Control.addVariable("LinacHeight",200.0); // Height+Depth from K01-20---6-G01---011; center communicated by Lali
  Control.addVariable("LinacDepth",150.0); // Height+Depth from K01-20---6-G01---011; center communicated by Lali
  Control.addVariable("LinacWallThick",50.0); // K01-20---6-G01---011
  Control.addVariable("LinacRoofThick",80.0); // ESS-0014987
  Control.addVariable("LinacFloorThick",75.0); // ESS-0014987
  Control.addVariable("LinacFloorWidthLeft",760.0/2.0+15.0); // K01-20---6-G01---011
  Control.addVariable("LinacFloorWidthRight",760.0/2.0-15.0); // K01-20---6-G01---011
  Control.addVariable("LinacWallMat","SkanskaConcrete");
 
  Control.addVariable("LinacAirMat","Air");
  Control.addVariable("LinacSteelMat","Stainless304"); // Lali says, but promised to check
  Control.addVariable("LinacAlMat","Aluminium"); // check it !!!
  Control.addVariable("LinacWaterMat","H2O");
  Control.addVariable("LinacCopperMat","Silver%Copper%0.015"); // Copper + 0.015% weight Ag since Ag is a gamma source (YJL said)
  Control.addVariable("LinacCopperMat","Copper");
  Control.addVariable("LinacGraphiteMat","Graphite");


  // Temporary shielding walls
  // Next two lines: Lali commnuicated: 15.05.2018
  Control.addVariable("LinacNTSW", 0);
  Control.addParse<double>("LinacTSW0Length", "LinacWidthLeft+LinacWidthRight");
  // To be optimised
  Control.addVariable("LinacTSW0Width", 70.0);
  Control.addVariable("LinacTSW0XStep", 4760.0);
  Control.addVariable("LinacTSW0XYAngle", 0.0);
  Control.addVariable("LinacTSW0Mat", "SkanskaConcrete");
  Control.addVariable("LinacTSW0NLayers", 1); // for biasing

  Control.copyVarSet("LinacTSW0", "LinacTSW1");
  Control.addVariable("LinacTSW1XStep", 4110.0);
  Control.addVariable("LinacTSW1Width", 140.0);
  Control.copyVarSet("LinacTSW0", "LinacTSW2");
  Control.addVariable("LinacTSW2XStep", 4330.0);

  
  // Beam dump
  Control.addVariable("LinacBeamDumpActive", 0);
  
  Control.addVariable("LinacBeamDumpYStep",100); // arbitrary number to put if after last DTL

  // Exact definition of borated concrete needed [email 3 Nov 2016]
  Control.addVariable("LinacBeamDumpConcreteMat", "SkanskaConcrete%Boron%99");

  Control.addVariable("LinacBeamDumpFrontWallLength",4*5.0); // doc SPLTDISH0001
  Control.addVariable("LinacBeamDumpFrontWallHeight",50.0); // doc SPLTDISH0001
  Control.addVariable("LinacBeamDumpFrontWallDepth",50.0); // doc SPLTDISH0001
  Control.addVariable("LinacBeamDumpFrontWallWidth",40.0); // doc SPLTDISH0001
  Control.addVariable("LinacBeamDumpFrontWallHoleRad",16.5/2.0); // doc SPLTDISH0001

  Control.addVariable("LinacBeamDumpFrontInnerWallHeight",15.0); // doc SPLTDISH0001
  Control.addVariable("LinacBeamDumpFrontInnerWallDepth",15.0); // doc SPLTDISH0001
  Control.addVariable("LinacBeamDumpFrontInnerWallLength",2*5.0); // doc SPLTDISH0001
  Control.addVariable("LinacBeamDumpFrontInnerWallHoleRad",10.4/2.0); // doc SPLTDISH0001
  Control.addVariable("LinacBeamDumpBackInnerWallLength",2*5.0+3.0); // doc SPLTDISH0001
  //ELog::EM << "measure LinacBeamDumpBackInnerWallGapLength" << ELog::endCrit;
  Control.addVariable("LinacBeamDumpBackInnerWallGapLength",1.0); // measure!

  Control.addVariable("LinacBeamDumpSideInnerWallThick", 2*5.0); // doc SPLTDISH0001
  Control.addVariable("LinacBeamDumpSideWallThick", 10.0); // Measure SPLTDISH0001 is 0.7*SCALE1=10.24 cm, but Lali & I decided to round it to 10 cm.

  Control.addVariable("LinacBeamDumpFloorLength",100.0); // doc SPLTDISH0001
  Control.addVariable("LinacBeamDumpFloorDepth",5*5.0); // doc SPLTDISH0001
  
  Control.addVariable("LinacBeamDumpPlate25Length",35.0); // doc SPLTDISH0001
  Control.addVariable("LinacBeamDumpPlate25Depth",3.0); // doc SPLTDISH0001

  // guess based on SPLTDISH0001: from the drawing, it should be the same as
  // height of plate 15, but it looks like plate 15 height (5cm) is wrong if
  // I compare it with e.g. plate 16 height
  Control.addVariable("LinacBeamDumpPlate38Depth",3.0);

  Control.addVariable("LinacBeamDumpBackWallLength",20.0);  // email Lali 3 Nov 2016
  Control.addParse<double>("LinacBeamDumpBackWallDepth",
			   "LinacBeamDumpFrontInnerWallDepth+LinacBeamDumpFloorDepth+LinacBeamDumpPlate25Depth");

  Control.addVariable("LinacBeamDumpRoofThick",20.0); // email from Lali 3 Nov 2016
  Control.addVariable("LinacBeamDumpRoofOverhangLength", 20.0); // guess based on SPLTDISH0001
  Control.addVariable("LinacBeamDumpInnerRoofThick",2*5.0); // doc SPLTDISH0001
  
  //ELog::EM << "measure LinacBeamDumpVacPipeFrontInnerWallDist" << ELog::endCrit;
  Control.addVariable("LinacBeamDumpVacPipeFrontInnerWallDist",5.0); // measure !!!
  Control.addVariable("LinacBeamDumpVacPipeLength",63.97); // SPLTDISH0052
  Control.addVariable("LinacBeamDumpVacPipeRad",6.85*SCALE52bl/2.0); // measured on SPLTDISH0052
  Control.addVariable("LinacBeamDumpVacPipeSideWallThick",(7.55-6.85)*SCALE52bl/2.0);  // measured on SPLTDISH0052

  Control.addVariable("LinacBeamDumpVacPipeLidRmax",10.4*SCALE52bl/2.0);    // measured on SPLTDISH0052
  Control.addVariable("LinacBeamDumpVacPipeLid1Length",1.7*SCALE52bl); // measured on SPLTDISH0052
  Control.addVariable("LinacBeamDumpVacPipeLid2Length",0.65*SCALE52tl);  // measured on SPLTDISH0052
  Control.addParse<double>("LinacBeamDumpVacPipeBaseLength",
			   "LinacBeamDumpVacPipeLength-LinacBeamDumpVacPipeLid1Length-5.95"); // top-left drawing in SPLTDISH0052
  Control.addVariable("LinacBeamDumpVacPipeOuterConeOffset", 10.8*SCALE52tr); // masured on SPLTDISH0052, see pencil note in the top-rigth part
  Control.addVariable("LinacBeamDumpVacPipeInnerConeTop", 2.0*SCALE52tr); // masured on SPLTDISH0052, see pencil note in the top-rigth part
  Control.addVariable("LinacBeamDumpWallThick", 0.9*SCALE52bl);   // measured on SPLTDISH0052
  
  Control.addVariable("LinacBeamDumpWaterPipeRadius", 0.25*SCALE52tr/2.0); // measured on SPLTDISH0052
  Control.addVariable("LinacBeamDumpWaterPipeLength", 10.2*SCALE52tr); // measured on SPLTDISH0052
  Control.addVariable("LinacBeamDumpWaterPipeOffsetX", 0.22*SCALE52tl); // measured on SPLTDISH0052
  Control.addVariable("LinacBeamDumpWaterPipeOffsetZ", 10.4); // measured on SPLTDISH005
  Control.addVariable("LinacBeamDumpWaterPipeDist", 0.05*SCALE52tr); // measured on SPLTDISH005

  
   // Faraday cup DTL 4 FC. Variables given also in script
  // Dimensions: https://confluence.esss.lu.se/display/BIG/DTL+FCs
  Control.addVariable("LinacFaradayCupActive", 1.0);
  Control.addVariable("LinacFaradayCupYStep", 206.3397); // arbitrary distance from the end of last DTL
  Control.addVariable("LinacFaradayCupEngineeringActive", 1);
  Control.addVariable("LinacFaradayCupLength", 3.25);
  Control.addVariable("LinacFaradayCupOuterRadius", 3.0);
  Control.addVariable("LinacFaradayCupInnerRadius", 2.0);
  Control.addVariable("LinacFaradayCupFaceLength", 0.0);
  Control.addVariable("LinacFaradayCupFaceRadius", 1.8);
  Control.addVariable("LinacFaradayCupAbsorberLength", 0.25);
  Control.addVariable("LinacFaradayCupAbsorberMat", "Graphite600K");
  Control.addVariable("LinacFaradayCupAbsorberTemp", 600.0);
  Control.addVariable("LinacFaradayCupCollectorLength", 0.7);
  Control.addParse<double>("LinacFaradayCupBaseLength",
		   "LinacFaradayCupLength-LinacFaradayCupAbsorberLength-LinacFaradayCupCollectorLength-0.5");
  Control.addVariable("LinacFaradayCupBaseMat", "Void"); // FC2: void, FC4: Graphite

  Control.addVariable("LinacFaradayCupCollectorMat", "Graphite");

  Control.addVariable("LinacFaradayCupWallMat", "Copper");

  // FC dimensiona as of Wolfgang Hees email 13 Feb 2019
  Control.addVariable("LinacFaradayCupNShieldLayers", 3);
  Control.addVariable("LinacFaradayCupShieldMat1","Air");
  Control.addVariable("LinacFaradayCupShieldMat2","SS304L");
  Control.addVariable("LinacFaradayCupShieldMat3","SkanskaConcrete");
  Control.addVariable("LinacFaradayCupShieldWidthLeft1",12.0);
  Control.addVariable("LinacFaradayCupShieldWidthLeft2",35.0);
  Control.addVariable("LinacFaradayCupShieldWidthLeft3",110.0);
  Control.addParse<double>("LinacFaradayCupShieldWidthRight1",
			   "LinacFaradayCupShieldWidthLeft1");
  Control.addParse<double>("LinacFaradayCupShieldWidthRight2",
			   "LinacFaradayCupShieldWidthLeft2");
  Control.addParse<double>("LinacFaradayCupShieldWidthRight3",
			   "LinacFaradayCupShieldWidthLeft3");
  Control.addVariable("LinacFaradayCupShieldHeight1",10.0);
  Control.addVariable("LinacFaradayCupShieldHeight2",25.0);
  Control.addVariable("LinacFaradayCupShieldHeight3",125.0);
  Control.addParse<double>("LinacFaradayCupShieldDepth1",
			   "LinacFaradayCupShieldHeight1");
  Control.addParse<double>("LinacFaradayCupShieldDepth2",
			   "LinacFaradayCupShieldHeight2");
  Control.addParse<double>("LinacFaradayCupShieldDepth3",
			   "LinacFaradayCupShieldHeight3");
  Control.addVariable("LinacFaradayCupShieldForwardLength1",10.0);
  Control.addVariable("LinacFaradayCupShieldForwardLength2",30.0);
  Control.addVariable("LinacFaradayCupShieldForwardLength3",130.0);
  Control.addVariable("LinacFaradayCupShieldBackLength",170.0);

    // Faraday cup DTL 2 FC.
  Control.copyVarSet("LinacFaradayCup", "LinacFC2");

  // Faraday cup DTL 2 FC.
  // Dimensions: https://confluence.esss.lu.se/display/BIG/DTL+FCs
  Control.addVariable("LinacFC2Length", 2.85); // ED 18 Feb 2019 (32.5 in Confluence is wrong)
  Control.addVariable("LinacFC2OuterRadius", 3.0);
  Control.addVariable("LinacFC2InnerRadius", 2.0);
  Control.addVariable("LinacFC2FaceLength", 0.0);
  Control.addVariable("LinacFC2FaceRadius", 1.8);
  Control.addVariable("LinacFC2AbsorberLength", 0.25);
  Control.addVariable("LinacFC2AbsorberMat", "Graphite600K");
  Control.addVariable("LinacFC2AbsorberTemp", 600.0);
  Control.addVariable("LinacFC2CollectorLength", 0.7);
  Control.addParse<double>("LinacFC2BaseLength",
			   "LinacFC2Length-LinacFC2AbsorberLength-LinacFC2CollectorLength-0.5");
  Control.addVariable("LinacFC2BaseMat", "Void"); // FC2: void, FC4: Graphite
  Control.addVariable("LinacFC2CollectorMat", "Graphite");
  Control.addVariable("LinacFC2WallMat", "Copper");

  // Faraday cup DTL 2 FC.
  Control.copyVarSet("LinacFaradayCup", "LinacFC1");

  // Faraday cup DTL 2 FC.
  // Dimensions: https://confluence.esss.lu.se/display/BIG/DTL+FCs
  Control.addVariable("LinacFC1XYAngle", 0.0);
  Control.addVariable("LinacFC1XStep", 0.0);
  Control.addVariable("LinacFC1Length", 2.85); // ED 18 Feb 2019 (32.5 in Confluence is wrong)
  Control.addVariable("LinacFC1OuterRadius", 3.0);
  Control.addVariable("LinacFC1InnerRadius", 2.0);
  Control.addVariable("LinacFC1FaceLength", 0.0);
  Control.addVariable("LinacFC1FaceRadius", 1.8);
  Control.addVariable("LinacFC1AbsorberLength", 0.25);
  Control.addVariable("LinacFC1AbsorberMat", "Graphite600K");
  Control.addVariable("LinacFC1AbsorberTemp", 600.0);
  Control.addVariable("LinacFC1CollectorLength", 0.7);
  Control.addParse<double>("LinacFC1BaseLength",
			   "LinacFC1Length-LinacFC1AbsorberLength-LinacFC1CollectorLength-0.5");
  Control.addVariable("LinacFC1BaseMat", "Void"); // FC1: void, FC4: Graphite
  Control.addVariable("LinacFC1CollectorMat", "Graphite");
  Control.addVariable("LinacFC1WallMat", "Copper");
  
  // DTL
  const size_t nDTL = 5;
  Control.addVariable("LinacNDTLTanks", nDTL);
  // DTL lengths are from Google drive / ESS DTL
  // 02 - Mechanical development and prototype construction.pdf
  // page 21
  // PMQ parameters are from:
  // DePrisco2015: 05 - PMQ Transverse Section and Data (Lali's google drive folder)
  Control.addVariable("LinacDTL1YStep", dtl1Start);

  Control.addVariable("LinacDTL1EngineeringActive", 0);
  //  Control.addVariable("LinacDTL1Length", 768.76);
  Control.addVariable("LinacDTL1IntertankLength", 15.3302); //updated to latest lattice file
  //  Control.addVariable("LinacDTL2Length", 717.11);
  Control.addVariable("LinacDTL2IntertankLength", 19.8368 ); //updated to latest lattice file
  //  Control.addVariable("LinacDTL3Length", 765.26);
  Control.addVariable("LinacDTL3IntertankLength", 24.3481); //updated to latest lattice file
  //  Control.addVariable("LinacDTL4Length", 791.71);
  Control.addVariable("LinacDTL4IntertankLength", 27.9125); //updated to lattest lattice file
  //  Control.addVariable("LinacDTL5Length", 775.79);
  Control.addVariable("LinacDTL5IntertankLength", 0.1); 
  
  Control.addVariable("LinacDTLIntertankRadius", 2.8); // MARS
  Control.addVariable("LinacDTLIntertankWallThick", 0.2); // MARS


  ELog::EM << "LinacDTL1Mat4: RB: mix of Cu with water; MARS: STST. What is correct?" << ELog::endCrit;

  std::string strtmp;
  for (size_t i=1; i<=nDTL; i++)
    {
      strtmp = std::to_string(i);
      Control.addVariable("LinacDTL"+strtmp+"PMQ1Mat5", "SS304L");  // DTL1 first half

      // these variables define radii of PMQ
      Control.addVariable("LinacDTL"+strtmp+"EngineeringActive", 0);
      Control.addVariable("LinacDTL"+strtmp+"NLayers", 7);
      Control.addVariable("LinacDTL"+strtmp+"Radius1", 1.0); // DePrisco2015, table 2
      Control.addVariable("LinacDTL"+strtmp+"Mat1", "Void");
      Control.addVariable("LinacDTL"+strtmp+"Radius2", 1.15); // DePrisco2015, table 2
      Control.addVariable("LinacDTL"+strtmp+"Mat2", "Copper"); // rbfrend2-9100
      Control.addVariable("LinacDTL"+strtmp+"Radius3", 2.9);
      Control.addVariable("LinacDTL"+strtmp+"Mat3", "SS304L");
      Control.addVariable("LinacDTL"+strtmp+"Radius4", 4.5);
      Control.addVariable("LinacDTL"+strtmp+"Mat4", "SS304L");
      Control.addVariable("LinacDTL"+strtmp+"Radius5", 25.95);  // DTL_model_picture.png - email from RB 14 Mar 2017
      Control.addVariable("LinacDTL"+strtmp+"Mat5", "Void");
      Control.addVariable("LinacDTL"+strtmp+"Radius6", 26);  // MARS
      Control.addVariable("LinacDTL"+strtmp+"Mat6", "Copper");
      Control.addVariable("LinacDTL"+strtmp+"Radius7", 31);  // MARS
      Control.addVariable("LinacDTL"+strtmp+"Mat7", "SS304L");
      Control.addVariable("LinacDTL"+strtmp+"AirMat", "Air");

      Control.addVariable("LinacDTL"+strtmp+"PMQNBars", 16);      // DePrisco2015, page 1
      Control.addVariable("LinacDTL"+strtmp+"PMQBarHeight", 1.4); // DePrisco2015, fig 2
      Control.addVariable("LinacDTL"+strtmp+"PMQBarThick",  0.4); // DePrisco2015, fig 2
      Control.addVariable("LinacDTL"+strtmp+"PMQBarMat",  "Sm2Co17"); // DePrisco2015, page 1
    }

  // PMQ lengths are read from the optics file and dumped in the
  // automaticall generated pmqVariables.cxx by the optics2var script
  EssLinacPMQVariables(Control);
  EssLinacSpokeVariables(Control); //Spoke Variables
  EssLinacMBetaVariables(Control); //MBCVT Variables
  EssLinacMEBTVariables(Control); //MEBT variables

  // Klystron Gallery
  Control.addParse<double>("KGLengthBack", "LinacLengthBack-100"); // -100 to avoid intersection with FEB
  Control.addVariable("KGWallThick", 20);
  //Control.addParse<double>("KGLengthFront", "37957.5-KGWallThick"); // ESS-0025905, number is approx -Batkov
  Control.addParse<double>("KGLengthFront", "47850.0-KGWallThick"); // 47850.0 from MARS
  // KG dimensions are measured from ESS-0308575 (email from Pontus 16.05)
  Control.addVariable("KGWidthLeft", 800);
  Control.addVariable("KGWidthRight", 800);
  Control.addVariable("KGHeight", 647/2.0);
  Control.addVariable("KGDepth", 647/2.0);
  Control.addVariable("KGRoofThick", 27.6);
  Control.addVariable("KGFloorThick", 20);
  Control.addVariable("KGRoofAngle", 4.0); // calculated
  Control.addVariable("KGWallMat", "SkanskaConcrete");
  Control.addVariable("KGAirMat", "Air");
  // 130 and 570 are stub legs lengths
  Control.addParse<double>("KGXStep",
			   "(LinacWidthLeft+KGWidthRight+LinacWallThick+KGWallThick+130+600-20.0)"); // check
  Control.addVariable("KGZStep", 573.5);

  const size_t nStubs = 27;
  Control.addVariable("LinacNStubs", nStubs);
  // Stub dimensions are measured from ESS-0308575 (email from Pontus 16.05)
  // Lengths are defined as in Sullivan, page 68
  Control.addVariable("Stub100WallThick", 30); // MARS
  Control.addParse<double>("Stub100Length1",
			   "340.0+LinacWidthLeft+LinacWallThick-Stub100WallThick");
  Control.addParse<double>("Stub100Length2", "610.0-2*Stub100WallThick");
  Control.addVariable("Stub100Width",   180.0);
  Control.addVariable("Stub100Height",  150.0);
  Control.addVariable("Stub100MainMat", "Air");
  Control.addVariable("Stub100WallMat", "SkanskaConcrete"); // check

  double stubOffset = 19.0; //cm offset added to match stub offset with magnets. There appears to be also
                            //a slight discrepency in the dtl1start and edge of FEB which gives positions for stubs
                            //thiis also around 19 cm

  Control.addVariable("Stub100YStep", 1845.0+stubOffset); // ESS-0025905 and MARS
  Control.addVariable("Stub100ZStep", -65.0);  //Added by ddju to get 10 cm from floor as in STEP file

  //Control.copyVarSet("Stub100", "Stub110"); -Batkov
  //Control.addVariable("Stub110YStep", 3761.5); // ESS-0025905 and MARS -Batkov
  
  // Position of the Stubs - MARS
  double Zstub[26] = {3761.5,  5939.0,  7643.0,  9347.0, 10693.5, 12397.5, 14101.5, 15805.5, 17509.5,
		      19213.5, 20917.5, 22621.5, 24325.5, 26029.5, 27733.5, 29437.5, 31141.5, 32845.5, 34549.5,
		      36253.5, 37957.5, 39661.5, 41365.5, 43069.5, 44773.5, 46477.5};

  for(int i=0;i<26;i++){
    Zstub[i] = Zstub[i]+stubOffset;
  }

  for(int i=0;i<(nStubs-1);i++){
    char numstr[21];
    sprintf(numstr, "Stub%d", 110+10*i);
    std::string stubY = numstr;
    std::string stubYStep = stubY + "YStep";
    Control.copyVarSet("Stub100", stubY);
    if(i>4){
      std::string stubWidth = stubY + "Width";
      std::string stubHeight = stubY + "Height";
      Control.addVariable(stubWidth,   120.0);
      Control.addVariable(stubHeight,  150.0);
    }
    Control.addVariable(stubYStep, Zstub[i]);
    //std::cout << stubYStep << std::endl;
  }

  // Stub Walls

  Control.addVariable("StubWall100Length",  260.0);
  Control.addVariable("StubWall100Width",   50.0);
  Control.addVariable("StubWall100Height",  300.0);
  Control.addVariable("StubWall100Mat",   "Air"); // check SkanskaConcrete
  Control.addVariable("StubWall100YStep",   -200.0); //350

  Control.copyVarSet("StubWall100", "StubWall110");
  Control.addVariable("StubWall110YStep",   -200.0); //235
  Control.addVariable("StubWall110GapActive", 1);
  Control.addVariable("StubWall110GapMat", "Air");
  Control.addVariable("StubWall110GapOffset", 53.25);
  Control.addVariable("StubWall110GapWidth", 23.8);
  Control.addVariable("StubWall110GapHeight", 67.64);
  Control.addVariable("StubWall110GapDist", 72.8);

  for(int i=0;i<(nStubs-2);i++){
    char numstr[21];
    sprintf(numstr, "StubWall%d", 120+10*i);
    std::string stubWall = std::string(numstr);   
    Control.copyVarSet("StubWall100", stubWall);
   }
 
  
  // Ducts in the stubs
  
  // Stub100Duct variables according to ESS-0318785.1
  // See email from Carl-Johan 3 Aug 2018
  Control.addVariable("Stub100NDucts", 2); // 8
  Control.addVariable("Stub100Duct1WallThick", 0.0);
  Control.addVariable("Stub100Duct1Width",   70.0);
  Control.addVariable("Stub100Duct1Height",  30.0);
  Control.addParse<double>("Stub100Duct1Length1", "Stub100Length1");
  Control.addParse<double>("Stub100Duct1Length2", "400+Stub100Duct1Height");
  Control.addVariable("Stub100Duct1MainMat", "StbTCABL");
  Control.addVariable("Stub100Duct1WallMat", "Void");
  Control.addParse<double>("Stub100Duct1ZStep", "(Stub100Duct1Height-Stub100Height)/2.0");
  Control.addParse<double>("Stub100Duct1YStep", "(Stub100Duct1Width-Stub100Width)/2.0");

  Control.copyVarSet("Stub100Duct1", "Stub100Duct2");
  Control.addVariable("Stub100Duct2Height",  37.0);
  Control.addParse<double>("Stub100Duct2Length1",
			   "Stub100Length1-Stub100Height+Stub100Duct2Height");
  Control.addParse<double>("Stub100Duct2Length2", "400+Stub100Duct2Height");
  Control.addParse<double>("Stub100Duct2ZStep",
			   "(Stub100Height-Stub100Duct2Height)/2.0");
  Control.addParse<double>("Stub100Duct2YStep",
			   "(Stub100Duct2Width-Stub100Width)/2.0");
  
  // Duct 3-8 length are approximate -Batkov  Don't appear to be used
  Control.addVariable("Stub100Duct3WallThick", 0.48);
  Control.addVariable("Stub100Duct3Width",   14.61);
  Control.addVariable("Stub100Duct3Height",  58.42);
  Control.addParse<double>("Stub100Duct3Length1", "Stub100Length1-Stub100Duct1Height-20");
  Control.addParse<double>("Stub100Duct3Length2", "400+Stub100Duct3Height");
  Control.addVariable("Stub100Duct3MainMat", "Air");
  Control.addVariable("Stub100Duct3WallMat", "Aluminium");
  Control.addParse<double>("Stub100Duct3YStep", "(Stub100Duct3Width-Stub100Width)/2.0+10");

  Control.copyVarSet("Stub100Duct3", "Stub100Duct4");
  Control.addParse<double>("Stub100Duct4YStep", "Stub100Duct3YStep+Stub100Duct3Width+10");

  Control.copyVarSet("Stub100Duct4", "Stub100Duct5");
  Control.addParse<double>("Stub100Duct5YStep", "Stub100Duct4YStep+Stub100Duct4Width+75");

  Control.copyVarSet("Stub100Duct5", "Stub100Duct6");
  Control.addParse<double>("Stub100Duct6YStep", "Stub100Duct6YStep+Stub100Duct5Width+10");

  Control.copyVarSet("Stub100Duct6", "Stub100Duct7");
  Control.addVariable("Stub100Duct7Height",   14.61);
  Control.addVariable("Stub100Duct7Width",  58.42);
  Control.addParse<double>("Stub100Duct7Length1", "Stub100Duct6Length1+Stub100Duct7Height+10");
  Control.addParse<double>("Stub100Duct7Length2", "Stub100Duct6Length2-45");
  Control.addParse<double>("Stub100Duct7YStep", "-(Stub100Duct7Width-Stub100Width)/2.0-Stub100Duct7WallThick-10");
  Control.addVariable("Stub100Duct7ZStep", -50);

  Control.copyVarSet("Stub100Duct7", "Stub100Duct8");
  Control.addParse<double>("Stub100Duct8Length1", "Stub100Duct6Length1-Stub100Duct6Height-10");
  Control.addVariable("Stub100Duct8ZStep", 50);
  

  // Stub110Duct variables according to ESS-0318786 rev 1.0
  // See email from Carl-Johan 3 Aug 2018
  Control.addVariable("Stub110NDucts", 2);
  Control.addVariable("Stub110Duct1WallThick", 0.0);
  Control.addVariable("Stub110Duct1Width",   60.0);
  Control.addVariable("Stub110Duct1Height",  35.0);
  Control.addParse<double>("Stub110Duct1Length1", "Stub110Length1");
  Control.addParse<double>("Stub110Duct1Length2", "400+Stub110Duct1Height");
  Control.addVariable("Stub110Duct1MainMat", "StbTCABL");
  Control.addVariable("Stub110Duct1WallMat", "Void");
  Control.addParse<double>("Stub110Duct1ZStep", "(Stub110Duct1Height-Stub110Height)/2.0");
  Control.addParse<double>("Stub110Duct1YStep", "(Stub110Duct1Width-Stub110Width)/2.0");

  Control.addVariable("Stub110Duct2WallThick", 0.0);
  Control.addVariable("Stub110Duct2Width",   60.0);
  Control.addVariable("Stub110Duct2Height",  35.0);
  Control.addParse<double>("Stub110Duct2Length1", "Stub110Length1");
  Control.addParse<double>("Stub110Duct2Length2", "400+Stub110Duct2Height");
  Control.addVariable("Stub110Duct2MainMat", "StbTCABL");
  Control.addVariable("Stub110Duct2WallMat", "Void");
  Control.addParse<double>("Stub110Duct2ZStep",
			   "(Stub110Duct2Height-Stub110Height)/2.0");
  Control.addParse<double>("Stub110Duct2YStep",
			   "-(Stub110Duct2Width-Stub110Width)/2.0");

  
  // Ducts for Stubs 120,130,140, and 150
  Control.addVariable("Stub120NDucts", 4);
  Control.addVariable("Stub120Duct1WallThick", 0.0);
  Control.addVariable("Stub120Duct1Width",   65.2);
  Control.addVariable("Stub120Duct1Height",  9.5);
  Control.addParse<double>("Stub120Duct1Length1", "Stub120Length1");
  Control.addParse<double>("Stub120Duct1Length2", "400+Stub120Duct1Height");
  Control.addVariable("Stub120Duct1MainMat", "StbTCABL_BOT");
  Control.addVariable("Stub120Duct1WallMat", "Void");
  Control.addParse<double>("Stub120Duct1ZStep", "(Stub120Duct1Height-Stub120Height)/2.0");
  Control.addParse<double>("Stub120Duct1YStep", "(Stub120Duct1Width-Stub120Width)/2.0");

  Control.addVariable("Stub120Duct2WallThick", 0.0);
  Control.addVariable("Stub120Duct2Width",   65.2);
  Control.addVariable("Stub120Duct2Height",  9.5);
  Control.addParse<double>("Stub120Duct2Length1", "Stub120Length1");
  Control.addParse<double>("Stub120Duct2Length2", "400+Stub120Duct2Height");
  Control.addVariable("Stub120Duct2MainMat", "StbTCABL_BOT");
  Control.addVariable("Stub120Duct2WallMat", "Void");
  Control.addParse<double>("Stub120Duct2ZStep",
			   "(Stub120Duct2Height-Stub120Height)/2.0");
  Control.addParse<double>("Stub120Duct2YStep",
			   "-(Stub120Duct2Width-Stub120Width)/2.0");

  Control.copyVarSet("Stub120Duct2", "Stub120Duct3");
  Control.addVariable("Stub120Duct3Width",  83.5);
  Control.addVariable("Stub120Duct3Height", 23.8);
  Control.addVariable("Stub120Duct3MainMat", "StbTCABL_TOP");
  Control.addParse<double>("Stub120Duct3Length1",
			   "Stub120Length1-Stub120Height+Stub120Duct3Height");
  Control.addParse<double>("Stub120Duct3Length2", "400+Stub120Duct3Height");
  Control.addParse<double>("Stub120Duct3ZStep",
			   "(Stub120Height-Stub120Duct3Height)/2.0");
  Control.addParse<double>("Stub120Duct3YStep",
			   "(Stub120Duct3Width-Stub120Width)/2.0");

  Control.copyVarSet("Stub120Duct3", "Stub120Duct4");
  Control.addVariable("Stub120Duct4Width",  83.5);
  Control.addVariable("Stub120Duct4Height",  23.8);
  Control.addVariable("Stub120Duct4MainMat", "StbTCABL_TOP");
  Control.addParse<double>("Stub120Duct4Length1",
			   "Stub120Length1-Stub120Height+Stub120Duct4Height");
  Control.addParse<double>("Stub120Duct4Length2", "400+Stub120Duct4Height");
  Control.addParse<double>("Stub120Duct4ZStep",
			   "(Stub120Height-Stub120Duct4Height)/2.0");
  Control.addParse<double>("Stub120Duct4YStep",
			   "-(Stub120Duct4Width-Stub120Width)/2.0");

  
  
  for(int i=0;i<4;i++){
      char numstr[21];
      sprintf(numstr, "Stub%d", 130+10*i);
      std::string stubDuct1 = std::string(numstr)+"Duct1";
      std::string stubDuct2 = std::string(numstr)+"Duct2";
      std::string stubDuct3 = std::string(numstr)+"Duct3";
      std::string stubDuct4 = std::string(numstr)+"Duct4";   
      std::string stubN = std::string(numstr) + "NDucts";     
      Control.addVariable(stubN, 4);
      Control.copyVarSet("Stub120Duct1", stubDuct1);
      Control.copyVarSet("Stub120Duct2", stubDuct2);
      Control.copyVarSet("Stub120Duct3", stubDuct3);
      Control.copyVarSet("Stub120Duct4", stubDuct4);
  }

  // Remaining ducts
  Control.addVariable("Stub170NDucts", 4);
  Control.addVariable("Stub170Duct1WallThick", 0.0);
  Control.addVariable("Stub170Duct1Width",   37.8);
  Control.addVariable("Stub170Duct1Height",   9.6);
  Control.addParse<double>("Stub170Duct1Length1", "Stub170Length1");
  Control.addParse<double>("Stub170Duct1Length2", "400+Stub170Duct1Height");
  Control.addVariable("Stub170Duct1MainMat", "StbTCABL_BOT");
  Control.addVariable("Stub170Duct1WallMat", "Void");
  Control.addParse<double>("Stub170Duct1ZStep", "(Stub170Duct1Height-Stub170Height)/2.0");
  Control.addParse<double>("Stub170Duct1YStep", "(Stub170Duct1Width-Stub170Width)/2.0");

  Control.addVariable("Stub170Duct2WallThick", 0.0);
  Control.addVariable("Stub170Duct2Width",   37.8);
  Control.addVariable("Stub170Duct2Height",   9.6);
  Control.addParse<double>("Stub170Duct2Length1", "Stub170Length1");
  Control.addParse<double>("Stub170Duct2Length2", "400+Stub170Duct2Height");
  Control.addVariable("Stub170Duct2MainMat", "StbTCABL_BOT");
  Control.addVariable("Stub170Duct2WallMat", "Void");
  Control.addParse<double>("Stub170Duct2ZStep",
			   "(Stub170Duct2Height-Stub170Height)/2.0");
  Control.addParse<double>("Stub170Duct2YStep",
			   "-(Stub170Duct2Width-Stub170Width)/2.0");

  Control.copyVarSet("Stub170Duct2", "Stub170Duct3");
  Control.addVariable("Stub170Duct3Width",  53.5);
  Control.addVariable("Stub170Duct3Height", 24.2);
  Control.addVariable("Stub170Duct3MainMat", "StbTCABL_TOP");
  Control.addParse<double>("Stub170Duct3Length1",
			   "Stub170Length1-Stub170Height+Stub170Duct3Height");
  Control.addParse<double>("Stub170Duct3Length2", "400+Stub170Duct3Height");
  Control.addParse<double>("Stub170Duct3ZStep",
			   "(Stub170Height-Stub170Duct3Height)/2.0");
  Control.addParse<double>("Stub170Duct3YStep",
			   "(Stub170Duct3Width-Stub170Width)/2.0");

  Control.copyVarSet("Stub170Duct3", "Stub170Duct4");
  Control.addVariable("Stub170Duct4Width",  53.5);
  Control.addVariable("Stub170Duct4Height", 24.2);
  Control.addVariable("Stub170Duct4MainMat", "StbTCABL_TOP");
  Control.addParse<double>("Stub170Duct4Length1",
			   "Stub170Length1-Stub170Height+Stub170Duct4Height");
  Control.addParse<double>("Stub170Duct4Length2", "400+Stub170Duct4Height");
  Control.addParse<double>("Stub170Duct4ZStep",
			   "(Stub170Height-Stub170Duct4Height)/2.0");
  Control.addParse<double>("Stub170Duct4YStep",
			   "-(Stub170Duct4Width-Stub170Width)/2.0");
  

  for(int i=0;i<20;i++){
    char numstr[21];
    sprintf(numstr, "Stub%d", 170+10*i);
    std::string stubDuct1 = std::string(numstr)+"Duct1";
    std::string stubDuct2 = std::string(numstr)+"Duct2";
    std::string stubDuct3 = std::string(numstr)+"Duct3";
    std::string stubDuct4 = std::string(numstr)+"Duct4"; 
    std::string stubN = std::string(numstr) + "NDucts";     
    Control.addVariable(stubN, 4);
    Control.copyVarSet("Stub170Duct1", stubDuct1);
    Control.copyVarSet("Stub170Duct2", stubDuct2);
    Control.copyVarSet("Stub170Duct3", stubDuct3);
    Control.copyVarSet("Stub170Duct4", stubDuct4);
  }
   //end of Stubs
   
  // FEB dimensions are measured from the STEP file received from
  // Carl-Johan 31.05.2018
  Control.addVariable("FEBLength",     2000.0);
  Control.addVariable("FEBWidthRight", 2667.0);
  Control.addVariable("FEBWidthLeft",   485.0); //Updated from Wolfgang
  Control.addVariable("FEBWallThick",    50.0);
  Control.addVariable("FEBMainMat",   "Air");
  Control.addVariable("FEBWallMat",   "SkanskaConcrete");
  Control.addVariable("FEBGapMat",   "Air"); // ???
  Control.addVariable("FEBFillingMat",   "S355_SteelFill3"); // 90% S355
  Control.addVariable("FEBGapALength", 180.0);
  Control.addVariable("FEBGapAHeightLow", 100.0);
  Control.addVariable("FEBGapAHeightTop", 130.0);
  Control.addVariable("FEBGapADist", 89.2); //From Drawing of holes
  Control.addParse<double>("FEBGapBLength",    "FEBGapALength");
  Control.addParse<double>("FEBGapBHeightLow", "FEBGapAHeightLow");
  Control.addParse<double>("FEBGapBHeightTop", "FEBGapAHeightTop");
  Control.addParse<double>("FEBGapBDist",      "FEBGapADist");
  Control.addVariable("FEBGapABDist", 635.03);

  Control.addParse<double>("FEBShieldWall1Offset", "LinacWidthLeft");
  Control.addVariable("FEBShieldWall1Thick", 100.0);
  Control.addVariable("FEBShieldWall1Length", 1700.0); //To get the FEB walls the correct lengths

  Control.addVariable("FEBShieldWall2Offset", 1000.0);
  Control.addParse<double>("FEBShieldWall2Thick", "FEBShieldWall1Thick");
  Control.addVariable("FEBShieldWall2Length", 800.0);

  Control.addVariable("FEBShieldWall3Offset", 715.0);
  Control.addVariable("FEBShieldWall3Thick",  100.0);
  Control.addVariable("FEBShieldWall3Length", 600.0);

  Control.addVariable("FEBDropHatchLength", 1100);
  Control.addVariable("FEBDropHatchWidth",   378.0);
  Control.addVariable("FEBDropHatchWallThick", 70.0);

  // RFQ
  // dimensions as in rbfrend2-9102
  Control.addVariable("RFQYStep",250.59); // From Lattice file
  Control.addVariable("RFQLength", 465.2072);  // From Lattice file
  Control.addVariable("RFQOuterWidth", 29.0);
  Control.addVariable("RFQInnerWidth", 18.0);
  Control.addVariable("RFQWallThick", 5.6);
  Control.addVariable("RFQMainMat", "Void");
  Control.addVariable("RFQWallMat", "Copper");
  Control.addVariable("RFQVaneThick", 2.0);
  Control.addVariable("RFQVaneLength", 6.8); // measured from tip to tip of triangles
  Control.addVariable("RFQVaneTipThick", 0.6);
  Control.addVariable("RFQVaneTipDist", 0.6);

  Control.addVariable("RFQCoolantMat", "H2O");
  Control.addVariable("RFQCoolantInnerRadius", 0.5);
  Control.addVariable("RFQCoolantInnerDist", 3.3);
  Control.addVariable("RFQCoolantOuterRadius", 1.0);
  Control.addVariable("RFQCoolantOuterDist", 11.75);

  // Berm
  Control.addParse<double>("BermLengthBack", "LinacLengthBack+100+FEBLength");
  Control.addParse<double>("BermLengthFront", "LinacLengthFront+100");
  Control.addVariable("BermHeight", 704); //Used 490 for drophatch after getting info form Wolfgang. Updated to 704 bsaed on Fabiens Scans for tunnel
  Control.addVariable("BermDepth",  500);
  Control.addVariable("BermWidthLeft", 3500);
  Control.addVariable("BermWidthRight", 4000);
  Control.addVariable("BermMat", "ClayRevised");
  Control.addVariable("BermRoofAngle", 2.7); // Was set t0 4.76 earlier. Now set to 2.7 based on Fabien's Scans for tunnel

  // CryoTransferLine

  Control.addVariable("CryoTransferLineWallThick", 30); // MARS
  Control.addVariable("CryoTransferLineRoofThick", 40); // Roof and floor the same
  Control.addParse<double>("CryoTransferLineLength1","480.0+315.0"); //leg1
  Control.addParse<double>("CryoTransferLineLength2","902.5+200.0");//leg2
  Control.addParse<double>("CryoTransferLineLength3","2250.0+200.0"); //leg3
  Control.addParse<double>("CryoTransferLineLength4","652.5.0+200.0"); //leg4
  Control.addVariable("CryoTransferLineWidth",   300.0); //entrance widthe
  Control.addVariable("CryoTransferLineWidthHall",   200.0); //height in legs 2,3,4
  Control.addVariable("CryoTransferLineHeight",  250.0);
  Control.addVariable("CryoTransferLineMainMat", "Air");
  Control.addVariable("CryoTransferLineWallMat", "SkanskaConcrete"); // check

  Control.addVariable("CryoTransferLineYStep", 36638.5); // ESS-0025905 and MARS
  Control.addVariable("CryoTransferLineZStep", -25.0);  //Should match the floor of hall
  

  
  return;
}

}  // NAMESPACE setVariable
