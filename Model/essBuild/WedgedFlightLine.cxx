/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/WedgedFlightLine.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfExpand.h"
#include "TaperedFlightLine.h"
#include "FixedOffset.h"
#include "WedgeItem.h"
#include "WedgedFlightLine.h"

namespace essSystem
{

WedgedFlightLine::WedgedFlightLine(const std::string& Key)  :
  moderatorSystem::TaperedFlightLine(Key),
  flightIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(flightIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

  /*
  WedgedFlightLine::WedgedFlightLine(const WedgedFlightLine&A) :
    moderatorSystem::TaperedFlightLine(A),
    flightIndex(A.flightIndex),cellIndex(A.cellIndex),
    nWedges(A.nWedges)
  {
  }*/
  
WedgedFlightLine::~WedgedFlightLine() 
 /*!
   Destructor
 */
{}

void
WedgedFlightLine::populate(const FuncDataBase& Control)
/*!
  Populate all the variables
  \param Control :: Variable data base
*/
{
  ELog::RegMethod RegA("WedgedFlightLine","populate");

  nWedges=Control.EvalDefVar<size_t>(keyName+"NWedges", 0);
}

void
WedgedFlightLine::buildWedges(Simulation& System,
			   const attachSystem::FixedComp& originFC,
			   const long int originIndex,
			   const attachSystem::FixedComp& innerFC,
			   const long int innerIndex,
			   const attachSystem::FixedComp& outerFC,
			   const long int outerIndex)
/*!
  Builds the flight line wedges.
  Arguments are the same as in createAll.
*/
{
  ELog::RegMethod RegA("WedgedFlightLine","buildWedges");

  if (nWedges<1) return;

  for (size_t i=0; i<nWedges; i++)
    {
      std::shared_ptr<WedgeItem> wedge(new WedgeItem(keyName+"Wedge", i+1));
      //      OR.addObject(wedge);
      wedge->createAll(System, outerFC, static_cast<int>(outerIndex), *this, -11, 12);
      wedges.push_back(wedge);
    }


  // rebuild the Inner cell
  const std::pair<int,double> MatInfo=deleteCellWithData(System, "Inner");

  std::string Out;
  int index(0);
  int sepIndex(0), sepIndexPrev(0);
  const std::string innerCut=innerFC.getSignedLinkString(innerIndex);
  const std::string outerCut=outerFC.getSignedLinkString(outerIndex);
  const std::string floor = " " + StrFunc::makeString(SMap.realSurf(flightIndex+5)) + " ";
  const std::string roof = " " + StrFunc::makeString(SMap.realSurf(flightIndex+6)) + " ";

  cellIndex += 1000; // offset away from TaperedFlightLine

  for (size_t i=0; i<nWedges+1; i++)
    {
      index = flightIndex+1000+static_cast<int>(i);
      if (i<nWedges)
	{
	  sepIndex = ModelSupport::buildPlaneRotAxis(SMap, index,
						     wedges[i]->getCentre(),
						     wedges[i]->getLinkAxis(0),
						     Z, 90)->getName();
	  ELog::EM << "Why sepIndex != index? " << sepIndex << " " << index << ELog::endCrit;
	}

      Out = floor + roof + innerCut + outerCut;
      if (i==0)
	{
	  Out += StrFunc::makeString(-sepIndex) + " " +
	    StrFunc::makeString(SMap.realSurf(flightIndex+3)) + " " +
	    //	StrFunc::makeString(-SMap.realSurf(flightIndex+4)) + 
	    "(" + wedges[i]->getLinkString(1) + ":" +
	    wedges[i]->getLinkString(3) + ")";
	}
      else if (i==nWedges)
	{
	  Out += StrFunc::makeString(sepIndex) + " " +
	    StrFunc::makeString(-SMap.realSurf(flightIndex+4)) + " " +
	    "(" + wedges[i-1]->getLinkString(1) + ":" + wedges[i-1]->getLinkString(2) + ")";
	  
	}
      else
	{
	  Out += StrFunc::makeString(-sepIndex) + " " + // current wedge direction
	    StrFunc::makeString(sepIndexPrev) + " " + // prev wedge direction
	    "(" + wedges[i]->getLinkString(1) + ":" +  wedges[i]->getLinkString(3) + ")" + // current wedge
	    "(" + wedges[i-1]->getLinkString(1) + ":" +  wedges[i-1]->getLinkString(2) + ")"; // prev wedge
	  
	    }
      sepIndexPrev = sepIndex;
      System.addCell(MonteCarlo::Qhull(cellIndex++,MatInfo.first,MatInfo.second,Out));
    }

  //  attachSystem::addToInsertSurfCtrl(System,*this,*FLWedge);

  return;
}
  

void
WedgedFlightLine::createAll(Simulation& System,
			   const attachSystem::FixedComp& originFC,
			   const long int originIndex,
			   const attachSystem::FixedComp& innerFC,
			   const long int innerIndex,
			   const attachSystem::FixedComp& outerFC,
			   const long int outerIndex)
  /*!
    Global creation of the basic flight line connecting two
    objects
    \param System :: Simulation to add vessel to
    \param originFC :: Origin
    \param originIndex :: Use side index from Origin
    \param innerFC :: Moderator Object
    \param innerIndex :: Use side index from moderator
    \param outerFC :: Edge of bulk shield 
    \param outerIndex :: Side index of bulk shield

  */
{
  ELog::RegMethod RegA("WedgedFlightLine","createAll");

  moderatorSystem::TaperedFlightLine::createAll(System,
						originFC, originIndex,
						innerFC, innerIndex,
						outerFC, outerIndex);
  populate(System.getDataBase());
  buildWedges(System, originFC, originIndex, innerFC, innerIndex,
	      outerFC, outerIndex);
    
  return;
}



  
}  // NAMESPACE essSystem
