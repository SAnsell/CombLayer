/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/ReactorLayout.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "inputParam.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "variableSetup.h"

namespace setVariable
{
  void DelftStandardModel(FuncDataBase&);
  void DelftCompactModel(mainSystem::inputParam&,FuncDataBase&);
  void DelftKaeriModel(mainSystem::inputParam&,FuncDataBase&);
  void DelftXCoreModel(FuncDataBase&);
  void DelftYCoreModel(FuncDataBase&);
  void DelftTestModel(FuncDataBase&);
  void DelftNukemModel(mainSystem::inputParam&,FuncDataBase&);

void
DelftCoreType(mainSystem::inputParam& IParam,
	      FuncDataBase& Control)
  /*!
    Controls the type of core needed 
    \param IParam :: Core type
   */
{
  ELog::RegMethod RegA("ReactorLayout[f]","DelftCoreType");
 
  const std::string CoreName=
      IParam.dataCnt("coreType") ? IParam.getValue<std::string>("coreType") :
        "Standard";
  if (CoreName=="Help" || CoreName=="help")
    {
      ELog::EM<<"Core types"<<ELog::endDiag;
      ELog::EM<<"-- Standard :: Existing Delft core" <<ELog::endDiag;
      ELog::EM<<"-- Compact :: Rectangular compact" <<ELog::endDiag;
      ELog::EM<<"-- Kaeri :: Kaeri X Type" <<ELog::endDiag;
      ELog::EM<<"-- Nukem :: Nukem type" <<ELog::endDiag;
      ELog::EM<<"-- XCore :: Center active core layout" <<ELog::endDiag;
      ELog::EM<<"-- YCore :: Center active core layout" <<ELog::endDiag;
      ELog::EM<<"-- Test :: Single component core" <<ELog::endDiag;
      return;
    }
  if (CoreName=="Standard")
    {
      DelftStandardModel(Control);
      return;
    }
  if (CoreName=="Compact")
    {
      DelftCompactModel(IParam,Control);
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
  if (CoreName=="Kaeri")
    {
      DelftKaeriModel(IParam,Control);
      return;
    }
  if (CoreName=="Nukem")
    {
      DelftNukemModel(IParam,Control);
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
DelftCompactModel(mainSystem::inputParam& IParam,
		  FuncDataBase& Control)
  /*!
    Function to set system for a given form of reactor.
    This is the compact model from 
    \param IParam :: Main input parameter system
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("ReactorLayout[f]","DelftCompactModel");

  
  IParam.setValue("refExtra",std::string("R2Surround"));


  return;  
}

void
DelftKaeriModel(mainSystem::inputParam& IParam,
		FuncDataBase& Control)
  /*!
    Function to set system for a given form of reactor.
    This is the compact model from 
    \param IParam :: Input stream
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("ReactorLayout[f]","DelftCompactModel");

  IParam.setValue("refExtra",std::string("R2Cube"));

  Control.addVariable("delftGridXSize",6);        // R1 --> R2
  Control.addVariable("delftGridYSize",7);        // NonViewed direction

  Control.addVariable("delftGridType","Null");        // All void
  Control.addVariable("delftGridTypeA0","Be");      
  Control.addVariable("delftGridTypeA1","Be");      
  Control.addVariable("delftGridTypeA2","Be");      
  Control.addVariable("delftGridTypeA3","Be");      
  Control.addVariable("delftGridTypeA4","Be");      
  Control.addVariable("delftGridTypeA5","Be");      
  Control.addVariable("delftGridTypeA6","Be");      
  // Second stack
  Control.addVariable("delftGridTypeB0","Be");      
  Control.addVariable("delftGridTypeB1","Be");      
  Control.addVariable("delftGridTypeB2","Fuel");      
  Control.addVariable("delftGridTypeB3","Fuel");      
  Control.addVariable("delftGridTypeB4","Fuel");      
  Control.addVariable("delftGridTypeB5","Be");      
  Control.addVariable("delftGridTypeB6","Be");      
  // Third stack
  Control.addVariable("delftGridTypeC0","Be");      
  Control.addVariable("delftGridTypeC1","Fuel");      
  Control.addVariable("delftGridTypeC2","Control");      
  Control.addVariable("delftGridTypeC3","Fuel");      
  Control.addVariable("delftGridTypeC4","Control");      
  Control.addVariable("delftGridTypeC5","Fuel");      
  Control.addVariable("delftGridTypeC6","Be");      
  // Fourth stack
  Control.addVariable("delftGridTypeD0","Be");      
  Control.addVariable("delftGridTypeD1","Be");      
  Control.addVariable("delftGridTypeD2","Fuel");      
  Control.addVariable("delftGridTypeD3","IRad");      
  Control.addVariable("delftGridTypeD4","Fuel");      
  Control.addVariable("delftGridTypeD5","IRad");      
  Control.addVariable("delftGridTypeD6","Be");      

  // Fifth stack
  Control.addVariable("delftGridTypeE0","Be");      
  Control.addVariable("delftGridTypeE1","Fuel");      
  Control.addVariable("delftGridTypeE2","Control");      
  Control.addVariable("delftGridTypeE3","Fuel");      
  Control.addVariable("delftGridTypeE4","Control");      
  Control.addVariable("delftGridTypeE5","Fuel");      
  Control.addVariable("delftGridTypeE6","Be");      

  // Sixth stack
  Control.addVariable("delftGridTypeF0","Be");      
  Control.addVariable("delftGridTypeF1","Be");      
  Control.addVariable("delftGridTypeF2","Fuel");      
  Control.addVariable("delftGridTypeF3","Fuel");      
  Control.addVariable("delftGridTypeF4","Fuel");      
  Control.addVariable("delftGridTypeF5","Be");      
  Control.addVariable("delftGridTypeF6","Be");      

  Control.addVariable("delftElementCladDepth",0.038);   
  Control.addVariable("delftElementNFuel",20);
  Control.addVariable("delftElementWaterDepth",0.278);
  Control.addVariable("delftElementFuelWidth",6.32);  
  Control.addVariable("delftElementFuelDepth",0.051);   

  Control.addVariable("delftControlAbsMat","10B4C");    


  Control.addVariable("delftFlightR2CapRadius",177.0);

  Control.addVariable("R2CubeLength",30.0);  
  Control.addVariable("R2CubeYStep",-1.3);  
  Control.addVariable("R2CubeActive",1);  
  Control.addVariable("R2CubeInnerRadius",-12.26);  
  Control.addVariable("R2CubeOuterWidth",34.0);  
  Control.addVariable("R2CubeOuterHeight",28.0);  


  Control.addVariable("Rabbit1GridKey","A5");      
  Control.addVariable("Rabbit1XStep",-7.8);      
  Control.addVariable("Rabbit1YStep",0.0);      
  Control.addVariable("Rabbit1ZStep",-10.0);      
  Control.addVariable("Rabbit1XYAngle",0.0);      
  Control.addVariable("Rabbit1ZAngle",0.0);      


  return;  
}

void
DelftNukemModel(mainSystem::inputParam& IParam,
		FuncDataBase& Control)
 /*!
    Function to set system for a given form of reactor.
    This is the compact model from 
    \param IParam :: Input paramter
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("ReactorLayout[f]","DelftNukemModel");


  return;  
}

void
DelftStandardModel(FuncDataBase& Control)
  /*!
    Function to set system for a given form of reactor.
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("ReactorLayout[f]","DelftStandardModel");

  Control.addVariable("delftGridType","Null");        // All void
  Control.addVariable("delftGridTypeA0","BeO");      
  Control.addVariable("delftGridTypeA1","Be");      
  Control.addVariable("delftGridTypeA2","Be");      
  Control.addVariable("delftGridTypeA3","Be");      
  Control.addVariable("delftGridTypeA4","Be");      
  Control.addVariable("delftGridTypeA5","Be");      
  Control.addVariable("delftGridTypeA6","Be");      
  // Second stack
  Control.addVariable("delftGridTypeB0","Be");      
  Control.addVariable("delftGridTypeB1","Be");      
  Control.addVariable("delftGridTypeB2","Fuel");      
  Control.addVariable("delftGridTypeB3","Fuel");      
  Control.addVariable("delftGridTypeB4","Fuel");      
  Control.addVariable("delftGridTypeB5","IRad");      
  Control.addVariable("delftGridTypeB6","Be");      
  // Third stack
  Control.addVariable("delftGridTypeC0","Be");      
  Control.addVariable("delftGridTypeC1","Fuel");      
  Control.addVariable("delftGridTypeC2","Control");      
  Control.addVariable("delftGridTypeC3","Fuel");      
  Control.addVariable("delftGridTypeC4","Control");      
  Control.addVariable("delftGridTypeC5","Fuel");      
  Control.addVariable("delftGridTypeC6","Be");      
  // Fourth stack
  Control.addVariable("delftGridTypeD0","Be");      
  Control.addVariable("delftGridTypeD1","Fuel");      
  Control.addVariable("delftGridTypeD2","Fuel");      
  Control.addVariable("delftGridTypeD3","IRad");      
  Control.addVariable("delftGridTypeD4","Fuel");      
  Control.addVariable("delftGridTypeD5","Fuel");      
  Control.addVariable("delftGridTypeD6","Be");      

  // Fifth stack
  Control.addVariable("delftGridTypeE0","Be");      
  Control.addVariable("delftGridTypeE1","Fuel");      
  Control.addVariable("delftGridTypeE2","Control");      
  Control.addVariable("delftGridTypeE3","Fuel");      
  Control.addVariable("delftGridTypeE4","Control");      
  Control.addVariable("delftGridTypeE5","Fuel");      
  Control.addVariable("delftGridTypeE6","Be");      

  // Sixth stack
  Control.addVariable("delftGridTypeF0","Be");      
  Control.addVariable("delftGridTypeF1","Be");      
  Control.addVariable("delftGridTypeF2","Fuel");      
  Control.addVariable("delftGridTypeF3","Fuel");      
  Control.addVariable("delftGridTypeF4","Fuel");      
  Control.addVariable("delftGridTypeF5","Be");      
  Control.addVariable("delftGridTypeF6","Be");      
  return;
}

void
DelftXCoreModel(FuncDataBase& Control)
  /*!
    Function to set system for a given form of reactor.
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("ReactorLayout[f]","DelftXCoreModel");

  Control.addVariable("delftGridType","Null");        // All void
  Control.addVariable("delftGridTypeA0","Be");      
  Control.addVariable("delftGridTypeA1","Be");      
  Control.addVariable("delftGridTypeA2","Be");      
  Control.addVariable("delftGridTypeA3","Be");      
  Control.addVariable("delftGridTypeA4","Be");      
  Control.addVariable("delftGridTypeA5","Be");      
  Control.addVariable("delftGridTypeA6","Be");      
  // Second stack
  Control.addVariable("delftGridTypeB0","Be");      
  Control.addVariable("delftGridTypeB1","Control");      
  Control.addVariable("delftGridTypeB2","Fuel");      
  Control.addVariable("delftGridTypeB3","Fuel");      
  Control.addVariable("delftGridTypeB4","Control");      
  Control.addVariable("delftGridTypeB5","Be");      
  Control.addVariable("delftGridTypeB6","Be");      
  // Third stack
  Control.addVariable("delftGridTypeC0","Be");      
  Control.addVariable("delftGridTypeC1","Fuel");      
  Control.addVariable("delftGridTypeC2","Fuel");      
  Control.addVariable("delftGridTypeC3","Fuel");      
  Control.addVariable("delftGridTypeC4","Fuel");      
  Control.addVariable("delftGridTypeC5","Be");      
  Control.addVariable("delftGridTypeC6","Be");      
  // Fourth stack
  Control.addVariable("delftGridTypeD0","Be");      
  Control.addVariable("delftGridTypeD1","Fuel");      
  Control.addVariable("delftGridTypeD2","Fuel");      
  Control.addVariable("delftGridTypeD3","Fuel");      
  Control.addVariable("delftGridTypeD4","Fuel");      
  Control.addVariable("delftGridTypeD5","Be");      
  Control.addVariable("delftGridTypeD6","Be");      

  // Fifth stack
  Control.addVariable("delftGridTypeE0","Be");      
  Control.addVariable("delftGridTypeE1","Control");      
  Control.addVariable("delftGridTypeE2","Fuel");      
  Control.addVariable("delftGridTypeE3","Fuel");      
  Control.addVariable("delftGridTypeE4","Control");      
  Control.addVariable("delftGridTypeE5","Be");      
  Control.addVariable("delftGridTypeE6","Be");      

  // Sixth stack
  Control.addVariable("delftGridTypeF0","Be");      
  Control.addVariable("delftGridTypeF1","Be");      
  Control.addVariable("delftGridTypeF2","Be");      
  Control.addVariable("delftGridTypeF3","Null");      
  Control.addVariable("delftGridTypeF4","Be");      
  Control.addVariable("delftGridTypeF5","Be");      
  Control.addVariable("delftGridTypeF6","Be");      
  return;
}

void
DelftYCoreModel(FuncDataBase& Control)
  /*!
    Function to set system for a given form of reactor.
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("ReactorLayout[f]","DelftYCoreModel");

  Control.addVariable("delftGridXSize",5);        // All void
  Control.addVariable("delftGridYSize",5);        // All void

  Control.addVariable("delftGridType","Null");        // All void
  Control.addVariable("delftGridTypeA0","Be");      
  Control.addVariable("delftGridTypeA1","Be");      
  Control.addVariable("delftGridTypeA2","Be");      
  Control.addVariable("delftGridTypeA3","Be");      
  Control.addVariable("delftGridTypeA4","Be");      
  Control.addVariable("delftGridTypeA5","Be");      
  Control.addVariable("delftGridTypeA6","Be");      
  // Second stack
  Control.addVariable("delftGridTypeB0","Be");      
  Control.addVariable("delftGridTypeB1","Fuel");      
  Control.addVariable("delftGridTypeB2","IRad");      
  Control.addVariable("delftGridTypeB3","Fuel");      
  Control.addVariable("delftGridTypeB4","Be");      
  Control.addVariable("delftGridTypeB5","Be");      
  Control.addVariable("delftGridTypeB6","Be");      
  // Third stack
  Control.addVariable("delftGridTypeC0","Fuel");      
  Control.addVariable("delftGridTypeC1",4);      
  Control.addVariable("delftGridTypeC2","Fuel");      
  Control.addVariable("delftGridTypeC3",4);      
  Control.addVariable("delftGridTypeC4","Fuel");      
  Control.addVariable("delftGridTypeC5","Be");      
  Control.addVariable("delftGridTypeC6","Be");      
  // Fourth stack
  Control.addVariable("delftGridTypeD0","Fuel");      
  Control.addVariable("delftGridTypeD1","Fuel");      
  Control.addVariable("delftGridTypeD2","IRad");      
  Control.addVariable("delftGridTypeD3","Fuel");      
  Control.addVariable("delftGridTypeD4","Fuel");      
  Control.addVariable("delftGridTypeD5","Be");      
  Control.addVariable("delftGridTypeD6","Be");      

  // Fifth stack
  Control.addVariable("delftGridTypeE0","Fuel");      
  Control.addVariable("delftGridTypeE1",4);      
  Control.addVariable("delftGridTypeE2","Fuel");      
  Control.addVariable("delftGridTypeE3",4);      
  Control.addVariable("delftGridTypeE4","Fuel");      
  Control.addVariable("delftGridTypeE5","Be");      
  Control.addVariable("delftGridTypeE6","Be");      

  // Sixth stack
  Control.addVariable("delftGridTypeF0","Be");      
  Control.addVariable("delftGridTypeF1","Fuel");      
  Control.addVariable("delftGridTypeF2","IRad");      
  Control.addVariable("delftGridTypeF3","Fuel");      
  Control.addVariable("delftGridTypeF4","Be");      
  Control.addVariable("delftGridTypeF5","Be");      
  Control.addVariable("delftGridTypeF6","Be");      
  return;
}

void
DelftTestModel(FuncDataBase& Control)
  /*!
    Function to set system for a given form of reactor.
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("ReactorLayout[f]","DelftTestModel");

  Control.addVariable("delftGridXSize",5);        
  Control.addVariable("delftGridYSize",7);
  Control.addVariable("delftGridWidth",5*46.254/6);  
  Control.addVariable("delftGridDepth",56.70); 
  Control.addVariable("delftGridXStep",0.0); 

  // Second stack
  Control.addVariable("delftGridType","Null");      
  Control.addVariable("delftGridTypeC3","Fuel");      
  Control.addVariable("delftGridNFuelDivideC3",11);      

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
  Control.addVariable("Box1Mat","Stainless304");   
  
  Control.addVariable("Box2Flag",1);      
  Control.addVariable("Box2XStep",0.0);      
  Control.addVariable("Box2YStep",0.0);      
  Control.addVariable("Box2ZStep",0.0);      
  Control.addVariable("Box2XYAngle",0.0);      
  Control.addVariable("Box2ZAngle",0.0);   

  Control.addVariable("Box2Length",5);   
  Control.addVariable("Box2Width",60.0);   
  Control.addVariable("Box2Height",60.0);   
  Control.addVariable("Box2Mat","Stainless304");   
  
  return;
}

} // NAMESPACE setVariable

