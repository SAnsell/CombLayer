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
#include "PipeGenerator.h"
#include "essLinacVariableSetup.h"

namespace setVariable
{
void
EssLinacSpokeVariables(FuncDataBase &Control)
  /*!
    Create all the PMQ variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("essVariables[F]","EssLinacSpokeVariables");

  // Beam Pipe

  setVariable::PipeGenerator PipeGen;
  PipeGen.setPipe(50.0,0.0);
  PipeGen.setWindow(50.0,0.0);
  PipeGen.setFlange(50.0,0.0);

  double spokeCavityLength = 98.8;
  double quadLength = 25.0;
  
  PipeGen.generatePipe(Control,"LinacSpokeSection0",7.71); //Adjusted to place first quad in correct spoke according to lattest lattice files
  Control.addVariable("LinacSpokeSection0VoidMat","Air");            

  Control.addVariable("LinacVacuumPipe0Thickness",0.2);
  Control.addVariable("LinacVacuumPipe0VoidMat","Void");
  Control.addVariable("LinacVacuumPipe0Length",7.71);
  Control.addVariable("LinacVacuumPipe0Mat","MARS_STST");
  Control.addVariable("LinacVacuumPipe0InnerRadius",2.001);
  
  PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(1),quadLength);
  Control.addVariable("LinacSpokeSection"+std::to_string(1)+"VoidMat","Air");
    
  Control.addVariable("LinacQXQuad5"+std::to_string(1)+"Length",quadLength);
  Control.addVariable("LinacQXQuad5"+std::to_string(1)+"VoidMat","Void");
  Control.addVariable("LinacQXQuad5"+std::to_string(1)+"Thickness",0.2);
  Control.addVariable("LinacQXQuad5"+std::to_string(1)+"PipeMat","MARS_STST");
  Control.addVariable("LinacQXQuad5"+std::to_string(1)+"YokeMat","MARS_QXYOKE");
  Control.addVariable("LinacQXQuad5"+std::to_string(1)+"CoilMat","MARS_QXCOIL");
  Control.addVariable("LinacQXQuad5"+std::to_string(1)+"PipeRad",2.855);
  
  PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(2),58.0);
  Control.addVariable("LinacSpokeSection"+std::to_string(2)+"VoidMat","Air");

  Control.addVariable("LinacVacuumPipe"+std::to_string(2)+"Thickness",0.2);
  Control.addVariable("LinacVacuumPipe"+std::to_string(2)+"VoidMat","Void");
  Control.addVariable("LinacVacuumPipe"+std::to_string(2)+"Length",58.0);
  Control.addVariable("LinacVacuumPipe"+std::to_string(2)+"Mat","MARS_STST");
  Control.addVariable("LinacVacuumPipe"+std::to_string(2)+"InnerRadius",2.855);

  PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(3),quadLength);
  Control.addVariable("LinacSpokeSection"+std::to_string(3)+"VoidMat","Air");
    
  Control.addVariable("LinacQXQuad5"+std::to_string(3)+"Length",quadLength);
  Control.addVariable("LinacQXQuad5"+std::to_string(3)+"VoidMat","Void");
  Control.addVariable("LinacQXQuad5"+std::to_string(3)+"Thickness",0.2);
  Control.addVariable("LinacQXQuad5"+std::to_string(3)+"PipeMat","MARS_STST");
  Control.addVariable("LinacQXQuad5"+std::to_string(3)+"YokeMat","MARS_QXYOKE");
  Control.addVariable("LinacQXQuad5"+std::to_string(3)+"CoilMat","MARS_QXCOIL");
  Control.addVariable("LinacQXQuad5"+std::to_string(3)+"PipeRad",2.855);
  
  PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(4),40.00);
  Control.addVariable("LinacSpokeSection"+std::to_string(4)+"VoidMat","Air");

  Control.addVariable("LinacVacuumPipe"+std::to_string(4)+"Thickness",0.2);
  Control.addVariable("LinacVacuumPipe"+std::to_string(4)+"VoidMat","Void");
  Control.addVariable("LinacVacuumPipe"+std::to_string(4)+"Length",40.00);
  Control.addVariable("LinacVacuumPipe"+std::to_string(4)+"Mat","MARS_STST");
  Control.addVariable("LinacVacuumPipe"+std::to_string(4)+"InnerRadius",2.855);

  PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(5),37.15);
  Control.addVariable("LinacSpokeSection"+std::to_string(5)+"VoidMat","Air");

  Control.addVariable("LinacVacuumPipe"+std::to_string(5)+"Thickness",0.2);
  Control.addVariable("LinacVacuumPipe"+std::to_string(5)+"VoidMat","Void");
  Control.addVariable("LinacVacuumPipe"+std::to_string(5)+"Length",37.15);
  Control.addVariable("LinacVacuumPipe"+std::to_string(5)+"Mat","MARS_STST");
  Control.addVariable("LinacVacuumPipe"+std::to_string(5)+"InnerRadius",2.801);
  
  int index = 0;
  //------------
  for(int i=(1+5);i<(13+5);i++){

    PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(i*12),spokeCavityLength);
    Control.addVariable("LinacSpokeSection"+std::to_string(i*12)+"VoidMat","Air");

    Control.addVariable("LinacSPKCVT"+std::to_string(i*12)+"Length",spokeCavityLength);
    Control.addVariable("LinacSPKCVT"+std::to_string(i*12)+"VoidMat","Void");
    Control.addVariable("LinacSPKCVT"+std::to_string(i*12)+"Thickness",0.4);
    Control.addVariable("LinacSPKCVT"+std::to_string(i*12)+"Mat","Niobium");

    
    PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(i*12+1),14.1);
    Control.addVariable("LinacSpokeSection"+std::to_string(i*12+1)+"VoidMat","Air");

    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+1)+"Thickness",0.2);
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+1)+"VoidMat","Void");
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+1)+"Length",14.1);
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+1)+"Mat","MARS_STST");
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+1)+"InnerRadius",2.801);
  
    PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(i*12+2),spokeCavityLength);
    Control.addVariable("LinacSpokeSection"+std::to_string(i*12+2)+"VoidMat","Air");
    
    Control.addVariable("LinacSPKCVT"+std::to_string(i*12+2)+"Length",spokeCavityLength);
    Control.addVariable("LinacSPKCVT"+std::to_string(i*12+2)+"VoidMat","Void");
    Control.addVariable("LinacSPKCVT"+std::to_string(i*12+2)+"Thickness",0.4);
    Control.addVariable("LinacSPKCVT"+std::to_string(i*12+2)+"Mat","Niobium");

  
    PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(i*12+3),37.15);
    Control.addVariable("LinacSpokeSection"+std::to_string(i*12+3)+"VoidMat","Air");

    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+3)+"Thickness",0.2);
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+3)+"VoidMat","Void");
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+3)+"Length",37.15);
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+3)+"Mat","MARS_STST");
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+3)+"InnerRadius",2.801);
    

  
    PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(i*12+4),16.0);
    Control.addVariable("LinacSpokeSection"+std::to_string(i*12+4)+"VoidMat","Air");

    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+4)+"Thickness",0.2);
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+4)+"VoidMat","Void");
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+4)+"Length",16.0);
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+4)+"Mat","MARS_STST");
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+4)+"InnerRadius",2.855);
 
    PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(i*12+5),quadLength);
    Control.addVariable("LinacSpokeSection"+std::to_string(i*12+5)+"VoidMat","Air");
    
    Control.addVariable("LinacQXQuad5"+std::to_string(i*12+5)+"Length",quadLength);
    Control.addVariable("LinacQXQuad5"+std::to_string(i*12+5)+"VoidMat","Void");
    Control.addVariable("LinacQXQuad5"+std::to_string(i*12+5)+"Thickness",0.2);
    Control.addVariable("LinacQXQuad5"+std::to_string(i*12+5)+"PipeMat","MARS_STST");
    Control.addVariable("LinacQXQuad5"+std::to_string(i*12+5)+"YokeMat","MARS_QXYOKE");
    Control.addVariable("LinacQXQuad5"+std::to_string(i*12+5)+"CoilMat","MARS_QXCOIL");
    Control.addVariable("LinacQXQuad5"+std::to_string(i*12+5)+"PipeRad",2.855);
 
    PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(i*12+6),58.0);
    Control.addVariable("LinacSpokeSection"+std::to_string(i*12+6)+"VoidMat","Air");

    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+6)+"Thickness",0.2);
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+6)+"VoidMat","Void");
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+6)+"Length",58.0);
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+6)+"Mat","MARS_STST");
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+6)+"InnerRadius",2.855);
     
    PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(i*12+7),quadLength);
    Control.addVariable("LinacSpokeSection"+std::to_string(i*12+7)+"VoidMat","Air");

    Control.addVariable("LinacQXQuad5"+std::to_string(i*12+7)+"Length",quadLength);
    Control.addVariable("LinacQXQuad5"+std::to_string(i*12+7)+"VoidMat","Void");
    Control.addVariable("LinacQXQuad5"+std::to_string(i*12+7)+"Thickness",0.2);
    Control.addVariable("LinacQXQuad5"+std::to_string(i*12+7)+"PipeMat","MARS_STST");
    Control.addVariable("LinacQXQuad5"+std::to_string(i*12+7)+"YokeMat","MARS_QXYOKE");
    Control.addVariable("LinacQXQuad5"+std::to_string(i*12+7)+"CoilMat","MARS_QXCOIL"); 
    Control.addVariable("LinacQXQuad5"+std::to_string(i*12+7)+"PipeRad",2.855);
  
    PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(i*12+8),16.0);
    Control.addVariable("LinacSpokeSection"+std::to_string(i*12+8)+"VoidMat","Air");
    
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+8)+"Thickness",0.2);
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+8)+"VoidMat","Void");
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+8)+"Length",16.0);
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+8)+"Mat","MARS_STST");
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+8)+"InnerRadius",2.855);

 
    PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(i*12+9),37.15);
    Control.addVariable("LinacSpokeSection"+std::to_string(i*12+9)+"VoidMat","Air");

    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+9)+"Thickness",0.2);
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+9)+"VoidMat","Void");
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+9)+"Length",37.15);
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+9)+"Mat","MARS_STST");
    Control.addVariable("LinacVacuumPipe"+std::to_string(i*12+9)+"InnerRadius",2.801);

    index = i*12+9;

  }
  index = index+1;
  PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(index),spokeCavityLength);
  Control.addVariable("LinacSpokeSection"+std::to_string(index)+"VoidMat","Air");
  
  Control.addVariable("LinacSPKCVT"+std::to_string(index)+"Length",spokeCavityLength);
  Control.addVariable("LinacSPKCVT"+std::to_string(index)+"VoidMat","Void");
  Control.addVariable("LinacSPKCVT"+std::to_string(index)+"Thickness",0.4);
  Control.addVariable("LinacSPKCVT"+std::to_string(index)+"Mat","Niobium");
  
    
  PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(index+1) ,14.1);
  Control.addVariable("LinacSpokeSection"+std::to_string(index+1)+"VoidMat","Air");
  
  Control.addVariable("LinacVacuumPipe"+std::to_string(index+1)+"Thickness",0.2);
  Control.addVariable("LinacVacuumPipe"+std::to_string(index+1)+"VoidMat","Void");
  Control.addVariable("LinacVacuumPipe"+std::to_string(index+1)+"Length",14.1);
  Control.addVariable("LinacVacuumPipe"+std::to_string(index+1)+"Mat","MARS_STST");
  Control.addVariable("LinacVacuumPipe"+std::to_string(index+1)+"InnerRadius",2.801);
  
  PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(index+2) ,spokeCavityLength);
  Control.addVariable("LinacSpokeSection"+std::to_string(index+2)+"VoidMat","Air");
  
  Control.addVariable("LinacSPKCVT"+std::to_string(index+2)+"Length",spokeCavityLength);
  Control.addVariable("LinacSPKCVT"+std::to_string(index+2)+"VoidMat","Void");
  Control.addVariable("LinacSPKCVT"+std::to_string(index+2)+"Thickness",0.4);
  Control.addVariable("LinacSPKCVT"+std::to_string(index+2)+"Mat","Niobium");
    
  PipeGen.generatePipe(Control,"LinacSpokeSection"+std::to_string(index+3),37.15);
  Control.addVariable("LinacSpokeSection"+std::to_string(index+3)+"VoidMat","Air");
  
  Control.addVariable("LinacVacuumPipe"+std::to_string(index+3)+"Thickness",0.2);
  Control.addVariable("LinacVacuumPipe"+std::to_string(index+3)+"VoidMat","Void");
  Control.addVariable("LinacVacuumPipe"+std::to_string(index+3)+"Length",37.15);
  Control.addVariable("LinacVacuumPipe"+std::to_string(index+3)+"Mat","MARS_STST");
  Control.addVariable("LinacVacuumPipe"+std::to_string(index+3)+"InnerRadius",2.801);

  

  
  return;
}
}
    
