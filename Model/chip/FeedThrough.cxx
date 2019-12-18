/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chip/FeedThrough.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <boost/multi_array.hpp>

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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "SimProcess.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "boxValues.h"
#include "boxUnit.h"
#include "BoxLine.h"
#include "HoleUnit.h"
#include "chipIRHutch.h"
#include "FeedThrough.h"

#include "debugMethod.h"

namespace hutchSystem
{

FeedThrough::FeedThrough(const std::string& Key,
			 const size_t Index)  :
  attachSystem::FixedUnit(Key+std::to_string(Index),0),
  baseName(Key),ID(Index),
  CollTrack(Key+std::to_string(Index))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

FeedThrough::FeedThrough(const FeedThrough& A) : 
  attachSystem::FixedUnit(A),baseName(A.baseName),ID(A.ID),
  CollTrack(A.CollTrack),height(A.height),
  width(A.width),Offset(A.Offset),CPts(A.CPts)
  /*!
    Copy constructor
    \param A :: FeedThrough to copy
  */
{}
  
FeedThrough&
FeedThrough::operator=(const FeedThrough& A)
  /*!
    Assignment operator
    \param A :: FeedThrough to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      CollTrack=A.CollTrack;
      height=A.height;
      width=A.width;
      Offset=A.Offset;
      CPts=A.CPts;
    }
  return *this;
}
  
FeedThrough::~FeedThrough() 
  /*!
    Destructor
  */
{}
  
void
FeedThrough::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Data base to use
  */
{
  ELog::RegMethod RegA("FeedThrough","populate");
  

  Offset=Control.EvalTail<Geometry::Vec3D>(keyName,baseName,"Offset");
  height=Control.EvalTail<double>(keyName,baseName,"Height");
  width=Control.EvalTail<double>(keyName,baseName,"Width");

  size_t index(0);
  CPts.clear();
  while(Control.hasVariable(keyName+"Track"+std::to_string(index)))
    {
      CPts.emplace_back(Control.EvalVar<Geometry::Vec3D>
		       (keyName+"Track"+std::to_string(index)));
      index++;
    }
  if (!index)
    {
      while(Control.hasVariable(baseName+"Track"+std::to_string(index)))
	{
	  CPts.emplace_back(Control.EvalVar<Geometry::Vec3D>
			    (baseName+"Track"+std::to_string(index)));
	  index++;
	}
    }

  if (!index)
    ColErr::EmptyContainer(keyName+" CPts::TrackPts");
  
  return;
}
    

void 
FeedThrough::insertColl(Simulation& System,
			const chipIRHutch& Hut)
  /*!
    Add a feed pipe to the collimator area
    \param System :: Simulation to add pipe to
    \param Hut :: Hutch item for outside surface intersect
  */
{
  ELog::RegMethod RegA("FeedThrough","insertColl");
  ELog::debugMethod DegA;


  Geometry::Vec3D LineOrg=Origin+
    X*Offset[0]+Y*Offset[1]+Z*Offset[2];

  const Geometry::Vec3D OP=Hut.calcSideIntercept(3,LineOrg,X);
  const Geometry::Vec3D XDir=Hut.calcSurfNormal(3);
  const Geometry::Vec3D YDir=XDir*Z;
  CollTrack.addPoint(OP-XDir);

  std::vector<Geometry::Vec3D >::const_iterator vc;
  for(vc=CPts.begin();vc!=CPts.end();vc++)
    {
      const Geometry::Vec3D& trackPt(*vc);
      Geometry::Vec3D realPoint=
	XDir*trackPt[0]+YDir*trackPt[1]+Z*trackPt[2];
      realPoint+=OP;
      CollTrack.addPoint(realPoint);
    }
        
  // make void
  CollTrack.addSection(width,height,0,0.0);
  CollTrack.setInitZAxis(Z);
  DegA.activate();
  CollTrack.build(System);

  return;
}
  
void
FeedThrough::build(Simulation& System,
		   const chipIRHutch& HutUnit)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param HutUnit :: Hutch point
  */
{
  ELog::RegMethod RegA("FeedThrough","createAll");

  return;
  populate(System.getDataBase());
  createUnitVector(HutUnit,0);
  insertColl(System,HutUnit); 

  //  insertPipes(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
