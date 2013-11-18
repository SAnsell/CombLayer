/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/SLayer.cxx
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
#include "pointTally.h"
#include "cellFluxTally.h"
#include "tallyFactory.h"
#include "Transform.h"
#include "Surface.h"
#include "Plane.h"
#include "Rules.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Intersect.h"
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
void addF4Tally(Simulation&,const int,
		const std::vector<int>&);    ///< Add tallies to the system

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


// ------------
// Collimator stuff
// ------------
  // CollA 
  Control.addVariable("collAStart",-159.5);      // Start of collimator A
  Control.addVariable("collALength",65.0);       // Length of collimator A
  Control.addVariable("collASRad",7.5/2.0);      // Start radius
  Control.addVariable("collAERad",13.7/2.0);     // End radius
  Control.Parse("collAStart+collALength");       // End of collimator A
  Control.addVariable("collAEnd");           
  Control.Parse("(collAERad-collASRad)/collALength");
  Control.addVariable("collATan");
  Control.Parse("collAEnd-collAERad/collATan");
  Control.addVariable("collAZero");

  // Coll B 
  Control.addVariable("collBStart",-40.0);         // Start of collimator B
  Control.addVariable("collBLength",40.0);       // Length of collimator B
  Control.addVariable("collBSRad",11.5/2.0);      // Start radius
  Control.addVariable("collBERad",13.8/2.0);     // End radius
  Control.Parse("collBStart+collBLength");       // End of collimator B
  Control.addVariable("collBEnd");           

  Control.Parse("(collBERad-collBSRad)/collBLength");
  Control.addVariable("collBTan");
  Control.Parse("collBEnd-collAERad/collBTan");
  Control.addVariable("collBZero");
  
  // Tube
  Control.addVariable("tubeInnerRad",10.0);
  Control.addVariable("tubeOuterRad",10.3);
  Control.addVariable("tubeVacRad",11.0);
  // Tube After collimator
  Control.addVariable("guideInnerRad",14.1);
  Control.addVariable("guideOuterRad",14.4);
  Control.addVariable("guideVacRad",14.8);

  // Box
  
  Control.addVariable("steelEnd",-100.0);          // Shielding going back around collimator
  Control.addVariable("boxInnerX",22.5);           // Vac Box on collimators 
  Control.addVariable("boxInnerZ",22.5);           // Vac Box on collimators 
  Control.addVariable("boxOuterX",80.25);          // Vac Box on collimators 
  Control.addVariable("boxOuterZ",80.25);          // Vac Box on collimators 
  Control.addVariable("boxVertA",49.70);           // Steel sheet dividers
  Control.addVariable("boxVertB",19.2);            // Steel sheet dividers
  Control.addVariable("boxVertC",-19.2);           // Steel sheet dividers
  Control.addVariable("boxVertD",-49.70);          // Steel sheet dividers
  Control.addVariable("boxVertE",-64.25);          // Steel sheet dividers

  // Q44 Diagonal magnet
  Control.addVariable("Q44CentreY",88.0);          // Middle of the magnet
  Control.addVariable("Q44MidLeng",35.40);          // Middle of the magnet
  Control.addVariable("Q44Leng",64.5);          // Middle of the magnet
  Control.addVariable("Q44MidHeight",121.8);        // Full Height  
  Control.addVariable("Q44Height",71.4);        // Full Height  

  // Q44 Magnet
  
  Control.addVariable("Q44SupCentreZ",-71.0);
  Control.addVariable("Q44SupHeight",-57.0);
  Control.addVariable("Q44SupWidth",128.0);


  // Next Tube after Magnets
  Control.addVariable("magTInnerRad",14.1);
  Control.addVariable("magTOuterRad",14.4);

  // Trolly
  Control.addVariable("trollyCentreY",250.0);          // Middle of the Trolly
  Control.addVariable("trollyCentreZ",-135.7);         // Middle of the Trolly
  Control.addVariable("trollyLeng",449.0);             // Full length
  Control.addVariable("trollyWidth",136.0);           // Full width
  Control.addVariable("trollyHeight",60.0);           // Full Height

  // Q45 Magnet
  Control.addVariable("Q45CentreY",210.7);          // Middle of the Trolly
  Control.addVariable("Q45Leng",150.0);             // Full length
  Control.addVariable("Q45Width",128.5);            // Full width
  Control.addVariable("Q45Height",135.7);            // Full Height

  Control.addVariable("Q45SupZ",-57.14);          // Middle of the Trolly
  Control.addVariable("Q45SupLeng",100.0);             // Full length
  Control.addVariable("Q45SupWidth",100.5);            // Full width
  Control.addVariable("Q45SupHeight",42.8);            // Full Height

  // Q46 Magnet
  Control.addVariable("Q46CentreY",364.2);          // Middle of the Trolly
  Control.addVariable("Q46Leng",150.0);             // Full length
  Control.addVariable("Q46Width",128.5);            // Full width
  Control.addVariable("Q46Height",135.7);            // Full Height

  Control.addVariable("Q46SupZ",-57.14);          // Middle of the Trolly
  Control.addVariable("Q46SupLeng",100.0);             // Full length
  Control.addVariable("Q46SupWidth",100.0);            // Full width
  Control.addVariable("Q46SupHeight",42.8);            // Full Height


  // ---------------
  // Walls
  // ---------------
  Control.addVariable("Floor",-188.0);              // Depth of floor
  Control.addVariable("Roof",190.0);                // Start of roof
  Control.addVariable("wallConcRight",-90.0);       // Wall (right to target)
  Control.addVariable("wallConcLeft",196.0);        // Wall (left to target)
  Control.addVariable("concLayer",16.0);            // Concreate layer thickness
  Control.Parse("wallConcRight-concLayer");         // Start of steel (right)
  Control.addVariable("wallSteelRight");            
  Control.Parse("wallConcLeft+concLayer");         // Start of steel (right)
  Control.addVariable("wallSteelLeft");            
  
// -------------
// Muon Target
// -------------
  Control.addVariable("muonThick",1.0);            
  Control.addVariable("muonSize",4.0);            
  Control.addVariable("muonPos",-236.0);            

// ------------
// Extent
// ------------
  Control.Parse("Floor-50.0");         // Start of steel (right)
  Control.addVariable("floorMax");
  Control.Parse("Roof+50.0");         // Start of steel (right)
  Control.addVariable("roofMax");
  Control.Parse("wallSteelRight-50");         // Start of steel (right)
  Control.addVariable("rightMax");
  Control.Parse("wallSteelLeft+50");         // Start of steel (right)
  Control.addVariable("leftMax");
  Control.addVariable("targetMax",-245.0);
  Control.addVariable("downStreamMax",500.0);

  return;
}

void
addF5Tally(Simulation& System,const int tNumber)
  /*!
    adds a point tally 
    \parma System :: Simulation item
    \param tNumber :: Tally Number
  */
{
  tallySystem::pointTally TX(tNumber);
  std::vector<Geometry::Vec3D> VList;
  VList.push_back(Geometry::Vec3D(-2,0,-11));
  VList.push_back(Geometry::Vec3D(2,0,-11));
  VList.push_back(Geometry::Vec3D(2,0,-7));
  VList.push_back(Geometry::Vec3D(-2,0,-7));
  TX.setWindow(VList);
  TX.setCentre(100.0);
  TX.setActive(1);
  TX.setParticles("n");                    /// F5 :: tally on neutrons
  TX.setEnergy("1.0e-10 155log 1e3");
  TX.setTime("1e-1 10log 1e6");
  System.addTally(TX);
}

void
addF4Tally(Simulation& System,const int cellNum,
	   const std::vector<int>& Units)
  /*!
    Addition of a tally to the mcnpx
    deck
    \param System :: Simulation class
    \param cellNum :: Cell number to tally 
    \param Unit :: 
  */
{
  tallySystem::cellFluxTally TX(cellNum);
  TX.setParticles("n");                    /// F4 tally on neutrons
  TX.setSD(1.0);                           /// Weight to 1.0
  TX.setActive(1);                         /// Turn it on
  TX.addCells(Units);
  System.addTally(TX);

  const std::vector<int> CL=System.getCellVector();
  System.getPC().setVolume(CL,1.0);
  System.getPC().setVolume(1,0);
  System.getPC().setCells(1,0);
  System.getPC().setHist(1);

  return;
}

int 
main(int argc,char* argv[])
{
  if (argc<3)
    {
      std::cerr<<"Insufficient files ::"
	       <<" Eng.i out.x"
	       <<std::endl;
      exit(1);
    }
    
  std::string Fname=argv[1];         // Master file name
  std::string Oname=argv[2];         // Master file name

  /*!
    The big variable setting
  */
  

  std::istringstream cx;
  setVariable(Sim.getDataBase());

  // Definitions section 
  try
    {
      Sim.readMaster(Fname);
      setWeights();
      Sim.refineSim();
      Sim.getPC().setNPS(100000);
      // Add Tallies for tracks
      // addF4Tally(Sim);
      
      Sim.write(Oname);
      Sim.writeCinder();
    }
  catch (ColErr::ExBase& A)
    {
      std::cerr<<A.getErrorStr()<<std::endl;
      exit(1);
    }
}
