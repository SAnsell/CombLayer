/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/AttachSupport.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "support.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Rules.h"
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
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SurInter.h"
#include "AttachSupport.h"



namespace attachSystem
{

long int
getLinkNumber(const std::string& Name)
  /*!
    Get a link number. Input is either a name 
    - Origin : Origin [0]
    - front : link point 1
    - back : link point 2 
    \param Name :: Link name / number 
    \return signed link number
  */
{
  const ELog::RegMethod RegA("AttachSupport[F]","getLinkNumber");
  
  long int linkPt(0);
  
  if (!Name.empty() && !StrFunc::convert(Name,linkPt))
    {
      if (Name=="origin") 
	linkPt=0;
      else if (Name=="front") 
	linkPt=1;
      else if (Name=="back")
	linkPt=2;
      else if (Name=="front-") 
	linkPt=-1;
      else if (Name=="back-")
	linkPt=-2;
      else 
	throw ColErr::InContainerError<std::string>(Name,"Name");
    }
  return linkPt;
}
  
  
void
addUnion(const MonteCarlo::Object& Obj,Rule*& outRule)
  /*!
    Given an Rule: intersect into it another object
    \param Obj :: Object add 
    \param outRule :: Rule to modify
  */
{
  const ELog::RegMethod RegA("AttachSupport[F]","addUnion<Obj,Rule>");

  createAddition(-1,Obj.topRule()->clone(),outRule);
  return;
}

void
addUnion(const int SN,const Geometry::Surface* SPtr,
	 Rule*& outRule)
  /*!
    Given an Rule: uninio into it another rule
    \param SN :: Rule number
    \param SPtr :: Surface to add at point
    \param outRule :: Rule to modify
  */
{
  const ELog::RegMethod RegA("AttachSupport[F]","addUnion<int,Rule>");
  createAddition(-1,new SurfPoint(SPtr,SN),outRule);  
  return;
}

void
addIntersection(const MonteCarlo::Object& Obj,Rule*& outRule)
  /*!
    Given an Rule: intersect into it another object
    \param Obj :: Object add 
    \param outRule :: Rule to modify
  */
{
  const ELog::RegMethod RegA("AttachSupport[F]","addIntersection<Obj,Rule>");

  createAddition(1,Obj.topRule()->clone(),outRule);
  return;
}

void
addIntersection(const int SN,const Geometry::Surface* SPtr,
		Rule*& outRule)
  /*!
    Given an Rule: intersect into it another object
    \param SN :: Surface number [signed]
    \param SPtr :: Surface Pointer
    \param outRule :: Rule to modify
  */
{
  const ELog::RegMethod RegA("AttachSupport[F]","addIntersection<int,Rule>");

  createAddition(1,new SurfPoint(SPtr,SN),outRule);  
  return;
}

void
createAddition(const int InterFlag,Rule* NRptr,
	       Rule*& outRule)
  /*!
    Function to actually do the addition of a rule to 
    avoid code repeat.
    \param InterFlag :: Intersection / Union [ 1 : -1 ] : 0 for new 
    \param NRptr :: New Rule pointer to add
    \param outRule :: Rule system to modify
   */
{
  const ELog::RegMethod RegA("AttachSupport","createAddition");

  // Null object : Set and return
  if (!outRule || !InterFlag)
    {
      delete outRule;
      outRule=NRptr;
      return;
    }
  // This is an intersection and we want to add our rule at the base
  // Find first item that is not an intersection
  Rule* RPtr;
  
  std::deque<Rule*> curLevel;
  curLevel.push_back(outRule);
  while(!curLevel.empty())
    {
      RPtr=curLevel.front();
      curLevel.pop_front();
      if (RPtr->type() != InterFlag)  // Success not an intersection
	{
	  Rule* parent=RPtr->getParent();     // grandparent
	  Rule* sA=RPtr;
	  Rule* Item(0);
	  if (InterFlag==1)
	    Item=new Intersection(parent,sA,NRptr);
	  else
	    Item=new Union(parent,sA,NRptr);
	  // Find place ot insert it
	  if (!parent)
	    outRule=Item;
	  else
	    parent->setLeaf(Item,parent->findLeaf(RPtr));

	  return;
	}
      Rule* LeafPtr;
      if ( (LeafPtr=RPtr->leaf(0)) )
	curLevel.push_back(LeafPtr);
      if ( (LeafPtr=RPtr->leaf(1)) )
	curLevel.push_back(LeafPtr);
    }

  throw ColErr::EmptyContainer
    ("Failed on rule structure :"+outRule->display());

  return;  
}

void
addToInsertControl(Simulation& System,
		   const attachSystem::FixedComp& OuterFC,
		   const attachSystem::FixedComp& InsertFC,
		   const std::string& groupName)
  /*!
    Adds this object to the containedComp to be inserted.
    FC is the fixed object that is to be inserted -- linkpoints
    must be set. It is tested against all the ojbect with
    this object .
    \param System :: Simulation to use
    \param OuterFC :: Object into which we are going to insert
    \param InsertFC :: FixedComp with the points
    \param groupName :: Contained Group to use
  */
{
  const ELog::RegMethod RegA("AttachSupport","addToInsertControl(FC,FC,string)");
  

  const std::string outerName=OuterFC.getKeyName();
  const std::set<int> cellVec=System.getObjectRange(outerName);

  attachSystem::ContainedGroup* CGPtr=
    System.getObjectThrow<attachSystem::ContainedGroup>
    (InsertFC.getKeyName(),"ContainedGroup");

  addToInsertControl(System,cellVec,InsertFC,
		     CGPtr->getCC(groupName));

  return;
}

void
addToInsertControl(Simulation& System,
		   const attachSystem::FixedComp& OuterFC,
		   const attachSystem::FixedComp& InsertFC)
/*!
  Adds this object to the containedComp to be inserted.
  FC is the fixed object that is to be inserted -- linkpoints
  must be set. It is tested against all the ojbect with
  this object .
  \param System :: Simulation to use
  \param OuterFC :: Object into which we are going to insert
  \param InsertFC :: FixedComp with the points
*/
{
  const ELog::RegMethod RegA("AttachSupport","addToInsertControl(FC,FC)");

  const std::string outerName=OuterFC.getKeyName();

  const std::set<int> cellVec=System.getObjectRange(outerName);
  attachSystem::ContainedComp* CCPtr=
    System.getObjectThrow<attachSystem::ContainedComp>
    (InsertFC.getKeyName(),"ContainedComp");
  addToInsertControl(System,cellVec,InsertFC,*CCPtr);

  return;
}

void
addToInsertControl(Simulation& System,
		   const std::string& OName,
		   const attachSystem::FixedComp& FC,
		   attachSystem::ContainedComp& CC)
/*!
  Adds this object to the containedComp to be inserted.
  FC is the fixed object that is to be inserted -- linkpoints
  must be set. It is tested against all the ojbect with
  this object .
  \param System :: Simulation to use
  \param OName :: Name of object to insert
  \param FC :: FixedComp with the points
  \param CC :: ContainedComp object to add to this
*/
{
  const ELog::RegMethod RegA("AttachSupport","addToInsertControl(string,FC,CC)");

  const std::set<int> cellVec=System.getObjectRange(OName);
  addToInsertControl(System,cellVec,FC,CC);

  return;
}

void
addToInsertControl(Simulation& System,
                   const CellMap& BaseObj,
                   const std::string& cellName,
                   const attachSystem::FixedComp& FC,
                   const attachSystem::ContainedComp& CC)
  /*!
    Insert the object if any of the link points are within
    the cells provided by BaseObj(cellName).
    \param System :: Simulation to use
    \param BaseObj :: CellMap key name
    \param cellName :: subname of cell from CellMap
    \param FC :: FixedComp with the points
    \param CC :: ContainedComp 
  */
{
  const ELog::RegMethod RegA("attachSuport[F]",
			     "addToInsertControl(CM,string,FC,CC)");

  const std::vector<Geometry::Vec3D> linkPts=FC.getAllLinkPts();

  const HeadRule& excludeHR=CC.getOuterSurf().complement();

  for(const int cN : BaseObj.getCells(cellName))
    {
      MonteCarlo::Object* CRPtr=System.findObject(cN);
      if (CRPtr)
	{
	  CRPtr->populate();
	  for(const Geometry::Vec3D& IP : linkPts)	  
	    {
	      if (CRPtr->isValid(IP))
		{
		  CRPtr->addIntersection(excludeHR);
		  break;
		}
	    }
	}
    }
  return;
}
                   
  
void
addToInsertControl(Simulation& System,
		   const std::set<int>& cellVec,
		   const attachSystem::FixedComp& FC,
		   const attachSystem::ContainedComp& CC)
  /*!
    Adds this object to the containedComp to be inserted.
    FC is the fixed object that is to be inserted -- linkpoints
    must be set. It is tested against all the ojbect with
    this object .
    \param System :: Simulation to use
    \param cellVec :: Vector of cells to test
    \param FC :: FixedComp with the points
    \param CC :: ContainedComp object to add to this
  */
{
  const ELog::RegMethod RegA("AttachSupport","addToInsertControl");

  const std::vector<Geometry::Vec3D> linkPts=FC.getAllLinkPts();
  const HeadRule excludeHR=CC.getOuterSurf().complement();

  for(const int CN : cellVec)
    {
      MonteCarlo::Object* CRPtr=System.findObject(CN);
      if (CRPtr)
	{
	  CRPtr->populate();
	  for(const Geometry::Vec3D& IP : linkPts)	  
	    {
	      if (CRPtr->isValid(IP))
		{
		  CRPtr->addIntersection(excludeHR);
		  break;
		}
	    }
	}
    }
  return;
}

// SURFACE INTERSECT
void
addToInsertSurfCtrl(Simulation& System,
		    const attachSystem::FixedComp& BaseFC,
		    attachSystem::ContainedComp& CC)
  /*!
    Adds this object to the containedComp to be inserted.
    FC is the fixed object that is to be inserted -- linkpoints
    must be set. It is tested against all the ojbect with
    this object .
    \param System :: Simulation to use
    \param BaseFC :: FixedComp for name
    \param CC :: ContainedComp object to add to this
  */
{
  const ELog::RegMethod RegA("AttachSupport","addToInsertSurfCtrl(FC,CC)");
  
  const std::set<int> cellVec=
    System.getObjectRange(BaseFC.getKeyName());
  addToInsertSurfCtrl(System,cellVec,CC);

  return;
}

void
addToInsertSurfCtrl(Simulation& System,
		    const attachSystem::CellMap& BaseCell,
		    const std::string& cellName,
		    attachSystem::ContainedComp& CC)
  /*!
    Adds this object to the containedComp to be inserted.
    FC is the fixed object that is to be inserted -- linkpoints
    must be set. It is tested against all the ojbect with
    this object .
    \param System :: Simulation to use
    \param BaseCell :: CellMap for cell numbers
    \param cellName :: cell item from CellMap
    \param CC :: ContainedComp object to add to this
  */
{
  ELog::RegMethod RegA("AttachSupport","addToInsertSurfCtrl(cellmap,CC)");
  
  const std::vector<int> cellVec=
    BaseCell.getCells(cellName);

  addToInsertSurfCtrl(System,cellVec,CC);
  return;
}


void
addToInsertSurfCtrl(Simulation& System,
		    const int cellA,
		    attachSystem::ContainedComp& CC)
 /*!
   Adds this object to the containedComp to be inserted.
   FC is the fixed object that is to be inserted -- linkpoints
   must be set. It is tested against all the ojbect with
   this object .
   \param System :: Simulation to use
   \param cellA :: cell number [to test]
   \param CC :: ContainedComp object to add to this
  */
{
  ELog::RegMethod RegA("AttachSupport","addToInsertSurfCtrl(int,int,CC)");

  MonteCarlo::Object* CRPtr=System.findObject(cellA);
  if (!CRPtr) return;
  
  CRPtr->populate();
  CRPtr->createSurfaceList();
  
  if (checkIntersect(CC,*CRPtr))
    CC.insertInCell(System,cellA);

  return;
}

void
addToInsertSurfCtrl(Simulation& System,
		    const std::vector<int>& cellVec,
		    attachSystem::ContainedComp& CC)
 /*!
   Adds this object to the containedComp to be inserted.
   FC is the fixed object that is to be inserted -- linkpoints
   must be set. It is tested against all the ojbect with
   this object .
   \param System :: Simulation to use
   \param cellVec :: CellVector / CellSet
   \param CC :: ContainedComp object to add to this
  */
{
  ELog::RegMethod RegA("AttachSupport","addToInsertSurfCtrl(int,vector,CC)");


  for(const int CN : cellVec)
    {
      MonteCarlo::Object* CRPtr=System.findObject(CN);
      if (CRPtr)
	{
	  CRPtr->populate();
	  CRPtr->createSurfaceList();

	  if (checkIntersect(CC,*CRPtr))
	    CC.insertInCell(System,CN);
	}
    }

  return;
}

void
addToInsertSurfCtrl(Simulation& System,
		    const std::set<int>& cellVec,
		    attachSystem::ContainedComp& CC)
 /*!
   Adds this object to the containedComp to be inserted.
   FC is the fixed object that is to be inserted -- linkpoints
   must be set. It is tested against all the ojbect with
   this object .
   \param System :: Simulation to use
   \param cellVec :: CellVector / CellSet
   \param CC :: ContainedComp object to add to this
  */
{
  ELog::RegMethod RegA("AttachSupport","addToInsertSurfCtrl(int,vector,CC)");

  for(const int CN : cellVec)
    {
      MonteCarlo::Object* CRPtr=System.findObject(CN);
      if (CRPtr)
	{
	  CRPtr->populate();
	  CRPtr->createSurfaceList();	  
	  if (checkIntersect(CC,*CRPtr))
	    CC.insertInCell(System,CN);
	}
    }
  CC.insertObjects(System);
  return;
}


bool
checkIntersect(const ContainedComp& CC,
	       const MonteCarlo::Object& CellObj)
   /*
     Determine if the surface group is in the Contained Component
     \param CC :: Contained Component
     \param CellObj :: Cell Object
     \return true/false
   */
{
  ELog::RegMethod RegA("AttachSupport","checkInsert");
  //  ELog::debugMethod DegA;

  const HeadRule& CCHR=CC.getOuterSurf();
  const std::set<const Geometry::Surface*>& CCsurfSet=
    CCHR.getSurfaces();
  const HeadRule& CellHR=CellObj.getHeadRule();
  const std::set<const Geometry::Surface*>& CellsurfSet=
    CellHR.getSurfaces();
    
  std::vector<Geometry::Vec3D> intersectPoints;

  std::set<const Geometry::Surface*>::const_iterator ac,bc,cc;
  // first check the cell object / surface intersection
  for(const Geometry::Surface* SPtr : CCsurfSet)
    {
      for(ac=CellsurfSet.begin();ac!=CellsurfSet.end();ac++)
	for(bc=CellsurfSet.begin();bc!=ac;bc++)
	  {	      
	    intersectPoints=
	      SurInter::processPoint(SPtr,*ac,*bc);
	    for(const Geometry::Vec3D& testPoint : intersectPoints)
	      {
		if (CellHR.isValid(testPoint) &&
		    CCHR.isValid(testPoint))
		  return 1;
	      }
	  }
    }
  // Test surface/surface/surface intersect
    for(ac=CCsurfSet.begin();ac!=CCsurfSet.end();ac++)
      for(bc=CCsurfSet.begin();bc!=ac;bc++)
	for(cc=CCsurfSet.begin();cc!=bc;cc++)
	  {
	    intersectPoints=
	      SurInter::processPoint(*ac,*bc,*cc);
	    for(const Geometry::Vec3D& testPoint : intersectPoints)
	      {
		if (CellObj.isValid(testPoint))
		  return 1;
	      }
	  }
    
    for(ac=CCsurfSet.begin();ac!=CCsurfSet.end();ac++)
      for(bc=CCsurfSet.begin();bc!=ac;bc++)
	for(const Geometry::Surface* SPtr : CellsurfSet)
	  {
	    intersectPoints=
	      SurInter::processPoint(*ac,*bc,SPtr);
	    for(const Geometry::Vec3D& testPoint : intersectPoints)
	      {
		if (CellHR.isValid(testPoint);
		    CCHR.isValid(testPoint))
		{
		  return 1;
		}
	    }
	}
  // All failed:
  return 0;
}

bool
checkPlaneIntersect(const Geometry::Plane& BPlane,
		    const MonteCarlo::Object& AObj,
		    const MonteCarlo::Object& BObj)
/*!
  Determine if the surface group intersects the CellObj based
  on the base. 
  The test is (a) interesection point valid in both object
  (b) the intersection point is on the surface of both object.
  \param BPlane :: Base plane
  \param AObj :: First object
  \param BObj :: Second object
  \return true/false
*/
{
  ELog::RegMethod RegA("AttachSupport","checkPlaneIntersect");

  const HeadRule& AObjHR=AObj.getHeadRule();
  const HeadRule& BObjHR=BObj.getHeadRule();
  const std::set<const Geometry::Surface*>& ASSet=
    AObjHR.getSurfaces(); 
  const std::set<const Geometry::Surface*>& BSSet=
    BObjHR.getSurfaces(); 
  
  std::vector<Geometry::Vec3D> intersectPoints;

  for(const Geometry::Surface* ASPtr : ASSet)
    for(const Geometry::Surface* BSPtr : BSSet)
      {	      
	intersectPoints=
	  SurInter::processPoint(&BPlane,ASPtr,BSPtr);
	const int aSN(ASPtr->getName());
	const int bSN(BSPtr->getName());
	
	for(const Geometry::Vec3D& testPoint : intersectPoints)
	  {
	    const bool aMinus=AObjHR.isValid(testPoint,-aSN);
	    const bool aPlus=AObjHR.isValid(testPoint,aSN);
	    const bool bMinus=BObjHR.isValid(testPoint,-bSN);
	    const bool bPlus=BObjHR.isValid(testPoint,bSN);
	    if (aMinus!=aPlus && bMinus!=bPlus)
	      return 1;
	  }
      }
  return 0;
}


void
addToInsertForced(Simulation& System,
		  const std::set<int>& cellVec,
		  attachSystem::ContainedComp& CC)
 /*!
   Force CC into the BaseFC objects
   \param System :: Simulation to use
   \param cellA :: First cell to use
   \param cellB :: last cell to use
   \param CC :: ContainedComp object to add to the BaseFC
 */
{
  ELog::RegMethod RegA("AttachSupport","addToInsertForce(int,int,CC)");

  for(const int CN : cellVec)
    {
      MonteCarlo::Object* CRPtr=System.findObject(CN);
      if (CRPtr)
	{
	  CRPtr->populate();
	  CRPtr->createSurfaceList();
	  CC.addInsertCell(CN);
	}
    }

  CC.insertObjects(System);

  return;
}  

void
addToInsertForced(Simulation& System,
		  const std::set<int>& cellVec,
		  attachSystem::ContainedGroup& CG)
 /*!
   Force CC into the BaseFC objects
   \param System :: Simulation to use
   \param cellA :: First cell to use
   \param cellB :: last cell to use
   \param CC :: ContainedComp object to add to the BaseFC
 */
{
  ELog::RegMethod RegA("AttachSupport","addToInsertForce(int,int,CG)");

  const HeadRule excludeHR(CG.getAllExclude());
  for(const int CN : cellVec)
    {
      MonteCarlo::Object* CRPtr=System.findObject(CN);
      if (CRPtr)
	{
	  CRPtr->populate();
	  CRPtr->createSurfaceList();
	  CRPtr->addIntersection(excludeHR);
	}
    }
  return;
}  


void
addToInsertForced(Simulation& System,
		  const attachSystem::FixedComp& BaseFC,
		  attachSystem::ContainedComp& CC)
 /*!
   Force CC into the BaseFC objects
  \param System :: Simulation to use
  \param BaseFC :: Object to get range for cells
  \param CC :: ContainedComp object to add to the BaseFC
 */
{
  ELog::RegMethod RegA("AttachSupport","addToInsertForced(FC,CC)");

  const std::set<int> cellVec=
    System.getObjectRange(BaseFC.getKeyName());
  addToInsertForced(System,cellVec,CC);
  return;
}  

void
addToInsertForced(Simulation& System,
		  const attachSystem::FixedComp& BaseFC,
		  attachSystem::ContainedGroup& CG)
 /*!
   Force CC into the BaseFC objects
  \param System :: Simulation to use
  \param BaseFC :: Object to get range for cells
  \param CG :: ContainedComp object to add to the BaseFC
 */
{
  ELog::RegMethod RegA("AttachSupport","addToInsertForced(FC,CC)");

  const std::set<int> cellVec=
    System.getObjectRange(BaseFC.getKeyName());
  addToInsertForced(System,cellVec,CG);
  return;
}  


HeadRule
unionLink(const attachSystem::FixedComp& FC,
	  const std::vector<long int>& LIndex)
  /*!
    Simple way to combined a number of link units into
    a HeadRule to export
    \param FC :: FixedComp unit
    \param LIndex :: List of signed link surfaces
    \return HeadRule of union join
   */
{
  ELog::RegMethod RegA("AttachSupport[F]","unionLink");

  HeadRule HR;

  for(const long int LI : LIndex)
    HR.addUnion(FC.getFullRule(LI));


  return HR;
}

HeadRule
intersectionLink(const attachSystem::FixedComp& FC,
		 const std::vector<long int>& LIndex)
  /*!
    Simple way to combined a number of link units into
    a HeadRule to export
    \param FC :: FixedComp unit
    \param LIndex :: List of signed link surfaces
    \return HeadRule of union join
   */
{
  const ELog::RegMethod RegA("AttachSupport[F]","intersectionLink");

  HeadRule HR;

  for(const long int LI : LIndex)
    HR.addIntersection(FC.getFullRule(LI));

  return HR;
}


}  // NAMESPACE attachSystem
