/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/MergeSurf.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <boost/shared_ptr.hpp>
#include <boost/functional.hpp>
#include <boost/bind.hpp>
 
#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "Transform.h"
#include "Rules.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "surfExpand.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Token.h"
#include "AttachSupport.h"
#include "MergeSurf.h"

namespace ModelSupport
{

MergeSurf::MergeSurf() : 
  InRule(0),OutRule(0)
  /*!
    Constructor
  */
{}

MergeSurf::MergeSurf(const MergeSurf& A) :
  InRule(A.InRule ? A.InRule->clone() : 0),
  OutRule(A.OutRule ? A.OutRule->clone() : 0)
  /*!
    Copy constructor
    \param A :: MergeSurf to copy
  */
{}

MergeSurf&
MergeSurf::operator=(const MergeSurf& A)
  /*!
    Assignment operator 
    \param A :: MergeSurf to copy
    \return *this
   */
{
  if (this!=&A)
    {
      delete InRule;
      delete OutRule;
      InRule=A.InRule ? A.InRule->clone() : 0;
      OutRule=A.OutRule ? A.OutRule->clone() : 0;
    }
  return *this;
}

MergeSurf::~MergeSurf()
  /*!
    Destructor
  */
{
  delete InRule;
  delete OutRule;
}

std::string 
MergeSurf::getInner() const
  /*!
    Access teh inner string
    \return String form of rule
   */
{
  return (InRule) ? InRule->display() : "";
}

std::string 
MergeSurf::getOuter() const
  /*!
    Access teh inner string
    \return String form of rule
   */
{
  return (OutRule) ? OutRule->display() : "";
}

void
MergeSurf::getInner(std::vector<Token>& IVec) const
  /*!
    Sets the output token string
    \param OVec :: Outer tokens
  */
{
  ELog::RegMethod RegA("MergeSurf","getInner");

  if (InRule)
    {
      IVec.push_back(Token('(')); 
      InRule->displayVec(IVec);
      IVec.push_back(Token(')')); 
    }
  return;
}

void
MergeSurf::getOuter(std::vector<Token>& OVec) const
  /*!
    Sets the output token string
    \param OVec :: Outer tokens
  */
{
  if (OutRule)
    {
      OVec.push_back(Token('(')); 
      OutRule->displayVec(OVec);
      OVec.push_back(Token(')')); 
    }

  return;
}

void
MergeSurf::clearRules()
  /*!
    Simple helper function to remove old rules
  */
{
  delete InRule;
  delete OutRule;
  InRule=0;
  OutRule=0;
  return;
}


// -------------------------------------------------------------------
//                  MERGEMULTI
// -------------------------------------------------------------------

template<typename T,typename U>
mergeMulti<T,U>::mergeMulti() : 

  MergeSurf(),PB(0)
  /*!
    Constructor
  */
{}

template<typename T,typename U>
mergeMulti<T,U>::mergeMulti(const int,
	    const Geometry::Surface* iBase,
	    const std::vector<const Geometry::Surface*>& SVec) :
  MergeSurf(),PB(0)
  /*!
    Constructor
    \param M :: Mirror flag
    \param iBase :: Base surface
    \param SVec :: Second surface(s) [only one used]
  */
{
  initSurfaces(iBase,SVec);  
}

template<typename T,typename U>
mergeMulti<T,U>::mergeMulti(const mergeMulti<T,U>& A) : 
  MergeSurf(A),PB(A.PB),secDir(A.secDir),
  PS(A.PS),OSPtr(A.OSPtr)
  /*!
    Copy constructor
    \param A :: mergeMulti to copy
  */
{}

template<typename T,typename U>
mergeMulti<T,U>&
mergeMulti<T,U>::operator=(const mergeMulti<T,U>& A)
  /*!
    Assignment operator
    \param A :: mergeMulti to copy
    \return *this
  */
{
  if (this!=&A)
    {
      MergeSurf::operator=(A);
      PB=A.PB;
      secDir=A.secDir;
      PS=A.PS;
      OSPtr=A.OSPtr;
    }
  return *this;
}

template<typename T,typename U>
mergeMulti<T,U>*
mergeMulti<T,U>::clone() const
  /*!
    Clone copy constructor
    \return new(*this);
  */
{
  return new mergeMulti<T,U>(*this);
}

template<typename T,typename U>
void
mergeMulti<T,U>::initSurfaces(const Geometry::Surface* iBase,
			      const std::vector<const Geometry::Surface*>& SVec)
  /*!
    Set the initial surfaces
    \param iBase :: inital base object
    \param SVec :: Vector of sub surfaces
  */
{
  ELog::RegMethod RegA("mergeMulti","initSurfaces");

  PB=dynamic_cast<const T*>(iBase);
  if (SVec.empty())
    throw ColErr::ExBase("Empty Vector \n"+RegA.getFull());
  
  if (!PB)
    {
      ELog::EM<<"Unable to create mergeMulti "<<ELog::endErr;
      ELog::EM<<"Failed to find surface "<<iBase->getName()
	      <<ELog::endErr;
      throw ColErr::ExitAbort(RegA.getFull());	
    }

  std::vector<const Geometry::Surface*>::const_iterator vc;
  for(vc=SVec.begin();vc!=SVec.end();vc++)
    {
      const U* PItem=dynamic_cast<const U*> (*vc);
      if (!PItem)
	{
	  ELog::EM<<"Failed to find surface "
		  <<(*vc)->getName()<<ELog::endErr;
	  throw ColErr::ExitAbort(RegA.getFull());	
	}
      secDir.push_back(1);
      PS.push_back(PItem);
    }  
  return;
}


template<typename T,typename U>
void
mergeMulti<T,U>::addRules() 
  /*!
    Adds the surface object to the rules in a simple way
  */
{
  ELog::RegMethod RegA("mergeMulti","addRules");

  clearRules();
  std::vector<Geometry::Surface*>::const_iterator ac;  
  const Geometry::Surface* SPtr(0);
  for(size_t i=0;i<OSPtr.size();i++)
    {
      SPtr = OSPtr[i];
      const int dirSign=secDir[i];
      attachSystem::addUnion(dirSign*SPtr->getName(),SPtr,InRule);
      attachSystem::addIntersection(-dirSign*SPtr->getName(),SPtr,OutRule);
    }
  return;
}

template<typename T,typename U>
void
mergeMulti<T,U>::setDirection(const std::vector<int>& sValue)
  /*!
    Set the direction based on the surface sign
    \param sValue :: list is surfaces [signed]
   */
{
  ELog::RegMethod RegA("mergeMulti","setDirection");

  for(size_t i=0;i<sValue.size() && i<secDir.size();i++)
    {
      secDir[i]=(sValue[i]>0) ? 1 : -1;
    }
  return;
}

template<>
int
mergeMulti<Geometry::Plane,Geometry::Plane>::
createSurf(int& newItem,const double fraction)

  /*!
    Divides two planes to the required fraction. Care is taken
    if the two planes have opposite.
    - IMPORTANT: the inner surface is master ie if we have -IN ON 
    then we get a normal that is opposite to ON.
    \param newItem :: Plane number to start with
    \param fraction :: Weight between the two surface
    \return plane number created
   */
{
  ELog::RegMethod RegA("mergeMulti<Plane>","createSurf");
  ModelSupport::surfIndex& SurI= ModelSupport::surfIndex::Instance();

  const Geometry::Vec3D NormA=PB->getNormal();
  const double DA=PB->getDistance();

  OSPtr.clear();  // No need to delete [not managed]

  std::vector<const Geometry::Plane*>::const_iterator vc;
  for(vc=PS.begin();vc!=PS.end();vc++)
    {
      Geometry::Vec3D NormB=(*vc)->getNormal();
      double DB=(*vc)->getDistance();
      double normNeg=1.0;
      if (NormA.dotProd(NormB)<0)
	{
	  normNeg=-1.0;
	  NormB*=-1.0;
	  DB*=-1.0;
	}
      NormB*=fraction;
      NormB+=NormA*(1.0-fraction);
      DB*=fraction;
      DB+=(1.0-fraction)*DA;
      
      // This needs to be checked / minimized
      Geometry::Plane* TP=SurI.createUniqSurf<Geometry::Plane>(newItem++);
      TP->setPlane(NormB*normNeg,DB*normNeg);
      OSPtr.push_back(SurI.addSurface(TP));
    }
  addRules();

  return OSPtr.back()->getName();
}


template<>
int
mergeMulti<Geometry::Cylinder,Geometry::Cylinder>::
createSurf(int& newItem,const double fraction)

  /*!
    Divides two planes to the required fraction. Care is taken
    if the two planes have opposite.
    - IMPORTANT: the inner surface is master ie if we have -IN ON 
    then we get a normal that is opposite to ON.
    \param newItem :: Clinder number to start with
    \param fraction :: Weight between the two surface
    \return plane number created
   */
{
  ELog::RegMethod RegA("mergeMulti<Cylinder,Cylinder>","createSurf");
  ModelSupport::surfIndex& SurI= ModelSupport::surfIndex::Instance();

  const Geometry::Vec3D CentA=PB->getCentre();
  const Geometry::Vec3D NormA=PB->getNormal();
  const double RA=PB->getRadius();  
  OSPtr.clear();  // No need to delete [not managed]
  std::vector<const Geometry::Cylinder*>::const_iterator vc;
  for(vc=PS.begin();vc!=PS.end();vc++)
    {
      Geometry::Vec3D CentB=(*vc)->getCentre();
      Geometry::Vec3D NormB=(*vc)->getNormal();
      double RB=(*vc)->getRadius();
      if (NormA.dotProd(NormB)<0)
	NormB*=-1.0;
      NormB*=fraction;
      NormB+=NormA*(1.0-fraction);
      CentB*=fraction;
      CentB+=CentA*(1.0-fraction);
      RB*=fraction;
      RB+=(1.0-fraction)*RA;

      Geometry::Cylinder* CPtr=
	SurI.createUniqSurf<Geometry::Cylinder>(newItem++);
      CPtr->setCylinder(CentB,NormB,RB);
      OSPtr.push_back(SurI.addSurface(CPtr));
    }
  addRules();

  return OSPtr.back()->getName();
}

template<>
int
mergeMulti<Geometry::Cylinder,Geometry::Plane>::
createSurf(int& newItem,const double fraction)

  /*!
    Divides a cylinder -> plane :
    Note that the object is always a Cylinder in THIS implementaiton
    - IMPORTANT: the inner surface is master ie if we have -IN ON 
    then we get a normal that is opposite to ON.
    \param newItem :: Cylinder number to start with
    \param fraction :: Weight between the two surface
    \return plane number created
   */
{
  ELog::RegMethod RegA("mergeMulti<Cylinder,Plane>","createSurf");
  ModelSupport::surfIndex& SurI= ModelSupport::surfIndex::Instance();

  const Geometry::Vec3D CentA=PB->getCentre();
  const Geometry::Vec3D NormA=PB->getNormal();
  const double RA=PB->getRadius();

  std::vector<const Geometry::Plane*>::const_iterator vc;
  for(vc=PS.begin();vc!=PS.end();vc++)
    {
      // Require normal that is "most" parallel to N
      Geometry::Vec3D NormB=(*vc)->getNormal();
      MonteCarlo::LineIntersectVisit LI(CentA,NormB);
      double RB=LI.getDist(*vc);
      if (RB<0)
	RB*=-1.0;
      RB*=fraction;
      RB+=(1.0-fraction)*RA;

      Geometry::Cylinder* CPtr=
	SurI.createUniqSurf<Geometry::Cylinder>(newItem++);
      CPtr->setCylinder(CentA,NormA,RB);
      OSPtr.push_back(SurI.addSurface(CPtr));
    }

  return OSPtr.back()->getName();
}


template<typename T,typename U>
std::vector<int>
mergeMulti<T,U>::getInnerRemove() const
  /*!
    Vector of the surfaces to remove in an inner merge
    ie if 1 -2 : typically 1 is the base [inner] and 2 is
    the outer plane
    \return Output set
  */
{
  std::vector<int> Out;
  if (PB && PB->getName())
    Out.push_back(PB->getName());
  return Out;
}

template<typename T,typename U>
std::vector<int>
mergeMulti<T,U>::getOuterRemove() const
  /*!
    Vector of the surfaces to remove in an inner merge
    ie if 1 -2 : typically 1 is the base [inner] and 2 is
    the outer plane
    \return Output set
  */
{
  std::vector<int> Out;
  typename std::vector<const U*>::const_iterator vc;
  for(vc=PS.begin();vc!=PS.end();vc++)
    {
      if (*vc && (*vc)->getName())
	Out.push_back((*vc)->getName());
    }
  return Out;
}


// -------------------------------------------------------------------
//                  MERGEADD
// -------------------------------------------------------------------


mergeAdd::mergeAdd() : 
  MergeSurf(),position(0)
  /*!
    Constructor
  */
{}

mergeAdd::mergeAdd(const int PFlag,const int dirFlag,
		   const Geometry::Surface* SPtr) :
  MergeSurf(),position(PFlag)
  /*!
    Constructor
    \param PFlag :: Position
    \param SPtr :: Surface number to ad
  */
{
  
  if (position & 1)
    InRule=new SurfPoint(dirFlag*SPtr->getName(),SPtr);
  if (position & 2)
    OutRule=new SurfPoint(dirFlag*SPtr->getName(),SPtr);
}


mergeAdd::mergeAdd(const mergeAdd& A) : 
  MergeSurf(A),position(A.position)
  /*!
    Copy constructor
    \param A :: mergePair to copy
  */
{}

mergeAdd&
mergeAdd::operator=(const mergeAdd& A)
  /*!
    Assignment operator
    \param A :: mergePair to copy
    \return *this
  */
{
  if (this!=&A)
    {
      MergeSurf::operator=(A);
      position=A.position;
    }
  return *this;
}

template<typename T,typename U>
mergeAdd*
mergeAdd::clone() const
  /*!
    Clone copy constructor
    \return new(*this);
  */
{
  return new mergeAdd(*this);
}
  
int
mergeAdd::createSurf(int& newItem,const double)

  /*!
    \param newItem :: Surefae number to start with
    \param :: Fraction unused][
    \return plane number created
   */
{
  ELog::RegMethod RegA("mergeAdd","createSurfe");
  return newItem;
}

std::vector<int>
mergeAdd::getInnerRemove() const
  /*!
    Vector of the surfaces to remove in an inner merge
    ie if 1 -2 : typically 1 is the base [inner] and 2 is
    the outer plane
    \return Output set
  */
{
  std::vector<int> Out;
  return Out;
}

std::vector<int>
mergeAdd::getOuterRemove() const
  /*!
    Vector of the surfaces to remove in an inner merge
    ie if 1 -2 : typically 1 is the base [inner] and 2 is
    the outer plane
    \return Output set
  */
{
  std::vector<int> Out;
  return Out;
}

void 
mergeAdd::getInner()


///\cond TEMPLATE

template class mergeMulti<Geometry::Plane,Geometry::Plane>;
template class mergeMulti<Geometry::Cylinder,Geometry::Cylinder>;
template class mergeMulti<Geometry::Cylinder,Geometry::Plane>;

///\endcond TEMPLATE




} // NAMESPACE ModelSupport

