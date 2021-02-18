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
EssLinacMEBTVariables(FuncDataBase &Control)
  /*!
    Create all the PMQ variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("essVariables[F]","MEBTVariables");

  // Beam Pipe

  setVariable::PipeGenerator PipeGen;
  PipeGen.setPipe(50.0,0.0);
  PipeGen.setWindow(50.0,0.0);
  PipeGen.setFlange(50.0,0.0);

  double MEBTCavityLength = 98.8;
  double quadLength = 25.0;
  
  PipeGen.generatePipe(Control,"MEBTSection0",76.15);
  Control.addVariable("MEBTSection0VoidMat","Air");

  Control.addVariable("MEBTVacuumPipe0Thickness",0.2);
  Control.addVariable("MEBTVacuumPipe0VoidMat","Void");
  Control.addVariable("MEBTVacuumPipe0Length",76.15);
  Control.addVariable("MEBTVacuumPipe0Mat","MARS_STST");
  Control.addVariable("MEBTVacuumPipe0InnerRadius",2.801);

  for(int i=1;i<11;i++){

    PipeGen.generatePipe(Control,"MEBTSection"+std::to_string(i),quadLength);
    Control.addVariable("MEBTSection"+std::to_string(i)+"VoidMat","Air");
    
    Control.addVariable("MEBTQuad"+std::to_string(i)+"Length",quadLength);
    Control.addVariable("MEBTQuad"+std::to_string(i)+"VoidMat","Void");
    Control.addVariable("MEBTQuad"+std::to_string(i)+"Thickness",0.2);
    Control.addVariable("MEBTQuad"+std::to_string(i)+"PipeMat","MARS_STST");
    Control.addVariable("MEBTQuad"+std::to_string(i)+"YokeMat","MARS_QXYOKE");
    Control.addVariable("MEBTQuad"+std::to_string(i)+"CoilMat","MARS_QXCOIL");
    Control.addVariable("MEBTQuad"+std::to_string(i)+"PipeRad",2.801);
  }


  
  return;
}
}
    
