/********************************************************************* 
  CombLayer : MCNP(X)  Input builder
 
 * File:   process/mergeMulti.cxx
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
#include "mathSupport.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "surfExpand.h"
#include "surfEqual.h"
#include "Quaternion.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Token.h"
#include "AttachSupport.h"
#include "surfDBase.h"
#include "mergeMulti.h"

namespace ModelSupport
{

template<typename T,typename U>
mergeMulti<T,U>::mergeMulti() : surfDBase(),
  InOutFlag(1),signSurfReplace(0),
  pSurf(0),PB(0),OSPtr(0)
  /*!
    Constructor
  */
{}


template<typename T,typename U>
mergeMulti<T,U>::mergeMulti(const mergeMulti<T,U>& A) : 
  surfDBase(A),InOutFlag(A.InOutFlag),
  signSurfReplace(A.signSurfReplace),
  InRule(A.InRule),OutRule(A.OutRule),
  pSurf(A.pSurf),sSurf(A.sSurf),secDir(A.secDir),
  PB(A.PB),PS(A.PS),OSPtr(A.OSPtr)
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
      InOutFlag=A.InOutFlag;
      signSurfReplace=A.signSurfReplace;
      InRule=A.InRule;
      OutRule=A.OutRule;
      pSurf=A.pSurf;
      sSurf=A.sSurf;
      secDir=A.secDir;
      PB=A.PB;
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
mergeMulti<T,U>::~mergeMulti()
  /*!
    Delete operator
    \return new(*this);
  */
{}



template<typename T,typename U>
void
mergeMulti<T,U>::clearRules()
  /*!
    Simple helper function to remove old rules
  */
{
  InRule.reset();
  OutRule.reset();
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
      InRule.addUnion(dirSign*SPtr->getName());
      OutRule.addIntersection(-dirSign*SPtr->getName());
//      attachSystem::addUnion(dirSign*SPtr->getName(),SPtr,InRule);
//      attachSystem::addIntersection(-dirSign*SPtr->getName(),SPtr,OutRule);
    }
  return;
}

template<typename T,typename U>
void
mergeMulti<T,U>::getInner(std::vector<Token>& IVec) const
  /*!
    Get the inner rule token string
    \param IVec :: Inner tokens
  */
{
  ELog::RegMethod RegA("mergeMulti","getInner");
  if (InRule.getTopRule())
    {
      IVec.push_back(Token('(')); 
      InRule.displayVec(IVec);
      IVec.push_back(Token(')')); 
    }
  return;
}

template<typename T,typename U>
void
mergeMulti<T,U>::getOuter(std::vector<Token>& OVec) const
  /*!
    Get the outerr rule token 
    \param OVec :: Inner tokens
  */
{
  ELog::RegMethod RegA("mergeMulti","getOuter");
  if (OutRule.getTopRule())
    {
      OVec.push_back(Token('(')); 
      OutRule.displayVec(OVec);
      OVec.push_back(Token(')')); 
    }
  return;
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


template<typename T,typename U>
void
mergeMulti<T,U>::processInnerOuterWithSign(const int outerFlag,
				     std::vector<Token>& Cell) const
  /*!
    Process the inner cells
    \param outerFlag :: Decide if outer/inner prog.
    \param Cell :: Cell to process
  */
{
  ELog::RegMethod RegA("mergeMulti","processInnerOuterWithSign");

  // Calculate the new substitution:
  std::vector<Token> inBoundary;
  std::vector<Token> outBoundary;
  std::vector<int> SIndex;

  getInner(inBoundary);
  getOuter(outBoundary);

  SIndex=(outerFlag) ? getOuterRemove() : getInnerRemove(); 

  if (!SIndex.empty())
    {
      for(size_t i=0;i<SIndex.size();i++)
	{
	  if (!i)
	    {
	      if (InOutFlag)
		replaceTokenWithSign(Cell,SIndex.front(),inBoundary,outBoundary);
	      else
		replaceTokenWithSign(Cell,SIndex.front(),outBoundary,inBoundary);
	    }
	  else
	    removeToken(Cell,SIndex[i]);
	}
    }
  return;
}

template<typename T,typename U>
void 
mergeMulti<T,U>::setPrimarySurf(const int iDir,const int surfN)
  /*!
    Set the primary surface number / inner direction
    
    \param iDir :: Set if outer->inner system is in use
    \param surfN :: Primary surface
   */
{
  InOutFlag=iDir;
  pSurf=surfN;
  return;
}


template<typename T,typename U>
void 
mergeMulti<T,U>::addSecondarySurf(const int SN)
  /*!
    Add a secondary surface
    \param SN :: Signed surface number
  */
{
  sSurf.push_back(SN);
  secDir.push_back((SN>0) ? 1 : -1);
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
createSurf(const double fraction,int& newItem)
  /*!
    Divides two planes to the required fraction. Care is taken
    if the two planes have opposite signs.
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
      double dProd=NormA.dotProd(NormB);
      double normNeg=1.0;
      if (dProd<0)
	{
	  normNeg=-1.0;
	  NormB*=-1.0;
	  DB*=-1.0;
	}
      NormB*=fraction;
      NormB+=NormA*(1.0-fraction);
      NormB.makeUnit();
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
createSurf(const double fraction,int& newItem)
  /*!
    Divides two planes to the required fraction. Care is taken
    if the two planes have opposite.
    - IMPORTANT: the inner surface is master ie if we have -IN ON 
    then we get a normal that is opposite to ON.
    \param newItem :: Cylinder number to start with
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
createSurf(const double fraction,int& newItem)
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
void
mergeMulti<T,U>::populate()
  /*!
    Populates all the surface
  */
{
  ELog::RegMethod RegA("mergeMulti","populate");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  PB=dynamic_cast<const T*>(SurI.getSurf(abs(pSurf)));
  if (!PB)
    ELog::EM<<"Failed on primary surface "<<pSurf<<ELog::endErr;
  std::vector<int>::const_iterator vc;
 
  PS.clear();
  for(vc=sSurf.begin();vc!=sSurf.end();vc++)
    {
      PS.push_back(dynamic_cast<const U*>(SurI.getSurf(abs(*vc))));
      if (PS.back()==0)
	ELog::EM<<"Failed on surface "<<*vc<<ELog::endErr;
    }
  return;
}


template<typename T,typename U>
void
mergeMulti<T,U>::processInnerOuter(const int outerFlag,
				   std::vector<Token>& Cell) const
  /*!
    Process the cells 
    \param outerFlag :: Decide if outer/inner process.
    \param Cell :: Cell to process
   */
{
  ELog::RegMethod RegA("mergeMulti","processInnerOuter");

  if (signSurfReplace) 
    {
      processInnerOuterWithSign(outerFlag,Cell);
      return;
    }
  // Calculate the new substitution:
  std::vector<Token> newBoundary;
  std::vector<int> SIndex;

  if (InOutFlag==outerFlag)         // Inner surf replacement
    getOuter(newBoundary);
  else
    getInner(newBoundary);

  SIndex=(outerFlag) ? getOuterRemove() : getInnerRemove(); 

  if (!SIndex.empty())
    {
      for(size_t i=0;i<SIndex.size();i++)
	{
	  if (!i && !newBoundary.empty())
	    {
	      replaceToken(Cell,SIndex.front(),newBoundary);
	    }
	  else
	    removeToken(Cell,SIndex[i]);
	}
    }
  return;
}

template<typename T,typename U>
void
mergeMulti<T,U>::write(std::ostream& OX) const
  /*!
    Write out stuff
    \param OX :: output stream
  */
{
  OX<<"In Rule == "<<InRule.display()<<std::endl;
  OX<<"Out Rule == "<<OutRule.display()<<std::endl;
  return;
}

///\cond TEMPLATE

template class mergeMulti<Geometry::Plane,Geometry::Plane>;
template class mergeMulti<Geometry::Cylinder,Geometry::Cylinder>;
template class mergeMulti<Geometry::Cylinder,Geometry::Plane>;

///\endcond TEMPLATE




} // NAMESPACE ModelSupport

