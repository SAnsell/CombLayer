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
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

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
SinbadVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Sinbad ()
    \param Control :: Function data base to add constants too
  */
{
  // Detectors

  Control.addVariable("49DetectorPositionN",4);  

  Control.addVariable("49Detector0Active",1);   
  Control.addVariable("49Detector1Active",0);   
  Control.addVariable("49Detector2Active",0);   
  Control.addVariable("49Detector3Active",0);   

  Control.addVariable("49DetectorXStep",2.5);  
  Control.addVariable("49DetectorYStep",0.0);  
  Control.addVariable("49DetectorZStep",-6.6);  
  Control.addVariable("49DetectorXYAngle",0.0);  
  Control.addVariable("49DetectorZAngle",0.0);  
  Control.addVariable("49DetectorMat","Rh");  
  Control.addVariable("49DetectorRadius",1.27);   
  Control.addVariable("49DetectorLength",0.1); 

  Control.addVariable("49DetectorYStep1",5.1);  
  Control.addVariable("49DetectorYStep2",10.18);  
  Control.addVariable("49DetectorYStep3",15.96);  
  Control.addVariable("49DetectorYStep4",21.74);  
  Control.addVariable("49DetectorYStep5",25.44);  
  Control.addVariable("49DetectorYStep6",27.44);  
  Control.addVariable("49DetectorYStep7",29.44);  

  Control.addVariable("49DetectorYStep8",29.94);  
  Control.addVariable("49DetectorYStep9",32.94);  
  Control.addVariable("49DetectorYStep10",34.44);  

  //FPlate side

  Control.addVariable("FPlateXStep",91.45);  
  Control.addVariable("FPlateYStep",-1.45);  
  Control.addVariable("FPlateZStep",95.5);  

  Control.addVariable("FPlateWidth",182.9);   
  Control.addVariable("FPlateHeight",191.0);  

  Control.addVariable("FPlateNSlab",6);
  Control.addVariable("FPlateThick0",0.70); 
  Control.addVariable("FPlateMat0","Void"); 
  Control.addVariable("FPlateThick1",15.0); 
  Control.addVariable("FPlateMat1","Graphite"); 
  Control.addVariable("FPlateThick2",0.60); 
  Control.addVariable("FPlateMat2","Void");  
  Control.addVariable("FPlateThick3",5.08); 
  Control.addVariable("FPlateMat3","sbadLead");  
  Control.addVariable("FPlateThick4",0.70); 
  Control.addVariable("FPlateMat4","Void");  
  Control.addVariable("FPlateThick5",3.18); 
  Control.addVariable("FPlateMat5","sbadMildSteel");  
  Control.addVariable("FPlateAlWindowRadius",56.06); 

  // define centre
  Control.addVariable("49ShieldXStep",0.0);  
  Control.addVariable("49ShieldYStep",0.0);  
  Control.addVariable("49ShieldZStep",0.0);  
  Control.addVariable("49ShieldXYAngle",0.0);  
  Control.addVariable("49ShieldZAngle",0.0);  

  Control.addVariable("49ShieldNSlab",26);
  
  Control.addVariable("49ShieldWidth",182.9);   
  Control.addVariable("49ShieldHeight",191.0);  

  Control.addVariable("49ShieldThick0",0.70); 
  Control.addVariable("49ShieldMat0","Void"); 
  Control.addVariable("49ShieldThick1",0.50); 
  Control.addVariable("49ShieldMat1","Boral5"); 
  Control.addVariable("49ShieldThick2",0.0015);
  Control.addVariable("49ShieldMat2","Void");  
  Control.addVariable("49ShieldThick3",5.08); 
  Control.addVariable("49ShieldMat3","sbadMildSteel");  
  Control.addVariable("49ShieldThick4",0.70); 
  Control.addVariable("49ShieldMat4","Void");  
  Control.addVariable("49ShieldThick5",5.08); 
  Control.addVariable("49ShieldMat5","sbadMildSteel");  
  Control.addVariable("49ShieldThick6",0.70); 
  Control.addVariable("49ShieldMat6","Void");  
  Control.addVariable("49ShieldThick7",5.08); 
  Control.addVariable("49ShieldMat7","sbadMildSteel");  
  Control.addVariable("49ShieldThick8",0.70); 
  Control.addVariable("49ShieldMat8","Void");  
  Control.addVariable("49ShieldThick9",3.00); 
  Control.addVariable("49ShieldMat9","Stainless304");  
  Control.addVariable("49ShieldThick10",18.31); 
  Control.addVariable("49ShieldMat10","H2O");  
  Control.addVariable("49ShieldThick11",3.00); 
  Control.addVariable("49ShieldMat11","Stainless304");  
  Control.addVariable("49ShieldThick12",0.70); 
  Control.addVariable("49ShieldMat12","Void");  
  Control.addVariable("49ShieldThick13",5.08); 
  Control.addVariable("49ShieldMat13","sbadMildSteel");  
  Control.addVariable("49ShieldThick14",0.70); 
  Control.addVariable("49ShieldMat14","Void");  
  Control.addVariable("49ShieldThick15",2.50); 
  Control.addVariable("49ShieldMat15","sbadSulphurSteel");
  Control.addVariable("49ShieldThick16",19.80); 
  Control.addVariable("49ShieldMat16","H2O");  
  Control.addVariable("49ShieldThick17",2.50); 
  Control.addVariable("49ShieldMat17","Stainless304");  
  Control.addVariable("49ShieldThick18",0.70); 
  Control.addVariable("49ShieldMat18","Void");  
  Control.addVariable("49ShieldThick19",5.08); 
  Control.addVariable("49ShieldMat19","sbadMildSteel");  
  Control.addVariable("49ShieldThick20",0.70); 
  Control.addVariable("49ShieldMat20","Void");  
  Control.addVariable("49ShieldThick21",5.08); 
  Control.addVariable("49ShieldMat21","sbadMildSteel");  
  Control.addVariable("49ShieldThick22",30.70); 
  Control.addVariable("49ShieldMat22","Void");  
  Control.addVariable("49ShieldThick23",15.24); 
  Control.addVariable("49ShieldMat23","Concrete");  
  Control.addVariable("49ShieldThick24",2.00); 
  Control.addVariable("49ShieldMat24","Void");  
  Control.addVariable("49ShieldThick25",15.24); 
  Control.addVariable("49ShieldMat25","Concrete");  



  //FPlate side

  Control.addVariable("FPlateXStep",91.45);  
  Control.addVariable("FPlateYStep",-1.45);  
  Control.addVariable("FPlateZStep",95.5);  
  Control.addVariable("FPlateXYAngle",0.0);  
  Control.addVariable("FPlateZAngle",0.0);  

  Control.addVariable("FPlateWidth",182.9);   
  Control.addVariable("FPlateHeight",191.0);  

  Control.addVariable("FPlateNSlab",6);
  Control.addVariable("FPlateThick0",0.70); 
  Control.addVariable("FPlateMat0","Void"); 
  Control.addVariable("FPlateThick1",15.0); 
  Control.addVariable("FPlateMat1","Graphite"); 
  Control.addVariable("FPlateThick2",0.60); 
  Control.addVariable("FPlateMat2","Void");  
  Control.addVariable("FPlateThick3",5.08); 
  Control.addVariable("FPlateMat3","sbadLead");  
  Control.addVariable("FPlateThick4",0.70); 
  Control.addVariable("FPlateMat4","Void");  
  Control.addVariable("FPlateThick5",3.18); 
  Control.addVariable("FPlateMat5","sbadMildSteel");  
  Control.addVariable("FPlateAlWindowRadius",56.06); 


  // Fission Plate

  Control.addVariable("49FissionPlateXStep",0.0);  
  Control.addVariable("49FissionPlateYStep",0.0);   
  Control.addVariable("49FissionPlateZStep",0.0);   
  Control.addVariable("49FissionPlateXYAngle",0.0);   
  Control.addVariable("49FissionPlateZAngle",0.0);   

  Control.addVariable("49FissionPlateHeight",102.1);  
  Control.addVariable("49FissionPlateWidth",119.0);  
  Control.addVariable("49FissionPlateNSlab",6);

  Control.addVariable("49FissionPlateThick0",1.2); 
  Control.addVariable("49FissionPlateThick1",0.1); 
  Control.addVariable("49FissionPlateThick2",0.1); 
  Control.addVariable("49FissionPlateThick3",0.2); 
  Control.addVariable("49FissionPlateThick4",0.1); 
  Control.addVariable("49FissionPlateThick5",1.2); 

  Control.addVariable("49FissionPlateMat0","sbadMildSteel");   
  Control.addVariable("49FissionPlateMat1","Void");   
  Control.addVariable("49FissionPlateMat2","sbadMildSteel");   
  Control.addVariable("49FissionPlateMat3","Uranium");   
  Control.addVariable("49FissionPlateMat4","Void");   
  Control.addVariable("49FissionPlateMat5","sbadMildSteel");   

  Control.addVariable("49FissionPlateDivIndex0",3);   
  Control.addVariable("49FissionPlateNXSpace",5);   
  Control.addVariable("49FissionPlateNZSpace",5);   
  
  Control.addVariable("49FissionPlateXPt0",-30.0);   
  Control.addVariable("49FissionPlateXPt1",-10.0);   
  Control.addVariable("49FissionPlateXPt2",0.0);   
  Control.addVariable("49FissionPlateXPt3",10.0);   
  Control.addVariable("49FissionPlateXPt4",-30.0);

  Control.addVariable("49FissionPlateZPt0",-30.0);   
  Control.addVariable("49FissionPlateZPt1",-10.0);   
  Control.addVariable("49FissionPlateZPt2",0.0);   
  Control.addVariable("49FissionPlateZPt3",10.0);   
  Control.addVariable("49FissionPlateZPt4",-30.0);

  Control.addVariable("49FissionPlateMatL0","Uranium");   
  Control.addVariable("49FissionPlateMatTempL0",300.0);   



  Control.addVariable("49Detector0Active",1);   
  Control.addVariable("49DetectorXStep",2.5);  
  Control.addVariable("49DetectorYStep",5.1);  
  Control.addVariable("49DetectorZStepZ1",-6.6);  

  Control.addVariable("49DetectorActive1",1);   
  Control.addVariable("49DetectorOffSetX1",2.5);  
  Control.addVariable("49DetectorOffSetY1",5.1);  
  Control.addVariable("49DetectorOffSetZ1",-6.6);  

  Control.addVariable("49DetectorActive2",1);   
  Control.addVariable("49DetectorOffSetX2",2.5);  
  Control.addVariable("49DetectorOffSetY2",10.18001);  
  Control.addVariable("49DetectorOffSetZ2",-6.6);  

  Control.addVariable("49DetectorActive3",1);   
  Control.addVariable("49DetectorOffSetX3",2.5);  
  Control.addVariable("49DetectorOffSetY3",15.96);  
  Control.addVariable("49DetectorOffSetZ3",-6.6);  

  Control.addVariable("49DetectorActive4",1);   
  Control.addVariable("49DetectorOffSetX4",2.5);  
  Control.addVariable("49DetectorOffSetY4",21.74);  
  Control.addVariable("49DetectorOffSetZ4",-6.6);  

  Control.addVariable("49DetectorActive5",1);   
  Control.addVariable("49DetectorOffSetX5",2.5);  
  Control.addVariable("49DetectorOffSetY5",25.44);  
  Control.addVariable("49DetectorOffSetZ5",-6.6);  

  Control.addVariable("49DetectorActive6",0);   
  Control.addVariable("49DetectorOffSetX6",2.5);  
  Control.addVariable("49DetectorOffSetY6",27.44);  
  Control.addVariable("49DetectorOffSetZ6",-6.6);  

  Control.addVariable("49DetectorActive7",0);   
  Control.addVariable("49DetectorOffSetX7",2.5);  
  Control.addVariable("49DetectorOffSetY7",29.44);  
  Control.addVariable("49DetectorOffSetZ7",-6.6);  

  Control.addVariable("49DetectorActive8",0);   
  Control.addVariable("49DetectorOffSetX8",2.5);  
  Control.addVariable("49DetectorOffSetY8",29.94);  
  Control.addVariable("49DetectorOffSetZ8",-6.6);  

  Control.addVariable("49DetectorActive9",1);   
  Control.addVariable("49DetectorOffSetX9",2.5);  
  Control.addVariable("49DetectorOffSetY9",32.94);  
  Control.addVariable("49DetectorOffSetZ9",-6.6);  

  Control.addVariable("49DetectorActive10",0);   
  Control.addVariable("49DetectorOffSetX10",2.5);  
  Control.addVariable("49DetectorOffSetY10",34.44);  
  Control.addVariable("49DetectorOffSetZ10",-6.6);  

  Control.addVariable("49DetectorActive11",0);   
  Control.addVariable("49DetectorOffSetX11",2.5);  
  Control.addVariable("49DetectorOffSetY11",38.94);  
  Control.addVariable("49DetectorOffSetZ11",-6.6);  

  Control.addVariable("49DetectorActive12",1);   
  Control.addVariable("49DetectorOffSetX12",2.5);  
  Control.addVariable("49DetectorOffSetY12",40.44);  
  Control.addVariable("49DetectorOffSetZ12",-6.6);  

  Control.addVariable("49DetectorActive13",0);   
  Control.addVariable("49DetectorOffSetX13",2.5);  
  Control.addVariable("49DetectorOffSetY13",43.44);  
  Control.addVariable("49DetectorOffSetZ13",-6.6);  

  Control.addVariable("49DetectorActive14",1);   
  Control.addVariable("49DetectorOffSetX14",2.5);  
  Control.addVariable("49DetectorOffSetY14",46.75);  
  Control.addVariable("49DetectorOffSetZ14",-6.6);  

  Control.addVariable("49DetectorActive15",1);   
  Control.addVariable("49DetectorOffSetX15",2.5);  
  Control.addVariable("49DetectorOffSetY15",52.53);  
  Control.addVariable("49DetectorOffSetZ15",-6.6);  

  Control.addVariable("49DetectorActive16",1);   
  Control.addVariable("49DetectorOffSetX16",2.5);  
  Control.addVariable("49DetectorOffSetY16",55.73);  
  Control.addVariable("49DetectorOffSetZ16",-6.6);  

  Control.addVariable("49DetectorActive17",1);   
  Control.addVariable("49DetectorOffSetX17",2.5);  
  Control.addVariable("49DetectorOffSetY17",57.33);  
  Control.addVariable("49DetectorOffSetZ17",-6.6);  

  Control.addVariable("49DetectorActive18",1);   
  Control.addVariable("49DetectorOffSetX18",2.5);  
  Control.addVariable("49DetectorOffSetY18",59.73);  
  Control.addVariable("49DetectorOffSetZ18",-6.6);  

  Control.addVariable("49DetectorActive19",1);   
  Control.addVariable("49DetectorOffSetX19",2.5);  
  Control.addVariable("49DetectorOffSetY19",63.73);  
  Control.addVariable("49DetectorOffSetZ19",-6.6);  

  Control.addVariable("49DetectorActive20",1);   
  Control.addVariable("49DetectorOffSetX20",2.5);  
  Control.addVariable("49DetectorOffSetY20",65.83);  
  Control.addVariable("49DetectorOffSetZ20",-6.6);  

  Control.addVariable("49DetectorActive21",1);   
  Control.addVariable("49DetectorOffSetX21",2.5);  
  Control.addVariable("49DetectorOffSetY21",71.43);  
  Control.addVariable("49DetectorOffSetZ21",-6.6);  

  Control.addVariable("49DetectorActive22",1);   
  Control.addVariable("49DetectorOffSetX22",2.5);  
  Control.addVariable("49DetectorOffSetY22",74.53);  
  Control.addVariable("49DetectorOffSetZ22",-6.6);  



  Control.addVariable("49DetectorLayerN",1);

 // Rh
  Control.addVariable("49DetectorDiameterLayer0",1.27);   
  Control.addVariable("49DetectorWidthLayer0",0.0015); 
  Control.addVariable("49DetectorMaterialLayer0","Rh");   



  // S cast
  // Control.addVariable("49DetectorDiameterLayer0",5.1);   
  //  Control.addVariable("49DetectorWidthLayer0",0.56); 
  // Control.addVariable("49DetectorMaterialLayer0",101);   


  // Cd
  // Control.addVariable("49DetectorDiameterLayer0",1.27);   
  // Control.addVariable("49DetectorWidthLayer0",0.127); //0.0 
  // Control.addVariable("49DetectorMaterialLayer0",106);   

  // Mn
  // Control.addVariable("49DetectorDiameterLayer1",1.27);   
  // Control.addVariable("49DetectorWidthLayer1",0.015); 
  // Control.addVariable("49DetectorMaterialLayer1",104);   




 
  // S cast



 //Cd
  Control.addVariable("49DetectorDiameterLayer2",1.27);   
  Control.addVariable("49DetectorWidthLayer2",0.127); //0.0 
  Control.addVariable("49DetectorMaterialLayer2","Cadnium");   
  // Au
  Control.addVariable("49DetectorDiameterLayer4",1.27);   
  Control.addVariable("49DetectorWidthLayer4",0.0005); 
  Control.addVariable("49DetectorMaterialLayer4","Gold");   

  // S pressed
  Control.addVariable("49DetectorDiameterLayerX",3.81);   
  Control.addVariable("49DetectorWidthLayerX",0.241); 
  Control.addVariable("49DetectorMateriaLayerX","Rh");   



  //FPlate side

  Control.addVariable("75CaveOffSetX",0.0);  
  Control.addVariable("75CaveOffSetY",0.0);  
  Control.addVariable("75CaveOffSetZ",0.0);  

  //L: left, R: right, U: up, D: down, I: in (towards nestor), O: out
  Control.addVariable("75CaveLengthL",90);   
  Control.addVariable("75CaveLengthR",90.);   
  Control.addVariable("75CaveHeightU",90.);  
  Control.addVariable("75CaveHeightD",90.);  
  Control.addVariable("75CaveWidthI",17.0);  
  Control.addVariable("75CaveWidthO",135.29);
  // filled up with water  
  Control.addVariable("75CaveMaterial","Uranium"); 
  Control.addVariable("75CaveTemperature",300.0); 


  Control.addVariable("75CaveSlabN",4);

  // incremental thickness
  Control.addVariable("75CaveLengthLSlab0",2.5);   
  Control.addVariable("75CaveLengthRSlab0",2.5);
  // h inventate   
  Control.addVariable("75CaveHeightUSlab0",2.5);  
  Control.addVariable("75CaveHeightDSlab0",2.5);  
  Control.addVariable("75CaveWidthISlab0",3.21);  
  Control.addVariable("75CaveWidthOSlab0",0.0);  
  Control.addVariable("75CaveMaterialSlab0","Void"); 
  // the al window in slab 0
  Control.addVariable("75CaveAlWindowRadius",56.06); 
  //

  Control.addVariable("75CaveLengthLSlab1",0.0);   
  Control.addVariable("75CaveLengthRSlab1",0.0);   
  Control.addVariable("75CaveHeightUSlab1",0.0);  
  Control.addVariable("75CaveHeightDSlab1",0.0);  
  Control.addVariable("75CaveWidthISlab1",5.38);  
  Control.addVariable("75CaveWidthOSlab1",0.0);  
  Control.addVariable("75CaveMaterialSlab1","Void"); 

  Control.addVariable("75CaveLengthLSlab2",-30.5);   
  Control.addVariable("75CaveLengthRSlab2",-30.5);   
  Control.addVariable("75CaveHeightUSlab2",0.0);  
  Control.addVariable("75CaveHeightDSlab2",0.0);  
  Control.addVariable("75CaveWidthISlab2",0.65);  
  Control.addVariable("75CaveWidthOSlab2",0.0);  
  Control.addVariable("75CaveMaterialSlab2","sbadLead");

  Control.addVariable("75CaveLengthLSlab3",0.0);   
  Control.addVariable("75CaveLengthRSlab3",0.0);   
  Control.addVariable("75CaveHeightUSlab3",0.0);  
  Control.addVariable("75CaveHeightDSlab3",0.0);  
  Control.addVariable("75CaveWidthISlab3",28.91);  
  Control.addVariable("75CaveWidthOSlab3",0.0);  
  Control.addVariable("75CaveMaterialSlab3",1); 



  Control.addVariable("75CaveWidthSlab0",2.0); 

  Control.addVariable("75CaveMaterialSlab0","Void"); 


  Control.addVariable("75CaveWidthSlab1",15.0); 
  Control.addVariable("75CaveMaterialSlab1",1); 

  return;
}

}  // NAMESPACE setVariable
