/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/boxUnit.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <cmath>
#include <complex> 
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <stack>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <memory>
 
#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "stringCombine.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "Line.h"
#include "Qhull.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "AttachSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LineTrack.h"
#include "pipeSupport.h"
#include "boxValues.h"
#include "boxUnit.h"

#include "localRotate.h"
#include "masterRotate.h"

namespace ModelSupport
{

boxUnit::boxUnit(const std::string& Key,const size_t index) : 
  attachSystem::FixedComp(StrFunc::makeString(Key,index),6),
  attachSystem::ContainedComp(),
  surfIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(surfIndex+1),prev(0),next(0),maxExtent(0.0),
  activeFlag(0),nSides(0)
 /*!
   Constructor
   \param Key :: keyname (base)
   \param index ::  offset number
  */
{}

boxUnit::boxUnit(const boxUnit& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),prev(A.prev),
  next(A.next),APt(A.APt),BPt(A.BPt),Axis(A.Axis),
  ANorm(A.ANorm),BNorm(A.BNorm),XUnit(A.XUnit),ZUnit(A.ZUnit),
  maxExtent(A.maxExtent),
  activeFlag(A.activeFlag),boxVar(A.boxVar),nSides(A.nSides)
  /*!
    Copy constructor
    \param A :: boxUnit to copy
  */
{}

boxUnit&
boxUnit::operator=(const boxUnit& A)
  /*!
    Assignment operator
    \param A :: boxUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      APt=A.APt;
      BPt=A.BPt;
      Axis=A.Axis;
      ANorm=A.ANorm;
      BNorm=A.BNorm;
      XUnit=A.XUnit;
      ZUnit=A.ZUnit;
      maxExtent=A.maxExtent;
      activeFlag=A.activeFlag;
      boxVar=A.boxVar;
      nSides=A.nSides;
    }
  return *this;
}

boxUnit::~boxUnit()
  /*!
    Destructor
  */
{}

size_t
boxUnit::getOuterIndex() const
  /*!
    Determine the outer index
    \return Index of outer unit 
  */
{
  ELog::RegMethod RegA("boxUnit","getOuterIndex");

  if (boxVar.empty()) 
    throw ColErr::EmptyValue<int>("boxVar vector empty");

  size_t index=boxVar.size()-1;
  if (!activeFlag) return index;
  size_t bitIndex=1 << (index+1);
  do
    {
      bitIndex>>=1;
    } while( !(bitIndex & activeFlag) && --index>0);

  return index;
}

void
boxUnit::setZUnit(const Geometry::Vec3D& ZA) 
  /*!
    Set the Z unit
    \param ZA :: Zaxis
  */
{
  ELog::RegMethod RegA("boxUnit","setZUnit");
  if (ZA.abs()<Geometry::zeroTol)
    ELog::EM<<"ZAxis unset "<<ELog::endErr;
  ZUnit=ZA.unit();
  return;
}


void
boxUnit::setASurf(const HeadRule& AR)
  /*!
    Set the initial surface
    \param AR :: Initial surface rule
  */
{
  ASurf=AR;
  return;
}

void
boxUnit::setBSurf(const HeadRule& BR)
  /*!
    Set the final surface
    \param BR :: Final surface rule
  */
{
  BSurf=BR;
  return;
}


void
boxUnit::setPoints(const Geometry::Vec3D& A,
		   const Geometry::Vec3D& B)
  /*!
    Allocate the points
    \param A :: First Point
    \param B :: Second point
  */
{
  ELog::RegMethod RegA("boxUnit","setPoints");
  
  Axis=(B-A).unit();
  APt=A;
  BPt=B;
  return;
}

void
boxUnit::calcNorm(const int PN,
		   const Geometry::Vec3D& AAxis,
		   const Geometry::Vec3D& BAxis)
   /*!
     Calculate the surface normal value from Pt on another Axis
     \param PN :: Point direction [0==Base/1==top]
     \param AAxis :: First joining axis
     \param BAxis :: second Joining Axis
   */
{
  ELog::RegMethod RegA("boxUnit","calcNorm");

  Geometry::Vec3D Norm=(BAxis+AAxis)/2.0;
  if (PN)
    BNorm=Norm;
  else    
    ANorm=Norm;
  return;
}

void 
boxUnit::checkForward()
  /*!
    Sets the next item if necessary
   */
{
  if (next)
    next->connectFrom(this);
  return;
}

void
boxUnit::populate(const size_t AF,const std::vector<boxValues>& CV)
  /*!
    Popuate everything assuming prev / next have been set
    along with APt and BPt 
    \param AF :: Axis flag
    \param CV :: Units
  */
{
  ELog::RegMethod RegA("boxUnit","populate");

  activeFlag=AF;
  boxVar=CV;

  checkForward();
  if (prev) ZUnit=prev->getZUnit();
  const Geometry::Vec3D& pAxis((prev) ? prev->getAxis() : Axis);
  const Geometry::Vec3D& nAxis((next) ? -next->getAxis() : -Axis);
  calcNorm(0,Axis,pAxis);
  calcNorm(1,-Axis,nAxis);
  calcXZ(pAxis,Axis);
  return;
}

void
boxUnit::calcXZ(const Geometry::Vec3D& PA,
		const Geometry::Vec3D& PB)
  /*!
    Calculate the Z-axis
    \param PA :: Previous Axis
    \param PB :: Unit vector [next point]
  */
{
  ELog::RegMethod RegA("boxUnit","calcXZ");

  const double cosA=PA.dotProd(PB);

  // same as privious:
  XUnit=ZUnit*PA;

  // No change:
  if (fabs(cosA-1.0)<Geometry::zeroTol)
    return;


  // Rotation vector:
  const double Angle=acos(cosA)/2.0;
  Geometry::Vec3D RotA=PA*PB;
  RotA*=sin(Angle);
  Geometry::Quaternion QR(cos(Angle),RotA);
  QR.rotate(ZUnit);
  QR.rotate(XUnit);

  return;
}

const Geometry::Vec3D&
boxUnit::getPt(const int side) const
  /*!
    Return a specific connection point
    \param side :: Side of object
    \return centre connection point
  */
{
  return (!side) ? APt : BPt;
}

void
boxUnit::connectFrom(boxUnit* Ptr) 
  /*!
    Set the previous connection
    \param Ptr :: Pointer to use
  */
{
  prev=Ptr;
  return;
}

void
boxUnit::connectTo(boxUnit* Ptr) 
  /*!
    Set the connection to the next object
    \param Ptr :: Pointer to use
  */
{
  next=Ptr;
  return;
}

void
boxUnit::createSurfaces() 
  /*!
    Create Surface components
  */
{
  ELog::RegMethod RegA("boxUnit","createSurface");

  // top / bottom plane
  if (!ASurf.hasRule())
    {
      ModelSupport::buildPlane(SMap,surfIndex+1,APt,ANorm);
      ASurf=HeadRule(StrFunc::makeString(SMap.realSurf(surfIndex+1)));
      ASurf.populateSurf();
    }
  if (!BSurf.hasRule())
    {
      ModelSupport::buildPlane(SMap,surfIndex+2,BPt,BNorm);
      BSurf=HeadRule(StrFunc::makeString(SMap.realSurf(surfIndex+2)));
      BSurf.populateSurf();
    }
  
  // No need to delete surfaces [in SMap]
  nSides=boxVar.back().getSides();    

  size_t bitIndex(1);    // Index for flag bit
  int surfOffset(surfIndex);         // Cnt
  for(size_t i=0;i<boxVar.size();i++)      // layers:
    {
      if (!activeFlag || (activeFlag & bitIndex))
	{
	  const double ExtVal=boxVar[i].getExtent();
	  if (ExtVal>maxExtent) maxExtent=ExtVal;

	  int cnt(3);
	  const boxValues& boxVal(boxVar[i]);
	  for(size_t j=0;j<nSides;j++)
	    {
	      // ELog::EM<<"Point["<<j<<"] == "<<
	      // 	boxVal.getDatum(j,APt,XUnit,ZUnit)<<":"<<
	      // 	boxVal.getAxis(j,XUnit,ZUnit)<<ELog::endDiag;
	      ModelSupport::buildPlane(SMap,surfOffset+cnt,
				       boxVal.getDatum(j,APt,XUnit,ZUnit),
				       boxVal.getAxis(j,XUnit,ZUnit));
	      cnt++;
	    }
	}
      surfOffset+=10;
      bitIndex<<=1;
    }
  return;
}

std::string
boxUnit::createCaps() const
  /*!
    Creates the caps with the appropiate surfaces
    \return caps string [inward pointing]
  */
{
  ELog::RegMethod RegA("pipeUnit","createCap");

  return ASurf.display()+" "+BSurf.display(); 
}

const HeadRule&
boxUnit::getCap(const int side) const
  /*!
    Get the end cap head rule
    \param side :: side value (true => end)
    \return cap rule
   */
{
  return (side) ? BSurf : ASurf;
}

void
boxUnit::createOuterObjects()
  /*!
    Construct the object and the outside bracket
    for the system
  */
{
  ELog::RegMethod RegA("boxUnit","createOuterObject");

  const size_t outerIndex=getOuterIndex();
  const int SI(surfIndex+static_cast<int>(outerIndex)*10);

  const std::string FBSurf=createCaps();

  // top / bottom plane
  std::ostringstream outerCX;
  for(int j=0;j<static_cast<int>(nSides);j++)
    outerCX<<-(j+3)<<" ";
  const std::string Out=
    ModelSupport::getComposite(SMap,SI,outerCX.str());
  addOuterSurf(Out+FBSurf);
  return;
}

void
boxUnit::createObjects(Simulation& System)
  /*!
    Construct the object and the outside bracket
    for the system
    \param System :: Simulation to build into
   */
{
  ELog::RegMethod RegA("boxUnit","createObjects");

  const std::string FBSurf=createCaps();  

  std::string Out;
  int SI(surfIndex);
  int SIprev(0);
  size_t bitIndex(1);
  // Sides string
  std::ostringstream outerCX;
  std::ostringstream innerCX;
  outerCX<<" ";
  innerCX<<" ( ";
  for(int j=0;j<static_cast<int>(nSides);j++)
    { 
      outerCX<<-(j+3)<<" ";
      if (j)
	innerCX<<":"<<(j+3);
      else
	innerCX<<(j+3);
    }
  innerCX<<") ";
  for(size_t i=0;i<boxVar.size();i++)
    {
      if (!activeFlag || (activeFlag & bitIndex))
	{
	  Out=ModelSupport::getComposite(SMap,SI,outerCX.str());
	  if (SIprev)
	    Out+=ModelSupport::getComposite(SMap,SIprev,innerCX.str());
	  Out+=FBSurf;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,
					   boxVar[i].getMat(),
					   boxVar[i].getTemp(),Out));
	  SIprev=SI;
	}
    }
  // Final outer layer is controlled by SIprev
  Out=ModelSupport::getComposite(SMap,SIprev,outerCX.str());
  Out+=FBSurf;
  addOuterSurf(Out);
  return;
}


void
boxUnit::calcLineTrack(Simulation& System,
		       const Geometry::Vec3D& XP,
		       const Geometry::Vec3D& YP,
		       std::map<int,MonteCarlo::Object*>& OMap) const
  /*!
    From a line determine thos points that the system
    intersect
    \param System :: Simuation to use
    \param XP :: A-Point of line
    \param YP :: B-Point of line
    \param OMap :: Object Map
  */
{
  ELog::RegMethod RegA("boxUnit","calcLineTrack");
  
  typedef std::map<int,MonteCarlo::Object*> MTYPE;
  
  LineTrack LT(XP,YP);
  LT.calculate(System);
  const std::vector<MonteCarlo::Object*>& OVec=LT.getObjVec();
  std::vector<MonteCarlo::Object*>::const_iterator oc;
  for(oc=OVec.begin();oc!=OVec.end();oc++)
    {
      const int ONum=(*oc)->getName();
      if (OMap.find(ONum)==OMap.end())
	OMap.insert(MTYPE::value_type(ONum,(*oc)));
    }
  return;
}

void
boxUnit::addInsertSet(const std::set<int>& S)
  /*!
    Insert the extra cell set into cellCut set
    \param S :: Set of cells to insert
  */
{
  cellCut.insert(S.begin(),S.end());
  return;
}

void
boxUnit::clearInsertSet()
  /*!
    Clear the insert set
   */
{
  cellCut.clear();
  return;
}
  
void
boxUnit::insertObjects(Simulation& System)
  /*!
    Insert the objects into the main simulation. It is separated
    from creation since we need to determine those object that 
    need to have an exclude item added to them.
    \param System :: Simulation to add object to
  */
{
  ELog::RegMethod RegA("boxUnit","insertObjects");

  System.populateCells();
  System.validateObjSurfMap();
    
  typedef std::map<int,MonteCarlo::Object*> MTYPE;
  MTYPE OMap;         // Set on index
  
  // Build track
  Geometry::Vec3D Axis(BPt-APt);
  Axis.makeUnit();
  const Geometry::Vec3D AX(Axis.crossNormal());
  const Geometry::Vec3D AY(AX*Axis);
  
  // Main loop over size:
  
  // This is a stack of the fractions between the 
  // Stack : low-frac high-frac Level
  typedef std::pair<double,double> DT;
  std::stack<DT> FStack;      // Fractional Stack:
  FStack.push(DT(0.0,1.0));
  
  const Geometry::Vec3D AEPt=getAExtra();
  const Geometry::Vec3D BEPt=getBExtra();
  std::vector<Geometry::Vec3D> AVec;
  std::vector<Geometry::Vec3D> BVec;

  // Outer points [Mid outer planes]:
  const boxValues& boxVal(boxVar.back());
  nSides=boxVal.getSides();      
  for(size_t j=0;j<nSides;j++)
    {
      AVec.push_back(boxVal.getDatum(j,AEPt,XUnit,ZUnit));
      BVec.push_back(boxVal.getDatum(j,BEPt,XUnit,ZUnit));
    }
  
  // Do special cases on origin and edge points:
  calcLineTrack(System,APt,BPt,OMap);
  for(size_t j=0;j<nSides;j++)
    {
      const Geometry::Vec3D& XP=AVec[j];
      const Geometry::Vec3D& YP=BVec[j];
      calcLineTrack(System,XP,YP,OMap);
    }
  size_t Cnt(OMap.size());
  

  // Loop over FSTACK : 
  // This finds the mid-point between the outer points and the 
  // origin and check to see if a new object is found. 
  // if so add that to the object map AND redivide the track
  while(!FStack.empty())
    {
      DT SPoint=FStack.top();
      FStack.pop();
      const double frac((SPoint.first+SPoint.second)/2.0);
      for(size_t j=0;j<nSides;j++)
	{
	  Geometry::Vec3D XP=AVec[j]*frac+AEPt*(1.0-frac);
	  Geometry::Vec3D YP=BVec[j]*frac+BEPt*(1.0-frac);
	  calcLineTrack(System,XP,YP,OMap);
	}
      if (Cnt!=OMap.size())
	{
	  Cnt=OMap.size();
	  FStack.push(DT(SPoint.first,frac));
	  FStack.push(DT(frac,SPoint.second));
	}
    }

  addExcludeStrings(OMap);
  return;
}

double
boxUnit::getTanAngle(const Geometry::Vec3D& OAxis) const
  /*!
    Determine the angle 
    \param OAxis : Original primary axis
   */
{
  // Both normalized [Assumed]
  const double A=OAxis.dotProd(Axis);  
  return tan(0.5*acos(A));
}

Geometry::Vec3D
boxUnit::getAExtra() const
  /*!
    Determine the extent point of the dividing plane on the 
    system. This is approximate and an over estimate [hence
    find for determine those objects to have an insert into
    \return centre extent point.
  */
{
  if (!prev) return APt;
  const double talpha=prev->getTanAngle(Axis);
  const double maxD=std::max(prev->getExtent(),getExtent());
  return APt-Axis*(maxD/talpha);
}

Geometry::Vec3D
boxUnit::getBExtra() const
  /*!
    Determine the extext point of the dividing plane on the 
    system. This is approximate and an over estimate [hence
    find for determine those objects to have an insert into
    \return centre extent point.
  */
{
  if (!next) return BPt;
  const double talpha=next->getTanAngle(Axis);
  const double maxD=std::max(next->getExtent(),getExtent());
  return BPt+Axis*(maxD/talpha);
}

void
boxUnit::addExcludeStrings(const std::map<int,MonteCarlo::Object*>& OMap) const
  /*!
    Adds the exclude strings to the objects
    \param OMap :: Object Map to add
   */
{
  ELog::RegMethod RegA("boxUnit","addExcludeString");

  // Add exclude string
  typedef std::map<int,MonteCarlo::Object*> MTYPE;
  MTYPE::const_iterator ac;
  for(ac=OMap.begin();ac!=OMap.end();ac++)
    {
      if (ac->first!=cellIndex-1)
	{
	  ac->second->addSurfString(getExclude());
	  ac->second->populate();
	  ac->second->createSurfaceList();
	}
    }
  return;
}

void
boxUnit::createAll(Simulation& System,
		    const size_t AF,
		    const std::vector<boxValues>& CV)
  /*!
    Build a pipe unit and insert it into the model
    \param System :: Simulation to 
    \param AF :: activeFlag
    \param CV :: Values for each layer
   */
{
  ELog::RegMethod RegA("boxUnit","createAll");

  populate(AF,CV);
  createSurfaces();
  createOuterObjects();
  insertObjects(System);
  createObjects(System);
  
  return;
}



} // NAMESPACE ModelSupport

