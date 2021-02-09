/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/AttachSupport.cxx
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
  ELog::RegMethod RegA("AttachSupport[F]","getLinkNumber");
  
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
  ELog::RegMethod RegA("AttachSupport[F]","addUnion<Obj,Rule>");

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
  ELog::RegMethod RegA("AttachSupport[F]","addUnion<int,Rule>");
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
  ELog::RegMethod RegA("AttachSupport[F]","addIntersection<Obj,Rule>");

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
  ELog::RegMethod RegA("AttachSupport[F]","addIntersection<int,Rule>");

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
  ELog::RegMethod RegA("AttachSupport","createAddition");

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
  ELog::RegMethod RegA("AttachSupport","addToInsertControl(FC,FC,string)");
  

  const std::string outerName=OuterFC.getKeyName();
  const std::vector<int> cellVec=System.getObjectRange(outerName);

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
  ELog::RegMethod RegA("AttachSupport","addToInsertControl(FC,FC)");

  const std::string outerName=OuterFC.getKeyName();

  const std::vector<int> cellVec=System.getObjectRange(outerName);
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
  ELog::RegMethod RegA("AttachSupport","addToInsertControl(string,FC,CC)");

  const std::vector<int> cellVec=System.getObjectRange(OName);
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
  ELog::RegMethod RegA("attachSuport[F]",
                       "addToInsertControl(CM,string,FC,CC)");

  const std::vector<Geometry::Vec3D> linkPts=FC.getAllLinkPts();
  const std::string excludeStr=CC.getExclude();

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
		  CRPtr->addSurfString(excludeStr);
		  break;
		}
	    }
	}
    }
  return;
}
                   
  
void
addToInsertControl(Simulation& System,
		   const std::vector<int>& cellVec,
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
  ELog::RegMethod RegA("AttachSupport","addToInsertControl");

  const std::vector<Geometry::Vec3D> linkPts=FC.getAllLinkPts();
  const std::string excludeStr=CC.getExclude();

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
		  CRPtr->addSurfString(excludeStr);
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
  ELog::RegMethod RegA("AttachSupport","addToInsertSurfCtrl(FC,CC)");
  
  const std::vector<int> cellVec=
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
addToInsertOuterSurfCtrl(Simulation& System,
			 const attachSystem::FixedComp& BaseFC,
			 attachSystem::ContainedComp& CC)
  /*!
    Adds this object to the containedComp to be inserted.
    FC is the fixed object that is to be inserted -- linkpoints
    must be set. It is tested against all the ojbect with
    this object .
    \param System :: Simulation to use
    \param BaseFC :: FixedComp for name / and CC
    \param CC :: ContainedComp object to add to this
  */
{
  ELog::RegMethod RegA("AttachSupport","addToInsertOuterSurfCtrl(FC,CC)");
  
  const attachSystem::ContainedComp* BaseCC=
    System.getObjectThrow<attachSystem::ContainedComp>
    (BaseFC.getKeyName(),"ContainedComp");
  const std::vector<int> cellVec=
    System.getObjectRange(BaseFC.getKeyName());

  addToInsertOuterSurfCtrl(System,cellVec,*BaseCC,CC);

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

  const std::vector<Geometry::Surface*> SVec=CC.getSurfaces();

  MonteCarlo::Object* CRPtr=System.findObject(cellA);
  if (!CRPtr) return;
  
  CRPtr->populate();
  CRPtr->createSurfaceList();
  const std::vector<const Geometry::Surface*>&
    CellSVec=CRPtr->getSurList();
  
  if (checkIntersect(CC,*CRPtr,CellSVec))
    CC.addInsertCell(cellA);

  CC.insertObjects(System);
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
   \param cellVec :: CellVector
   \param CC :: ContainedComp object to add to this
  */
{
  ELog::RegMethod RegA("AttachSupport","addToInsertSurfCtrl(int,int,CC)");

  const std::vector<Geometry::Surface*> SVec=CC.getSurfaces();

  for(const int CN : cellVec)
    {
      MonteCarlo::Object* CRPtr=System.findObject(CN);
      if (CRPtr)
	{
	  CRPtr->populate();
	  CRPtr->createSurfaceList();
	  const std::vector<const Geometry::Surface*>&
	    CellSVec=CRPtr->getSurList();
	  
	  if (checkIntersect(CC,*CRPtr,CellSVec))
	    CC.addInsertCell(CN);
	}
    }
  CC.insertObjects(System);
  return;
}

void
addToInsertOuterSurfCtrl(Simulation& System,
			 const std::vector<int>& cellVec,
			 const attachSystem::ContainedComp& BaseCC,
			 attachSystem::ContainedComp& CC)
 /*!
   Adds this object to the containedComp to be inserted.
   FC is the fixed object that is to be inserted -- linkpoints
   must be set. It is tested against all the object with
   this object .
   \param System :: Simulation to use
   \param BaseCC :: Only search using the base Contained Comp
   \param cellVec :: Range of cells [to test]
   \param BaseCC :: ContainedComp object use as the dermination cell
   \param CC :: ContainedComp object to add to this
  */
{
  ELog::RegMethod RegA("AttachSupport","addToInsertSurfCtrl(vec,CC)");

  const std::vector<Geometry::Surface*> SVec=CC.getSurfaces();


  // Populate and createSurface list MUST have been called      
  const std::vector<const Geometry::Surface*>
    CellSVec=BaseCC.getConstSurfaces();

  for(const int CN : cellVec)
    {
      MonteCarlo::Object* CRPtr=System.findObject(CN);
      if (CRPtr && checkIntersect(CC,*CRPtr,CellSVec))
	CC.addInsertCell(CN);
    }

  CC.insertObjects(System);
  return;
}

bool
checkIntersect(const ContainedComp& CC,const MonteCarlo::Object& CellObj,
	       const std::vector<const Geometry::Surface*>& CellSVec)
   /*
     Determine if the surface group is in the Contained Component
     \param CC :: Contained Component
     \param CellObj :: Cell Object
     \param CellSVec :: Cell vector
     \return true/false
   */
{
  ELog::RegMethod RegA("AttachSupport","checkInsert");
  //  ELog::debugMethod DegA;
  const std::vector<Geometry::Surface*>& SVec=CC.getSurfaces(); 
  std::vector<Geometry::Vec3D> Out;
  std::vector<Geometry::Vec3D>::const_iterator vc;

  for(size_t iA=0;iA<SVec.size();iA++)
    for(size_t iB=0;iB<CellSVec.size();iB++)
      for(size_t iC=iB+1;iC<CellSVec.size();iC++)
	{	      
	  Out=SurInter::processPoint(SVec[iA],CellSVec[iB],CellSVec[iC]);
	  for(vc=Out.begin();vc!=Out.end();vc++)
	    {
	      std::set<int> boundarySet;
	      boundarySet.insert(CellSVec[iB]->getName());
	      boundarySet.insert(CellSVec[iC]->getName());		  
	      // Outer valid returns true if out of object
	      if (CellObj.isValid(*vc,boundarySet) &&
		  !CC.isOuterValid(*vc,SVec[iA]->getName()))
		return 1;
	    }
	}
  for(size_t iA=0;iA<SVec.size();iA++)
    for(size_t iB=iA+1;iB<SVec.size();iB++)
      for(size_t iC=iB+1;iC<SVec.size();iC++)
	{
	  Out=SurInter::processPoint(SVec[iA],SVec[iB],SVec[iC]);
	  for(vc=Out.begin();vc!=Out.end();vc++)
	    {
	      if (CellObj.isValid(*vc))
		return 1;
	    }
	}
  for(size_t iA=0;iA<SVec.size();iA++)
    for(size_t iB=iA+1;iB<SVec.size();iB++)
      for(size_t iC=0;iC<CellSVec.size();iC++)
	{
	  Out=SurInter::processPoint(SVec[iA],SVec[iB],CellSVec[iC]);
	  for(vc=Out.begin();vc!=Out.end();vc++)
	    {
	      std::set<int> boundarySet;
	      boundarySet.insert(SVec[iA]->getName());
	      boundarySet.insert(SVec[iB]->getName());
	      if (CellObj.isValid(*vc,CellSVec[iC]->getName()) &&
		  !CC.isOuterValid(*vc,boundarySet))
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

  const std::vector<const Geometry::Surface*>& ASVec=AObj.getSurList(); 
  const std::vector<const Geometry::Surface*>& BSVec=BObj.getSurList(); 

  std::vector<Geometry::Vec3D> Out;
  std::vector<Geometry::Vec3D>::const_iterator vc;

  for(size_t iA=0;iA<ASVec.size();iA++)
    for(size_t iB=0;iB<BSVec.size();iB++)
      {	      
	Out=SurInter::processPoint(&BPlane,ASVec[iA],BSVec[iB]);
	  for(vc=Out.begin();vc!=Out.end();vc++)
	    {
	      if (BObj.isValid(*vc,BSVec[iB]->getName()) &&
		  AObj.isValid(*vc,ASVec[iA]->getName()) )
		{
		  if (BObj.isDirectionValid(*vc,BSVec[iB]->getName()) !=
		      BObj.isDirectionValid(*vc,-BSVec[iB]->getName()) && 
		      AObj.isDirectionValid(*vc,ASVec[iA]->getName()) !=
		      AObj.isDirectionValid(*vc,-ASVec[iA]->getName()) ) 
		    return 1;
		}
	    }
	}
  return 0;
}

bool
findPlaneIntersect(const Geometry::Plane& BPlane,
		   const MonteCarlo::Object& AObj,
		   const MonteCarlo::Object& BObj,
		   std::vector<int>& ASurf,
		   std::vector<int>& BSurf)
/*!
  Determine if the surface group intersects the CellObj based
  on the base
  \param BPlane :: Base plane
  \param AObj :: First object
  \param BObj :: Second object
  \param ASurf :: AObj surf intersect pair
  \param BSurf :: BObj surf intersect pair
  \return true/false
*/
{
  ELog::RegMethod RegA("AttachSupport","findPlaneIntersect");

  const std::vector<const Geometry::Surface*>& ASVec=AObj.getSurList(); 
  const std::vector<const Geometry::Surface*>& BSVec=BObj.getSurList(); 

  std::vector<Geometry::Vec3D> Out;
  std::vector<Geometry::Vec3D>::const_iterator vc;

  for(size_t iA=0;iA<ASVec.size();iA++)
    for(size_t iB=0;iB<BSVec.size();iB++)
      {	      
	Out=SurInter::processPoint(&BPlane,ASVec[iA],BSVec[iB]);
	  for(vc=Out.begin();vc!=Out.end();vc++)
	    {
	      if (BObj.isValid(*vc,BSVec[iB]->getName()) &&
		  AObj.isValid(*vc,ASVec[iA]->getName()) )
		{
		  ASurf.push_back(ASVec[iA]->getName());
		  BSurf.push_back(BSVec[iB]->getName());
		}
	    }
      }
  
  return (ASurf.empty()) ? 0 : 1;
}

void
addToInsertForced(Simulation& System,
		  const std::vector<int>& cellVec,
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
		  const std::vector<int>& cellVec,
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

  const HeadRule HR(CG.getAllExclude());
  for(const int CN : cellVec)
    {
      MonteCarlo::Object* CRPtr=System.findObject(CN);
      if (CRPtr)
	{
	  CRPtr->populate();
	  CRPtr->createSurfaceList();
	  CRPtr->addUnion(HR);
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

  const std::vector<int> cellVec=
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

  const std::vector<int> cellVec=
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

  HeadRule Out;

  for(const long int LI : LIndex)
    Out.addUnion(FC.getLinkString(LI));


  return Out;
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
  ELog::RegMethod RegA("AttachSupport[F]","intersectionLink");
  HeadRule Out;

  for(const long int LI : LIndex)
    Out.addIntersection(FC.getLinkString(LI));

  return Out;
}
  
}  // NAMESPACE attachSystem
