/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/power.cxx
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
#include "NameStack.h"
#include "RegMethod.h"
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
#include "pointTally.h"
#include "heatTally.h"
#include "surfaceTally.h"
#include "tallyFactory.h"
#include "TallyCreate.h"
#include "Transform.h"
#include "Surface.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "weightManager.h"
#include "PhysImp.h"
#include "PhysicsCards.h"
#include "TallyCreate.h"
#include "Simulation.h"

namespace ELog 
{
  ELog::OutputLog<EReport> EM;
  ELog::OutputLog<FileReport> FM("Spectrum.log");
  ELog::OutputLog<FileReport> RN("Renumber.txt");   ///< Renumber
  ELog::OutputLog<StreamReport> CellM;
}

Simulation Sim;

void setVariable(FuncDataBase&); ///< Set all the variables
void setWeights();               ///< Set the weight system
void addHeatBlock();             ///< Add heat for all cells and all particles

// Extras
void addIradBeamline(Simulation&,FuncDataBase&);

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

  // Definitions section       Sim.removeDeadSurfaces();         // Generic
  try
    {
      Sim.readMaster(Fname);
      Sim.getPC().setCells("imp",1,0);                // Set a zero cell
      
      std::vector<int> C;
      C.push_back(11);
      tallySystem::addF5Tally(Sim,15,605,601,602,603,604,-100.0);
      tallySystem::modF5TallyCells(Sim,15,C);
      Sim.removeDeadSurfaces();         // Generic
      Sim.processCellsImp();            // Required since no-TS1/TS2 remove called
      setWeights();
      Sim.write(Oname);
    }
  catch (ColErr::ExBase& A)
    {
      std::cerr<<A.what()<<std::endl;
      exit(1);
    }
}

void
addF1Tally(Simulation& System,const int tNum,
	   const int surfNum)
  /*!
    Addition of a tally to the mcnpx deck
    \param System :: Simulation class
    \param tNum :: tally number
    \param surfNum :: surface to tally
  */
{
  tallySystem::surfaceTally TX(tNum);
  TX.setParticles("n");                  /// F1 Tally on neutrons
  TX.addSurface(surfNum);
  TX.setEnergy("1.0e-10 155log 1e3");
  TX.setTime("1.0 179log 1e6");
  TX.setPrintField("f d u s m e t c");
  System.addTally(TX);

  return;
}

void
addF5Tally(Simulation& System,const int tNumber,
	   const int bPlane,const int A,const int B,
	   const int C, const int D,const double Distance)
  /*!
    adds a point tally . Forms agroup of
    pairs (A-B) and (C-D)
    \parma System :: Simulation item
    \param tNumber :: Tally Number
    \param A :: First part of pair
    \param B :: Second part of pair
    \param C :: First part of pair
    \param D :: Second part of pair
    \param Distance :: distance from teh surface to the tally point
  */
{
  tallySystem::pointTally TX(tNumber);
  std::vector<Geometry::Vec3D> VList;
  std::vector<Geometry::Vec3D> Out;
  // Get Methane plane intercepts:

  System.calcVertex(bPlane,A,D,Out);
  VList.push_back(Out[0]);
  System.calcVertex(bPlane,A,C,Out);
  VList.push_back(Out[0]);
  System.calcVertex(bPlane,B,C,Out);
  VList.push_back(Out[0]);
  System.calcVertex(bPlane,B,D,Out);
  VList.push_back(Out[0]);
  TX.setWindow(VList);
  TX.setCentre(Distance);
  TX.setActive(1);
  TX.setParticles("n");                    /// F5 :: tally on neutrons
  TX.setEnergy("1.0e-10 155log 1e5");
  TX.setTime("1.0 20log 1e8");
  System.addTally(TX);
}


void
addF5Tally(Simulation& System,const int tNumber)
  /*!
    adds a point tally 
    \param System :: Simulation item
    \param tNumber :: Tally Number
    \parma Point :: Data point for the tally
  */
{
  ELog::RegMethod RegA("addF5Tally");
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
  TX.setEnergy("5.0e-10 148log 1e3");
  TX.setTime("1e-1 10log 1e6");
  System.addTally(TX);
}


void
addHeatBlock()
  /*!
    Adds a f6 for everthing
  */
{
  const std::vector<int> Units=Sim.getNonVoidCellVector();
  const std::vector<int> CL=Sim.getCellVector();
  Sim.getPC().setVolume(CL,1.0);
  Sim.getPC().setVolume(1,0);
  tallySystem::heatTally TX(6);
  TX.setPlus(1);
  TX.setActive(1);                         /// Turn it on
  TX.addCells(Units);
  TX.addLine("e6 800");
  Sim.addTally(TX);

  char* part[]={"n","p","h"};
  for(int i=0;i<3;i++)
    {
      TX.setKey(16+10*i);
      TX.setPlus(0);
      TX.setParticles(part[i]);
      Sim.addTally(TX);
    }
  return;
}

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
  Eval[0]=1e-10;
  Eval[1]=3e-10;
  Eval[2]=1e-9;
  Eval[3]=1e-8;
  Eval[4]=1e-6;
  Eval[5]=1e7;
  WR.setEnergy('n',Eval);

  Sim.populateWCells();

  std::vector<double> WT(6);
  WT[0]=0.4;WT[1]=0.4;WT[2]=0.5;
  WT[3]=0.6;WT[4]= 0.6;WT[5]=0.7;
  WR.balanceScale('n',WT);
  WR.maskCell('n',1);
  // WATER Weights
  WT[0]=0.04;WT[1]=0.02;WT[2]=0.07;
  WT[3]=0.15;WT[4]=0.2;WT[5]=0.7;
  WR.setCell('n',11,WT);
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
  Control.addVariable("one",1.0);      // Zero

// ------------
// Target stuff
// ------------
  Control.addVariable("radius",75.1);        // Radius of the target
  Control.addVariable("radiusB",5.1);        // Radius of the target
  Control.addVariable("radiusA",3.1);        // Radius of the target
  Control.addVariable("width",12.0);
  Control.addVariable("height",12.0);
  Control.addVariable("zthick",5.0);
  Control.addVariable("drift",5.0);
  Control.Parse("2*radius+40.0");
  Control.addVariable("bewidth");
  Control.addVariable("mercury",39);        // Mercury
  Control.addVariable("water",11);            // Water
  Control.addVariable("be",37);            // Water
  Control.addVariable("energy",1300);            // Water
  return;
}

void
addIradBeamline(Simulation& System,FuncDataBase& Control)
  /*!
    Add an irradiation beamline
    \param System :: Simulation model to add
    \param Control :: Variables list
   */
{
  const double IHS=Control.EvalVar<double>("iradHsize");
//  const double sThick=Control.EvalVar<double>("shieldthick");
  if (IHS<=0.0)
    return;

  
  Qhull* voidPtr=System.findQhull(75);
  if (voidPtr==0)
    {
      std::cerr<<"Not BASE CELL "<<75<<std::endl;
      exit(1);
    }

  // If we have the void the add the outside line
  voidPtr->addSurfString("(21401 : -21402 : -21403 : 21404 : 21406 : -21405 : -185 )");
  voidPtr->addSurfString("(21301:-21302:-21303:21304:21405:-185)");


  return;
}


