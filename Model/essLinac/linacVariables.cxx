/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/linacVariables.cxx
 *
 * Copyright (c) 2017 by Konstantin Batkov
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

  Control.addVariable("LinacEngineeringActive",1);

  Control.addVariable("LinacLength",10000.0); // Lali
  Control.addVariable("LinacWidthLeft",600./2.0+15.0); // K01-20---6-G01---011
  Control.addVariable("LinacWidthRight",600./2.0-15.0); // K01-20---6-G01---011
  Control.addVariable("LinacHeight",200.0); // Height+Depth from K01-20---6-G01---011; center communicated by Lali
  Control.addVariable("LinacDepth",150.0); // Height+Depth from K01-20---6-G01---011; center communicated by Lali
  Control.addVariable("LinacWallThick",50.0); // K01-20---6-G01---011
  Control.addVariable("LinacRoofThick",80.0); // K01-20---6-G01---011
  Control.addVariable("LinacFloorThick",75.0); // K01-20---6-G01---011
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

  Control.addParse<double>("LinacTSWLength", "LinacWidthLeft+LinacWidthRight-120.0"); // Lali said
  Control.addVariable("LinacTSWWidth", 100.0); // Lali said
  Control.addVariable("LinacTSWGap", 120.0); // Lali said
  Control.addVariable("LinacTSWOffsetY", 300.0-150); // some location (must be 20 cm off the beam dump)
  Control.addVariable("LinacTSWMat", "SkanskaConcrete");
  Control.addVariable("LinacTSWNLayers", 1); // for biasing

  // Beam dump
  Control.addVariable("LinacBeamDumpActive", 0);
  
  Control.addVariable("LinacBeamDumpYStep",-10); // just to have Origin in the middle of its front wall

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
  ELog::EM << "measure LinacBeamDumpBackInnerWallGapLength" << ELog::endCrit;
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
  
  ELog::EM << "measure LinacBeamDumpVacPipeFrontInnerWallDist" << ELog::endCrit;
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

  // Faraday cup
  Control.addVariable("LinacFaradayCupYStep", -50.0); // don't start it in the origin
  // Dimensions are based on email from LT 13 Mar 2017 (Fc_design.pdf)
  Control.addVariable("LinacFaradayCupEngineeringActive", 1);
  Control.addVariable("LinacFaradayCupLength", 3.2);
  Control.addVariable("LinacFaradayCupOuterRadius", 3.0); // d2/2
  Control.addVariable("LinacFaradayCupInnerRadius", 2.0); // d1/2
  Control.addVariable("LinacFaradayCupFaceLength", 0.1*SCALE3);
  Control.addVariable("LinacFaradayCupFaceRadius", 2.0*SCALE3); // approx
  Control.addVariable("LinacFaradayCupAbsorberLength", 0.7);
  Control.addVariable("LinacFaradayCupAbsorberMat", "Graphite");
  Control.addVariable("LinacFaradayCupBaseLength", 1.5); // e1
  
  Control.addVariable("LinacFaradayCupCollectorLength", 0.5);
  Control.addVariable("LinacFaradayCupCollectorMat", "Graphite");

  Control.addVariable("LinacFaradayCupWallMat", "Copper");

  Control.addVariable("LinacFaradayCupShieldMat","SkanskaConcrete");
  Control.addVariable("LinacFaradayCupShieldRadius",100.0);
  Control.addVariable("LinacFaradayCupShieldForwardLength",100.0);
  Control.addVariable("LinacFaradayCupShieldBackLength",100.0);
  Control.addVariable("LinacFaradayCupShieldInnerRadius",10.0);
  Control.addVariable("LinacFaradayCupShieldInnerLength",10.0);

  // DTL
  Control.addVariable("LinacNDTLTanks", 5);
  // DTL lengths are from Google drive / ESS DTL
  // 02 - Mechanical development and prototype construction.pdf
  // page 21
  Control.addVariable("LinacDTL1YStep", 950.0);  // an arbitrary number

  Control.addVariable("LinacDTL1Length", 768.76);
  Control.addVariable("LinacDTL1IntertankLength", 10.83);
  Control.addVariable("LinacDTL2Length", 717.11);
  Control.addVariable("LinacDTL2IntertankLength", 16.84);
  Control.addVariable("LinacDTL3Length", 765.26);
  Control.addVariable("LinacDTL3IntertankLength", 21.85);
  Control.addVariable("LinacDTL4Length", 791.71);
  Control.addVariable("LinacDTL4IntertankLength", 24.91);
  Control.addVariable("LinacDTL5Length", 775.79);
  Control.addVariable("LinacDTL5IntertankLength", 100.0);
  
  Control.addVariable("LinacDTLNLayers", 3);
  Control.addVariable("LinacDTLRadius1", 25.95);  // DTL_model_picture.png - email from RB 14 Mar 2017
  Control.addVariable("LinacDTLRadius2", 26);  // MARS
  Control.addVariable("LinacDTLRadius3", 31);  // MARS
  Control.addVariable("LinacDTLMat1", "Void"); 
  Control.addVariable("LinacDTLMat2", "Copper"); 
  Control.addVariable("LinacDTLMat3", "SS304L");
  Control.addVariable("LinacDTLCoverThick", 3.6); // MARS

  Control.addVariable("LinacDTLIntertankRadius", 2.8); // MARS
  Control.addVariable("LinacDTLIntertankWallThick", 0.2); // MARS

  Control.addVariable("LinacDTL1NPMQ", 1);
  Control.addVariable("LinacDTL2NPMQ", 0);
  Control.addVariable("LinacDTL3NPMQ", 0);
  Control.addVariable("LinacDTL4NPMQ", 0);
  Control.addVariable("LinacDTL5NPMQ", 0);
  
  Control.addVariable("LinacDTL1EngineeringActive", 1);
  Control.addVariable("LinacDTL1PMQ1Length", 10);
  Control.addVariable("LinacDTL1PMQ1IntertankLength", 2);
  Control.addVariable("LinacDTL1PMQ1IntertankRadius", 3);
  Control.addVariable("LinacDTL1PMQ1IntertankWallThick", 1);
  Control.addVariable("LinacDTL1PMQ1NLayers", 1);
  Control.addVariable("LinacDTL1PMQ1Radius1", 5);
  Control.addVariable("LinacDTL1PMQ1Mat1", "SS304L");
  Control.addVariable("LinacDTL1PMQ1CoverThick", 0.2);
  Control.addVariable("LinacDTL1AirMat", "Air");

  
  // Control.addVariable("LinacDTLRadius1", 1.0); 
  // Control.addVariable("LinacDTLRadius2", 1.15); 
  // Control.addVariable("LinacDTLRadius3", 2.90); 
  // Control.addVariable("LinacDTLRadius4", 4.50); 
  // Control.addVariable("LinacDTLRadius5", 26.0); 
  // Control.addVariable("LinacDTLMat3", "Void"); 
  // Control.addVariable("LinacDTLMat5", "Void"); 
  // Control.addVariable("LinacDTLMat6", "H2O"); 
  // Control.addVariable("LinacDTLMat7", "SS316L"); 
 
  return;
}

}  // NAMESPACE setVariable
