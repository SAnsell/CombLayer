/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chip/insertBaseInfo.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <functional>
#include <iterator>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/functional.hpp>

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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "MaterialSupport.h"
#include "surfFunctors.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "insertInfo.h"
#include "insertBaseInfo.h"

namespace shutterSystem
{

insertBaseInfo::insertBaseInfo(Simulation& S,
			       ModelSupport::surfRegister& SM) :
  SMapRef(SM),System(S),fullLength(0.0),fPlane(0),bPlane(0)
  /*!
    Constructor
    \param S :: Simulation to use
    \param SM :: Surface register to use
  */
{}

insertBaseInfo::insertBaseInfo(const insertBaseInfo& A) :
  SMapRef(A.SMapRef),System(A.System),
  PipeAxis(A.PipeAxis),PipeCent(A.PipeCent),
  Axis(A.Axis),Z(A.Z),Pt(A.Pt),ZOffset(A.ZOffset),
  fullLength(A.fullLength),
  fPlane(A.fPlane),bPlane(A.bPlane),
  Sides(A.Sides),
  Radius(A.Radius),Boundary(A.Boundary),
  Components(A.Components)
  /*!
    Copy Constructor
    \param A :: insertBaseInfo item to copy
  */
{}

insertBaseInfo&
insertBaseInfo::operator=(const insertBaseInfo& A) 
  /*!
    Assignment operator
    \param A :: insertBaseInfo item to copy
    \return *this
  */
{
  if (this!=&A)
    {
      PipeAxis=A.PipeAxis;
      PipeCent=A.PipeCent;
      Axis=A.Axis;
      Z=A.Z;
      Pt=A.Pt;
      ZOffset=A.ZOffset;
      fullLength=A.fullLength;
      fPlane=A.fPlane;
      bPlane=A.bPlane;
      Sides=A.Sides;
      Radius=A.Radius;
      Boundary=A.Boundary;
      Components=A.Components;
    }
  return *this;
}

void 
insertBaseInfo::setSides(const int A,const int B,const int C,const int D)
  /*!
    Set the four sides
    \param A :: Signed side
    \param B :: Signed side
    \param C :: Signed side
    \param D :: Signed side
   */
{
  ELog::RegMethod RegA("insertBaseInfo","setSides");
  Sides.clear();
  Sides.push_back(A);
  Sides.push_back(B);
  Sides.push_back(C);
  Sides.push_back(D);
  return;
}

void
insertBaseInfo::setInit(const Geometry::Vec3D& pP,const Geometry::Vec3D& pAX,
			const Geometry::Vec3D& P,const Geometry::Vec3D& AX)
 /*!
   Initializes the plane and axis of the cylinder
   \param pP :: Pipe Centre
   \param pAX :: Pipe Axis line
   \param P ::  Point for first centre (without shift)
   \param AX :: Axis line
  */
{
  PipeAxis=pAX.unit();
  PipeCent=pP;
  Pt=P;
  Axis=AX.unit();
  ZOffset=0;
  return;
}

std::string
insertBaseInfo::getFullSides() const 
  /*!
    Get a list of the full sides in the
    case that it is required
    \return FullSide string
  */
{
  std::ostringstream cx;
  copy(Sides.begin(),Sides.end(),std::ostream_iterator<int>(cx," "));
  return cx.str();
}

void
insertBaseInfo::populate(const std::string& Part)
  /*!
    Sets the population of object
    Read name as part:[RLM]:index
    \param Part :: Fractional name
   */
{
  FuncDataBase& Control=System.getDataBase();
  Components.clear();

  int Rcnt(1);
  int rFlag(1);
  int lFlag(1);
  int Lcnt;
  double TLength;
  //
  // This loop just creates the Component list:
  //  -- Later we check for completeness
  //  --  
  do
    {
      std::ostringstream cx;
      cx<<Part<<"R"<<Rcnt;
      if ( (rFlag=Control.hasVariable(cx.str())) )
        {
	  Lcnt=1;
	  const double Rx=Control.EvalVar<double>(cx.str());
 	  TLength=0.0;        // Forward step
	  do
	    {
	      std::ostringstream dx;
	      dx<<Rcnt<<"_"<<Lcnt;
	      if ( (lFlag=Control.hasVariable(Part+"L"+dx.str())) )
		{
		  const double Lx=Control.EvalVar<double>(Part+"L"+dx.str());
		  const int MatX=ModelSupport::EvalMat<int>
		    (Control,Part+"M"+dx.str());
		  Components.push_back(insertInfo(Lcnt,Rcnt,MatX,TLength,Rx,Lx));
		  Components.back().setCoordinates(Pt,Axis);
		  TLength+=Lx;
		  Lcnt++;
		}
	    } while(lFlag);
	}
      Rcnt++;
    } while (rFlag);
  // EXTERNAL:
  std::ostringstream cx;
  Lcnt=1;
  TLength=0.0;        // Forward step
  do
    {
      std::ostringstream dx;
      dx<<Lcnt;
      if ( (lFlag=Control.hasVariable(Part+"LE_"+dx.str())) )
        {
	  const double Lx=Control.EvalVar<double>(Part+"LE_"+dx.str());
	  const int MatX=ModelSupport::EvalMat<int>
	    (Control,Part+"ME_"+dx.str());
	  Components.push_back(insertInfo(Lcnt,0,MatX,TLength,0.0,Lx));
	  Components.back().setCoordinates(Pt,Axis);
	  TLength+=Lx;
	  Lcnt++;
	}
    } while(lFlag);
  
  return;
}

Geometry::Vec3D
insertBaseInfo::calcHorrVec() const
  /*!
    Calcuate the horrizontal vector
    \return Horizontal vector reative to Vertial (1,0,0)
    and the pipe direction
  */
{
  Geometry::Vec3D Pt(Axis*Geometry::Vec3D(1,0,0));
  Pt.makeUnit();
  return Pt;
}

std::vector<Geometry::Vec3D>
insertBaseInfo::calcSidesIntercept(const int CyNum,const int testPlane) const
  /*!
    Calcuate the intersection of the sides with the cylinder and the testPlane
    \param CyNum :: Cylinder Number
    \param testPlane :: Addition signed plane to test points from intersect
    \return Vector of 4 point intersect
  */
{
  ELog::RegMethod RegA("insertBaseInfo","calcSidesIntercept");

  std::vector<Geometry::Vec3D> Out;
  const Geometry::Surface* CySurf=SMapRef.realSurfPtr(CyNum);
  const Geometry::Surface* TestSurf=SMapRef.realSurfPtr(abs(testPlane));
  const int sign((testPlane>0) ? 1 : -1);
  if (!CySurf)
    throw ColErr::InContainerError<int>(CyNum,RegA.getBase());
  if (!TestSurf && testPlane)
    throw ColErr::InContainerError<int>(testPlane,RegA.getBase());
  
  // Calcualte edge intersecton
  std::vector<const Geometry::Surface*> Edges;
  if (Sides.size()<4)
    {
      ELog::EM<<"Failed to set sides "<<Sides.size()<<ELog::endErr;
      throw ColErr::IndexError<size_t>(Sides.size(),4,RegA.getBase());
    }
  for(size_t i=0;i<4;i++)
    {
      Edges.push_back(SMapRef.realSurfPtr(abs(Sides[i])));
      if (!Edges.back())
	throw ColErr::InContainerError<int>(Sides[i],RegA.getBase());
    }
  // Points are a-b, c-d hence swap b/c
  std::swap(Edges[1],Edges[2]);
  // Calculate the points at the guides

  for(size_t i=0;i<4;i++)
    {
      std::vector<Geometry::Vec3D> PtOut=
	SurInter::processPoint(Edges[i],Edges[(i+1) % 4],CySurf);
      if (testPlane)
        {
	  std::vector<Geometry::Vec3D>::iterator vc=
	    remove_if(PtOut.begin(),PtOut.end(),
		      boost::bind(&Geometry::surfaceCheck,sign,TestSurf,_1));
	  PtOut.erase(vc,PtOut.end());
	}
      if (!PtOut.empty())
	Out.push_back(PtOut.front());
    }  
  return Out;
}

std::string
insertBaseInfo::cylinderIntercept(const int CyNum,
				  const int FP,const int BP) const
 /*!
    Edge planes are Top:Bottom / Side:Side
    \param CyNum :: Axial Cylinder to test
    \param FP :: Front plane
    \param BP :: Back plane
    \return string for cell inclusion
 */
{
  ELog::RegMethod RegA("insertBaseInfo","cyclinerIntercept");

  std::vector<const Geometry::Quadratic*> Edges;
  for(size_t i=0;i<4;i++)
    Edges.push_back(SMapRef.realPtr<const Geometry::Quadratic>(abs(Sides[i])));

  const Geometry::Cylinder* Axiel=SMapRef.realPtr<const Geometry::Cylinder>(CyNum);
  const Geometry::Plane* FrontP=SMapRef.realPtr<const Geometry::Plane>(FP);
  const Geometry::Quadratic* BackP=SMapRef.realPtr<const Geometry::Quadratic>(BP);

  std::vector<size_t> outIndex; // results

  // NEW 
  for(size_t i=0;i<4;i++)
    {
      std::vector<Geometry::Vec3D> OutPt=
	SurInter::makePoint(Edges[i],FrontP,Axiel);
      if (!OutPt.empty())  
	outIndex.push_back(i);
      else
        {
	  OutPt=SurInter::makePoint(Edges[i],Axiel,BackP);
	  if (!OutPt.empty())
	    outIndex.push_back(i);
	}
    }


  std::vector<size_t>::const_iterator vc;
  std::ostringstream cx;
  for(vc=outIndex.begin();vc!=outIndex.end();vc++)
    cx<<Sides[*vc]<<" ";

  return cx.str();
}

void
insertBaseInfo::createObjects(const int initSurf,int& initCell)
  /*!
    Processes the info to start
    Requires a sorted Components list
    \param initSurf :: First surface number 
    \param initCell :: First cell number
  */
{
  ELog::RegMethod RegA("insertBaseInfo","createObjects");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // BUILD Surfaces Cylinders
  sort(Components.begin(),Components.end(),insertInfoSort());

  int surfNum=initSurf;                     // New surface number
  std::vector<insertInfo>::iterator vc;

  int rItem(0);
  int innerPlane(0);
  int outerPlane(0);
  // CONSTRUCT Cylinders:
  for(vc=Components.begin();vc!=Components.end();vc++)
    {
      // Create Cylinders:
      if (vc->rNum!=rItem)           // New cylinder:
        {
	  if (vc->rNum)              // Is internal:
	    {
	      Geometry::Cylinder* CPtr=
		SurI.createUniqSurf<Geometry::Cylinder>(surfNum);
	      CPtr->setCylinder(vc->CPt+Z*ZOffset,vc->Axis,vc->ORad);
	      vc->outerRID=SMapRef.registerSurf(surfNum++,CPtr);
	      // CPtr=SMapRef.realPtr<Geometry::Cylinder>(vc->outerRID);
	    }
	  // This is always executed
	  innerPlane=outerPlane;
	  outerPlane=vc->outerRID;
	  vc->innerRID=innerPlane;
	  rItem=vc->rNum;
	}
      else          // Continuation
        {
	  vc->outerRID=outerPlane;
	  vc->innerRID=innerPlane;
	}
    }
  
  // CONSTRUCT Planes:
  for(vc=Components.begin();vc!=Components.end();vc++)
    {
      // Back (long) plane
      Geometry::Plane* PPtr=SurI.createUniqSurf<Geometry::Plane>(surfNum++);
      PPtr->setPlane(vc->calcStepPoint()+Z*ZOffset,vc->Axis);
      vc->bPlane=SMapRef.registerSurf(PPtr);  

      // Front (short) Plane
      if (fabs(vc->Doffset)>Geometry::shiftTol)
        {
	  PPtr=SurI.createUniqSurf<Geometry::Plane>(surfNum++);	  
	  PPtr->setPlane(vc->calcFrontPoint()+Z*ZOffset,vc->Axis);
	  vc->fPlane=SMapRef.registerSurf(PPtr);  
	  // PPtr=dynamic_cast<Geometry::Plane*>(SMapRef.realSurfPtr(surfNum));
	}
      else  // special case of no offset:
       {
	 vc->fPlane=fPlane;
       }
    }
  continuityCheck();
  createSides();
  // BUILD OBJECTS:
  for(vc=Components.begin();vc!=Components.end();vc++)
    System.addCell(MonteCarlo::Qhull(initCell++,
				     vc->matID,0.0,vc->cellString()));
  return;
}

void
insertBaseInfo::createSides()
  /*!
    Creates side values
  */
{
  ELog::RegMethod RegA("insertBaseInfo","createSides");

  std::vector<insertInfo>::iterator vc;
  for(vc=Components.begin();vc!=Components.end();vc++)
    {
      if (vc->outerRID)
	vc->Sides=cylinderIntercept(vc->outerRID,fPlane,bPlane);
      else 
	vc->Sides=fullSides();
    }
  return;
}

std::string
insertBaseInfo::fullSides() const 
  /*!
    Creates a full sides list
    \return sides string in MCNPX format
  */
{
  ELog::RegMethod RegA("insertBaseInfo","fullSides");

  std::ostringstream cx;
  std::vector<int>::const_iterator vc;
  for(vc=Sides.begin();vc!=Sides.end();vc++)
    cx<<SMapRef.realSurf(*vc)<<" ";
  return cx.str();
}


int
insertBaseInfo::continuityCheck()
  /*!
    Handle the problem of objects drifting out of range
    Currently only adds voids at the end of each cylinder
    block to make up the maximum distance.
    \return Number of items added
  */
{
  ELog::RegMethod RegA("insertBaseInfo","continuityCheck");

  // Sort through an put in the inner/outer radius numbers:
  //  int innerN(Components.front().rNum);
  //  int currentR(Components.front().outerRID);

  std::vector<insertInfo>::iterator vc;
  double maxLength(0.0);
  insertInfo* maxComp(0);
  for(vc=Components.begin();vc!=Components.end();vc++)
    {
      // get max length
      if (vc->calcFullLength()>maxLength)
        {
	  maxLength=vc->calcFullLength();
	  maxComp=&(*vc);
	}
    }
  if (!maxComp) return 0;
  bPlane=maxComp->bPlane;
  const double FL=maxComp->calcFullLength();
  // Create Additional void items (or modify trailing void items)
  unsigned int i(1);
  while(i<=Components.size())  // Size of Components changes in this loop
    {
      // Want last item , so get next first:
      // Adjusted to also pick up External item
      if ((i==Components.size() || Components[i].lNum==1) 
	  && Components[i-1].bPlane!=bPlane)
        {
	  insertInfo& IR=Components[i-1];
	  if (IR.matID!=0)
	    {
	      const double lastLen=IR.calcFullLength();	      
	      Components.insert(Components.begin()+1,
				insertInfo(bPlane,FL-lastLen,IR));
	    }
	  else 
	    {
	      // modify for increase length
	      IR.bPlane=bPlane;
	      IR.Length=FL-IR.Doffset;
	    }
	}
      i++;
    }
  fullLength=maxLength;

//  printComponents();

  // COJOIN:

  
  return 0;
}

void
insertBaseInfo::printComponents() const
  /*!
    Display the components to be created
  */
{
  // Now loop over all Components and check that they join:
  ELog::EM<<"Id matID innerRID outerID fPlane bPlane"
	  <<" : Doffset Length ORad"<<ELog::endDiag;
  copy(Components.begin(),Components.end(),
       std::ostream_iterator<insertInfo>(ELog::EM.Estream(),"\n"));
  ELog::EM.diagnostic();
  return;
}


}  // NAMESPACE shutterSystem
