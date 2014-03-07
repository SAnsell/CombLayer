/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delft/ReactorLayout.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include "inputParam.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "variableSetup.h"

namespace setVariable
{

  void DelftStandardModel(FuncDataBase&);
  void DelftXCoreModel(FuncDataBase&);
  void DelftYCoreModel(FuncDataBase&);
  void DelftTestModel(FuncDataBase&);

void
DelftCoreType(const mainSystem::inputParam& IParam,
	      FuncDataBase& Control)
  /*!
   */
{
  ELog::RegMethod RegA("delftReactorLayout[f]","DelftCoreType");
 
  const std::string CoreName=
      IParam.dataCnt("coreType") ? IParam.getValue<std::string>("coreType") :
        "Standard";
  if (CoreName=="Standard")
    {
      DelftStandardModel(Control);
      return;
    }
  if (CoreName=="XCore")
    {
      DelftXCoreModel(Control);
      return;
    }
  if (CoreName=="YCore")
    {
      DelftYCoreModel(Control);
      return;
    }
  if (CoreName=="Test")
    {
      DelftTestModel(Control);
      return;
    }
  throw ColErr::InContainerError<std::string>(CoreName,"Core name unknown");
  return;
}

void
DelftStandardModel(FuncDataBase& Control)
  /*!
    Function to set system for a given form of reactor.
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod Rega("delftReactorLayout[f]","DelftStandardModel");

  Control.addVariable("delftGridType",0);        // All void
  Control.addVariable("delftGridTypeA0",5);      
  Control.addVariable("delftGridTypeA1",5);      
  Control.addVariable("delftGridTypeA2",5);      
  Control.addVariable("delftGridTypeA3",5);      
  Control.addVariable("delftGridTypeA4",5);      
  Control.addVariable("delftGridTypeA5",5);      
  Control.addVariable("delftGridTypeA6",5);      
  // Second stack
  Control.addVariable("delftGridTypeB0",5);      
  Control.addVariable("delftGridTypeB1",5);      
  Control.addVariable("delftGridTypeB2",1);      
  Control.addVariable("delftGridTypeB3",1);      
  Control.addVariable("delftGridTypeB4",1);      
  Control.addVariable("delftGridTypeB5",5);      
  Control.addVariable("delftGridTypeB6",5);      
  // Third stack
  Control.addVariable("delftGridTypeC0",5);      
  Control.addVariable("delftGridTypeC1",1);      
  Control.addVariable("delftGridTypeC2",2);      
  Control.addVariable("delftGridTypeC3",1);      
  Control.addVariable("delftGridTypeC4",2);      
  Control.addVariable("delftGridTypeC5",1);      
  Control.addVariable("delftGridTypeC6",5);      
  // Fourth stack
  Control.addVariable("delftGridTypeD0",5);      
  Control.addVariable("delftGridTypeD1",1);      
  Control.addVariable("delftGridTypeD2",1);      
  Control.addVariable("delftGridTypeD3",3);      
  Control.addVariable("delftGridTypeD4",1);      
  Control.addVariable("delftGridTypeD5",1);      
  Control.addVariable("delftGridTypeD6",5);      

  // Fifth stack
  Control.addVariable("delftGridTypeE0",5);      
  Control.addVariable("delftGridTypeE1",1);      
  Control.addVariable("delftGridTypeE2",2);      
  Control.addVariable("delftGridTypeE3",1);      
  Control.addVariable("delftGridTypeE4",2);      
  Control.addVariable("delftGridTypeE5",1);      
  Control.addVariable("delftGridTypeE6",5);      

  // Sixth stack
  Control.addVariable("delftGridTypeF0",5);      
  Control.addVariable("delftGridTypeF1",5);      
  Control.addVariable("delftGridTypeF2",1);      
  Control.addVariable("delftGridTypeF3",1);      
  Control.addVariable("delftGridTypeF4",1);      
  Control.addVariable("delftGridTypeF5",5);      
  Control.addVariable("delftGridTypeF6",5);      
  return;
}

void
DelftXCoreModel(FuncDataBase& Control)
  /*!
    Function to set system for a given form of reactor.
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod Rega("delftReactorLayout[f]","DelftXCoreModel");

  Control.addVariable("delftGridType",0);        // All void
  Control.addVariable("delftGridTypeA0",5);      
  Control.addVariable("delftGridTypeA1",5);      
  Control.addVariable("delftGridTypeA2",5);      
  Control.addVariable("delftGridTypeA3",5);      
  Control.addVariable("delftGridTypeA4",5);      
  Control.addVariable("delftGridTypeA5",5);      
  Control.addVariable("delftGridTypeA6",5);      
  // Second stack
  Control.addVariable("delftGridTypeB0",5);      
  Control.addVariable("delftGridTypeB1",2);      
  Control.addVariable("delftGridTypeB2",1);      
  Control.addVariable("delftGridTypeB3",1);      
  Control.addVariable("delftGridTypeB4",2);      
  Control.addVariable("delftGridTypeB5",5);      
  Control.addVariable("delftGridTypeB6",5);      
  // Third stack
  Control.addVariable("delftGridTypeC0",5);      
  Control.addVariable("delftGridTypeC1",1);      
  Control.addVariable("delftGridTypeC2",1);      
  Control.addVariable("delftGridTypeC3",1);      
  Control.addVariable("delftGridTypeC4",1);      
  Control.addVariable("delftGridTypeC5",5);      
  Control.addVariable("delftGridTypeC6",5);      
  // Fourth stack
  Control.addVariable("delftGridTypeD0",5);      
  Control.addVariable("delftGridTypeD1",1);      
  Control.addVariable("delftGridTypeD2",1);      
  Control.addVariable("delftGridTypeD3",1);      
  Control.addVariable("delftGridTypeD4",1);      
  Control.addVariable("delftGridTypeD5",5);      
  Control.addVariable("delftGridTypeD6",5);      

  // Fifth stack
  Control.addVariable("delftGridTypeE0",5);      
  Control.addVariable("delftGridTypeE1",2);      
  Control.addVariable("delftGridTypeE2",1);      
  Control.addVariable("delftGridTypeE3",1);      
  Control.addVariable("delftGridTypeE4",2);      
  Control.addVariable("delftGridTypeE5",5);      
  Control.addVariable("delftGridTypeE6",5);      

  // Sixth stack
  Control.addVariable("delftGridTypeF0",5);      
  Control.addVariable("delftGridTypeF1",5);      
  Control.addVariable("delftGridTypeF2",5);      
  Control.addVariable("delftGridTypeF3",0);      
  Control.addVariable("delftGridTypeF4",5);      
  Control.addVariable("delftGridTypeF5",5);      
  Control.addVariable("delftGridTypeF6",5);      
  return;
}

void
DelftYCoreModel(FuncDataBase& Control)
  /*!
    Function to set system for a given form of reactor.
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod Rega("delftReactorLayout[f]","DelftYCoreModel");

  Control.addVariable("delftGridXSize",5);        // All void
  Control.addVariable("delftGridYSize",5);        // All void

  Control.addVariable("delftGridType",0);        // All void
  Control.addVariable("delftGridTypeA0",5);      
  Control.addVariable("delftGridTypeA1",5);      
  Control.addVariable("delftGridTypeA2",5);      
  Control.addVariable("delftGridTypeA3",5);      
  Control.addVariable("delftGridTypeA4",5);      
  Control.addVariable("delftGridTypeA5",5);      
  Control.addVariable("delftGridTypeA6",5);      
  // Second stack
  Control.addVariable("delftGridTypeB0",5);      
  Control.addVariable("delftGridTypeB1",1);      
  Control.addVariable("delftGridTypeB2",3);      
  Control.addVariable("delftGridTypeB3",1);      
  Control.addVariable("delftGridTypeB4",5);      
  Control.addVariable("delftGridTypeB5",5);      
  Control.addVariable("delftGridTypeB6",5);      
  // Third stack
  Control.addVariable("delftGridTypeC0",1);      
  Control.addVariable("delftGridTypeC1",4);      
  Control.addVariable("delftGridTypeC2",1);      
  Control.addVariable("delftGridTypeC3",4);      
  Control.addVariable("delftGridTypeC4",1);      
  Control.addVariable("delftGridTypeC5",5);      
  Control.addVariable("delftGridTypeC6",5);      
  // Fourth stack
  Control.addVariable("delftGridTypeD0",1);      
  Control.addVariable("delftGridTypeD1",1);      
  Control.addVariable("delftGridTypeD2",3);      
  Control.addVariable("delftGridTypeD3",1);      
  Control.addVariable("delftGridTypeD4",1);      
  Control.addVariable("delftGridTypeD5",5);      
  Control.addVariable("delftGridTypeD6",5);      

  // Fifth stack
  Control.addVariable("delftGridTypeE0",1);      
  Control.addVariable("delftGridTypeE1",4);      
  Control.addVariable("delftGridTypeE2",1);      
  Control.addVariable("delftGridTypeE3",4);      
  Control.addVariable("delftGridTypeE4",1);      
  Control.addVariable("delftGridTypeE5",5);      
  Control.addVariable("delftGridTypeE6",5);      

  // Sixth stack
  Control.addVariable("delftGridTypeF0",5);      
  Control.addVariable("delftGridTypeF1",1);      
  Control.addVariable("delftGridTypeF2",3);      
  Control.addVariable("delftGridTypeF3",1);      
  Control.addVariable("delftGridTypeF4",5);      
  Control.addVariable("delftGridTypeF5",5);      
  Control.addVariable("delftGridTypeF6",5);      
  return;
}

void
DelftTestModel(FuncDataBase& Control)
  /*!
    Function to set system for a given form of reactor.
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod Rega("delftReactorLayout[f]","DelftTestModel");

  Control.addVariable("delftGridXSize",5);        
  Control.addVariable("delftGridYSize",7);
  Control.addVariable("delftGridWidth",5*46.254/6);  
  Control.addVariable("delftGridDepth",56.70); 
  Control.addVariable("delftGridXStep",0); 


  // Second stack
  Control.addVariable("delftGridType",0);      
  Control.addVariable("delftGridTypeC3",1);      
  Control.addVariable("delftGridNFuelDivideC3",10);      

  // blocks filling space
  Control.addVariable("Box1Flag",1);      
  Control.addVariable("Box1XStep",0.0);      
  Control.addVariable("Box1YStep",0.0);      
  Control.addVariable("Box1ZStep",0.0);      
  Control.addVariable("Box1XYAngle",0.0);      
  Control.addVariable("Box1ZAngle",0.0);   

  Control.addVariable("Box1Length",5);   
  Control.addVariable("Box1Width",60.0);   
  Control.addVariable("Box1Height",60.0);   
  Control.addVariable("Box1Mat",3);   
  
  Control.addVariable("Box2Flag",1);      
  Control.addVariable("Box2XStep",0.0);      
  Control.addVariable("Box2YStep",0.0);      
  Control.addVariable("Box2ZStep",0.0);      
  Control.addVariable("Box2XYAngle",0.0);      
  Control.addVariable("Box2ZAngle",0.0);   

  Control.addVariable("Box2Length",5);   
  Control.addVariable("Box2Width",60.0);   
  Control.addVariable("Box2Height",60.0);   
  Control.addVariable("Box2Mat",3);   
  
  
  

  return;
}

} // NAMESPACE setVariable

