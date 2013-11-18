/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/tupgrade.cxx
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
#include <string>
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
#include "pointTally.h"
#include "heatTally.h"
#include "tallyFactory.h"
#include "TallyCreate.h"
#include "Transform.h"
#include "Surface.h"
#include "Rules.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Intersect.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Weights.h"
#include "PhysImp.h"
#include "PhysicsCards.h"
#include "Simulation.h"

/*!
  \mainpage ANSYS

  \section Introduction
 
  Main program to read all the data in an make the global structure 

  \section Useage

  The program is designed to be used by
*/


Simulation Sim;

void addFlightLines(Simulation&,FuncDataBase&);           ///< Adds the flight lines to the objects
void setVirtualContainers(Simulation&);                   ///< Sets the virtual containers

void setVariable(FuncDataBase&); ///< Set all the variables
void setWeights();               ///< Set the weight system
void addDecoupler(Simulation&,FuncDataBase&);
std::string getSlice(const int,const int,const int,const int,const int);

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
      addFlightLines(Sim,Sim.getDataBase());
      setVirtualContainers(Sim);
      addDecoupler(Sim,Sim.getDataBase());
      Sim.removeTS1DeadCells();           // Uses specific info in removeDead and AlterSurf
      Sim.removeDeadSurfaces();         // Generic
      Sim.getPC().setCells("imp",1,0);	
      Sim.getPC().setCells("imp",70212,0);	
      tallySystem::addF5Tally(Sim,15,10001,5001,5002,5005,5006);         // Methane
      tallySystem::addF5Tally(Sim,25,20002,5005,5006,21003,21004);         // Methane
//      addHeatBlock();
      setWeights();
      Sim.write(Oname);
//      Sim.writeCinder();
    }
  catch (ColErr::ExBase& A)
    {
      std::cerr<<A.what()<<std::endl;
      exit(1);
    }
}

void
setVirtualContainers(Simulation& System)
{
  System.makeVirtual(1000);        // Container for the methane moderator
  System.makeVirtual(2000);        // Container for the hydrogen moderator
  System.makeVirtual(7000);        // Container for the water
  System.makeVirtual(8000);        // Container for the merlin moderator
  System.makeVirtual(9000);        // Container for the merlin moderator
  return;
}

void
addFlightLines(Simulation& System,FuncDataBase& Control)
  /*!
    Add flight lines 
    \param System :: Simulation object
    \param Control :: FunctionDatabase 
   */
{
  Geometry::Vec3D Nvec;
  Geometry::Vec3D Diff;
  Geometry::Vec3D Mid;
  double S,hSep;
  std::vector<Geometry::Vec3D> OutA;
  std::vector<Geometry::Vec3D> OutB;

  // WATER flight
  Control.Parse("watAngleA");
  const double WA=Control.Eval();
  Control.Parse("watAngleB");
  const double WB=Control.Eval();
  Control.Parse("watAngleC");
  const double WC=Control.Eval();
  Control.Parse("watAngleD");
  const double WD=Control.Eval();
  // MERLIN
  Control.Parse("merlinAngleA");
  const double OA=Control.Eval();
  Control.Parse("merlinAngleB");
  const double OB=Control.Eval();
  
  // MERLIN TOP WATER
  Nvec=Geometry::Vec3D(0.0,cos(OA*M_PI/180.0),-sin(OA*M_PI/180.0));
  // Now get Intercept points on outside edge and creap in
  if (!System.calcVertex(50001,50004,212,OutA) ||
      !System.calcVertex(50001,50003,212,OutB) )
    {
      std::cerr<<"Error finding intercept for 50001 50003/4 212"<<std::endl;
      exit(1);
    }
  Diff=(OutB[0]-OutA[0])/2.0;
  S=Diff.abs();
  Mid=(OutA[0]+OutB[0])/2.0;
  hSep=Control.EvalVar<double>("merlinViewHorr")/2.0;
  
  System.addPlane(6021,Nvec,Mid-Diff*(hSep/S));
  Nvec=Geometry::Vec3D(0.0,cos(OB*M_PI/180.0),-sin(OB*M_PI/180.0));
  System.addPlane(6022,Nvec,Mid+Diff*(hSep/S));

  // TOP WATER
  Nvec=Geometry::Vec3D(0.0,cos(WA*M_PI/180.0),-sin(WA*M_PI/180.0));
  // Now get Intercept points on outside edge and creap in
  if (!System.calcVertex(40001,40003,212,OutA) ||
      !System.calcVertex(40001,40004,212,OutB) )
    {
      std::cerr<<"Error finding intercept for 40001 40003/4 212"<<std::endl;
      exit(1);
    }
  Diff=(OutB[0]-OutA[0])/2.0;
  S=Diff.abs();
  Mid=(OutA[0]+OutB[0])/2.0;
  hSep=Control.EvalVar<double>("waterViewHorr")/2.0;
  
  System.addPlane(6001,Nvec,Mid-Diff*(hSep/S));
  Nvec=Geometry::Vec3D(0.0,cos(WB*M_PI/180.0),-sin(WB*M_PI/180.0));
  System.addPlane(6002,Nvec,Mid+Diff*(hSep/S));
  // BASE WATER
  Nvec=Geometry::Vec3D(0.0,cos(WC*M_PI/180.0),-sin(WC*M_PI/180.0));
  // Now get Intercept points on outside edge and creap in
  if (!System.calcVertex(40002,40003,212,OutA) ||
      !System.calcVertex(40002,40004,212,OutB) )
    {
      std::cerr<<"Error finding intercept for 40002 40003/4 212"<<std::endl;
      exit(1);
    }
  Diff=(OutB[0]-OutA[0])/2.0;
  S=Diff.abs();
  Mid=(OutA[0]+OutB[0])/2.0;
  hSep=Control.EvalVar<double>("waterViewHorr")/2.0;
  
  System.addPlane(6011,Nvec,Mid-Diff*(hSep/S));
  Nvec=Geometry::Vec3D(0.0,cos(WD*M_PI/180.0),-sin(WD*M_PI/180.0));
  System.addPlane(6012,Nvec,Mid+Diff*(hSep/S));

  // METHANE flight
  Control.Parse("methAngleHSide");
  const double MA=Control.Eval();
  Control.Parse("methAngleFar");
  const double MM=Control.Eval();
  Nvec=Geometry::Vec3D(0.0,cos(MA*M_PI/180.0),-sin(MA*M_PI/180.0));

  // Now get Intercept points on outside edge and creap in
  if (!System.calcVertex(10001,10003,202,OutA) ||
      !System.calcVertex(10001,10004,202,OutB) )
    {
      std::cerr<<"Error finding intercept for 10001 10003/4 202"<<std::endl;
      exit(1);
    }
  Diff=(OutB[0]-OutA[0])/2.0;
  S=Diff.abs();
  Mid=(OutA[0]+OutB[0])/2.0;
  hSep=Control.EvalVar<double>("methViewHorr")/2.0;
  
  System.addPlane(5001,Nvec,Mid-Diff*(hSep/S));

//  System.addPlane(5001,Nvec,10001,10003,202);


  Nvec=Geometry::Vec3D(0.0,cos(MM*M_PI/180.0),-sin(MM*M_PI/180.0));
  System.addPlane(5002,Nvec,Mid+Diff*(hSep/S));
//  System.addPlane(5002,Nvec,10001,10004,202);
  
  //
  // Methane other Flight
  //
  // METHANE flight
  Control.Parse("HMAngleHSide");
  const double HA=Control.Eval();
  Control.Parse("HMAngleFarSide");
  const double HM=Control.Eval();
  Nvec=Geometry::Vec3D(0.0,cos(HA*M_PI/180.0),-sin(HA*M_PI/180.0));

  if (!System.calcVertex(10002,10003,202,OutA) ||
      !System.calcVertex(10002,10004,202,OutB) )
    {
      std::cerr<<"Error finding intercept for 10002 10003/4 202"<<std::endl;
      exit(1);
    }

  Diff=(OutB[0]-OutA[0])/2.0;
  S=Diff.abs();
  Mid=(OutA[0]+OutB[0])/2.0;
  System.addPlane(5011,Nvec,Mid-Diff*(hSep/S));

  Nvec=Geometry::Vec3D(0.0,cos(HM*M_PI/180.0),-sin(HM*M_PI/180.0));
  System.addPlane(5012,Nvec,Mid+Diff*(hSep/S));

 
  // Hydrogen flight

  Control.Parse("HAngleSide");
  const double HX=Control.Eval();
  Nvec=Geometry::Vec3D(0.0,cos(HX*M_PI/180.0),-sin(HX*M_PI/180.0));
  System.addPlane(5021,Nvec,20002,20003,202);

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
  Eval[0]=1.5e-10;
  Eval[1]=6.0e-10;
  Eval[2]=1e-9;
  Eval[3]=3.0e-9;
  Eval[4]=3.0e-8;
  Eval[5]=1e7;
  WR.setEnergy('n',Eval);

  Sim.populateWCells();

  std::vector<double> WT(6);
  WT[0]=0.002;WT[1]=0.07;WT[2]=0.1;
  WT[3]=0.3;WT[4]= 0.4;WT[5]=0.7;
  WR.balanceScale('n',WT);
  WR.maskCell('n',1);
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
  Control.addVariable("beamsize",3.4);            // Radius the beam
  Control.addVariable("flightradius",3.6);        // Radius the beam in the flightline
  Control.addVariable("cladding",0.1);            // Ta Cladding on W
  Control.addVariable("radiusInner",3.4);         // Inner Radius
  Control.addVariable("radiusOuter",1.5);         // Inner Radius
  Control.addVariable("twater",0.15);             // Inner Radius
  Control.addVariable("steel",0.5);
  Control.addVariable("targetlength",25);         // Target length

  Control.Parse("(beamsize/3.0)*((8.0*log(2.0))^0.5)");
  Control.addVariable("gaussbeam"); 
  Control.addVariable("targetDX",0.0);           // Shift in target height
  Control.addVariable("targetDY",0.0);           // Shift in target horrizontal


  Control.addVariable("tungmat",38);              // Material for target (W pure)
  Control.addVariable("coolant",31);              // Material for coolant 
  Control.addVariable("cladmat",32);              // Cladding material

// ------------
// Decoupler
// ------------ 

  Control.addVariable("nvane",3);
  Control.addVariable("nslice",0);
  Control.addVariable("vaneThick",0.12);
  Control.addVariable("vanemat",7);        // cadmium

// ----------
// Split
// ----------

  Control.addVariable("hotCentre",14.55);         // This needs to be fixed
  Control.addVariable("hotViewHeight",5.0);       // The half-height 
  Control.addVariable("watAngleA",160);    // Top angle
  Control.addVariable("watAngleB",122);    // top
  Control.addVariable("watAngleC",160-48);    //
  Control.addVariable("watAngleD",134);    // 
  Control.addVariable("merlinAngleA",65);    // Merlin side (85)
  Control.addVariable("merlinAngleB",95);    // Merlin side (80)
  Control.addVariable("waterViewHorr",12.0);      // Total view of water
  Control.addVariable("merlinViewHorr",10.0);      // Total view of merlin

  Control.addVariable("coldCentre",-14.55);       // This needs to be fixed
  Control.addVariable("coldViewHeight",5.0);          // The half-height 

  
  Control.addVariable("methAngleFar",26);      // Extra rotation off the main flight angle
  Control.addVariable("methAngleHSide",75);      // Extra rotation off the main flight angle
  Control.addVariable("methViewHorr",12.0);      // Total view of methane
  Control.addVariable("HMAngleHSide",45);   // Rotation off methane (hydrogen side)
  Control.addVariable("HMAngleFarSide",75);   // Rotation off methant (far side)

  Control.addVariable("HAngleSide",85);   // Rotation off hydrogen
  // WATER VIEW

// ---------------
// WATER Moderator
// ---------------
  Control.addVariable("watCY",0.0);         // 
  Control.addVariable("watCZ",21.0);    //  from the target front (same a ch4)
  Control.addVariable("wangle",-58.0);   // 

  Control.addVariable("watHeight",5.0);       // methane away from target
  Control.addVariable("watBase",5.0);         // methane size to the target
  Control.addVariable("watRWidth",6.0);       // methane width
  Control.addVariable("watLWidth",6.0);       // methane width
  Control.addVariable("watThick",2.1);        //  methane thickness
  Control.addVariable("waterAl",0.3);

  Control.Parse("watBase+waterAl");
  Control.addVariable("wTotalBase");
  Control.Parse("watHeight+waterAl");
  Control.addVariable("wTotalTop");
  Control.Parse("watThick+waterAl");
  Control.addVariable("wTotalThick");
  Control.Parse("watLWidth+waterAl");
  Control.addVariable("wTotalLWidth");
  Control.Parse("watRWidth+waterAl");
  Control.addVariable("wTotalRWidth");

// ---------------
// Merlin Moderator
// ---------------
  Control.addVariable("merlinCY",0.0);     // 
  Control.addVariable("merlinCZ",5.0);    // from the target front (same a ch4)
  Control.addVariable("oangle",80.0);     //  merlin angle

  Control.addVariable("merlinHeight",5.0);       // methane away from target
  Control.addVariable("merlinBase",5.0);         // methane size to the target
  Control.addVariable("merlinRWidth",6.0);       // methane width
  Control.addVariable("merlinLWidth",6.0);       // methane width
  Control.addVariable("merlinThick",2.1);        //  methane thickness

  Control.Parse("merlinBase+waterAl");
  Control.addVariable("oTotalBase");
  Control.Parse("merlinHeight+merlinerAl");
  Control.addVariable("oTotalTop");
  Control.Parse("merlinThick+waterAl");
  Control.addVariable("oTotalThick");
  Control.Parse("merlinLWidth+waterAl");
  Control.addVariable("oTotalLWidth");
  Control.Parse("watRWidth+waterAl");
  Control.addVariable("oTotalRWidth");

// -----------------
// Methane moderator
// -----------------

  Control.addVariable("methCY",0.0);         // 
  Control.addVariable("methCZ",21.0);         //  shift down from the target front
  Control.addVariable("mangle",58.0);         // 

  Control.addVariable("methPoison",0.005);       // Methane poisoning
  Control.addVariable("methHeight",5.0);          // methane away from target
  Control.addVariable("methBase",5.0);          // methane size to the target
  Control.addVariable("methRWidth",6.0);          // methane width
  Control.addVariable("methLWidth",6.0);          // methane width
  Control.addVariable("methThick",2.1);         //  methane thickness

  Control.addVariable("methInnerAl",0.3);
  Control.addVariable("methVac",0.2);
  Control.addVariable("methOuterAl",0.2);

  Control.addVariable("mPreAl",0.3);
  Control.addVariable("mPreRWidth",0.0);
  Control.addVariable("mPreLWidth",0.0);
  Control.addVariable("mPreBase",0.0);
  Control.addVariable("mPreTop",0.6);       // toward target

  Control.addVariable("mliqtemp",90);           // Methan temperature
  Control.addVariable("methmat",29);            // Methane material (liq)
  Control.addVariable("gdmat",6);            // Methane material (liq)

  Control.Parse("methInnerAl+methVac+methOuterAl");
  Control.addVariable("mLayers");

  // PreMod::
  Control.Parse("methThick+mLayers");
  Control.addVariable("mTotalThick");

  if (Control.EvalVar<double>("mPreTop")>0.0)
    Control.Parse("methHeight+mLayers+mPreTop+mPreAl");
  else
    Control.Parse("methHeight+mLayers");
  Control.addVariable("mTotalTop");

  if (Control.EvalVar<double>("mPreBase")>0.0)
    Control.Parse("methBase+mLayers+mPreBase+mPreAl");
  else
    Control.Parse("methBase+mLayers");
  Control.addVariable("mTotalBase");

  if (Control.EvalVar<double>("mPreRWidth")>0.0)
    Control.Parse("methRWidth+mLayers+mPreRWidth+mPreAl");
  else
    Control.Parse("methRWidth+mLayers");
  Control.addVariable("mTotalRWidth");

  if (Control.EvalVar<double>("mPreLWidth")>0.0)
    Control.Parse("methLWidth+mLayers+mPreLWidth+mPreAl");
  else
    Control.Parse("methLWidth+mLayers");
  Control.addVariable("mTotalLWidth");

// -----------------
// Hydrogen Moderator
// -----------------
  Control.addVariable("hliqtemp",20);            // Hydrogen material
  Control.addVariable("hymat",25);            // Hydrogen material
  Control.addVariable("almat",5);             // aluminium material
  Control.addVariable("watmat",11);             // aluminium material

  Control.addVariable("HBase",6.2);          // The half-height (towards target)
  Control.addVariable("hCY",0.0);                 // Centre position
  Control.addVariable("hCZ",5.0);                //Centre position
  Control.addVariable("hangle",100.0);             // hydrogen angle

// Liquid table
  Control.addVariable("hLiqWidth",5.0);
  Control.addVariable("hLiqBackDepth",1.5);
  Control.addVariable("hLiqFrontDepth",3.5);
  Control.addVariable("hLiqHeight",5.0);          //

// First layer Al
  Control.addVariable("hAlInnerThick",0.4);    // Thickness of inner aluminium
// Vac layer 1:
  Control.addVariable("hvacgap",0.2);         // Vac gap
// Second layer Al
  Control.addVariable("hAlMidThick",0.2);    // Thickness of inner aluminium
// Third layer Al
  Control.addVariable("hAlOuterThick",0.2);    // Thickness of inner aluminium

// Pre-mod Al:
  Control.addVariable("preWater",1.0);
  Control.addVariable("preBaseWater",1.0);
  Control.addVariable("preTopWater",1.0);
  Control.addVariable("preBackWater",1.5);
  Control.addVariable("preModAl",0.3);
  


// ----------
// Reflector
// ----------

  Control.addVariable("refmat",37);         // Reflector material (37 == solid be@rt)

  Control.addVariable("refsize",35);       // size of the reflector (half width)
  Control.addVariable("refedge",9);        // size of the flat ends
  Control.addVariable("refxshift",0);      // x shift of reflector
  Control.addVariable("refyshift",0);      // y shift of reflector
  Control.addVariable("refzshift",15);    // z shift of reflector


  // USEFUL ABBRIVIATIONS:

  Control.Parse("methCY*sind(mangle)+methCZ*cosd(mangle)");
  Control.addVariable("mrotX");
  Control.Parse("methCY*cosd(mangle)-methCZ*sind(mangle)");
  Control.addVariable("mrotY");


  Control.Parse("methInnerAl+methVac+methOuterAl");
  Control.addVariable("mLayers");
  
  // WATER

  Control.Parse("watCY*sind(wangle)+watCZ*cosd(wangle)");
  Control.addVariable("wrotX");
  Control.Parse("watCY*cosd(wangle)-watCZ*sind(wangle)");
  Control.addVariable("wrotY");

  // MERLIN

  Control.Parse("merlinCY*sind(oangle)+merlinCZ*cosd(oangle)");
  Control.addVariable("orotX");
  Control.Parse("merlinCY*cosd(oangle)-merlinCZ*sind(oangle)");
  Control.addVariable("orotY");

  // HYDROGEN

  Control.Parse("hCY*sind(mangle)+hCZ*cosd(mangle)");
  Control.addVariable("hrotX");
  Control.Parse("hCY*cosd(mangle)-hCZ*sind(mangle)");
  Control.addVariable("hrotY");

// hLiqWidth+
  Control.Parse("hAlInnerThick+2*hvacgap+hAlMidThick+hAlOuterThick");
  Control.addVariable("hLayers");
  Control.Parse("hLiqWidth+hLayers+preModAl+preWater");
  Control.addVariable("hFullWidth");              // Full width of the who 

  Control.addVariable("hLiqBackDepth",1.5);
  Control.Parse("hLiqWidth+hLayers+preModAl+preWater");

  Control.Parse("hLiqBackDepth+hLayers+preModAl+preBackWater");
  Control.addVariable("hBackDepth");          //  methane thickness

  Control.Parse("hLiqFrontDepth+hLayers");
  Control.addVariable("hFrontDepth");          //  methane thickness

  Control.Parse("hLiqHeight+hLayers+preModAl+preTopWater");
  Control.addVariable("HTop");          // The half-height (away from target)

  Control.Parse("hLiqHeight+hLayers+preModAl+preBaseWater");
  Control.addVariable("HBase");          // The half-height (away from target)

// Stuff for Be ratios:

  Control.Parse("wTotalTop");            // Add max after merlin
  Control.addVariable("hotTop");
  Control.Parse("wTotalBase");
  Control.addVariable("hotBase");
  Control.Parse("max(HBase,mTotalBase)");
  Control.addVariable("coldBase");
  Control.Parse("max(HTop,mTotalTop)");
  Control.addVariable("coldTop");

  return;
}


void
addDecoupler(Simulation& System,FuncDataBase& Control)
  /*!
    ADDS TO CELL :: 70001 
  */
{
  const int nvane=static_cast<int>(Control.EvalVar<double>("nvane"));
  const int nslice=static_cast<int>(Control.EvalVar<double>("nslice"));
  if (nvane<=0)  // No work
    return;
  const double vaneT=Control.EvalVar<double>("vaneThick");
  const int vmat=static_cast<int>(Control.EvalVar<double>("vanemat"));
  const double Vrho=System.getMaterial(vmat).getAtomDensity();
//  const int almat=static_cast<int>(Control.EvalVar<double>("almat"));
//  const double Alrho=System.getMaterial(almat).getAtomDensity();

  // Get  Edges
  Control.Parse("radiusInner+3*cladding+2*twater+radiusOuter+steel");
  const double YA=-Control.Eval();
  Control.Parse("-(refsize+refedge)+refyshift");
  const double YB=Control.Eval();

  const double XA= -YA;
  Control.Parse("refsize+refedge+refyshift");
  const double XB=Control.Eval();
  const double StepY=(YB-YA)/(nvane+1);
  const double StepX=(XB-XA)/(nvane+1);
    
  int cellN=7001;
  int planeN=7001;
  int sliceN=8001;

  Geometry::Vec3D Nvec(0,1,0);
  Geometry::Vec3D Zvec(0,0,1);
  Geometry::Vec3D PVec(0,0,0);
  Qhull* BasePtr=System.findQhull(70001);
  Qhull* BasePtrPlus=System.findQhull(70032);
  if (BasePtr==0 || BasePtrPlus==0)
    {
      std::cerr<<"Not BASE CELL "<<70001<<std::endl;
      exit(1);
    }
  std::vector<Geometry::Vec3D> OutA;
  std::vector<Geometry::Vec3D> OutB;
  Geometry::Vec3D VStep;
  std::ostringstream cx;
  // 70001: LEFT
  for(int i=0;i<nvane;i++)
    {
      PVec[1]=YA+(i+1)*StepY;
      System.addPlane(planeN,Nvec,PVec);
      PVec[1]-=vaneT;
      System.addPlane(planeN+1,Nvec,PVec);
      // ADD SLICES (Z-planes)
      System.calcVertex(102,planeN,211,OutA);
      System.calcVertex(108,planeN,211,OutB);
      VStep=(OutB[0]-OutA[0])/(nslice+1.0);
      std::cerr<<"Vane == "<<VStep<<std::endl;
      for(int j=1;j<nslice;j++)
	System.addPlane(sliceN+j,Zvec,OutA[0]+VStep*j);

      Qhull A;
      cx.str("");
      cx<<cellN<<" "<<vmat<<" "<<Vrho<<" "
	  <<"101 103  -107 202 -211 ";
      cx<<-planeN<<" "<<planeN+1;
      cx<<getSlice(nslice,i % 2,-102,-108,sliceN);
      
      A.setObject(cx.str());
      while(!System.addCell(cellN,A))
	cellN++;

      Qhull Aplus;  // 70032 object
      cx.str("");
      cx<<cellN<<" "<<vmat<<" "<<Vrho<<" "
	<<"101 -102 103 -107 -108 -201 110 ";
      cx<<-planeN<<" "<<planeN+1;
      Aplus.setObject(cx.str());
      while(!System.addCell(cellN,Aplus))
	cellN++;

      cx.str("");
      cx<<" ( "<<planeN<<" : "<<-(planeN+1)<<") ";
      BasePtr->addSurfString(cx.str());
      BasePtrPlus->addSurfString(cx.str());
      planeN+=2;
    }

  // 70001: RIGHT
  for(int i=0;i<nvane;i++)
    {
      PVec[1]=XA+(i+1)*StepX;
      System.addPlane(planeN,Nvec,PVec);
      PVec[1]-=vaneT;
      System.addPlane(planeN+1,Nvec,PVec);
      Qhull A;
      cx.str("");
      cx<<cellN<<" "<<vmat<<" "<<Vrho<<" "
	<<"-104 -105 -106 -107 202 -211 ";
      cx<<-planeN<<" "<<planeN+1;
       A.setObject(cx.str());
      while(!System.addCell(cellN,A))
	cellN++;

      Qhull Aplus;  // 70032 object
      cx.str("");
      cx<<cellN<<" "<<vmat<<" "<<Vrho<<" "
	<<"-104 -105 -106 -107 -201 110 ";
      cx<<-planeN<<" "<<planeN+1;
      Aplus.setObject(cx.str());
      while(!System.addCell(cellN,Aplus))
	cellN++;

      cx.str("");
      cx<<" ( "<<planeN<<" : "<<-(planeN+1)<<") ";
      BasePtr->addSurfString(cx.str());
      BasePtrPlus->addSurfString(cx.str());
      planeN+=2;
    }


  return;
}


std::string
getSlice(const int N,const int F,const int A,const int B,const int S)
{
  std::ostringstream cx;
  if (N<1)
    {
      cx<<" "<<A<<" "<<B;
      return cx.str();
    }

//  int first;

  cx<<" "<<((!F) ? A : S);
  int i;
  for(i=F;i<N-1;i+=2)
    cx<<" ( "<<-(S+i)<<" : "<<S+i+1<<" )";
  if (i==N)
    cx<<" "<<B;
  else
    cx<<" "<<-(S+i);

  return  cx.str();
}
