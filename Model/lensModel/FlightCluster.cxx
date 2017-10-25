/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   lensModel/FlightCluster.cxx
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
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FlightLine.h"
#include "FlightCluster.h"

namespace lensSystem
{

FlightCluster::FlightCluster(const std::string& Key) :  
  keyName(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param N :: Index value
    \param Key :: Name for item in search
  */
{}

FlightCluster::FlightCluster(const FlightCluster& A) : 
  keyName(A.keyName),FL(A.FL),InsertCells(A.InsertCells)
  /*!
    Copy constructor
    \param A :: FlightCluster to copy
  */
{}

FlightCluster&
FlightCluster::operator=(const FlightCluster& A)
  /*!
    Assignment operator
    \param A :: FlightCluster to copy
    \return *this
  */
{
  if (this!=&A)
    {
      FL=A.FL;
      InsertCells=A.InsertCells;
    }
  return *this;
}

FlightCluster::~FlightCluster() 
  /*!
    Destructor
  */
{}


void
FlightCluster::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Func Database
  */
{
  ELog::RegMethod RegA("FlightCluster","populate");

  const size_t flightN=Control.EvalVar<size_t>(keyName+"Number");
  for(size_t i=0;i<flightN;i++)
    {
      std::ostringstream cx;
      cx<<"FL"<<i+1;
      FL.push_back(moderatorSystem::FlightLine(cx.str()));
    }

  return;
}

const attachSystem::FixedComp& 
FlightCluster::getLine(const size_t Port) const
  /*!
    Flight Cluster
    \param Port :: Number
  */
{
  ELog::RegMethod RegA("FlightCluster","getLine");

  if (Port>=FL.size())
    throw ColErr::IndexError<size_t>(Port,FL.size(),
				  "Port number");
  return FL[Port];
}
  

void
FlightCluster::createLines(Simulation& System,
			   const attachSystem::FixedComp& FC,
			   const int outerSurf)
  /*!
    Construct all the flight lines
    \param System :: Simulation system
    \param FC :: Fixed component with 
    \param outerSurf :: outer surf to add
  */

{
  ELog::RegMethod RegA("FlightCluster","createLines");
  
  std::string Outer("outer");
  std::vector<moderatorSystem::FlightLine>::iterator vc;
  for(vc=FL.begin();vc!=FL.end();vc++)
    {
      for(const int IC : InsertCells)
	vc->addInsertCell(Outer,IC);

      if (outerSurf)
	{
	  vc->addBoundarySurf("inner",outerSurf);
	  vc->addBoundarySurf("outer",outerSurf);
	}
      vc->createAll(System,FC,2);
    }
  return;
}

void
FlightCluster::addInsertCell(const int CN)
  /*!
    Insert the cell nubmer into the Flight cluster
    \param CN :: Cell number						
   */
{
  ELog::RegMethod RegA("FlightCluster","addInsertCell");
  // Check if unique
  if (find(InsertCells.begin(),
	   InsertCells.end(),CN)==InsertCells.end())
    InsertCells.push_back(CN);
  
  
  return;
}

void
FlightCluster::createAll(Simulation& System,
			 const attachSystem::FixedComp& FC,
			 const int outerSurf)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed component
  */
{
  ELog::RegMethod RegA("FlightCluster","createAll");
  populate(System.getDataBase());
  createLines(System,FC,outerSurf);
  return;
}
  
}  // NAMESPACE shutterSystem
