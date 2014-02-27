/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/pairItem.cxx
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
#include "Quaternion.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "pairBase.h"
#include "pairItem.h"

namespace ModelSupport
{

template<typename T,typename U>
const T*
pairItem<T,U>::findSurfA(const int SNum)
  /*!
    Get surface A based on the surface number
    \param SNum :: Surface number
    \return Surf [or throw error]
  */
{
  ELog::RegMethod RegA("pairItem","findSurfA");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  const T* Ptr=dynamic_cast<const T*>(SurI.getSurf(SNum));
  if (!Ptr)
    throw ColErr::InContainerError<int>(SNum,"Surface Number");

  return Ptr;
}

template<typename T,typename U>
const U*
pairItem<T,U>::findSurfB(const int SNum)
  /*!
    Get surface A based on the surface number
    \param SNum :: Surface number
    \return Surf [or throw error]
  */
{
  ELog::RegMethod RegA("pairItem","findSurfB");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  const U* Ptr=dynamic_cast<const U*>(SurI.getSurf(SNum));
  if (!Ptr)
    throw ColErr::InContainerError<int>(SNum,"Surface Number");

  return Ptr;
}

template<typename T,typename U>
pairItem<T,U>::pairItem(const T* APtr,const U* BPtr) :
  pairBase(),surfA(APtr),surfB(BPtr),sOut(0)
  /*!
    Constructor [not default]
    \param APtr :: First surface [inner]
    \param BPtr :: Second surface [outer]
   */
{}

template<typename T,typename U>
pairItem<T,U>::pairItem(const int ANum,const int BNum) :
  pairBase(),surfA(findSurfA(BNum)),
  surfB(findSurfB(ANum)),sOut(0)
  /*!
    Constructor [not default]
    \param ANum :: First surface [inner]
    \param BNum :: Second surface [outer]
   */
{}

template<typename T,typename U>
pairItem<T,U>::pairItem(const pairItem<T,U>& A) : 
  pairBase(A),surfA(A.surfA),surfB(A.surfB),
  sOut(A.sOut)
  /*!
    Copy constructor
    \param A :: pairItem to copy
  */
{}

template<typename T,typename U>
pairItem<T,U>&
pairItem<T,U>::operator=(const pairItem<T,U>& A)
  /*!
    Assignment operator
    \param A :: pairItem to copy
    \return *this
  */
{
  if (this!=&A)
    {
      pairBase::operator=(A);
      sOut=0;
    }
  return *this;
}

template<typename T,typename U>
pairItem<T,U>*
pairItem<T,U>::clone() const
  /*!
    Virtual copy constructor
    \return new (this)
  */
{
  return new pairItem<T,U>(*this);
}


template<typename T,typename U>
std::string
pairItem<T,U>::typeINFO() const
  /*!
    Combinded typenaem info 
    \return nameed types spaced deliminated
  */
{
  return U::classType()+" "+T::classType();
}

template<typename T,typename U>
int
pairItem<T,U>::createSurface(const double,const int)
  /*!
    Dummy do nothing call for all none active objects
  */
{
  ELog::EM<<"Calling empty function:"
	  <<typeINFO()<<ELog::endErr;
  return 0;
}


// PLANE : PLANE :: PLANE
template<>
int
pairItem<Geometry::Plane,Geometry::Plane>::
createSurface(const double fraction,const int newItem)
  /*!
    Create a surface based on the fraction
    \param newItem :: Plane number to start with
    \param fraction :: Weight between the two surface
    \return surface number created
  */
{
  ELog::RegMethod RegA("pairItem<Plane,Plane>","createSurface");
  ModelSupport::surfIndex& SurI= ModelSupport::surfIndex::Instance();
  
  const Geometry::Vec3D NormA=surfA->getNormal();
  const double DA=surfA->getDistance();

  Geometry::Vec3D NormB=surfB->getNormal();
  double DB=surfB->getDistance();
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
  Geometry::Plane* TP=SurI.createUniqSurf<Geometry::Plane>(newItem);
  TP->setPlane(NormB*normNeg,DB*normNeg);
  sOut=SurI.addTypeSurface(TP);

  return sOut->getName();
}

// CYLINDER: CYLINDER: CYLINDER
template<>
int
pairItem<Geometry::Cylinder,Geometry::Cylinder>::
createSurface(const double fraction,const int newItem)
  /*!
    Divides two cylinder to the required fraction. 
    \param newItem :: Clinder number to start with
    \param fraction :: Weight between the two surface
    \return cylinder number created
   */
{
  ELog::RegMethod RegA("pairItem<Cylinder,Cylinder>",
		       "createSurface");
  ModelSupport::surfIndex& SurI= ModelSupport::surfIndex::Instance();

  const Geometry::Vec3D CentA=surfB->getCentre();
  const Geometry::Vec3D NormA=surfB->getNormal();
  const double RA=surfB->getRadius();  

  Geometry::Vec3D CentB=surfA->getCentre();
  Geometry::Vec3D NormB=surfA->getNormal();
  double RB=surfA->getRadius();
  if (NormA.dotProd(NormB)<0)
    NormB*=-1.0;
  NormB*=fraction;
  NormB+=NormA*(1.0-fraction);
  CentB*=fraction;
  CentB+=CentA*(1.0-fraction);
  RB*=fraction;
  RB+=(1.0-fraction)*RA;
  
  Geometry::Cylinder* CPtr=
    SurI.createUniqSurf<Geometry::Cylinder>(newItem);
  CPtr->setCylinder(CentB,NormB,RB);
  sOut=SurI.addTypeSurface(CPtr);

  return sOut->getName();
}

// CYLINDER: PLANE: CYLINDER
template<>
int
pairItem<Geometry::Cylinder,Geometry::Plane>::
createSurface(const double fraction,const int newItem)
  /*!
    Divides cylinder/Plane to the required fraction. 
    This tries to keep the fractional area of the cylinder AND
    the mid point correct. That obviously can be done by having
    two variables (a) the centre and (b) the radius. The centre 
    follows the line 
    \param newItem :: Clinder number to start with
    \param fraction :: Weight between the two surface
    \return cylinder number created
   */
{
  ELog::RegMethod RegA("pairItem<Cylinder,Plane,Cylinder>",
		       "createSurface");
  ModelSupport::surfIndex& SurI= ModelSupport::surfIndex::Instance();

  const Geometry::Vec3D NormA=surfB->getNormal();

  Geometry::Vec3D CentB=surfA->getCentre();
  Geometry::Vec3D NormB=surfA->getNormal();
  double RB=surfA->getRadius();

  if (NormA.dotProd(NormB)<0)
    NormB*=-1.0;
  NormB*=fraction;
  NormB+=NormA*(1.0-fraction);
  // This is wrong:
  const Geometry::Vec3D CentA = CentB;
  const double RA=RB*50.0;


  CentB*=fraction;
  CentB+=CentA*(1.0-fraction);
  RB*=fraction;
  RB+=(1.0-fraction)*RA;
  
  Geometry::Cylinder* CPtr=
    SurI.createUniqSurf<Geometry::Cylinder>(newItem);
  CPtr->setCylinder(CentB,NormB,RB);
  sOut=SurI.addTypeSurface(CPtr);

  return sOut->getName();
}

/// \cond TEMPLATE

template class pairItem<Geometry::Plane,Geometry::Plane>;
template class pairItem<Geometry::Cylinder,Geometry::Cylinder>; 
template class pairItem<Geometry::Cylinder,Geometry::Plane>; 
template class pairItem<Geometry::Sphere,Geometry::Sphere>; 

// Not implemented
template class pairItem<Geometry::Plane,Geometry::Cylinder>; 
template class pairItem<Geometry::Cylinder,Geometry::Sphere>; 
template class pairItem<Geometry::Plane,Geometry::Sphere>; 
template class pairItem<Geometry::Sphere,Geometry::Plane>; 
template class pairItem<Geometry::Sphere,Geometry::Cylinder> ;

// template class  
//   pairItem<Geometry::Plane,Geometry::Cylinder,Geometry::Plane>; 

/// \endcond TEMPLATE
}
