/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/AttachSupportLine.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include <deque>
#include <string>
#include <algorithm>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "surfRegister.h"

#include "varList.h"
#include "Code.h"

#include "FuncDataBase.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "Vec3D.h"
#include "interPoint.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "LineUnit.h"
#include "LineTrack.h"
#include "AttachSupport.h"


///\file AttachSupportLine.cxx

namespace attachSystem
{

bool
checkLineIntersect(const FixedComp& InsertFC,
                   const MonteCarlo::Object& CellObj)
  /*!
    Calculate if any of the link points or the lines connecting
    the link points from the FixedComp, intersect the
    cell object.      
    \param InsertFC :: FixedComp to get intersect 
    \param CellObj :: Cell Object
    \return true if the line/points intersect
   */
{
  ELog::RegMethod RegA("AttachSupportLine[F]","checkLineInsert");

  const std::vector<Geometry::Vec3D> linkPts=
    InsertFC.getAllLinkPts();

  HeadRule HR=CellObj.getHeadRule();
  HR.populateSurf();
  for(const Geometry::Vec3D& IP : linkPts)
    {
      if (HR.isValid(IP))
	return 1;
    }

  // Check line intersection:
  const std::set<const Geometry::Surface*>& surfSet=
    HR.getSurfaces();

  for(const Geometry::Vec3D& IP : linkPts)
    {
      for(const Geometry::Vec3D& JP : linkPts)
        {
          Geometry::Vec3D UV=JP-IP;
          const double LLen=UV.makeUnit();
          if (LLen>Geometry::zeroTol)
            {
              MonteCarlo::LineIntersectVisit LI(IP,UV);
	      for(const Geometry::Surface* SPtr : surfSet)
                SPtr->acceptVisitor(LI);

	      
              const std::vector<Geometry::interPoint>&
		IPts(LI.getInterVec());
              for(const Geometry::interPoint& iPt : IPts)
                {
                  if ((iPt.D>0.0 && iPt.D<LLen) &&
                      CellObj.isValid(iPt.Pt))
		    return 1;
                }
	    }
        }
    }
  return 0;
}

  
void
addToInsertLineCtrl(Simulation& System,
		    const attachSystem::FixedComp& OuterFC,
		    const attachSystem::FixedComp& InsertFC)
  /*!
    Convert the InsertFC to a ContainedComp. Then check all
    lines connecting linkPt and linkPts from OuterFC to
    see if any intersect the InsertFC. 

    It is tested by 
      -- A link point is within the object
      -- A line between link points intersect the surface of CellObject
      
    It is tested by tracking lines from each link point
    to another link point and seeing if they intersect the 
    surfaces within the BaseFC object. Then the validity of
    the point is tested REGARDLESS of being in the CC, to 
    being in the BaseFC. If it is an insert is made
    \param System :: Simulation to use
    \param OuterFC :: FixedComp for name
    \param InsertFC :: FixedComp with a ContainedComp/containedGroup
    dynamics cast
  */
{
  ELog::RegMethod RegA("AttachSupport[F]","addToInsertLineCtrl(FC,FC)");
  
  const attachSystem::ContainedComp* CCPtr=
    dynamic_cast<const attachSystem::ContainedComp*>(&InsertFC);
  if (!CCPtr)
    {
      ELog::EM<<InsertFC.getKeyName()<<" does not have CC component"
	      <<ELog::endErr;
      return;
    }
  addToInsertLineCtrl(System,OuterFC,InsertFC,*CCPtr);
  return;
}

void
addToInsertLineCtrl(Simulation& System,
                    const attachSystem::CellMap& BaseCell,
		    const std::string& cellName,
		    const attachSystem::FixedComp& InsertFC,
                    const attachSystem::ContainedComp& CC)
  /*!
    Adds this object to the containedComp to be inserted.
    CC is the fixed object that is to be inserted -- linkpoints
    must be set. It is tested by tracking lines from each link point
    to another link point and seeing if they intersect the 
    surfaces within the BaseFC object. Then the validity of
    the point is tested REGARDLESS of being in the CC, to 
    being in the BaseFC. If it is an insert is made
    \param System :: Simulation to use
    \param BaseCell :: CellMap to extract cells for testing
    \param cellName :: Particular cells to use from BaseCell
    \param InsertFC :: FixedComp with a ContainedComp/containedGroup
    dynamics cast
    \param CC :: Container for insersion
  */
{
  ELog::RegMethod RegA("AttachSupport[F]","addtoInsectLineCtrl(insertFC,CC)");

  for(const int cn : BaseCell.getCells(cellName))
    {
      addToInsertLineCtrl(System,InsertFC,CC,cn);
    }
  return;
}

void
addToInsertLineCtrl(Simulation& System,
		    const attachSystem::FixedComp& OuterFC,
		    const attachSystem::FixedComp& InsertFC,
                    const attachSystem::ContainedComp& CC)
  /*!
    Adds this object to the containedComp to be inserted.
    CC is the fixed object that is to be inserted -- linkpoints
    must be set. It is tested by tracking lines from each link point
    to another link point and seeing if they intersect the 
    surfaces within the BaseFC object. Then the validity of
    the point is tested REGARDLESS of being in the CC, to 
    being in the BaseFC. If it is an insert is made
    \param System :: Simulation to use
    \param OuterFC :: FixedComp to get name for cells
    \param InsertFC :: FixedComp for tracking etc 
    \param CC :: Container for insersion
  */
{
  ELog::RegMethod RegA("AttachSupport[F]","addtoInsectLineCtrl(FC,FC,CC)");

  const std::set<int> CNum=
    System.getObjectRange(OuterFC.getKeyName());

  for(const int CN : CNum)
    {
      addToInsertLineCtrl(System,InsertFC,CC,CN);
    }

  return;
}

void
addToInsertLineCtrl(Simulation& System,
		    const attachSystem::FixedComp& InsertFC,
                    const attachSystem::ContainedComp& CC,
                    const int cellN)
  /*!
    Adds this object to the containedComp to be inserted.

    CC is the ContainedComp that is to be inserted.
    It is tested by 
      -- A link point is within the object
      -- A line between link points intersect the surface of CellObject

    \param System :: Simulation to use
    \param InsertFC :: FixedComp to get linkpoint (inserting object)
    \param CC :: Contained comp of insertion (often InsertFC by cast)
    \param cellN :: Cell number of object to test
  */
{
  ELog::RegMethod RegA("AttachSupport[F]","addtoInsectLineCtrl(FC,CC,int)");

  MonteCarlo::Object* CRPtr=System.findObject(cellN);

  if (CRPtr && checkLineIntersect(InsertFC,*CRPtr))
    {
      const HeadRule excludeHR=CC.getOuterSurf().complement();
      CRPtr->addIntersection(excludeHR);
    }
  return;
}

void
lineIntersect(Simulation& System,
	      const attachSystem::FixedComp& FC,
	      std::map<int,MonteCarlo::Object*>& OMap)
  /*!
    For all the tracks between fixed points in the FC 
    find the cell in the model the tracks intersect.
    \param System :: Simualation to use
    \param FC :: Fixed Comp
    \param OMap :: Object map to add extra units to
  */
{
  ELog::RegMethod RegA("AttachSupportLine[F]","lineIntersect(Pt,Axis)");

  const long int NC(static_cast<long int>(FC.NConnect()));

  for(long int i=0;i<=NC;i++)
    {
      const Geometry::Vec3D APt(FC.getLinkPt(i));
      for(long int j=i+1;j<=NC;j++)
	{
	  const Geometry::Vec3D BPt(FC.getLinkPt(j));
	  lineIntersect(System,APt,BPt,OMap);
	}
    }
  return;
}
		    	  
  
void
lineIntersect(Simulation& System,
	      const Geometry::Vec3D& APt,
	      const Geometry::Vec3D& BPt,
	      std::map<int,MonteCarlo::Object*>& OMap)
  /*!
    For the line from APt to BPt Axis find all the intercepts
    in the model and add them to cells
    \param System :: Simualation to use
    \param APt :: Start Point
    \param BPt :: End Point
    \param OMap :: Object map to add extra units to
  */
{
  ELog::RegMethod RegA("AttachSupportLine[F]","lineIntersect(Vec3D)");

  // construct lines 

  ModelSupport::LineTrack LT(APt,BPt);
  LT.calculate(System);

  const std::vector<ModelSupport::LineUnit>&
    LUnits=LT.getTrackPts();
  for(const ModelSupport::LineUnit& lu : LUnits)
    {	  
      const int ONum=lu.cellNumber;
      if (ONum && OMap.find(ONum)==OMap.end())
	OMap.emplace(ONum,lu.objPtr);
    }
  return;
}

}  // NAMESPACE attachSystem
