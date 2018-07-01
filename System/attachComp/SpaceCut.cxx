 /********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/SpaceCut.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <cstddef>
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
#include <iterator>
#include <memory>
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "writeSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "SurInter.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Qhull.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "AttachSupport.h"
#include "SurInter.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SpaceCut.h"


namespace attachSystem
{

SpaceCut::SpaceCut()  :
  active(0),noPrimaryInsert(0),
  primaryCell(0),buildCell(0),
  nDirection(8),instepFrac(0.05),
  LCutters(2)
  /*!
    Constructor 
  */
{}

SpaceCut::SpaceCut(const SpaceCut& A) : 
  FCName(A.FCName),active(A.active),noPrimaryInsert(A.noPrimaryInsert),
  primaryCell(A.primaryCell),buildCell(A.buildCell),
  nDirection(A.nDirection),instepFrac(A.instepFrac),
  outerSurfBox(A.outerSurfBox),primaryBBox(A.primaryBBox),
  BBox(A.BBox),outerCut(A.outerCut),LCutters(A.LCutters),
  ABLink(A.ABLink)
  /*!
    Copy constructor
    \param A :: SpaceCut to copy
  */
{}

SpaceCut&
SpaceCut::operator=(const SpaceCut& A)
  /*!
    Assignment operator
    \param A :: SpaceCut to copy
    \return *this
  */
{
  if (this!=&A)
    {
      FCName=A.FCName;
      active=A.active;
      noPrimaryInsert=A.noPrimaryInsert;
      primaryCell=A.primaryCell;
      buildCell=A.buildCell;
      nDirection=A.nDirection;
      instepFrac=A.instepFrac;
      outerSurfBox=A.outerSurfBox;
      primaryBBox=A.primaryBBox;
      BBox=A.BBox;
      outerCut=A.outerCut;
      LCutters=A.LCutters;
      ABLink=A.ABLink;
    }
  return *this;
}

SpaceCut::~SpaceCut()
  /*!
    Deletion operator
  */
{}
  
void
SpaceCut::setSpaceConnect(const size_t Index,
			  const Geometry::Vec3D& C,
			  const Geometry::Vec3D& A)
 /*!
   Set the axis of the linked component
   \param Index :: Link number
   \param C :: Centre coordinate
   \param A :: Axis direciton
 */
{
  ELog::RegMethod RegA("SpaceCut","setConnect");
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),"LU.size/index");

  LCutters[Index].setConnectPt(C);
  LCutters[Index].setAxis(A);
  active=1;
  return;
}

void
SpaceCut::setSpaceLinkSurf(const size_t Index,
			   const HeadRule& HR) 
  /*!
    Set a surface to output
    \param Index :: Link number
    \param HR :: HeadRule to add
  */
{
  ELog::RegMethod RegA("SpaceCut","setSpaceLinkSurf(HR)");
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),"LU size/Index");

  LCutters[Index].setLinkSurf(HR);
  active=1;
  return;
}

void
SpaceCut::setSpaceLinkSurf(const size_t Index,
				 const int SN) 
  /*!
    Set a surface to output
    \param Index :: Link number
    \param SN :: surf to add
  */
{
  ELog::RegMethod RegA("SpaceCut","setSpaceLinkSurf(int)");
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),"LU size/Index");

  LCutters[Index].setLinkSurf(SN);
  active=1;
  return;
}

void
SpaceCut::setSpaceLinkCopy(const size_t Index,
			   const FixedComp& FC,
			   const long int sideIndex)
  /*!
    Copy the opposite (as if joined) link surface 
    Note that the surfaces are complemented
    \param Index :: Link number
    \param FC :: Other Fixed component to copy object from
    \param sideIndex :: link unit of other object
  */
{
  ELog::RegMethod RegA("SpaceCut","setSpaceLinkCopy");
  
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),
				     "LCutters size/index");
  
  LCutters[Index]=FC.getSignedLU(sideIndex);
  active=1;
  return;
}

void
SpaceCut::setOuterSurf(const std::string& objString)
  /*!
    Set the outer surf object [the part that cuts the new space]
    \param objString :: Outer surfaces 
   */
{
  ELog::RegMethod RegA("SpaceCut","setOuterSurf(string)");

  outerSurfBox=HeadRule(objString);
  return;
}

void
SpaceCut::setOuterSurf(const HeadRule& objHR)
  /*!
    Set the outer surf object [the part that cuts the new space]
    \param objHR :: HeadRule to set
   */
{
  ELog::RegMethod RegA("SpaceCut","setOuterSurf(HR)");

  outerSurfBox=objHR;
  return;
}

void
SpaceCut::setOuterSurf(const Simulation& System,
		       const int cellN)
  /*!
    Set the outer surf object [the part that cuts the new space]
    \param System :: Simulation to use for cells
    \param cellN :: Cell number
   */
{
  ELog::RegMethod RegA("SpaceCut","setOuterSurf(cell)");

  const MonteCarlo::Qhull* outerObj=System.findQhull(cellN);
  if (!outerObj)
    throw ColErr::InContainerError<int>(cellN,"cellN on found");

  outerSurfBox=outerObj->getHeadRule();
  return;
}

void
SpaceCut::setPrimaryCell(const HeadRule& objHR)
  /*!
    Set the primary cell which is divided
    \param objHR :: HeadRule to set
    \param cellN :: Cell number
   */
{
  ELog::RegMethod RegA("SpaceCut","setPrimaryCell(HR)");

  primaryBBox=objHR;
  return;
}

void
SpaceCut::setPrimaryCell(const Simulation& System,
			 const int cellN)
  /*!
    Set the primary cell which is divided
    \param System :: Simulation to use for cells
    \param cellN :: Cell number
   */
{
  ELog::RegMethod RegA("SpaceCut","setPrimaryCell(cell)");

  const MonteCarlo::Qhull* outerObj=System.findQhull(cellN);
  if (!outerObj)
    throw ColErr::InContainerError<int>(cellN,"cellN on found");

  primaryBBox=outerObj->getHeadRule();
  primaryCell=cellN;
  return;
}
  
HeadRule
SpaceCut::calcBoundary(const Simulation& System,
		       const int cellN,
		       const size_t NDivide,
		       const LinkUnit& ALink,
		       const LinkUnit& BLink)
/*!
    Construct a bounding box in a cell based on the 
    link surfaces
    \param System :: Simulation to use
    \param cellN :: Cell number
    \param NDivide :: division in link point
    \param ALink :: first link point
    \param BLink :: second link point
    \return HeadRule of bounding box [- link surf]
  */
{
  ELog::RegMethod RegA("SpaceCut","calcBoundary(System)");

  const MonteCarlo::Qhull* outerObj=System.findQhull(cellN);
  if (!outerObj)
    throw ColErr::InContainerError<int>(cellN,"cellN on found");

  const HeadRule objHR=outerObj->getHeadRule();

  return calcBoundary(objHR,NDivide,ALink,BLink);
}

std::map<int,const Geometry::Surface*>
SpaceCut::createSurfMap(const HeadRule& objHR)
  /*!
    Create a map of surface number / surface 
    for the HeadRule object [assuming it is populated]
    \param objHR :: Object head rule
    \return map (int,surface)
  */
{
  ELog::RegMethod Rega("SpaceCut","createSurfMap");

  std::map<int,const Geometry::Surface*> OutMap;
  
  std::vector<const Geometry::Surface*> SVec=
    objHR.getSurfaces();

  for(const Geometry::Surface* SPtr : SVec)
    {
      if (SPtr)
	OutMap.emplace(SPtr->getName(),SPtr);
    }
  return OutMap;
}

int
SpaceCut::testPlaneDivider
(const std::map<int,const Geometry::Surface*>& activeSurf,
 const int SN,const Geometry::Vec3D& impactPt,
 const Geometry::Vec3D& axis)
  /*!
    Test if a surface has a divider
    \param activeSurf :: Map of surface numbers : Surface Ptr
    \param SN :: Surface that may need a divide plane
    \param impactPt :: Point that the surface was intersect with line
    \param Axis :: Axis direction of line
  */
{
  ELog::RegMethod RegA("SpaceCut","testPlaneDivider");

  typedef std::map<int,const Geometry::Surface*> MTYPE;
  if (SN>=0) return 0;
  
  MTYPE::const_iterator mc;
  mc=activeSurf.find(-SN);
  if (mc==activeSurf.end()) return 0;
  const Geometry::Surface* SPtr=mc->second;

  if (!dynamic_cast<const Geometry::Cylinder*>(SPtr) &&
      !dynamic_cast<const Geometry::Sphere*>(SPtr) )
    return 0;
  
  for(const MTYPE::value_type& TUnit : activeSurf)
    {
      const Geometry::Plane* TPtr=
	dynamic_cast<const Geometry::Plane*>(TUnit.second);
      if (TPtr)
	{
	  const double DProd=TPtr->getNormal().dotProd(axis);
	  if (1.0-(std::abs(DProd))<0.5)
	    {
	      Geometry::Vec3D Impact =
		SurInter::getLinePoint(impactPt,axis,TPtr);
	      if (SPtr->side(Impact)<0)
		return (DProd<0.0) ? -TUnit.first : TUnit.first;
	    }
	}
    }
  return 0;
}
				 

  
HeadRule
SpaceCut::calcBoundary(const HeadRule& objHR,
		       const size_t NDivide,
		       const LinkUnit& ALink,
		       const LinkUnit& BLink)
  /*!
    Construct a bounding box in a cell based on the 
    link surfaces
    \param objHR :: HeadRule of boundary
    \param NDivide :: division in link point
    \param ALink :: first link point
    \param BLink :: second link point
    \return HeadRule of bounding box [- link surf]
  */
{
  ELog::RegMethod RegA("SpaceCut","calcBoundary(headRule)");

  std::set<int> linkSN;

  for(const LinkUnit& LU : {ALink,BLink})
    {
      if (LU.isComplete())
	{
	  const int SN=LU.getLinkSurf();
	  linkSN.insert(SN);
	}
    }
  if (linkSN.empty())
    throw ColErr::EmptyContainer("LinkPoints empty");
  
  std::set<int> surfN;
  // mid points moved in by 10% of distance
  const Geometry::Vec3D& APoint=(ALink.isComplete()) ?
    ALink.getConnectPt() : BLink.getConnectPt();
  const Geometry::Vec3D& BPoint=(BLink.isComplete()) ?
    BLink.getConnectPt() : ALink.getConnectPt();
  const Geometry::Vec3D& YA=(ALink.isComplete()) ?
    ALink.getAxis() : BLink.getAxis();
  const Geometry::Vec3D& YB=(BLink.isComplete()) ?
    BLink.getAxis() : ALink.getAxis();

  const Geometry::Vec3D CPoint((APoint+BPoint)/2.0);
  const Geometry::Vec3D YY= (YA.dotProd(YB)>0.0) ?
    (YA+YB).unit() : (YB-YA).unit();
  const Geometry::Vec3D XX=YY.crossNormal();
      const Geometry::Vec3D ZZ=YY*XX;

  if (!objHR.isValid(CPoint))
    {
      ELog::EM<<"Object == "<<objHR<<ELog::endDiag;
      ELog::EM<<"Point == "<<CPoint<<ELog::endDiag;
      throw ColErr::InContainerError<Geometry::Vec3D>
	(CPoint,"CPoint out of object");
    }

  std::map<int,const Geometry::Surface*> objSurfMap=
    createSurfMap(objHR);
  
  const std::vector<Geometry::Vec3D> CP=
    {
      CPoint,
      APoint*0.95+CPoint*0.05,
      BPoint*0.95+CPoint*0.05
    };
  for(const Geometry::Vec3D& Org : CP)
    {
      const double angleStep=2.0*M_PI/static_cast<double>(NDivide);
      double angle(0.0);
      for(size_t i=0;i<NDivide;i++)
	{
	  const Geometry::Vec3D Axis=XX*cos(angle)+ZZ*sin(angle);
	  double D;
	  const int SN=objHR.trackSurf(Org,Axis,D,linkSN);
	  if (SN && surfN.find(-SN)==surfN.end())
	    {
	      surfN.insert(-SN);
	      // test if cylinder/sphere and extra plane exist
	      const int divideSN=
		testPlaneDivider(objSurfMap,SN,Org+Axis*(D*1.1),Axis);
	      if (divideSN)
		surfN.insert(-divideSN);
	    }
	  angle+=angleStep;
	}
    }
  // forward going trajectory
  if (!ALink.isComplete() || !BLink.isComplete())
    {
      double D;
      const int SN=objHR.trackSurf(CPoint,-YA,D);
      if (SN)
	surfN.insert(-SN);
    } 	

  
  // NOW eliminate all surfaces NOT in surfN
  const std::set<int> fullSurfN=objHR.getSurfSet();
  HeadRule outBox=objHR;
  for(const int SN : fullSurfN)
    {
      if (surfN.find(SN) == surfN.end())
	outBox.removeItems(SN);
    }
  
  // Check for no negative repeats:
  for(const int SN : surfN)
    {
      if (SN==0 || (SN<0 && surfN.find(-SN)!=surfN.end()))
	throw ColErr::InContainerError<int>(SN,"Surf repeated");
    }
  return outBox;
}
  
void
SpaceCut::calcBoundaryBox(const Simulation& System)
  /*!
    Boundary calculator
    \param System :: Simulation for objects [if needed]
  */
{
  ELog::RegMethod RegA("SpaceCut","calcBoundaryBox");
  if (!primaryBBox.hasRule())
    BBox=calcBoundary(System,primaryCell,nDirection,LCutters[0],LCutters[1]);
  else 
    BBox=calcBoundary(primaryBBox,nDirection,LCutters[0],LCutters[1]);
  return;
}

void
SpaceCut::registerSpaceCut(const long int linkA,
			   const long int linkB)
  /*!
    Register the surface space
    \param linkA :: Signed link point
    \param linkB :: Signed link point
  */
{
  ABLink.first=linkA;
  ABLink.second=linkB;
  active=1;
  return;
}

void
SpaceCut::registerSpaceIsolation
  (const long int linkA,const long int linkB)
  /*!
    Register the surface space and dont place primary
    \param linkA :: Signed link point
    \param linkB :: Signed link point
  */
{
  active=1;
  noPrimaryInsert=1;
  registerSpaceCut(linkA,linkB);
  return;
}

void
SpaceCut::buildWrapCell(Simulation& System,
			const int pCell,
			const int cCell)
  /*!
    Build the cells within the bounding space that
    contains the complex outerSurf
    \param System :: Simulation
    \param pCell :: Primary cell
    \param cCell :: Contained cell
  */
{
  ELog::RegMethod RegA("SpaceCut","buildWrapCell");

  int matN=0;
  double matTemp=0.0;
  if (pCell)
    {
      const MonteCarlo::Qhull* outerObj=System.findQhull(pCell);
      if (!outerObj)
	throw ColErr::InContainerError<int>
	  (pCell,"Primary cell does not exist");
      matN=outerObj->getMat();
      matTemp=outerObj->getTemp();
    }

  // First make inner vacuum
  // removeing front/back surfaces

  outerCut.reset();  
  HeadRule innerVacuum(outerSurfBox);
  for(const LinkUnit& LU : LCutters)
    {
      if (LU.isComplete())
	{
	  const int SN=LU.getLinkSurf();
	  innerVacuum.removeItems(-SN);
	  outerCut.addIntersection(-SN);
	}
    }

  // Make new outer void
  HeadRule newOuterVoid(BBox);
  for(const LinkUnit& LU : LCutters)
    {
      if (LU.isComplete())
	newOuterVoid.addIntersection(-LU.getLinkSurf());
    }

  newOuterVoid.addIntersection(innerVacuum.complement());
  System.addCell(cCell,matN,matTemp,newOuterVoid.display());

  CellMap* CMapPtr=dynamic_cast<CellMap*>(this);
  if (CMapPtr)
    CMapPtr->addCell("OuterSpace",cCell);

  
  return;
}

void
SpaceCut::clear()
  /*!
    Reset link units
   */
{
  ABLink.first=0;
  ABLink.second=0;
  LCutters[0]=LinkUnit();
  LCutters[1]=LinkUnit();
  buildCell=0;
  primaryCell=0;
  return;
}

void
SpaceCut::initialize()
  /*!
    Initializer to be run after object is built
  */
{
  ELog::RegMethod RegA("SpaceCut","initialize");

  FixedComp* FC=dynamic_cast<FixedComp*>(this);
  if (!FC) return;

  FCName=FC->getKeyName();
  if (active)
    {
      if (ABLink.first)
	SpaceCut::setSpaceLinkCopy(0,*FC,ABLink.first);
      if (ABLink.second)
	SpaceCut::setSpaceLinkCopy(1,*FC,ABLink.second);

      if (!buildCell)
	buildCell=FC->nextCell();
    }

  return;
}

void
SpaceCut::initialize(FixedComp& FC)
  /*!
    Initializer to be run after object is built
    \param initializer with Give FC
  */
{
  ELog::RegMethod RegA("SpaceCut","initialize(FC)");

  FCName=FC.getKeyName();
  if (FCName=="BalderOpticsHutChicane0")
    ELog::EM<<"IFC == "<<FCName<<" "<<active<<ELog::endDiag;
  if (active)
    {
      if (ABLink.first)
	SpaceCut::setSpaceLinkCopy(0,FC,ABLink.first);
      if (ABLink.second)
	SpaceCut::setSpaceLinkCopy(1,FC,ABLink.second);

      if (!buildCell)
	buildCell=FC.nextCell();
    }

  return;
}
  

void
SpaceCut::insertPair(Simulation& System,
		     const std::vector<int>& insertCells,
		     const attachSystem::FixedComp& FCA,
		     const long int linkA,
		     const attachSystem::FixedComp& FCB,
		     const long int linkB)
  /*!
    Build the cells within the bounding space that
    contains the complex outerSurf
    \param System :: Simulation 
    \param insertCells :: insert cells
    \param FCA :: FixedComp A
    \param linkA :: Link point [outward]
    \param FCB :: FixedComp B
    \param linkB :: Link point [outward]
  */
{
  ELog::RegMethod RegA("SpaceCut","insertPair");

  HeadRule BBox;
  BBox.addUnion(FCA.getFullRule(linkA));
  BBox.addUnion(FCB.getFullRule(linkB));

  for(const int pCell : insertCells)
     {
      MonteCarlo::Qhull* outerObj=System.findQhull(pCell);
      if (!outerObj)
	throw ColErr::InContainerError<int>
	  (pCell,"Primary cell does not exist");
      outerObj->addSurfString(BBox.display());
    }      
  return;
}

void
SpaceCut::insertObjects(Simulation& System)
  /*!
    Assumption that we can put this object into a space
    Makes the object bigger 
    Then constructs the outer boundary as if that object is 
    the outer boundary!
    \param System :: simulation system
  */
{
  ELog::RegMethod RegA("SpaceCut","insertObjects");
      
  System.populateCells();
  initialize();
  
  if ((primaryCell || primaryBBox.hasRule()) && buildCell)
    {
      calcBoundaryBox(System);
      buildWrapCell(System,primaryCell,buildCell);	    
    }
  
  if (!noPrimaryInsert && primaryCell && buildCell)
    {
      MonteCarlo::Qhull* outerObj=System.findQhull(primaryCell);
      if (outerObj)
	outerObj->addSurfString(outerCut.complement().display());
      else
	throw ColErr::InContainerError<int>(primaryCell,
					    "Cell not in Simulation");
    }  
  return;
}


void
SpaceCut::insertObjects(Simulation& System,
			attachSystem::FixedComp& FC)
  /*!
    Assumption that we can put this object into a space
    Makes the object bigger 
    Then constructs the outer boundary as if that object is 
    the outer boundary!
    This passes FC because in the group containers *this is
    not always the top FC object
    \param System :: simulation system
    \param FC :: FixedComponent
  */
{
  ELog::RegMethod RegA("SpaceCut","insertObjects");
      
  System.populateCells();
  initialize(FC);

  if ((primaryCell || primaryBBox.hasRule()) && buildCell)
    {
      calcBoundaryBox(System);
      buildWrapCell(System,primaryCell,buildCell);	    
    }
  if (FCName=="BalderOpticsHutChicane0")
    ELog::EM<<"FCS == "<<FCName<<" "<<buildCell<<ELog::endDiag;

  if (!noPrimaryInsert && primaryCell && buildCell)
    {
      MonteCarlo::Qhull* outerObj=System.findQhull(primaryCell);
      if (outerObj)
	outerObj->addSurfString(outerCut.complement().display());
      else
	throw ColErr::InContainerError<int>(primaryCell,
					    "Cell not in Simulation");
    }  
  return;
}
  
}  // NAMESPACE attachSystem
