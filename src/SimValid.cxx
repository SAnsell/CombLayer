/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/SimValid.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <complex>
#include <string>
#include <sstream>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <algorithm>
#include <iterator>
#include <random>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Random.h"
#include "varList.h"
#include "MapSupport.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "ObjSurfMap.h"
#include "interPoint.h"
#include "particle.h"
#include "eTrack.h"
#include "surfRegister.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "LineTrack.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"

#include "SimValid.h"

#include "debugMethod.h"

namespace ModelSupport
{

SimValid::SimValid() :
  Centre(Geometry::Vec3D(0.15,-0.45,0.15))
  /*!
    Constructor
  */
{}

SimValid::SimValid(const SimValid& A) : 
  Centre(A.Centre)
  /*!
    Copy constructor
    \param A :: SimValid to copy
  */
{}

SimValid&
SimValid::operator=(const SimValid& A)
  /*!
    Assignment operator
    \param A :: SimValid to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Centre=A.Centre;
    }
  return *this;
}


bool
SimValid::nextPoint(const std::vector<Geometry::interPoint>& TPts,
		    size_t& indexA,size_t& indexB,size_t& indexC,
		    Geometry::Vec3D& outPt)
/*!
  Ugly function to return the point in the double index loop
  \param outPt :: new Point [if set]
  \return 1 if valid (within loop)
*/
{
  const size_t midSlice(3);

  if (indexC<1) indexC=0;
  if (indexA>=indexB)
    indexB=indexA+1;

  indexC++;
  if (indexC>=midSlice)
    {
      indexC=1;
      indexB++;
    }

  if(indexB>=TPts.size())
    {
      indexA++;
      indexB=indexA+1;
      if (indexB>=TPts.size())
	return 0;
    }
  const Geometry::Vec3D diffV=(TPts[indexB].Pt-TPts[indexA].Pt)
    /static_cast<double>(midSlice);
  outPt=TPts[indexA].Pt+diffV*static_cast<double>(indexC);

  return 1;
}
  
bool
SimValid::checkPoint(const Geometry::Vec3D& TP, 
		     const std::set<const MonteCarlo::Object*>& checkObj,
		     const int CylN,const int PlnN)
  /*!
    This checks is a given obect that has point at surface intersections
    (TPts) along surfaces CN and PN (cyl/plane). that the object
    is isValid for any combination of CN / PN.
    \param OPtr :: Object Pointer to test
    \param TPts :: Points on the line
    \param CN :: Cylinder number
    \param PN :: Plane number
    \return true if points are in the object
  */
{
  ELog::RegMethod RegA("SimValid","checkPoint");

  // For all combinations of  CylN/PlnN true/false we must get
  // one and only cell which is true

  std::map<int,int> surfState({{CylN,-1},{PlnN,-1}});
  do
    {
      std::set<const MonteCarlo::Object*> foundSet;
      for(const MonteCarlo::Object* TOPtr : checkObj)
	{
	  if (TOPtr->isValid(TP,surfState))
	    foundSet.emplace(TOPtr);
	}
      
      if (foundSet.size()!=1)
	{
	  ELog::EM<<"Flag["<<CylN<<" "<<PlnN<<"] -- :"
		  <<surfState[CylN]<<" "<<surfState[PlnN]<<ELog::endDiag;
	  for(const MonteCarlo::Object* fObj : foundSet)
	    ELog::EM<<"Valid["<<fObj->getName()<<"] "
		    <<fObj->getHeadRule().display(TP)<<"\n";
	  return 0;
	}
    } while (!MapSupport::iterateBinMap<int>(surfState,-1,1));

  return 1;
}
  
bool
SimValid::findTouch(const MonteCarlo::Object* OPtr,
		    const Geometry::Cylinder* CPtr,
		    const Geometry::Plane* PPtr,		    
		    std::vector<Geometry::interPoint>& TPts)
  /*! 
    Check if an object has a touch
    \param OPtr :: Object Ptr
    \param CPtr :: Cylinder to check
    \param PPtr :: Plane to check
    \param TPts :: Points found [OUTPUT]
   */
{
  ELog::RegMethod RegA("SimValid","findTouch");

  const double touchTol(1e-6);
  const Geometry::Vec3D& COrg=CPtr->getCentre();
  const Geometry::Vec3D& CAxis=CPtr->getNormal();
  const double R=CPtr->getRadius();
  const Geometry::Vec3D& PAxis=PPtr->getNormal();
  const double dProd=CAxis.dotProd(PAxis);

  if (std::abs(dProd)<touchTol)
    {
      // signed value:
      const double dist=PPtr->distance(COrg);
      const double distA=std::abs(dist);
      if (std::abs(distA-R) < touchTol)
	{
	  Geometry::Vec3D tPoint=COrg-PAxis*dist;
	  // Line of intersection
	  const HeadRule mainHR=OPtr->getHeadRule();
	  mainHR.calcSurfIntersection(tPoint,CAxis,TPts);
	  if (TPts.size()>1) return 1;
	}
    }
  return 0;
}
  
void
SimValid::calcTouch(const Simulation& System) const
  /*!
    Calculate touches between curved and non-curved
    surfaces
    \parma System :: Simulation to use
  */
{
  ELog::RegMethod RegA("SimValid","calcTouch");
  const Simulation::OTYPE OMap=System.getCells();
  for(const auto [cn,OPtr] : OMap)
    {
      std::set<const Geometry::Cylinder*> CylSet;
      std::set<const Geometry::Plane*> PlaneSet;
      const std::set<const Geometry::Surface*>& SSet=
	OPtr->getSurfPtrSet();
      // Construct set of cylinder / plane
      for(const Geometry::Surface* SPtr : SSet)
	{
	  const Geometry::Cylinder* CPtr=
	    dynamic_cast<const Geometry::Cylinder*>(SPtr);
	  if (CPtr)CylSet.emplace(CPtr);
	  const Geometry::Plane* PPtr=
	    dynamic_cast<const Geometry::Plane*>(SPtr);
	  if (PPtr) PlaneSet.emplace(PPtr);
	}
      // Check each plane/surface pair for touch
      // if so add to a list of line points for each
      // and check all other objects:
      for(const Geometry::Cylinder* CPtr : CylSet)
	for(const Geometry::Plane* PPtr : PlaneSet)
	  {
	    std::vector<Geometry::interPoint> TPts;
	    if (findTouch(OPtr,CPtr,PPtr,TPts))
	      {
		const int CylN=CPtr->getName();
		const int PlnN=PPtr->getName();
		// Now find set of flags which OPtr is NOT valid for all the points
		// as at this point both can be true / false.
		// we assume that only objects that have both surfaces can be involved:
			    
		std::set<const MonteCarlo::Object*> checkObj;   
		for(const auto [cnB,BOPtr] : OMap)
		  {
		    const std::set<int>& BSet=
		      BOPtr->getHeadRule().getSurfaceNumbers();
		    const bool flagA=BSet.find(CylN)!=BSet.end();
		    const bool flagB=BSet.find(PlnN)!=BSet.end();
		    if (flagA | flagB)
		      checkObj.emplace(BOPtr);  // note this INCLUDES OPtr
		  }
		Geometry::Vec3D testPt;
		bool outFlag(0);   
		size_t indexA(0),indexB(0),indexC(0);
		while(!outFlag && nextPoint(TPts,indexA,indexB,indexC,testPt))
		  {
		    if (!checkPoint(testPt,checkObj,CylN,PlnN))
		      {
			ELog::EM<<"TN == "<<TPts.size()<<" :: "<<testPt<<ELog::endDiag;
			ELog::EM<<"Objects -> Primary["<<OPtr->getName()<<"] \n"<<ELog::endCrit;
			ELog::EM<<"        -> Primary "<<*OPtr;
			ELog::EM<<"        ---------- "<<ELog::endDiag;
			outFlag=1;
		      }
		  }
	      }
	  }
    }
  return;
}


bool
SimValid::runUnit(const Simulation& System,
		  const Geometry::Vec3D& initPos,
		  const Geometry::Vec3D& axis,
		  MonteCarlo::Object* initObj) 
  /*!
    Runs a single unit:
    \param System :: Simulation
    \param initPos :: Inital position
   */
{
  static ModelSupport::LineTrack LT(initPos,axis,1e38);

  LT.setPts(initPos,axis);
  LT.clearAll();
  //  const ModelSupport::ObjSurfMap* OSMPtr =System.getOSM();

  return (LT.calculate(System,initObj)) ? 0 : 1;
} 

void
SimValid::diagnostics(const Simulation& System,
		     const std::vector<simPoint>& Pts) const
  /*!
    Write out some diagnostic information
    \param System :: simuation to use
    \param Pts :: Points in track
   */
{
  ELog::RegMethod RegA("SimValid","diagnostics");

  const ModelSupport::ObjSurfMap* OSMPtr =System.getOSM();
    
  ELog::EM<<"------------"<<ELog::endCrit;

  for(size_t j=0;j<Pts.size();j++)
    {
      ELog::EM<<"Pos["<<j<<"]=="<<Pts[j].Pt<<" :: Obj:"
	      <<Pts[j].objN<<" Surf:"<<Pts[j].surfN<<ELog::endDiag;
    }
  
  if (Pts.size()>=3)
    {
      double aDist;
      const Geometry::Surface* SPtr;          // Output surface
      const size_t index(Pts.size()-3);
      MonteCarlo::eTrack TNeut(Pts[index].Pt,Pts[index].Dir);
                                      
      ELog::EM<<"Base Obj == "<<*Pts[index].OPtr<<ELog::endDiag;
      ELog::EM<<"Next Obj == "<<*Pts[index+1].OPtr<<ELog::endDiag;
      // RESET:
      TNeut.Pos=Pts[index].Pt;  // Reset point
      const MonteCarlo::Object* OPtr=Pts[index].OPtr;
      const int SN=Pts[index].surfN;

      ELog::EM<<"RESET POS== "<<TNeut.Pos<<ELog::endDiag;
      ELog::EM<<"RESET Obj== "<<OPtr->getName()<<ELog::endDiag;
      ELog::EM<<"RESET SurfN== "<<Pts[index].surfN<<ELog::endDiag;
      ELog::EM<<"----------------------------------"<<ELog::endDiag ;
      OPtr=OSMPtr->findNextObject(Pts[index].surfN,
				  TNeut.Pos,OPtr->getName());	    
      if (OPtr)
	{
	  ELog::EM<<"Found Obj == "<<*OPtr<<" :: "<<Pts[index].Pt<<" "
		  <<OPtr->pointStr(Pts[index].Pt)<<ELog::endDiag;
	  ELog::EM<<OPtr->isValid(Pts[index].Pt)<<ELog::endDiag;
	}
      else
	ELog::EM<<"No object "<<ELog::endDiag;

      TNeut.Pos += TNeut.uVec*0.00001;
      
      MonteCarlo::Object* NOPtr=System.findCell(TNeut.Pos,0);
      if (NOPtr)
	{
	  ELog::EM<<"ETrack == "<<TNeut<<ELog::endDiag;
	  ELog::EM<<"Actual object == "<<*NOPtr<<ELog::endDiag;
	  ELog::EM<<" IMP == "<<NOPtr->isZeroImp()<<ELog::endDiag;
	}
      ELog::EM<<"TRACK to NEXT"<<ELog::endDiag;
      ELog::EM<<"--------------"<<ELog::endDiag;
      
      OPtr->trackCell(TNeut.Pos,TNeut.uVec,aDist,SPtr,abs(SN));
    }

  return;
}
  
int
SimValid::runPoint(const Simulation& System,
		   const Geometry::Vec3D& CP,
		   const size_t nAngle) const
  /*!
    Calculate the tracking
    \param System :: Simulation to use
    \param CP :: Centre point
    \param nAngle :: Number of points to test
    \return true if valid
  */
{
  ELog::RegMethod RegA("SimValid","runPoint");
  
  std::set<Geometry::Vec3D> MultiPoint;
  const ModelSupport::ObjSurfMap* OSMPtr =System.getOSM();
  MonteCarlo::Object* InitObj(0);
  const Geometry::Surface* SPtr;          // Output surface
  double aDist;       

  // Note for sphere that you can use X,Y,Z in any orthogonal 
  // directiron
  double phi,theta;

  // Find Initial cell [Store for next time]
  //  Centre+=Geometry::Vec3D(0.001,0.001,0.001);
  int initSurfNum(0);
  Geometry::Vec3D Pt(CP);
  //  Pt=Geometry::Vec3D(1443.65575933,1317.51292257,20.8777340012);
  do
    {
      if (initSurfNum)
	{
	  Pt+=Geometry::Vec3D(Random::rand()*0.01,
			      Random::rand()*0.01,
			      Random::rand()*0.01);
	}
      InitObj=System.findCell(Pt,InitObj);
      initSurfNum=InitObj->isOnSurface(Pt);
    }
  while(initSurfNum);
      
  // check surfaces
  ELog::EM<<"NAngle == "<<nAngle<<" :: "<<CP<<ELog::endDiag;
  double fullTime(0.0);
  for(size_t i=0;i<nAngle;i++)
    {
      if (nAngle>10000 && i*10==nAngle)
	ELog::EM<<"ValidPoint Angle[ == "<<i<<"]"<<ELog::endDiag;
      std::vector<simPoint> Pts;
      // Get random starting point on edge of volume
      phi=Random::rand()*M_PI;
      theta=2.0*Random::rand()*M_PI;
      Geometry::Vec3D uVec(cos(theta)*sin(phi),
			     sin(theta)*sin(phi),
			     cos(phi));

      MonteCarlo::eTrack THold(Pt,uVec);
      MonteCarlo::Object* OPtr=InitObj;
      int SN(-initSurfNum);

      Pts.push_back(simPoint(Pt,uVec,OPtr->getName(),SN,OPtr));

      bool outFlag=runUnit(System,Pt,uVec,InitObj);
      if (!outFlag)
	{
	  bool newFlag=runUnit(System,Pt,uVec,InitObj);
	  ELog::EM<<"NEW FLAG == "<<newFlag<<ELog::endDiag;
	  ELog::EM<<"OPtr not found["<<i<<"] at : "<<Pt<<ELog::endCrit;
	  ELog::EM<<"EHOLD:"<<THold<<ELog::endCrit;
	  ELog::EM<<"EHOLD:"<<CP<<ELog::endCrit;
	  ELog::EM<<"Line SEARCH == "<<i<<ELog::endCrit;
	  for(const simPoint& SP : Pts)
	    ELog::EM<<"Pt == "<<SP<<ELog::endDiag;
	  ModelSupport::LineTrack LT(Pt,uVec,10000.0);
	  LT.calculate(System);
	  ELog::EM<<"LT == "<<LT<<ELog::endDiag;
	  ELog::EM<<"END Line SEARCH == "<<ELog::endCrit;
	  
	  if (!InitObj)
	    ELog::EM<<"Failed to calculate INITIAL cell correctly: "<<ELog::endCrit;
	  else
	    ELog::EM<<"Initial Cell ="<<*InitObj<<ELog::endDiag;
	  //	  diagnostics(System,Pts);
	  return 0;
	}

    }
  return 1;
}

int
SimValid::runFixedComp(const Simulation& System,
		       const size_t N) const
  /*!
    Calculate the tracking from fixedcomp
    \param System :: Simulation to use
    \param N :: Number of points to test
    \return true if valid
  */
{
  ELog::RegMethod RegA("SimValid","runFixedComp");
  

  typedef std::shared_ptr<attachSystem::FixedComp> CTYPE;
  typedef std::map<std::string,CTYPE> cMapTYPE;

  const cMapTYPE& CM=System.getComponents();

  for(const cMapTYPE::value_type& FCitem : CM)
    {
      const CTYPE& FC = FCitem.second;
      const std::vector<Geometry::Vec3D> FCPts=
	FC->getAllLinkPts();
      for(const Geometry::Vec3D& Pt : FCPts)
	{
	  ELog::EM<<"PT == "<<FC->getKeyName()<<" :: "<<Pt<<ELog::endDiag;
	  runPoint(System,Pt,N);
	}
    }
  
  ELog::EM<<"Finished Validation check"<<ELog::endDiag;
  return 1;
}

int
SimValid::checkPoint(const Simulation& System,
		     const Geometry::Vec3D& Pt) 
  /*!
    Calculates if a point is within multiple cells:
    \param System :: Simulation to use
    \param Pt :: Point to test
    \return true if valid / 0 if invalid
  */
{
  ELog::RegMethod RegA("SimValid","checkPoint");

  const Simulation::OTYPE& cellMap=System.getCells();
  std::vector<const MonteCarlo::Object*> activeCell;

  for(const auto& [CN,OPtr] : cellMap)
    {
      if (OPtr->isValid(Pt))
	activeCell.push_back(OPtr);
    }

  if (activeCell.size()==1) return 0;  // good point
  
  // compare pairs
  for(size_t i=0;i<activeCell.size();i++)
    for(size_t j=i+1;j<activeCell.size();j++)
      {
	const MonteCarlo::Object* APtr=activeCell[i];
	const MonteCarlo::Object* BPtr=activeCell[j];

	// unsigned set:
	const std::set<int> ASurf=APtr->surfValid(Pt);
	const std::set<int> BSurf=BPtr->surfValid(Pt);
	
	std::set<int> commonSurf;
	std::set_intersection(ASurf.begin(),ASurf.end(),
			      BSurf.begin(),BSurf.end(),
			      std::inserter(commonSurf,commonSurf.begin()) );
	int errFlag(1);
	std::map<int,int> SNeg,SPlus;
	for(const int SN : commonSurf)
	  {
	    SNeg.emplace(SN,-1);
	    SPlus.emplace(SN,-1);
	  }
	for(const int SN : commonSurf)
	  {
	    // both reset to -1 state:
	    for(auto& [sideName,flag] : SNeg) flag=-1;
	    for(auto& [sideName,flag] : SPlus) flag=-1;
	    SNeg[SN]=-1;
	    SPlus[SN]=1;
	    do
	      {
		if ((APtr->isValid(Pt,SNeg) != BPtr->isValid(Pt,SNeg)) ||
		    (APtr->isValid(Pt,SPlus) != BPtr->isValid(Pt,SPlus)) )
		  {
		    errFlag=0;
		  }
	      }	 while(errFlag &&
		       !MapSupport::iterateBinMapLocked(SNeg,SN,-1,1) && 
		       !MapSupport::iterateBinMapLocked(SPlus,SN,-1,1));
	    if (!errFlag) break;
	  }
	if (errFlag)
	  {
	    ELog::EM<<std::setprecision(16);
	    ELog::EM<<"Central Point Check ERROR ::"<<Pt<<"\n";
	    for(const MonteCarlo::Object* OPtr : activeCell)
	      ELog::EM<<"Cell "<<OPtr->getName()<<"\n";
	    
	    ELog::EM<<ELog::endErr;
	  }
      }

  return 0;
}




} // NAMESPACE ModelSupport
