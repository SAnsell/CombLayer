/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/TS1layer.cxx
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
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <boost/regex.hpp>

#include "support.h"
#include "Exception.h"
#include "InputControl.h"
#include "OutputLog.h"
#include "Element.h"
#include "Material.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Tensor.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "cellFluxTally.h"
#include "tallyFactory.h"
#include "TallyCreate.h"
#include "Transform.h"
#include "Surface.h"
#include "Rules.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Hull.h"
#include "Weights.h"
#include "PhysImp.h"
#include "PhysicsCards.h"

#include "Simulation.h"

Simulation Sim;

void setVariable(FuncDataBase&); ///< Set all the variables
void setWeights();               ///< Set the weight system

void
setWeights()
  /*!
    Function to set up the weights system.
    It replaces the old file read system.
  */
{
  Weights& WR(Sim.getWeight());
  WR.addParticle('n');
  std::vector<double> Eval(6);
  Eval[0]=0.001;
  Eval[1]=0.01;
  Eval[2]=0.1;
  Eval[3]=2.0;
  Eval[4]=100.0;
  Eval[5]=800.0;
  WR.setEnergy('n',Eval);
  Sim.populateWCells();

  std::vector<double> WT(6);
  WT[0]=0.002;WT[1]=0.007;WT[2]=0.02;
  WT[3]=0.07;WT[4]= 0.2;WT[5]=0.7;
  WR.balanceScale('n',WT);               // set weights
  WR.maskCell('n',1);
  return;
}


void
setVariable(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    \param Control :: Function data base to add constants too
  */
{
// -----------
// GLOBAL stuff
// -----------

  Control.addVariable("zero",0.0);      // Zero

// ------------
// Target stuff
// ------------
  Control.addVariable("beamsize",1.7);      // Radius the beam

// -----------
// Moderators
// -----------

  Control.addVariable("hymat",11);          // Hydrogen para 
  

  return;
}

int 
main(int argc,char* argv[])
{
  std::vector<std::string> Names;  
  InputControl::mainVector(argc,argv,Names);
  // We don't currently use any option flags
  if (Names.size()<2)
    {
      std::cerr<<"Insufficient files ::"
	       <<" Eng.i out.x"
	       <<std::endl;
      exit(1);
    }    
  std::string Fname=Names[0];         // Master file name
  std::string Oname=Names[1];         // Output file name
  /*!
    The big variable setting
  */
  
  std::istringstream cx;
  setVariable(Sim.getDataBase());

  // Definitions section 
  try
    {
      Sim.readMaster(Fname);
      Sim.setCutter(99999);
      setWeights();
      Sim.refineSim();
      Sim.getPC().setNPS(100000);
      Sim.getPC().setCells("imp",1,0);	
      // Add Tallies for tracks
      
      Sim.write(Oname);
      Sim.writeCinder();
    }
  catch (ColErr::ExBase& A)
    {
      std::cerr<<A.what()<<std::endl;
      exit(1);
    }
}
