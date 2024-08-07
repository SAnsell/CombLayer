/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/TubeCollimator.cxx
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
#include "interPoint.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfRegister.h"
#include "SurInter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "gridUnit.h"
#include "hexUnit.h"
#include "TubeCollimator.h"

namespace photonSystem
{

TubeCollimator::TubeCollimator(const std::string& Key)  :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

TubeCollimator::TubeCollimator(const TubeCollimator& A) : 
  attachSystem::FixedRotate(A),attachSystem::ContainedComp(A),
  layoutType(A.layoutType),boundaryType(A.boundaryType),
  nLinks(A.nLinks),nTubeLayers(A.nTubeLayers),GGrid(A.GGrid),
  length(A.length),radius(A.radius),wallThick(A.wallThick),
  centSpc(A.centSpc),wallMat(A.wallMat),boundaryRotAngle(A.boundaryRotAngle),
  layoutRotAngle(A.layoutRotAngle),gridExclude(A.gridExclude),
  AAxis(A.AAxis),BAxis(A.BAxis),CAxis(A.CAxis),
  boundary(A.boundary),voidBoundary(A.voidBoundary)
  /*!
    Copy constructor
    \param A :: TubeCollimator to copy
  */
{}

TubeCollimator&
TubeCollimator::operator=(const TubeCollimator& A)
  /*!
    Assignment operator
    \param A :: TubeCollimator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      layoutType=A.layoutType;
      boundaryType=A.boundaryType;
      nLinks=A.nLinks;
      nTubeLayers=A.nTubeLayers;
      GGrid=A.GGrid;
      length=A.length;
      radius=A.radius;
      wallThick=A.wallThick;
      centSpc=A.centSpc;
      wallMat=A.wallMat;
      boundaryRotAngle=A.boundaryRotAngle;
      layoutRotAngle=A.layoutRotAngle;
      gridExclude=A.gridExclude;
      AAxis=A.AAxis;
      BAxis=A.BAxis;
      CAxis=A.CAxis;
      boundary=A.boundary;
      voidBoundary=A.voidBoundary;
    }
  return *this;
}

TubeCollimator::~TubeCollimator() 
  /*!
    Destructor
  */
{
  clearGGrid();
}

void
TubeCollimator::clearGGrid()
  /*!
    Remove all the points from a map
   */
{
  for(MTYPE::value_type& mv : GGrid)
    delete mv.second;
  
 GGrid.clear();
 return;
}

void
TubeCollimator::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("VacuumVessel","populate");

  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  centSpc=Control.EvalVar<double>(keyName+"CentSpc");
  length=Control.EvalVar<double>(keyName+"Length");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  layoutType=Control.EvalVar<std::string>(keyName+"LayoutType");
  boundaryType=Control.EvalVar<std::string>(keyName+"BoundaryType");

  nTubeLayers=Control.EvalDefVar<long int>(keyName+"NTubeLayers",0);
  boundaryRotAngle=
    Control.EvalDefPair<double>(keyName+"BoundaryRotAngle",
				keyName+"RotAngle",0.0);
  layoutRotAngle=
    Control.EvalDefPair<double>(keyName+"LayoutRotAngle",
				keyName+"RotAngle",0.0);
  gridExclude=Control.EvalDefVar<int>(keyName+"GridExclude",0);

  return;
}

constructSystem::gridUnit*
TubeCollimator::newGridUnit(const long int i,const long int j,
			    const Geometry::Vec3D& C) const
  /*!
    Allocate a new grid based on layout
    \param i :: First Index 
    \param j :: Second Index 
    \param C :: Centre 
    \return new gridUnit [correct type ]
   */
{
  if (layoutType=="hexagon")
    return new constructSystem::hexUnit(i,j,C);
    
  return 0;
}

const Geometry::Vec3D&
TubeCollimator::getGridAxis(const size_t Index) const
  /*!
    Calculate the direction based on a Index 
    \param Index :: Index value [0-6] / [0-4]
    \return Vector for normal of plane
  */
{
  static const Geometry::Vec3D* HVUnit[3]={&AAxis,&CAxis,&BAxis};
  static const Geometry::Vec3D* SVUnit[2]={&AAxis,&BAxis};
  
  if (layoutType=="square")
    return *(SVUnit[Index % 2]);

  return *(HVUnit[Index % 3]);
}
  
void
TubeCollimator::setLayout()
  /*!
    Sets the layout vectors baesd on the 
    value in layoutType.
  */
{
  ELog::RegMethod RegA("TubeCollimator","setLayout");

  if (layoutType=="square" || layoutType=="Square")
    {
      layoutType="square";  // simple check later
      AAxis=X;
      BAxis=Z;
      nLinks=4;
    }
  else if (layoutType=="hexagon" || layoutType=="Hexagon")
    {
      layoutType="hexagon";  // simple check later
      AAxis=X;
      BAxis=X*cos(M_PI/3.0)+Z*sin(M_PI/3.0);
      nLinks=6;
    }
  else
    {
      throw ColErr::InContainerError<std::string>
	(layoutType,"layout type not supported");
    }

  const Geometry::Quaternion Qab=
    Geometry::Quaternion::calcQRotDeg(layoutRotAngle,Y);
  Qab.rotate(AAxis);
  Qab.rotate(BAxis);

  return;
}

  
void
TubeCollimator::setBoundary(const FuncDataBase& Control)
  /*!
    Set the boundary
    \param Control :: 
   */
{
  ELog::RegMethod RegA("TubeCollimator","setBoundary");

  std::string Out;

  if (boundaryType=="Rectangle" || boundaryType=="rectangle")
    {
      const double width=
	Control.EvalVar<double>(keyName+"BoundaryWidth");
      const double height=
	Control.EvalVar<double>(keyName+"BoundaryHeight");
      Geometry::Vec3D AX(X);
      Geometry::Vec3D BZ(Z);
       const Geometry::Quaternion Qab=
	 Geometry::Quaternion::calcQRotDeg(boundaryRotAngle,Y);
       Qab.rotate(AX);
       Qab.rotate(BZ);

       ModelSupport::buildPlane(SMap,buildIndex+5003,Origin-AX*(width/2.0),AX);
       ModelSupport::buildPlane(SMap,buildIndex+5004,Origin+AX*(width/2.0),AX);
       ModelSupport::buildPlane(SMap,buildIndex+5005,Origin-BZ*(height/2.0),BZ);
       ModelSupport::buildPlane(SMap,buildIndex+5006,Origin+BZ*(height/2.0),BZ);

       // created for outer boundary
       ModelSupport::buildPlane(SMap,buildIndex+6003,Origin-AX*((width+centSpc)/2.0),AX);
       ModelSupport::buildPlane(SMap,buildIndex+6004,Origin+AX*((width+centSpc)/2.0),AX);
       ModelSupport::buildPlane(SMap,buildIndex+6005,Origin-BZ*((height+centSpc)/2.0),BZ);
       ModelSupport::buildPlane(SMap,buildIndex+6006,Origin+BZ*((height+centSpc)/2.0),BZ);

       
       boundary=ModelSupport::getHeadRule
	 (SMap,buildIndex,"5003 -5004 5005 -5006");
       boundary.populateSurf();
       voidBoundary=
	 ModelSupport::getHeadRule(SMap,buildIndex,"6003 -6004 6005 -6006");
       voidBoundary.populateSurf();
    }
  else if (boundaryType=="Circle" || boundaryType=="circle")
    {
      const double radius=
	Control.EvalVar<double>(keyName+"Radius");
      ModelSupport::buildCylinder(SMap,buildIndex+5007,Origin,Y,radius);
      // Outer boundary
      ModelSupport::buildCylinder(SMap,buildIndex+5007,Origin,Y,radius+centSpc/2.0);
      
      boundary=HeadRule(SMap,buildIndex,-5007);
      boundary.populateSurf();
      voidBoundary=HeadRule(SMap,buildIndex,-6007);
      voidBoundary.populateSurf();
    }
  else if (boundaryType=="Free" || boundaryType=="free")
    {

      if (nTubeLayers<=0)
	throw ColErr::RangeError<long int>
	  (0,50,nTubeLayers,"boundaryType:Free > 0");
    }
  else
    {
      throw ColErr::InContainerError<std::string>
	(boundaryType,"boundary type not supported");
    }  
  return;
}

  
void
TubeCollimator::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
/*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: Link point surface to use as origin/basis.
  */
{
  ELog::RegMethod RegA("TubeCollimator","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  // Default layout 
  setLayout();
  
  return;
}


void
TubeCollimator::createCentres()
  /*!
    Calculate the positions of all the centres
  */
{
  ELog::RegMethod RegA("TubeCollimator","createCentres");

  clearGGrid();
  bool acceptFlag(1);
  long int step(0);   
  while(acceptFlag || (nTubeLayers>0 && step<nTubeLayers))
    {
      acceptFlag=0;
      for(long int i=-step;i<=step;i++)
	for(long int j=-step;j<=step;j++)
	  {
	    if (std::abs(i)==step || std::abs(j)==step)
	      {
		const Geometry::Vec3D CPoint=Origin+
		  AAxis*(centSpc*static_cast<double>(i))+
		  BAxis*(centSpc*static_cast<double>(j));
		if (boundary.isValid(CPoint))
		  {
		    acceptFlag=1;
		    GGrid.insert(MTYPE::value_type((i*1000+j),
						   newGridUnit(i,j,CPoint)));
		  }
	      }
	  }
      step++;
    }
  return;
}

void
TubeCollimator::createJoinLinks()
  /*!
    Create all the linked points
    \todo thst can be combined with createJoinSurf
   */
{
  ELog::RegMethod RegA("TubeCollimator","createJoinLinks");

  const size_t nLinksHalf(nLinks/2);
  for(MTYPE::value_type& AG : GGrid)
    {
      constructSystem::gridUnit* APtr=AG.second;
      for(size_t i=0;i<nLinks;i++)
        {
          const int JA=AG.first+APtr->gridIndex(i);
          MTYPE::iterator bc=GGrid.find(JA);
          if (bc!=GGrid.end())
            {
              bc->second->setLink(i+nLinksHalf,APtr);
	      APtr->setLink(i,bc->second);
            }
        }
    }
  return;
}
  
void
TubeCollimator::createJoinSurf()
  /*!
    Create the link surfaces
  */
{
  ELog::RegMethod RegA("TubeCollimator","createLinkSurf");

  const size_t nLinksHalf(nLinks/2);
  int planeIndex(buildIndex+1001);

  for(MTYPE::value_type& AG : GGrid)
    {
      constructSystem::gridUnit* APtr=AG.second;
      // iterate over the index set [6/4]
      for(size_t i=0;i<nLinks;i++)
	{
	  if (APtr->hasLink(i) && !APtr->hasSurfLink(i))
	    {
	      const constructSystem::gridUnit* BPtr=APtr->getLink(i);
	      const Geometry::Vec3D DCent=
                (APtr->getCentre()+BPtr->getCentre())/2.0;
	      const Geometry::Vec3D HAxis=
                (APtr->getCentre()-BPtr->getCentre()).unit();

	      int surNum;
	      ModelSupport::buildPlane(SMap,planeIndex,DCent,HAxis);
	      surNum=SMap.realSurf(planeIndex);
	      const int JA=AG.first+APtr->gridIndex(i);
	      MTYPE::iterator bc=GGrid.find(JA);
	      if (bc!=GGrid.end())
		bc->second->setSurf(i+nLinksHalf,-surNum);
	      APtr->setSurf(i,surNum);
	    }	      
	  planeIndex++;
	}
      calcGapInside(APtr);

    }
  return;
}

bool
TubeCollimator::calcGapInside(constructSystem::gridUnit* APtr)
  /*!
    Determine if the cell is closed within the boundary
    \param APtr :: Pointer to gridUnit which maybe closed
    \return true if close in open (and set flag in APtr)
  */
{
  ELog::RegMethod RegA("TubeCollimator","calcGapInside");

  if (!APtr->isComplete())
    {
      size_t gapA;
      for(gapA=0;gapA<nLinks;gapA++)
        {
          if (APtr->hasSurfLink(gapA) &&
              !APtr->hasSurfLink(gapA+1))
            break;
        }
      
      size_t gapB;
      for(gapB=1;gapB<=nLinks;gapB++)
        {
          if (APtr->hasSurfLink(gapB) &&
              !APtr->hasSurfLink(gapB-1))
            {
              gapB %= nLinks;
              break;
            }
        }

      if (gapA!=gapB &&
          gapA!=nLinks && gapB!=nLinks &&
          ((gapA+1) % nLinks !=gapB) &&
          ((gapB+1) % nLinks !=gapA) )
        {
          // Test gap
          // Calc mid point of plane:
	  const Geometry::Vec3D& CentPt=APtr->getCentre();
	  const Geometry::Plane* PlanePtrA=
            SMap.realPtr<Geometry::Plane>(APtr->getSurf(gapA));
	  const Geometry::Plane* PlanePtrB=
            SMap.realPtr<Geometry::Plane>(APtr->getSurf(gapB));
          const Geometry::Vec3D MidPtA=
            SurInter::getLinePoint(CentPt,PlanePtrA->getNormal(),*PlanePtrA);
          const Geometry::Vec3D MidPtB=
            SurInter::getLinePoint(CentPt,PlanePtrB->getNormal(),*PlanePtrB);
          const Geometry::Vec3D LineNormalA=(MidPtA-CentPt).unit()*Y;
          const Geometry::Vec3D LineNormalB=(MidPtB-CentPt).unit()*Y;
          const double lnDP(LineNormalA.dotProd(LineNormalB));
          if (std::abs(lnDP)<1.0-Geometry::zeroTol)
            {
              Geometry::Line lineA(MidPtA,LineNormalA);
              Geometry::Line lineB(MidPtB,LineNormalB);
              const std::pair<Geometry::Vec3D,Geometry::Vec3D>
                CP=lineA.closestPoints(lineB);
              if (CP.first.Distance(CP.second)<Geometry::zeroTol)
                {
                  if (voidBoundary.isValid(CP.first))
                    {
                      APtr->setBoundary(gapA+1,gapB+1);
                      return 1;
                    }
                }
            }
        }
    }
  return 0;
}

void
TubeCollimator::createCells(Simulation& System)
  /*!
    Create the inidivual cells
    \param System :: Simulation to add to system
   */
{
  ELog::RegMethod RegA("TubeCollimator","createTubes");
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);

  const HeadRule FBHR=
    ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 ");
  const HeadRule outBoundaryHR=boundaryHR();
  
  int RI(buildIndex);
  for(MTYPE::value_type& MU : GGrid)
    {
      constructSystem::gridUnit* APtr= MU.second;
      // Create Inner plane here just to help order stuff
      ModelSupport::buildCylinder(SMap,RI+7,APtr->getCentre(),Y,radius);
      ModelSupport::buildCylinder(SMap,RI+8,APtr->getCentre(),Y,radius+wallThick);

      const HeadRule CylA=ModelSupport::getHeadRule(SMap,RI,"-7");
      const HeadRule CylB=ModelSupport::getHeadRule(SMap,RI,"7 -8");
      HeadRule HR=APtr->getInner()*ModelSupport::getHeadRule(SMap,RI,"8");

      if (!APtr->isComplete())
	{
	  const HeadRule OutB=calcBoundary(APtr);
	  HR*=OutB;
	}

      System.addCell(cellIndex++,0,0.0,CylA*FBHR);
      System.addCell(cellIndex++,wallMat,0.0,CylB*FBHR);
      System.addCell(cellIndex++,0,0.0,HR*FBHR);
      RI+=10;
    }
  addOuterSurf(outBoundaryHR+FBHR);
  return;
}

  
HeadRule
TubeCollimator::calcBoundary(constructSystem::gridUnit* APtr) const
 /*!
   Calculate the intersection of the headrule of the gridUnit
   and the various sides of the boundary.
   \param APtr :: GridUnit to use
   \return string to cut with
 */
{
  ELog::RegMethod RegA("TubeCollimator","calcBoundary");
  
  // The gridUnit has a convex closed boundary.
  // The only surfaces that can freely intersect the boundary are the open
  // surface.

  HeadRule outHR;
  for(size_t i=0;i<nLinks;i++)
    {
      if (APtr->hasSurfLink(i) &&
	  (!APtr->hasSurfLink(i+1) || !APtr->hasSurfLink(nLinks+i-1)))
	{
	  // maybe overlap with neighbouring cutting surfaces
	  constructSystem::gridUnit* LPtr=APtr->getLink(i);
	  const int extraSurfN=LPtr->clearBoundary((i+3) % nLinks);
	  if (extraSurfN)
	    outHR*=HeadRule(extraSurfN);

	  // Calc mid point of plane:
	  const Geometry::Vec3D& CentPt=APtr->getCentre();
	  const Geometry::Plane* PlanePtr=
	    SMap.realPtr<Geometry::Plane>(APtr->getSurf(i));
	  // could just calc distance and project along normal but
	  // this is easier.
	  const Geometry::Vec3D MidPt=
	    SurInter::getLinePoint(CentPt,PlanePtr->getNormal(),*PlanePtr);
	  //          Geometry::Vec3D LineNormal=((MidPt-CentPt)*Y).unit();
	  const Geometry::Vec3D LineNormal=(MidPt-CentPt)*Y;
	  
	  std::vector<Geometry::interPoint> Pts;
	  HeadRule InnerControl(APtr->getInner());
	  InnerControl.populateSurf();
	  
	  if (voidBoundary.calcSurfIntersection(MidPt,LineNormal,Pts))
	    {
	      for(const Geometry::interPoint& PItem : Pts)
		{
		  if (InnerControl.isSideValid(PItem.Pt,PItem.SNum))
		    outHR*=HeadRule(-PItem.SNum);
		}
	    }
	}
    }
  return outHR;
  
}

  
void
TubeCollimator::createTubes(Simulation& System)
  /*!
    Create simple structures
    \param System :: simluation to use
  */
{
  ELog::RegMethod RegA("TubeCollimator","createTubes");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);

  const HeadRule FBHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"1 -2");


  HeadRule HR;
  int RI(buildIndex);

  // Outer boundary:
  const HeadRule outBoundaryHR=boundaryHR();
  HeadRule HoleHR;
      
  bool acceptFlag(1);
  long int step(0);   
  while(acceptFlag || (nTubeLayers>0 && step<nTubeLayers))
    {
      acceptFlag=0;
      for(long int i=-step;i<=step;i++)
	for(long int j=-step;j<=step;j++)
	  {
	    const Geometry::Vec3D CPoint=
	      AAxis*(centSpc*static_cast<double>(i))+
	      BAxis*(centSpc*static_cast<double>(j));
	    if ((std::abs(i)==step || std::abs(j)==step) &&
		boundary.isValid(CPoint))
	      {
		acceptFlag=1;
		ModelSupport::buildCylinder(SMap,RI+7,CPoint,Y,radius);
		ModelSupport::buildCylinder(SMap,RI+8,CPoint,Y,radius+wallThick);
		
		HR=ModelSupport::getHeadRule(SMap,RI,"-7");
		System.addCell(cellIndex++,0,0.0,HR*FBHR);
		
		HR=ModelSupport::getHeadRule(SMap,RI,"7 -8");
		System.addCell(cellIndex++,wallMat,0.0,HR*FBHR);
		
		if (!gridExclude)
		  HR=ModelSupport::getHeadRule(SMap,RI,"8");
		
		GGrid.insert(MTYPE::value_type((i*1000+j),newGridUnit(i,j,CPoint)));
		HoleHR*=HR;
		RI+=10;
	      }
	  }
      step++;
    }
  
  System.addCell(cellIndex++,0,0.0,outBoundaryHR*FBHR*HoleHR);
  addOuterSurf(outBoundaryHR+FBHR);
  
  return; 
}

HeadRule
TubeCollimator::boundaryHR() const
  /*!
    Create the boundary string
    \return boundary HR
  */
{
  ELog::RegMethod RegA("TubeCollimator","createBoundary");

  if (boundaryType=="Rectangle" || boundaryType=="rectangle")
    return ModelSupport::getHeadRule(SMap,buildIndex,"6003 -6004 6005 -6006");
  
  if (boundaryType=="Circle" || boundaryType=="circle")
    return ModelSupport::getHeadRule(SMap,buildIndex,"-6007");

  return HeadRule();
}


void
TubeCollimator::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("VacuumVessel","createLinks");
  
  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  return;
}

void
TubeCollimator::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: surface offset
   */
{
  ELog::RegMethod RegA("TubeCollimator","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  setBoundary(System.getDataBase());
  if (gridExclude)
    {
      createCentres();
      createJoinLinks();
      createJoinSurf();
      createCells(System);
    }
  else
    createTubes(System);
        
  createLinks();
  insertObjects(System);       



  return;
}


}
