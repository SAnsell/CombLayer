/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Upgrade/DefUnitsTS1Mark.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <vector>
#include <set>
#include <list>
#include <map>
#include <string>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "InputControl.h"
#include "inputParam.h"
#include "support.h"
#include "defaultConfig.h"
#include "DefUnitsTS1Mark.h"

namespace mainSystem
{

void 
setDefUnits(FuncDataBase& Control,
	    inputParam& IParam)
  /*!
    Based on the defaultConf set up the model
    \param Control :: FuncDataBase
    \param IParam :: input system
  */
{
  ELog::RegMethod RegA("DefUnitsTS1Mark","setDefUnits");

  defaultConfig A("");
  if (IParam.flag("defaultConfig"))
    {
      const std::string Key=IParam.getValue<std::string>("defaultConfig");
      if (Key=="TS1MarkIV")
	setTS1MarkIV(A);
      else if (Key=="TS1MarkV")
	setTS1MarkV(A);
      else if (Key=="TS1MarkVI")
	setTS1MarkVI(A);
      else if (Key=="TS1MarkVIb")
	setTS1MarkVIb(A);
      else if (Key=="help")
	{
	  ELog::EM<<"Options : "<<ELog::endDiag;
	  ELog::EM<<"  TS1MarkIV  "<<ELog::endDiag;
	  ELog::EM<<"  TS1MarkV  "<<ELog::endDiag;
	  ELog::EM<<"  TS1MarkVI  "<<ELog::endDiag;
	  ELog::EM<<"  TS1MarkVIb  "<<ELog::endDiag;
	  throw ColErr::InContainerError<std::string>
	    (Key,"Iparam.defaultConfig");	  
	}
      else 
	{
	  ELog::EM<<"Unknown Default Key ::"<<Key<<ELog::endDiag;
	  throw ColErr::InContainerError<std::string>
	    (Key,"Iparam.defaultConfig");
	}

    }
  else
    {
      ELog::EM<<"Generic build [TS1MarkVIb]" <<ELog::endDiag;
      setTS1MarkVIb(A);
    }
  A.process(Control,IParam);
	
  return;
}

void
setTS1MarkIV(defaultConfig& A)
  /*!
    Default configureation for MarkIV
    \param A :: Paramter for default config
   */
{
  ELog::RegMethod RegA("DefUnitsTS1Mark[F]","setTS1MarkII");

  A.setOption("targetType","t1CylFluxTrap");
  A.setOption("CH4PreType","Flat");

  A.setVar("sdefZOffset",0.0);
  A.setVar("TriModPoisonThick",0.005);
  A.setVar("TriModPCladThick",0.3);
  A.setVar("TriModPoisonMat","Gadolinium");
  A.setVar("TriModXYangle",-95.0); 
  A.setVar("TriModXStep",0.0);
  A.setVar("TriModYStep",0.0);
  A.setVar("TriFlightAAngleXY1",30.0);
  A.setVar("TriFlightAAngleXY2",0.0);
  A.setVar("TriFlightAXStep",-2.0);
  A.setVar("TriFlightAWidth",33.0);
  A.setVar("TriFlightBAngleXY2",5.0);
  A.setVar("TriFlightBWidth",23.0);

  A.setVar("TriModNCorner",3);  
  A.setVar("TriModCorner1",Geometry::Vec3D(2.1,23.88,0)); 
  A.setVar("TriModCorner2",Geometry::Vec3D(11.63,-13.82,0)); 
  A.setVar("TriModCorner3",Geometry::Vec3D(-11.61,13.79,0.0)); 
  A.setVar("TriModAbsolute1",1);
  A.setVar("TriModAbsolute2",1);
  A.setVar("TriModAbsolute3",1);

  A.setVar("TriModNInnerUnits",1); 
  A.setVar("TriModInner1Corner1",Geometry::Vec3D(-5.86,10.95,0.0)); 
  A.setVar("TriModInner1Corner2",Geometry::Vec3D(1.41,16.12,0.0)); 
  A.setVar("TriModInner1Corner3",Geometry::Vec3D(9.04,-8.4,0)); 
  A.setVar("TriModInner1Absolute1",1);
  A.setVar("TriModInner1Absolute2",1);
  A.setVar("TriModInner1Absolute3",1);

  A.setVar("TriFlightASideIndex",3);
  A.setVar("TriFlightBSideIndex",4);

  A.setVar("TriModNInnerLayers",4); 
  A.setVar("TriModInnerThick0",0.3); 
  A.setVar("TriModInnerThick1",0.005); 
  A.setVar("TriModInnerThick2",0.3); 
  A.setVar("TriModInnerMat0","Aluminium");
  A.setVar("TriModInnerMat1","Gadolinium");
  A.setVar("TriModInnerMat2","Aluminium");
  A.setVar("TriModInnerMat3","Void");

  return;
}

void
setTS1MarkV(defaultConfig& A)
  /*!
    Default configureation for MarkIV
    \param A :: Paramter for default config
   */
{
  ELog::RegMethod RegA("DefUnitsTS1Mark[F]","setTS1MarkV");

  A.setOption("targetType","t1CylFluxTrap");
  A.setOption("CH4ModType","Layer");
  A.setOption("H2ModType","Layer");
  A.setOption("CH4PreType","Flat");

  A.setVar("sdefZOffset",0.0);

  A.setVar("TriModPoisonThick",0.005);
  A.setVar("TriModPCladThick",0.3);
  A.setVar("TriModPoisonMat","Gadolinium");
  A.setVar("TriModXYangle",-95.0); 
  A.setVar("TriModXStep",0.0);
  A.setVar("TriModYStep",0.0);
  A.setVar("TriFlightAAngleXY1",30.0);
  A.setVar("TriFlightAAngleXY2",0.0);
  A.setVar("TriFlightAXStep",-2.0);
  A.setVar("TriFlightAWidth",33.0);
  A.setVar("TriFlightBAngleXY2",5.0);
  A.setVar("TriFlightBWidth",23.0);

  A.setVar("TriModNCorner",3);  
  A.setVar("TriModCorner1",Geometry::Vec3D(2.1,23.88,0)); 
  A.setVar("TriModCorner2",Geometry::Vec3D(11.63,-13.82,0)); 
  A.setVar("TriModCorner3",Geometry::Vec3D(-11.61,13.79,0.0)); 
  A.setVar("TriModAbsolute1",1);
  A.setVar("TriModAbsolute2",1);
  A.setVar("TriModAbsolute3",1);

  A.setVar("TriModNInnerUnits",1); 
  A.setVar("TriModInner1Corner1",Geometry::Vec3D(-5.86,10.95,0.0)); 
  A.setVar("TriModInner1Corner2",Geometry::Vec3D(1.41,16.12,0.0)); 
  A.setVar("TriModInner1Corner3",Geometry::Vec3D(9.04,-8.4,0)); 
  A.setVar("TriModInner1Absolute1",1);
  A.setVar("TriModInner1Absolute2",1);
  A.setVar("TriModInner1Absolute3",1);

  A.setVar("TriFlightASideIndex",3);
  A.setVar("TriFlightBSideIndex",4);

  A.setVar("TriModNInnerLayers",4); 
  A.setVar("TriModInnerThick0",0.3); 
  A.setVar("TriModInnerThick1",0.005); 
  A.setVar("TriModInnerThick2",0.3); 
  A.setVar("TriModInnerMat0","Aluminium");
  A.setVar("TriModInnerMat1","Gadolinium");
  A.setVar("TriModInnerMat2","Aluminium");
  A.setVar("TriModInnerMat3","Void");

  return;
}

void
setTS1MarkVI(defaultConfig& A)
  /*!
    Default configureation for MarkVI
    \param A :: Paramter for default config
   */
{
  ELog::RegMethod RegA("DefUnitsTS1Mark[F]","setTS1MarkVI");

  A.setOption("targetType","t1CylFluxTrap");
  A.setOption("CH4ModType","Layer");
  A.setOption("H2ModType","Layer");
  A.setOption("WaterModType","Layer");
  A.setOption("CH4PreType","Flat");

  A.setVar("sdefZOffset",0.0);
  A.setVar("t1CylFluxTrapNCone",0);

  A.setVar("TriModPoisonThick",0.005);
  A.setVar("TriModPCladThick",0.3);
  A.setVar("TriModPoisonMat","Gadolinium");
  A.setVar("TriModXYangle",-95.0); 
  A.setVar("TriModXStep",0.0);
  A.setVar("TriModYStep",0.0);
  A.setVar("TriFlightAAngleXY1",30.0);
  A.setVar("TriFlightAAngleXY2",0.0);
  A.setVar("TriFlightAXStep",-2.0);
  A.setVar("TriFlightAWidth",33.0);
  A.setVar("TriFlightBAngleXY2",5.0);
  A.setVar("TriFlightBWidth",23.0);

  A.setVar("TriModNCorner",4);  
  A.setVar("TriModCorner1",Geometry::Vec3D(2.1,23.88,0.0)); 
  A.setVar("TriModCorner2",Geometry::Vec3D(-11.61,13.79,0.0)); 
  A.setVar("TriModCorner3",Geometry::Vec3D(11.63,-13.82,0)); 
  A.setVar("TriModCorner4",Geometry::Vec3D(21.48,-2.1,0.0)); 
  A.setVar("TriModAbsolute1",1);
  A.setVar("TriModAbsolute2",1);
  A.setVar("TriModAbsolute3",1);
  A.setVar("TriModAbsolute4",1);

  A.setVar("TriModNInnerUnits",0); 
  A.setVar("TriModInner1Corner1",Geometry::Vec3D(-5.86,10.95,0.0)); 
  A.setVar("TriModInner1Corner2",Geometry::Vec3D(1.41,16.12,0.0)); 
  A.setVar("TriModInner1Corner3",Geometry::Vec3D(9.04,-8.4,0)); 
  A.setVar("TriModInner1Corner4",Geometry::Vec3D(13.10,-1.6,0)); 
  A.setVar("TriModInner1Absolute1",1);
  A.setVar("TriModInner1Absolute2",1);
  A.setVar("TriModInner1Absolute3",1);
  A.setVar("TriModInner1Absolute4",1);


  A.setVar("TriFlightASideIndex",3);
  A.setVar("TriFlightBSideIndex",5);

  A.setVar("TriModNInnerLayers",2); 
  A.setVar("TriModInnerThick0",0.3); 
  A.setVar("TriModInnerThick1",0.005); 
  A.setVar("TriModInnerThick2",0.3); 
  A.setVar("TriModInnerMat0","Aluminium");
  A.setVar("TriModInnerMat1","Gadolinium");
  A.setVar("TriModInnerMat0","Aluminium");
  A.setVar("TriModInnerMat1","Void");


  A.setVar("WaterPipeNSegIn",1);
  // Central point:
  ///  A.setVar("WaterPipePPt0",Geometry::Vec3D(0.0,-1.0,15.0));
  //A.setVar("WaterPipePPt1",Geometry::Vec3D(0.0,29.25,15.0));

  A.setVar("WaterPipePPt0",Geometry::Vec3D(14.65,-10.0,2.70));
  A.setVar("WaterPipePPt1",Geometry::Vec3D(14.65,29.25,2.70));
  // Central point [Top]:

  A.setVar("WaterPipeNRadii",3);
  A.setVar("WaterPipeRadius0",0.40);
  A.setVar("WaterPipeRadius1",0.60);
  A.setVar("WaterPipeRadius2",1.0);

  A.setVar("WaterPipeMat0","H2O");
  A.setVar("WaterPipeMat1","Aluminium");
  A.setVar("WaterPipeMat2","Void");

  A.setVar("WaterPipeTemp0",0.0);
  A.setVar("WaterPipeTemp1",0.0);
  A.setVar("WaterPipeTemp2",0.0);

  A.setVar("WaterPipeActive0",3);
  A.setVar("WaterPipeActive1",3);

  A.setVar("WaterReturnNSegIn",1);
  // Central point:
  A.setVar("WaterReturnPPt0",Geometry::Vec3D(-15.68,-1.0,2.62));
  A.setVar("WaterReturnPPt1",Geometry::Vec3D(-15.68,29.25,2.62));
  // Central point [Top]:

  A.setVar("WaterReturnNRadii",3);
  A.setVar("WaterReturnRadius0",0.4);
  A.setVar("WaterReturnRadius1",0.6);
  A.setVar("WaterReturnRadius2",1.0);

  A.setVar("WaterReturnMat0","H2O");
  A.setVar("WaterReturnMat1","Aluminium");
  A.setVar("WaterReturnMat2","Void");

  A.setVar("WaterReturnTemp0",0.0);
  A.setVar("WaterReturnTemp1",0.0);
  A.setVar("WaterReturnTemp2",0.0);

  A.setVar("WaterReturnActive0",3);
  A.setVar("WaterReturnActive1",3);

  A.setVar("H2LayerXStep",5.2);      


  A.setVar("H2LayerXStep",5.2);      
  A.setVar("H2LayerYStep",20.25);  // +9.8    
  A.setVar("H2LayerZStep",-12.20);     
  A.setVar("H2LayerXYangle",100.0);  
  A.setVar("H2LayerZangle",0.0);  

  
  A.setVar("H2LayerNPoison",0); 
  A.setVar("H2LayerPYStep1",0.0);       // for 1 poison layer
  A.setVar("H2LayerPYStep2",0.8);       // for 2 poison layer
  A.setVar("H2LayerPGdThick",0.005);      //
  A.setVar("H2LayerPCladThick",0.1);      //             
  A.setVar("H2LayerPoisonMat","Gadolinium");      
  
  A.setVar("H2LayerPCladMat","Aluminium");      
  A.setVar("H2LayerPoisonMat","Gadolinium");      


  A.setVar("H2LayerNLayer",9);   

  A.setVar("H2LayerHeight1",13.8);   
  A.setVar("H2LayerWidth1",14.2);   
  A.setVar("H2LayerFront1",2.0);   
  A.setVar("H2LayerBack1",0.0);   
  A.setVar("H2LayerMat1","H2O");   

  // al inner
  A.setVar("H2LayerLayer2",0.3); 
  A.setVar("H2LayerHeight2",0.0);
  A.setVar("H2LayerWidth2",0.0);   
  A.setVar("H2LayerFront2",0.0);   
  A.setVar("H2LayerBack2",0.3);             
  A.setVar("H2LayerMat2","Aluminium");   
  
  // void
  A.setVar("H2LayerLayer3",0.4);
  A.setVar("H2LayerHeight3",0.0);      
  A.setVar("H2LayerWidth3",0.0);    
  A.setVar("H2LayerFront3",0.0);   
  A.setVar("H2LayerBack3",0.4);   
  A.setVar("H2LayerMat3","Void");     

  // Al layer
  A.setVar("H2LayerLayer4",0.0);
  A.setVar("H2LayerHeight4",0.0);        
  A.setVar("H2LayerWidth4",0.0);   
  A.setVar("H2LayerFront4",0.0);   
  A.setVar("H2LayerBack4",0.3);   
  A.setVar("H2LayerMat4","Aluminium");  
  A.setVar("H2LayerTemp4",20.0);   
      
  // H2 
  A.setVar("H2LayerLayer5",0.0);
  A.setVar("H2LayerHeight5",0.0);           
  A.setVar("H2LayerFront5",0.0);   
  A.setVar("H2LayerBack5",3.5);   
  A.setVar("H2LayerMat5","ParaOrtho%80");      // H2 
  A.setVar("H2LayerTemp5",20.0);   

  // al
  A.setVar("H2LayerLayer6",0.3); 
  A.setVar("H2LayerFront6",0.3); 
  A.setVar("H2LayerBack6",0.3); 
  A.setVar("H2LayerHeight6",0.3); 
  //  A.setVar("H2LayerHeight6",0.3);        
  A.setVar("H2LayerMat6","Aluminium");   
  A.setVar("H2LayerTemp6",20.0); 

  // Outer vac layer 
  A.setVar("H2LayerLayer7",0.5);
  A.setVar("H2LayerFront7",0.0);   
  A.setVar("H2LayerBack7",0.2);        
  A.setVar("H2LayerMat7","Void");     

  // al
  A.setVar("H2LayerLayer8",0.3); 
  A.setVar("H2LayerFront8",0.3);   
  A.setVar("H2LayerBack8",0.3);         
  A.setVar("H2LayerMat8","Aluminium");   
 
  // Outer vac layer 
  A.setVar("H2LayerLayer9",0.5);   
  A.setVar("H2LayerMat9","Void");     

  A.setVar("CH4LayerYStep",1.8);  // +9.8    
  A.setVar("CH4LayerNLayer",7);   
  A.setVar("CH4LayerHeight1",12.1);   
  A.setVar("CH4LayerLeft1",5.5);   
  A.setVar("CH4LayerRight1",5.5);   
  A.setVar("CH4LayerFront1",1.5);   
  A.setVar("CH4LayerBack1",2.1);   
  A.setVar("CH4LayerMat1","Void");   

  // al inner
  A.setVar("CH4LayerLayer2",0.3); 
  A.setVar("CH4LayerLeft2",0.3);    
  A.setVar("CH4LayerRight2",0.3);    
  A.setVar("CH4LayerFront2",0.3);   
  A.setVar("CH4LayerBack2",0.3);   
  A.setVar("CH4LayerHeight2",0.3);        
  A.setVar("CH4LayerMat2","Aluminium");   
  A.setVar("CH4LayerTemp2",110.0); 
  
  // outerlik layer
  A.setVar("CH4LayerLayer3",2.6);
  A.setVar("CH4LayerHeight3",0.4);   
  A.setVar("CH4LayerWidth3",3.0);    
  A.setVar("CH4LayerFront3",2.2);   
  A.setVar("CH4LayerBack3",2.4);   
  A.setVar("CH4LayerMat3","CH4Liq110K");   
  A.setVar("CH4LayerTemp3",110.0); 

  // 
  A.setVar("CH4LayerLayer4",0.3);   
  A.setVar("CH4LayerWidth4",0.3);   
  A.setVar("CH4LayerMat4","Aluminium");     
  A.setVar("CH4LayerTemp4",110.0); 

  // Vac layer
  A.setVar("CH4LayerFront5",0.3);   
  A.setVar("CH4LayerBack5",0.3);   
  A.setVar("CH4LayerRight5",0.3);   
  A.setVar("CH4LayerLeft5",0.3);   
  A.setVar("CH4LayerTop5",0.3);   
  A.setVar("CH4LayerBase5",0.3);   
  A.setVar("CH4LayerMat5","Void");     

  // Outer Al layer
  A.setVar("CH4LayerFront6",0.3);   
  A.setVar("CH4LayerBack6",0.3);   
  A.setVar("CH4LayerRight6",0.3);   
  A.setVar("CH4LayerLeft6",0.3);   
  A.setVar("CH4LayerTop6",0.3);   
  A.setVar("CH4LayerBase6",0.3);   
  A.setVar("CH4LayerMat6","Aluminium");     

  // Outer Clerance layer
  A.setVar("CH4LayerLayer7",0.3);   
  A.setVar("CH4LayerMat7","Void");     

  return;
}

void
setTS1MarkVIb(defaultConfig& A)
  /*!
    Default configuration for MarkVIb
    \param A :: Paramter for default config
   */
{
  ELog::RegMethod RegA("DefUnitsTS1Mark[F]","setTS1MarkVIb");

  setTS1MarkVI(A);
  A.setOption("CH4ModType","Basic");
  return;
}

} // NAMESPACE mainSystem
