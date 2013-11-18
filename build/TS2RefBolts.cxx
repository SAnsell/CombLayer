/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   build/TS2RefBolts.cxx
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
#include <boost/array.hpp>
#include <boost/format.hpp>
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
RefBolts(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for TS2
    \param Control :: Function data base to add constants too
  */
{
  using Geometry::Vec3D;
  // Neumonic bolts [EAST]
  Control.addVariable("refBoltNim0Mat",59);  
  Control.addVariable("refBoltNim0Radius0",0.8);  

  Control.addVariable("refBoltNim0Track0",Vec3D(-7.5,78.0,34.8));  
  Control.addVariable("refBoltNim0Track1",Vec3D(-7.5,78.0,-34.8));  

  Control.addVariable("refBoltNim1Track0",Vec3D(-7.5,7.5,34.8));  
  Control.addVariable("refBoltNim1Track1",Vec3D(-7.5,7.5,-34.8));  

  Control.addVariable("refBoltNim2Track0",Vec3D(-35.0,42.0,34.8));  
  Control.addVariable("refBoltNim2Track1",Vec3D(-35.0,42.0,1.85));  

  // Neumonic bolts [WEST]
  Control.addVariable("refBoltNim3Track0",Vec3D(6.0,5.0,34.8));  
  Control.addVariable("refBoltNim3Track1",Vec3D(6.0,5.0,-34.8));  

  Control.addVariable("refBoltNim4Track0",Vec3D(7.5,78.0,34.8));  
  Control.addVariable("refBoltNim4Track1",Vec3D(7.5,78.0,-34.8));  

  Control.addVariable("refBoltNim5Track0",Vec3D(30.0,30.0,34.8));  
  Control.addVariable("refBoltNim5Track1",Vec3D(30.0,30.0,1.85));  

  // PreMod Removable Block [EAST]
  Control.addVariable("refBoltNim6Radius0",0.8);  
  Control.addVariable("refBoltNim6Radius1",1.2);  
  Control.addVariable("refBoltNim6Track0",Vec3D(-15.252,33.846,-17.02));  
  Control.addVariable("refBoltNim6Track1",Vec3D(-16.774,33.846,-17.02));  
  Control.addVariable("refBoltNim6Track2",Vec3D(-24.382,33.846,-17.02));  

  Control.addVariable("refBoltNim7Track0",Vec3D(-7.065,48.161,-17.02));  
  Control.addVariable("refBoltNim7Track1",Vec3D(-12.138,48.161,-17.02));  
  Control.addVariable("refBoltNim7Track2",Vec3D(-51.079,48.161,-17.02));  

  // PreMod Removable Block [WEST]
  Control.addVariable("refBoltNim8Track0",Vec3D(1.267,0,-10.0));  
  Control.addVariable("refBoltNim8Track1",Vec3D(1.267,2.5,-10.0));  
  Control.addVariable("refBoltNim8Track2",Vec3D(1.267,10.5,-10.0));  

  Control.addVariable("refBoltNim9Track0",Vec3D(1.267,0,-25.0));  
  Control.addVariable("refBoltNim9Track1",Vec3D(1.267,2.5,-25.0));  
  Control.addVariable("refBoltNim9Track2",Vec3D(1.267,10.5,-25.0));  
  

  // Coupled Bucket [EAST]
  Control.addVariable("refBoltSteel0Mat",3);  
  Control.addVariable("refBoltSteel0Radius0",0.5);  
  Control.addVariable("refBoltSteel0Radius1",0.8);    

  Control.addVariable("refBoltSteel0Track0",Vec3D(-9.255,22.855,-5.3));  
  Control.addVariable("refBoltSteel0Track1",Vec3D(-9.255,22.855,-6.3));  
  Control.addVariable("refBoltSteel0Track2",Vec3D(-9.255,22.855,-9.3));  

  Control.addVariable("refBoltSteel1Track0",Vec3D(-16.388,25.173,-5.3));  
  Control.addVariable("refBoltSteel1Track1",Vec3D(-16.388,25.173,-6.3));  
  Control.addVariable("refBoltSteel1Track2",Vec3D(-16.388,25.173,-9.3));  

  Control.addVariable("refBoltSteel2Track0",Vec3D(-6.438,55.795,-5.3));  
  Control.addVariable("refBoltSteel2Track1",Vec3D(-6.438,55.795,-6.3));  
  Control.addVariable("refBoltSteel2Track2",Vec3D(-6.438,55.795,-9.3));  

  Control.addVariable("refBoltSteel3Track0",Vec3D(-11.193,57.490,-5.3));
  Control.addVariable("refBoltSteel3Track1",Vec3D(-11.193,57.490,-6.3));
  Control.addVariable("refBoltSteel3Track2",Vec3D(-11.193,57.490,-9.3));

  // Coupled Bucket [WEST]
  Control.addVariable("refBoltSteel4Track0",Vec3D(9.7,6.87,-4.0));  
  Control.addVariable("refBoltSteel4Track1",Vec3D(9.7,6.87,-5.0));  
  Control.addVariable("refBoltSteel4Track2",Vec3D(9.7,6.87,-8.0));  
  
  Control.addVariable("refBoltSteel5Track0",Vec3D(8.924,51.12,-4.0));  
  Control.addVariable("refBoltSteel5Track1",Vec3D(8.924,51.12,-5.0));  
  Control.addVariable("refBoltSteel5Track2",Vec3D(8.924,51.12,-8.0));  

  Control.addVariable("refBoltSteel6Track0",Vec3D(31.742,40.447,-4.0));  
  Control.addVariable("refBoltSteel6Track1",Vec3D(31.742,40.447,-5.0));  
  Control.addVariable("refBoltSteel6Track2",Vec3D(31.742,40.447,-8.0));  

  // Decoupled Bucket [EAST]
  Control.addVariable("refBoltBuck0Mat",3);  
  Control.addVariable("refBoltBuck0Radius0",0.8);  
  Control.addVariable("refBoltBuck0Radius1",1.2);    

  Control.addVariable("refBoltBuck0Track0",Vec3D(-5.0,25.026,34.8));  
  Control.addVariable("refBoltBuck0Track1",Vec3D(-5.0,25.026,33.0));  
  Control.addVariable("refBoltBuck0Track2",Vec3D(-5.0,25.026,23.0));  

  Control.addVariable("refBoltBuck1Track0",Vec3D(-5.0,49.026,34.8));  
  Control.addVariable("refBoltBuck1Track1",Vec3D(-5.0,49.026,33.0));  
  Control.addVariable("refBoltBuck1Track2",Vec3D(-5.0,49.026,23.0));  

  Control.addVariable("refBoltBuck2Track0",Vec3D(-8.9,37.026,31.0));  
  Control.addVariable("refBoltBuck2Track1",Vec3D(-10.5,37.026,31.0));  
  Control.addVariable("refBoltBuck2Track2",Vec3D(-20.3,37.026,31.0));  

  Control.addVariable("refBoltBuck3Track0",Vec3D(-8.9,37.026,24.25));  
  Control.addVariable("refBoltBuck3Track1",Vec3D(-10.5,37.026,24.25));  
  Control.addVariable("refBoltBuck3Track2",Vec3D(-20.3,37.026,24.25));  

  // Decoupled Bucket [WEST]

  Control.addVariable("refBoltBuck4Track0",Vec3D(5.0,25.026,34.8));  
  Control.addVariable("refBoltBuck4Track1",Vec3D(5.0,25.026,33.0));  
  Control.addVariable("refBoltBuck4Track2",Vec3D(5.0,25.026,23.0));  

  Control.addVariable("refBoltBuck5Track0",Vec3D(5.0,49.026,34.8));  
  Control.addVariable("refBoltBuck5Track1",Vec3D(5.0,49.026,33.0));  
  Control.addVariable("refBoltBuck5Track2",Vec3D(5.0,49.026,23.0));  

  Control.addVariable("refBoltBuck6Track0",Vec3D(8.9,37.026,31.0));  
  Control.addVariable("refBoltBuck6Track1",Vec3D(10.5,37.026,31.0));  
  Control.addVariable("refBoltBuck6Track2",Vec3D(17.62,37.026,31.0));  

  Control.addVariable("refBoltBuck7Track0",Vec3D(8.9,37.026,24.25));  
  Control.addVariable("refBoltBuck7Track1",Vec3D(10.5,37.026,24.25));  
  Control.addVariable("refBoltBuck7Track2",Vec3D(17.62,37.026,24.25));  

  
  return;

}

} // NAMESPACE setVariable
