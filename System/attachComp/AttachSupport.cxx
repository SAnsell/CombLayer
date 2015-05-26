/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/AttachSupport.cxx
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
#include <set>
#include <map>
#include <deque>
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "surfRegister.h"
#include "objectRegister.h"

#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "objectRegister.h"
#include "Qhull.h"
#include "Simulation.h"
#include "SurInter.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "AttachSupport.h"

#include "Debug.h"
#include "debugMethod.h"

namespace attachSystem
{

void createAddition(const int,Rule*,Rule*&);


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
    Given an Rule: intersect into it another object
    \param Obj :: Object add 
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
    \param NRPtr :: New Rule pointer to add
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
  Rule* RPtr(outRule);
  
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
  ELog::EM<<"Failed on rule structure  "<<ELog::endErr;
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
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const std::string outerName=OuterFC.getKeyName();
  const int cellN=OR.getCell(outerName);
  const int cellR=OR.getRange(outerName);
  attachSystem::ContainedGroup* CGPtr=
    OR.getObject<attachSystem::ContainedGroup>(InsertFC.getKeyName());
  
  if (!CGPtr)
    throw ColErr::CastError<void>(0,"Cannot convert "+
				  InsertFC.getKeyName()+"to containedGroup");

  addToInsertControl(System,cellN,cellN+cellR,InsertFC,
		     CGPtr->getKey(groupName));

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
  ELog::RegMethod RegA("AttachSupport","addToInsertControl");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const std::string outerName=OuterFC.getKeyName();
  const int cellN=OR.getCell(outerName);
  const int cellR=OR.getRange(outerName);
  attachSystem::ContainedComp* CCPtr=
    OR.getObject<attachSystem::ContainedComp>(InsertFC.getKeyName());
  if (!CCPtr)
    throw ColErr::CastError<void>(0,"Cannot convert "+
				  InsertFC.getKeyName()+"to contained Comp");
  addToInsertControl(System,cellN,cellN+cellR,InsertFC,*CCPtr);

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
  ELog::RegMethod RegA("AttachSupport","addToInsertControl");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  const int cellN=OR.getCell(OName);
  addToInsertControl(System,cellN,cellN+OR.getRange(OName),FC,CC);

  return;
}

void
addToInsertControl(Simulation& System,
		   const int cellA,const int cellB,
		   const attachSystem::FixedComp& FC,
		   const attachSystem::ContainedComp& CC)
/*!
    Adds this object to the containedComp to be inserted.
    FC is the fixed object that is to be inserted -- linkpoints
    must be set. It is tested against all the ojbect with
    this object .
    \param System :: Simulation to use
    \param CellA :: First cell number [to test]
    \param CellB :: Last cell number  [to test]
    \param FC :: FixedComp with the points
    \param CC :: ContainedComp object to add to this
  */
{
  ELog::RegMethod RegA("AttachSupport","addToInsertControl");

  const size_t NPoint=FC.NConnect();
  const std::string excludeStr=CC.getExclude();
  for(int i=cellA+1;i<=cellB;i++)
    {
      MonteCarlo::Qhull* CRPtr=System.findQhull(i);
      if (i==cellA+1 && !CRPtr)
	throw ColErr::InContainerError<int>(i,"Object not build");
      else if (!CRPtr)
	break;

      CRPtr->populate();
      for(size_t j=0;j<NPoint;j++)
	{
	  const Geometry::Vec3D& Pt=FC.getLinkPt(j);
	  if (CRPtr->isValid(Pt))
	    {
	      CRPtr->addSurfString(excludeStr);
	      break;
	    }
	}
    }
  return;
}

void
addToInsertLineCtrl(Simulation& System,
		    const attachSystem::FixedComp& OuterFC,
		    const attachSystem::FixedComp& InsertFC)
  /*!
    Adds this object to the containedComp to be inserted.
    CC is the fixed object that is to be inserted -- linkpoints
    must be set. It is tested by tracking lines from each link point
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
  ELog::RegMethod RegA("AttachSupport[F]","addtoInsectLineCtrl(FC,FC)");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // Determin cells to scan
  const int cellN=OR.getCell(OuterFC.getKeyName());
  const int cellR=OR.getRange(OuterFC.getKeyName());
  const attachSystem::ContainedComp* CCPtr=
    dynamic_cast<const attachSystem::ContainedComp*>(&InsertFC);
  if (!CCPtr)
    {
      ELog::EM<<InsertFC.getKeyName()<<" does not have CC component"
	      <<ELog::endErr;
      return;
    }

  // Calc and insert objects
  const size_t NPoint=InsertFC.NConnect();
  const std::string excludeStr=CCPtr->getExclude();

  for(int i=cellN+1;i<=cellN+cellR;i++)
    {
      MonteCarlo::Qhull* CRPtr=System.findQhull(i);
      if (i==cellN+1 && !CRPtr)
	throw ColErr::InContainerError<int>(i,"Object not build");
      else if (!CRPtr)
	break;
            
      CRPtr->populate();
      CRPtr->createSurfaceList();
      const std::vector<const Geometry::Surface*>& SurList=
	CRPtr->getSurList();

      // Check link points first:
      int cellInter(0);
      for(size_t j=0;!cellInter && j<NPoint;j++)
	{
	  const Geometry::Vec3D& IP=InsertFC.getLinkPt(j);
	  if (CRPtr->isValid(IP))
	    cellInter=1;
	}
      // Check line intersection:
      for(size_t j=0;!cellInter && j<NPoint;j++)
	{
	  const Geometry::Vec3D& IP=InsertFC.getLinkPt(j);
	  for(size_t k=j+1;!cellInter && k<NPoint;k++)
	    {
	      Geometry::Vec3D UV=InsertFC.getLinkPt(k)-IP;
	      const double LLen=UV.makeUnit();
	      if (LLen>Geometry::zeroTol)
		{
		  MonteCarlo::LineIntersectVisit LI(IP,UV);
		  std::vector<const Geometry::Surface*>::const_iterator vc;
		  for(vc=SurList.begin();vc!=SurList.end();vc++)
		    (*vc)->acceptVisitor(LI);
		  
		  const std::vector<double>& distVec(LI.getDistance());
		  const std::vector<Geometry::Vec3D>& dPts(LI.getPoints());
		  const std::vector<const Geometry::Surface*>& 
		    surfPts=LI.getSurfIndex();
		  
		  for(size_t dI=0;dI<dPts.size();dI++)
		    {
		      if ((distVec[dI]>0.0 && distVec[dI]<LLen) &&
			  CRPtr->isValid(dPts[dI],surfPts[dI]->getName()))
			{
			  cellInter=1;
			  break;
			}
		    }
		}
	    }
	}
      if (cellInter)
	{
	  CRPtr->addSurfString(excludeStr);
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
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  const int cellN=OR.getCell(BaseFC.getKeyName());
  const int cellR=OR.getRange(BaseFC.getKeyName());
  addToInsertSurfCtrl(System,cellN,cellN+cellR,CC);

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
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  const attachSystem::ContainedComp* BaseCC=
    OR.getObject<attachSystem::ContainedComp>(BaseFC.getKeyName());
  if (!BaseCC)
    throw ColErr::InContainerError<std::string>
      (BaseFC.getKeyName(),"getObject from base");
  
  const int cellN=OR.getCell(BaseFC.getKeyName());
  const int cellR=OR.getRange(BaseFC.getKeyName());
  addToInsertOuterSurfCtrl(System,cellN,cellN+cellR,*BaseCC,CC);

  return;
}

void
addToInsertSurfCtrl(Simulation& System,
		    const int cellA,const int cellB,
		    attachSystem::ContainedComp& CC)
 /*!
   Adds this object to the containedComp to be inserted.
   FC is the fixed object that is to be inserted -- linkpoints
   must be set. It is tested against all the ojbect with
   this object .
   \param System :: Simulation to use
   \param CellA :: First cell number [to test]
   \param CellB :: Last cell number  [to test]
   \param CC :: ContainedComp object to add to this
  */
{
  ELog::RegMethod RegA("AttachSupport","addToInsertSurfCtrl(int,int,CC)");

  const std::vector<Geometry::Surface*> SVec=CC.getSurfaces();

  std::vector<Geometry::Vec3D> Out;		
  std::vector<Geometry::Vec3D>::const_iterator vc;

  for(int i=cellA+1;i<=cellB;i++)
    {
      MonteCarlo::Qhull* CRPtr=System.findQhull(i);
      if (i==cellA+1 && !CRPtr)
	throw ColErr::InContainerError<int>(i,"Object not build");
      else if (!CRPtr)
	break;
      
      CRPtr->populate();
      CRPtr->createSurfaceList();
      const std::vector<const Geometry::Surface*>&
	CellSVec=CRPtr->getSurList();
      
      if (checkIntersect(CC,*CRPtr,CellSVec))
	CC.addInsertCell(i);
    }
  CC.insertObjects(System);
  return;
}

void
addToInsertOuterSurfCtrl(Simulation& System,
			 const int cellA,const int cellB,
			 const attachSystem::ContainedComp& BaseCC,
			 attachSystem::ContainedComp& CC)
 /*!
   Adds this object to the containedComp to be inserted.
   FC is the fixed object that is to be inserted -- linkpoints
   must be set. It is tested against all the object with
   this object .
   \param System :: Simulation to use
   \param BaseCC :: Only search using the base Contained Comp
   \param CellA :: First cell number [to test]
   \param CellB :: Last cell number  [to test]
   \param CC :: ContainedComp object to add to this
  */
{
  ELog::RegMethod RegA("AttachSupport","addToInsertSurfCtrl(int,int,CC)");

  const std::vector<Geometry::Surface*> SVec=CC.getSurfaces();

  std::vector<Geometry::Vec3D> Out;		
  std::vector<Geometry::Vec3D>::const_iterator vc;

  // Populate and createSurface list MUST have been called      
  const std::vector<const Geometry::Surface*>
    CellSVec=BaseCC.getConstSurfaces();

  for(int i=cellA+1;i<=cellB;i++)
    {
      MonteCarlo::Qhull* CRPtr=System.findQhull(i);
      if (i==cellA+1 && !CRPtr)
	throw ColErr::InContainerError<int>(i,"Object not build");
      else if (!CRPtr)
	break;

      if (checkIntersect(CC,*CRPtr,CellSVec))
	CC.addInsertCell(i);
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
  ELog::debugMethod DegA;
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
		  ELog::EM<<"FOUND point == "<<*vc<<ELog::endDebug;
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
		  const int cellA,const int cellB,
		  attachSystem::ContainedComp& CC)
 /*!
   Force CC into the BaseFC objects
  \param System :: Simulation to use
  \param BaseFC :: FixedComp object to have CC inserted into it.
  \param CC :: ContainedComp object to add to the BaseFC
 */
{
  ELog::RegMethod RegA("AttachSupport","addToInsertForce(int,int,CC)");

  for(int i=cellA+1;i<=cellB;i++)
    {
      MonteCarlo::Qhull* CRPtr=System.findQhull(i);
      if (i==cellA+1 && !CRPtr)
	throw ColErr::InContainerError<int>(i,"Object not built");
      else if (!CRPtr)
	break;
      CRPtr->populate();
      CRPtr->createSurfaceList();
      CC.addInsertCell(i);
    }

  CC.insertObjects(System);

  return;
}  


void
addToInsertForced(Simulation& System,
		  const attachSystem::FixedComp& BaseFC,
		  attachSystem::ContainedComp& CC)
 /*!
   Force CC into the BaseFC objects
  \param System :: Simulation to use
  \param CellA :: First cell number [to test]
   \param CellB :: Last cell number  [to test]
   
  \param CC :: ContainedComp object to add to the BaseFC
 */
{
  ELog::RegMethod RegA("AttachSupport","addToInsertForced(FC,CC)");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  const int cellN=OR.getCell(BaseFC.getKeyName());
  const int cellR=OR.getRange(BaseFC.getKeyName());
  addToInsertForced(System,cellN,cellN+cellR,CC);
  return;
}  

double
calcLinkDistance(const FixedComp& FC,const long int sideIndexA,
		 const long int sideIndexB)
/*!
  Calculate the distance between two link point
  \param FC :: FixedComp to use
  \param sideIndexA :: First point +1
  \param sideIndexB :: Second point +1 
  \return distance between points
*/

{
  ELog::RegMethod RegA("AttachSupport","calcLinkDistance");

  const Geometry::Vec3D PtA=FC.getSignedLinkPt(sideIndexA);
  const Geometry::Vec3D PtB=FC.getSignedLinkPt(sideIndexB);
  return PtA.Distance(PtB);
  
}

}  // NAMESPACE attachSystem
