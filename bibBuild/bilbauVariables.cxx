/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   bibBuild/bilbauVariables.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "variableSetup.h"

namespace setVariable
{

void
BilbauVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Bilbau 
    \param Control :: Function data base to add constants too
  */
{
  // -----------
  // GLOBAL stuff
  // -----------


  Control.addVariable("zero",0.0);     // Zero
  Control.addVariable("one",1.0);      // one

  /** VARIABLES DEL DISCO DEL BLANCO */

  Control.addVariable("RotorWheelRadius",100.0);      // Valor nominal 100 cm
  Control.addVariable("RotorWheelWaterThick",0.5);   // 5mm water
  Control.addVariable("RotorWheelWallThick",0.5);
  Control.addVariable("RotorWheelBeThick",1.1);      
  Control.addVariable("RotorWheelBeLength",18.0);      
  Control.addVariable("RotorWheelBackVac",6.0);
  Control.addVariable("RotorWheelFrontVac",3.0);    // proton side
  Control.addVariable("RotorWheelEndVac",3.0);
  Control.addVariable("RotorWheelOutWallThick",0.5);      

  Control.addVariable("RotorWheelBoxFront",54.75);  
  Control.addVariable("RotorWheelBoxBack",57.75);      
  Control.addVariable("RotorWheelBoxIn",14.0);      
  Control.addVariable("RotorWheelBoxOut",167.0);      
  Control.addVariable("RotorWheelBoxTop",110.0);      
  Control.addVariable("RotorWheelBoxBase",110.0);      
  Control.addVariable("RotorWheelBoxInterior",32.5);      
  Control.addVariable("RotorWheelSteelThick",5.0);      


  Control.addVariable("RotorWheelPbDepth",105.0);      
  Control.addVariable("RotorWheelPbThick",5.0);      


  Control.addVariable("RotorWheelBeMat","Berilio"); 	//Be S-200F
  Control.addVariable("RotorWheelWallMat","Aluminium"); 	// Al-6061
  Control.addVariable("RotorWheelWaterMat","H2O");
  Control.addVariable("RotorWheelVesselMat","Stainless304"); 
  Control.addVariable("RotorWheelPolyMat","B-Poly"); //shielding for main drive
  Control.addVariable("RotorWheelPbMat","Lead"); // gamma shielding 


  // REFLECTOR
  Control.addVariable("ReflXStep",0.0);
  Control.addVariable("ReflYStep",0.0);
  Control.addVariable("ReflZStep",0.0);
  Control.addVariable("ReflXYAngle",0.0);
  Control.addVariable("ReflZAngle",0.0);

  Control.addVariable("ReflBeHeight",51.0); // Altura total del reflector
  Control.addVariable("ReflBeDepth",51.0);   // Depth total del reflector
  Control.addVariable("ReflBeRadius",60.0); 
  Control.addVariable("ReflBeMat","Be300K");

  Control.addVariable("ReflInnerRadius",62.0);    
  Control.addVariable("ReflInnerHeight",53.0); 
  Control.addVariable("ReflInnerDepth",53.0); 
  Control.addVariable("ReflInnerMat","Stainless304");

  Control.addVariable("ReflPbRadius",72.0); 	
  Control.addVariable("ReflPbHeight",63.0); 
  Control.addVariable("ReflPbDepth",63.0); 
  Control.addVariable("ReflPbMat","Lead");

  Control.addVariable("ReflMidRadius",152.0); 	
  Control.addVariable("ReflMidHeight",143.0); 
  Control.addVariable("ReflMidDepth",143.0); 
  Control.addVariable("ReflMidMat","B-Poly");   // poly (borated)

  Control.addVariable("ReflOuterRadius",157.0); 	
  Control.addVariable("ReflOuterHeight",148.0); 
  Control.addVariable("ReflOuterDepth",148.0); 
  Control.addVariable("ReflOuterMat","Stainless304");   // Steel

  Control.addVariable("ReflOuterPbRadius",162.0); 	
  Control.addVariable("ReflOuterPbHeight",153.0); 
  Control.addVariable("ReflOuterPbDepth",153.0); 
  Control.addVariable("ReflOuterPbMat","Lead");   // Pb

  // BLINDAJE EXTERIOR: PAREDES DEL AREA DEL TARGET

  Control.addVariable("ConcWallXStep",0.0);
  Control.addVariable("ConcWallYStep",0.0);
  Control.addVariable("ConcWallZStep",0.0);
  Control.addVariable("ConcWallXYAngle",0.0);
  Control.addVariable("ConcWallZAngle",0.0);

  Control.addVariable("ConcWallInnerRadius",500.0);
  Control.addVariable("ConcWallBase",200.0);
  Control.addVariable("ConcWallWallHeight",300.0); 
  Control.addVariable("ConcWallMat","Concrete");

  Control.addVariable("ConcWallWallThick",70.0);

  // PROTON PIPE

  Control.addVariable("ProtonObjXStep",0.0);
  Control.addVariable("ProtonObjYStep",0.0);
  Control.addVariable("ProtonObjZStep",0.0);
  Control.addVariable("ProtonObjXYAngle",-135.0);
  Control.addVariable("ProtonObjZAngle",0.0);
  Control.addVariable("ProtonObjRadius",6.0);
  Control.addVariable("ProtonObjInnerWallThick",1.0); // Espesor del la pared
  Control.addVariable("ProtonObjWallThick",0.5); // Espesor del la pared
  Control.addVariable("ProtonObjInnerLength",200.0);
  Control.addVariable("ProtonObjLength",700.0);
  Control.addVariable("ProtonObjVoidMat","Void");   	
  Control.addVariable("ProtonObjInnerWallMat","Void");   	
  Control.addVariable("ProtonObjWallMat","Stainless304");





  /* Moderador de Agua */

  Control.addVariable("WatModXStep",4.0);      // 
  Control.addVariable("WatModYStep",-12.0);      // 
  Control.addVariable("WatModZStep",0.0);      // 
  Control.addVariable("WatModXYangle",-45.0);      // 
  Control.addVariable("WatModZangle",0.0);      // 
  Control.addVariable("WatModWidth",12.0);      // 
  Control.addVariable("WatModHeight",12.0);      // 
  Control.addVariable("WatModDepth",2.0);      // 
  Control.addVariable("WatModWallThick",0.3);      // 
  Control.addVariable("WatModVertGap",2.0);      // 
  Control.addVariable("WatModSideGap",3.0);      // 
  Control.addVariable("WatModFrontGap",2.0);     // 
  Control.addVariable("WatModBackGap",2.0);      // 
  Control.addVariable("WatModWaterMat","H2O");      // water H2O
  Control.addVariable("WatModWallMat","Aluminium");        // aluminium
  Control.addVariable("WatModWaterTemp",300.0);  // water H2O
  
  // Blindaje de la linea de protones

  Control.addVariable("Blindaje_Protones_Espesor_Acero",5);
  Control.addVariable("Blindaje_Protones_Espesor_Polietileno",15);
  Control.addVariable("Blindaje_Protones_Mat1","B-Poly");
  Control.addVariable("Blindaje_Protones_Mat2","Stainless304");


  Control.addVariable("Guide1XStep",0.0);      //
  Control.addVariable("Guide1YStep",0.0);      //
  Control.addVariable("Guide1ZStep",0.0);      //
  Control.addVariable("Guide1XYangle",0.0);      //
  Control.addVariable("Guide1Zangle",0.0);      //
  Control.addVariable("Guide1Width",12.0); 
  Control.addVariable("Guide1Height",8.0);      //
  Control.addVariable("Guide1Length",1000.0);      //
  Control.addVariable("Guide1NiRadius",50.0);      //
  Control.addVariable("Guide1NiThickness",1.0);    
  Control.addVariable("Guide1Mat","Nickel");

  Control.addVariable("Guide2XStep",0.0);      //
  Control.addVariable("Guide2YStep",0.0);      //
  Control.addVariable("Guide2ZStep",0.0);      //
  Control.addVariable("Guide2XYangle",-20.0);      //
  Control.addVariable("Guide2Zangle",0.0);      //
  Control.addVariable("Guide2Width",12.0); 
  Control.addVariable("Guide2Height",8);      //
  Control.addVariable("Guide2Length",1000.0);      //
  Control.addVariable("Guide2NiRadius",50.0);      //
  Control.addVariable("Guide2NiThickness",1.0);    
  Control.addVariable("Guide2Mat","Nickel");

  // COLD 2 guides
  Control.addVariable("Guide3XStep",0.0);      //
  Control.addVariable("Guide3YStep",0.0);      //
  Control.addVariable("Guide3ZStep",0.0);      //
  Control.addVariable("Guide3XYangle",-20.0);      //
  Control.addVariable("Guide3Zangle",0.0);      //
  Control.addVariable("Guide3Width",12.0); 
  Control.addVariable("Guide3Height",8);      //
  Control.addVariable("Guide3Length",1000.0);      //
  Control.addVariable("Guide3NiRadius",50.0);      //
  Control.addVariable("Guide3NiThickness",1.0);    
  Control.addVariable("Guide3Mat","Nickel");

  Control.addVariable("Guide4XStep",0.0);      //
  Control.addVariable("Guide4YStep",0.0);      //
  Control.addVariable("Guide4ZStep",0.0);      //
  Control.addVariable("Guide4XYangle",0.0);      //
  Control.addVariable("Guide4Zangle",0.0);      //
  Control.addVariable("Guide4Width",12.0); 
  Control.addVariable("Guide4Height",8);      //
  Control.addVariable("Guide4Length",1000.0);      //
  Control.addVariable("Guide4NiRadius",50.0);      //
  Control.addVariable("Guide4NiThickness",1.0);    
  Control.addVariable("Guide4Mat","Nickel");

  Control.addVariable("Guide5XStep",0.0);      //
  Control.addVariable("Guide5YStep",0.0);      //
  Control.addVariable("Guide5ZStep",0.0);      //
  Control.addVariable("Guide5XYangle",20.0);      //
  Control.addVariable("Guide5Zangle",0.0);      //
  Control.addVariable("Guide5Width",12.0); 
  Control.addVariable("Guide5Height",8);      //
  Control.addVariable("Guide5Length",1000.0);      //
  Control.addVariable("Guide5NiRadius",50.0);      //
  Control.addVariable("Guide5NiThickness",1.0);    
  Control.addVariable("Guide5Mat","Nickel");
  
  
  
  // COLD MODERATOR forward
  
  Control.addVariable("ColdModXStep",0.0);      //
  Control.addVariable("ColdModYStep",7.5);      //
  Control.addVariable("ColdModZStep",0.0);      //
  Control.addVariable("ColdModXYangle",0.0);      //
  Control.addVariable("ColdModZangle",0.0);      //
  Control.addVariable("ColdModWidth",12.0);      //
  Control.addVariable("ColdModHeight",12.0);      //
  Control.addVariable("ColdModDepth",2.0);      //
  Control.addVariable("ColdModWallThick",0.3);      //
  Control.addVariable("ColdModVertGap",3.0);      //
  Control.addVariable("ColdModSideGap",4.0);      //
  Control.addVariable("ColdModFrontGap",4.0);     //
  Control.addVariable("ColdModBackGap",2.0);      //
  Control.addVariable("ColdModWaterMat",11);      // water H2O
  Control.addVariable("ColdModModMat","CH4inFoam");        // Metano a 20 k
  Control.addVariable("ColdModModTemp",20.0);        // Metano a 20 k
  Control.addVariable("ColdModWallMat","Aluminium");        // aluminium 6061
  Control.addVariable("ColdModPremThick",1.5);      // Premoderator
  Control.addVariable("ColdModWallPremThick",0.2);  //
  Control.addVariable("ColdModWaterTemp",300.0);     // water H2O
  Control.addVariable("ColdModPremGap",0.5);        //
  
  
  
  // GUIDE  1
  
  Control.addVariable("GuideObj_1XYangle",0.0);      //
  Control.addVariable("GuideObj_1Zangle",0.0);      //
  Control.addVariable("GuideObj_1Niquel_Thickness",1);      //Donde empieza de verdad la guia
  Control.addVariable("GuideObj_1Mat_Guia","Nickel");
  
  /*
   * BLINDAJE DE LAS GUIAS
   */
  
  // Blindaje guia 1
  Control.addVariable("Blindaje_guia_Espesor_Acero",5);
  Control.addVariable("Blindaje_guia_Espesor_Polietileno",15);
  Control.addVariable("Blindaje_guia_Mat1",52);
  Control.addVariable("Blindaje_guia_Mat2","Stainless304");
  
  
  // GUIDE 2
  
  Control.addVariable("GuideObj_2XYangle",-20.0);      // Tiene que ser menor que -18
  Control.addVariable("GuideObj_2Zangle",0.0);      //
  Control.addVariable("GuideObj_2Niquel_Thickness",1);      //Donde empieza de verdad la guia
  Control.addVariable("GuideObj_2Mat_Guia","Nickel");
  
  
  // Blindaje guia 2
  Control.addVariable("Blindaje_guia_2_Espesor_Acero",5);
  Control.addVariable("Blindaje_guia_2_Espesor_Polietileno",15);
  Control.addVariable("Blindaje_guia_2_Mat1","B-Poly");
  Control.addVariable("Blindaje_guia_2_Mat2","Stainless304");

      
  // BE FILTER FORWARD
  Control.addVariable("BeFilterForwardXStep",0.0);      //B
  Control.addVariable("BeFilterForwardYStep",0.0);      //
  Control.addVariable("BeFilterForwardZStep",0.0);      //
  Control.addVariable("BeFilterForwardAngleA",0.0);      // Angulo 0
  Control.addVariable("BeFilterForwardAngleB",-20.0);     // Angulo 0
  Control.addVariable("BeFilterForwardWallThick",0.3);      //
  Control.addVariable("BeFilterForwardWallGap",0.2);
  Control.addVariable("BeFilterForwardLength",20.0);
  Control.addVariable("BeFilterForwardBeMat","Be300K"); // Beryllium at RT
  Control.addVariable("BeFilterForwardBeTemp",75.0);      // be temp
  Control.addVariable("BeFilterForwardWallMat","Aluminium");   // aluminium

  // Dont like this -- could be used off link points more generically?
  Control.Parse("ColdModWidth+2.0");
  Control.addVariable("BeFilterForwardWidth");
  Control.Parse("ColdModHeight+2.0");
  Control.addVariable("BeFilterForwardHeight");

  
  // COLD MODERATOR 2
  
  Control.addVariable("ColdMod2XStep",-12.5);      //
  Control.addVariable("ColdMod2YStep",-12.5);      //
  Control.addVariable("ColdMod2ZStep",0.0);      //
  Control.addVariable("ColdMod2XYangle",-225.0);      //
  Control.addVariable("ColdMod2Zangle",0.0);      //
  Control.addVariable("ColdMod2Width",12.0);      //
  Control.addVariable("ColdMod2Height",12.0);      //
  Control.addVariable("ColdMod2Depth",2.0);      //
  Control.addVariable("ColdMod2WallThick",0.3);      //
  Control.addVariable("ColdMod2VertGap",3.0);      //
  Control.addVariable("ColdMod2SideGap",4.0);      //
  Control.addVariable("ColdMod2FrontGap",4.0);     //
  Control.addVariable("ColdMod2BackGap",2.0);      //
  Control.addVariable("ColdMod2WaterMat",11);      // water H2O
  Control.addVariable("ColdMod2ModMat","CH4inFoam");      // Methane at 20I
  Control.addVariable("ColdMod2WallMat","Aluminium");        // aluminium 6061
  Control.addVariable("ColdMod2WaterTemp",20.0);      // water H2O
  Control.addVariable("ColdMod2PremThick",1.5);      // Premoderator
  Control.addVariable("ColdMod2WallPremThick",0.2);        //
  Control.addVariable("ColdMod2ModTemp",20.0);      // water H2O
  Control.addVariable("ColdMod2WaterTemp",300.0);      // water H2O
  Control.addVariable("ColdMod2PremGap",0.5);      //


  // BE FILTER Backward
  Control.addVariable("BeFilterBackwardXStep",0.0);      //B
  Control.addVariable("BeFilterBackwardYStep",0.0);      //
  Control.addVariable("BeFilterBackwardZStep",0.0);      //
  Control.addVariable("BeFilterBackwardAngleA",-25.0);      // Angulo 0
  Control.addVariable("BeFilterBackwardAngleB",-18.0);     // Angulo 0
  Control.addVariable("BeFilterBackwardWallThick",0.3);      //
  Control.addVariable("BeFilterBackwardWallGap",0.2);
  Control.addVariable("BeFilterBackwardLength",20.0);
  Control.addVariable("BeFilterBackwardBeMat","Be77K");  // Beryllium 
  Control.addVariable("BeFilterBackwardBeTemp",75.0);      // be temp
  Control.addVariable("BeFilterBackwardWallMat","Aluminium");  // aluminium 6063
  // Dont like this -- could be used off link points more generically?
  Control.Parse("ColdMod2Width+2*ColdMod2PremThick");
  Control.addVariable("BeFilterBackwardWidth");
  Control.Parse("ColdMod2Height+2*ColdMod2PremThick");
  Control.addVariable("BeFilterBackwardHeight");

  Control.addVariable("GShieldNLayer",3);      //
  Control.addVariable("GShieldWidth1",2.6);      
  Control.addVariable("GShieldWidth1",2.6);      
  Control.addVariable("GShieldHeight1",2.6);     
  Control.addVariable("GShieldMat1","Void");      // Vac
  Control.addVariable("GShieldWidth2",15.0);     
  Control.addVariable("GShieldHeight2",15.0);    
  Control.addVariable("GShieldMat2","B-Poly");      // ??
  Control.addVariable("GShieldWidth3",5.0);      
  Control.addVariable("GShieldHeight3",5.0);      
  Control.addVariable("GShieldMat3","Stainless304");      // steel
  
  return;
}

}  // NAMESPACE setVariable
