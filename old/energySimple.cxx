/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/energySimple.cxx
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
#include <vector>
#include <set>
#include <map>
#include <list>
#include <string>
#include <algorithm>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "MersenneTwister.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "InputControl.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Tensor.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "TallyCreate.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "PhysCard.h"
#include "PhysImp.h"
#include "KGroup.h"
#include "Source.h"
#include "PhysicsCards.h"
#include "TMRCreate.h"
#include "MainProcess.h"
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"

MTRAnd Rand(12345UL);

namespace ELog 
{
  ELog::OutputLog<EReport> EM;
  ELog::OutputLog<FileReport> FM("Spectrum.log");
  ELog::OutputLog<FileReport> RN("Renumber.txt");   ///< Renumber
  ELog::OutputLog<StreamReport> CellM;
}

void setVariable(FuncDataBase&); ///< Set all the variables
Simulation Sim;

int 
main(int argc,char* argv[])
{
  ELog::RegMethod RControl("","main");
  // Set up output information:
  ELog::EM.setNBasePtr(RControl.getBasePtr());
  ELog::EM.setTypeFlag(0);
  ELog::EM.setActive(255);
  ELog::FM.setActive(255);

  std::vector<std::string> Names;              
  std::map<std::string,std::string> AddValues;       // Variable to chaneg
  std::map<std::string,std::string> Values;       // Variable to change
  std::map<std::string,double> IterVal;           // Variable to iterate 
  InputControl::mainVector(argc,argv,Names);
  int multi(1);
  const int multiFlag(InputControl::flagVExtract(Names,"m","multi",multi));
  int nps(600000);
  InputControl::flagVExtract(Names,"n","nps",nps);
  long int randNum(375642321L);
  InputControl::flagVExtract(Names,"r","random",randNum);

  std::string varName;
  std::string varExpr;
  while(InputControl::flagVExtract(Names,"v","value",varName,varExpr))
    {
      Values[varName]=varExpr;
    }

  double iValue;
  while(InputControl::flagVExtract(Names,"i","iterate",varName,iValue))
    {
      IterVal[varName]=iValue;
    }

  // Now count parameters:
  if (Names.size()<2)
    {
      std::cerr<<"Insufficient files ::"
	       <<" Eng.i out.x"<<std::endl
	       <<"Options :"<<std::endl
	       <<" -m multinum :: Number of index files with different RND seeds."<<std::endl
	       <<" -n Nps :: Number of point"<<std::endl
	       <<" -v VarName ValueStr :: Change a variable at runtime"<<std::endl
	       <<" -i VarName DeltaIndex :: Change a variable at runtime iteration"<<std::endl
	       <<std::endl;
      exit(1);
    }
  
  std::string Fname=Names[0];         // Master file name
  std::string Oname=Names[1];         // Output file name
  /*!
    The big variable setting
  */

  std::istringstream cx;

//  Sim.registerAlterSurface(new AlterSurfTS2(Sim.getDataBase()));
//  Sim.registerRemoveCell(new TS2remove());   // database/cells get set later

  // Definitions section       Sim.removeDeadSurfaces();         // Generic
  const std::string EnergyStr="1.00E-10 4.14E-07 1.12E-06 5.04E-06 2.26E-05 4.54E-04 "
    "3.35E-03 1.50E-02 8.65E-02 2.24E-01 4.98E-01 9.07E-01 "       
    "1.35E+00 2.02E+00 3.01E+00 4.49E+00 6.70E+00 1.00E+01 "      
    "1.35E+01 1.75E+01 2.25E+01 2.75E+01 3.50E+01 4.50E+01 "      
    "5.50E+01 6.50E+01 8.00E+01 1.00E+02 1.20E+02 1.60E+02 "      
    "2.00E+02 2.50E+02 3.00E+02 3.50E+02 4.00E+02 800";
  double EValue[]={
    1.00E-10,4.14E-07,1.12E-06,5.04E-06,2.26E-05,4.54E-04,
    3.35E-03,1.50E-02,8.65E-02,2.24E-01,4.98E-01,9.07E-01,       
    1.35E+00,2.02E+00,3.01E+00,4.49E+00,6.70E+00,1.00E+01,       
    1.35E+01,1.75E+01,2.25E+01,2.75E+01,3.50E+01,4.50E+01,       
    5.50E+01,6.50E+01,8.00E+01,1.00E+02,1.20E+02,1.60E+02,       
    2.00E+02,2.50E+02,3.00E+02,3.50E+02,4.00E+02,800 };
  
  try
    {
      for(int MCIndex=0;MCIndex<multi;MCIndex++)
        {
	  Sim.resetAll();
	  setVariable(Sim.getDataBase());
	  mainSystem::setRunTimeVariable(Sim.getDataBase(),Values,AddValues);

	  FuncDataBase& Control=Sim.getDataBase();
	  Control.setVariable("topEnergy",EValue[MCIndex+1]);
	  Control.setVariable("baseEnergy",EValue[MCIndex]);

	  Sim.readMaster(Fname);
	  Sim.getPC().setNPS(nps);
	  Sim.getPC().setRND(randNum);


	  // 90 degree:
	  tallySystem::addSimpleF5Tally(Sim,15,Geometry::Vec3D(0,100,0),
					"n",EnergyStr,"0 1e8");
	  // 10 degree:
	  tallySystem::addSimpleF5Tally(Sim,25,Geometry::Vec3D(100*cos(10*M_PI/180.0),100*sin(10*M_PI/180),0),
					"n",EnergyStr,"0 1e8");
	  // 45 degree:
	  tallySystem::addSimpleF5Tally(Sim,35,Geometry::Vec3D(100*cos(45*M_PI/1800.),100*sin(45*M_PI/180),0),
					"n",EnergyStr,"0 1e8");
	  // 150 degree:
	  tallySystem::addSimpleF5Tally(Sim,45,Geometry::Vec3D(100*cos(150*M_PI/180.0),100*sin(150*M_PI/180),0),
					"n",EnergyStr,"0 1e8");
	  
	  Sim.getPC().setCells("imp",1,0);                // Set a zero cell
	  ELog::EM.lock();
	  Sim.removeComplements();
	  Sim.removeDeadCells();            // Generic
	  ELog::EM.dispatch(0);
	  ELog::EM.lock();
	  Sim.removeDeadSurfaces(0);         // Generic
	  ELog::EM.dispatch(0);

	  Sim.processCellsImp();            // Required since no-TS1/TS2 remove called

	  
	  SimProcess::writeIndexSim(Sim,Oname,MCIndex);
	}
      Sim.writeCinder();
    }
  catch (ColErr::ExBase& A)
    {
      std::cerr<<A.what()<<std::endl;
      exit(1);
    }
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
  Control.addVariable("one",1.0);       // One

// ------------
// Target stuff
// ------------
  Control.addVariable("baseEnergy",300.0);        // Energy of beam
  Control.addVariable("topEnergy",400.0);        // Energy of beam

  return;
}
