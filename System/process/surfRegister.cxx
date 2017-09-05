/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/surfRegister.cxx
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
#include <string>
#include <algorithm>
#include <functional>

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
#include "Surface.h"
#include "surfIndex.h"
#include "surfEqual.h"
#include "surfRegister.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Sphere.h"
#include "Cone.h"
#include "Cylinder.h"
#include "EllipticCyl.h"

namespace ModelSupport
{

surfRegister::surfRegister() 
 /*! 
   Constructor 
 */
{}

surfRegister::surfRegister(const surfRegister& A) :
  Index(A.Index)
  /*! 
    Copy Constructor 
    \param A :: surfRegister to copy
  */
{}


surfRegister&
surfRegister::operator=(const surfRegister& A) 
  /*! 
    Assignment operator
    \param A :: surfRegister to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Index=A.Index;
    }
  return *this;
}

void
surfRegister::reset()
  /*!
    Reset of system 
  */
{
  Index.erase(Index.begin(),Index.end());
  return;
}

void 
surfRegister::addMatch(const int phantomS,const int realS)
  /*!
    Add a surface translation
    \param phantomS :: surface number to work with
    \param realS :: actual real surface in Simulation
   */
{
  Index.insert(MTYPE::value_type(phantomS,realS));
  return;
}

int
surfRegister::realSurf(const int SN) const
  /*!
    Change a surface number if required
    \param SN :: surface number to modify
    \return real surface number
  */
{
  if (SN>0)
    {
      MTYPE::const_iterator mc=Index.find(SN);
      return (mc!=Index.end()) ? mc->second : SN;
    }
  MTYPE::const_iterator mc=Index.find(-SN);
  return (mc!=Index.end()) ? -mc->second : SN;
}

bool
surfRegister::hasSurf(const int SN) const
  /*!
    Determine if we have this surface number
    \param SN :: surface number to modify
    \return true if surface found
  */
{
  MTYPE::const_iterator mc=Index.find(abs(SN));
  return (mc!=Index.end());
}

Geometry::Surface*
surfRegister::realSurfPtr(const int SN) const
  /*!
    Aquire the real surface from a given number
    \param SN :: surface number 
    \return Surface Ptr
  */
{
  const int out=abs(realSurf(SN));
  return ModelSupport::surfIndex::Instance().getSurf(out);
}

void 
surfRegister::process(std::vector<int>& VecList) const
  /*!
    Set a surface translation to a vector.
    \param VecList :: Surface to change
  */
{
  transform(VecList.begin(),VecList.end(),
	    VecList.begin(),std::bind(&surfRegister::realSurf,
				      this,std::placeholders::_1));
  return;
}

Geometry::Plane*
surfRegister::createMirrorPlane(const Geometry::Surface* SPtr) const
  /*!
    Reverses the plane              [CREATES MEMORY TO BE MANAGED]
    \param SPtr :: Surface Pointer
    \return Reversed Plane [if possible]
  */
{
  ELog::RegMethod RegA("surfRegister","createMirrorPlane");

  const Geometry::Plane* PPtr=
    dynamic_cast<const Geometry::Plane*>(SPtr);
  if (!PPtr) return 0;
  Geometry::Plane* PX=new Geometry::Plane(*PPtr);
  PX->mirrorSelf();
  return PX;
}


int
surfRegister::registerSurf(Geometry::Surface* SPtr)
  /*!
    Fast access to registerSurf(int,Surface) if original name 
    is pre-set in the surface object.
    \param SPtr :: surface object
    \return Surface Number
  */
{
  return registerSurf(SPtr->getName(),SPtr);
}

void
surfRegister::addToIndex(const int addSurfIndex,const int realSurfIndex)
  /*!
    Adds a cover surface to the main system
    \param addSurfIndex :: new cover number
    \param realSurfIndex :: real surface number
   */
{
  ELog::RegMethod RegA("surfRegister","addToIndex");

  if (Index.find(addSurfIndex)!=Index.end())
    throw ColErr::InContainerError<int>(addSurfIndex," from realSurf:"+
					std::to_string(realSurfIndex));

  Index.insert(MTYPE::value_type(addSurfIndex,realSurfIndex));  
  Index.insert(MTYPE::value_type(-addSurfIndex,-realSurfIndex));  
  return;
}  

int
surfRegister::registerSurf(const int origN,Geometry::Surface* SPtr)
  /*!
    Handles a surface which may be equal to an existing
    surface. The original number does not need to be the SPtr number 
    on entry or exit
    \param origN :: Original number
    \param SPtr :: surface object
    \return Number
  */
{
  ELog::RegMethod RegA("surfRegister","registerSurf");

  ModelSupport::surfIndex& SurI=
    ModelSupport::surfIndex::Instance();
  if (origN<0)
    ELog::EM<<"Missing "<<SPtr->getName()<<ELog::endErr;

  const int N=ModelSupport::equalSurfNum(SPtr);
  // Check
  const int negSurf((origN<0) ? -1 : 1);

  if (N!=SPtr->getName())         // MATCHING SURFACE FOUND
    {
      delete SPtr;
      addToIndex(origN,N*negSurf);
      return N*negSurf;
    }
  const int oppSurf=SurI.findOpposite(SPtr);
  if (oppSurf)
    {
      delete SPtr;
      addToIndex(origN,-oppSurf*negSurf);
      return -oppSurf*negSurf;
    }

  SurI.insertSurface(SPtr);
  addToIndex(origN,N*negSurf);

  return N*negSurf;
}

template<typename T>
T*
surfRegister::realPtr(const int SN) const
  /*!
    Convert to the surface type or throw exception
    \param SN :: surface number [NOT Signed]
    \return Surface Ptr
  */
{
  Geometry::Surface* SPtr=realSurfPtr(SN);
  T* OutPtr=dynamic_cast<T*>(SPtr);
  if (!OutPtr)
    throw ColErr::InContainerError<int>
      (SN,"surfRegister::realPtr::"+T::classType());
  return OutPtr;
}

template<typename T>
T*
surfRegister::findPtr(const Geometry::Vec3D& A,const Geometry::Vec3D& B,
		      const Geometry::Vec3D& C) const
  /*!
    Given three points find the surface that matches
    \param A :: First point
    \param B :: Second point    
    \param C :: Third point
    \return Ptr of surface [type T]
  */
{
  MTYPE::const_iterator mc;
  const ModelSupport::surfIndex& SI=ModelSupport::surfIndex::Instance();
  for(mc=Index.begin();mc!=Index.end();mc++)
    {
      const T* Ptr=dynamic_cast<T*>(SI.getSurf(mc->second));
      if (Ptr && Ptr->onSurface(A) && 
	  Ptr->onSurface(B) && Ptr->onSurface(C))
	return Ptr;
    }
  return 0;
}

void
surfRegister::setKeep(const int SN,const int status) const
  /*!
    Sets the keep flag of a surface so that deletion
    due to "not-used" checks do not delete this surface
    \param SN :: Surface number
    \param status :: status number
  */
{
  ELog::RegMethod RegA("surfRegister","setKeep");
  ModelSupport::surfIndex& SI=ModelSupport::surfIndex::Instance();
  const int SReal=realSurf(SN);
  SI.setKeep(SReal,status);
  return;
}

///\cond TEMPLATE
template Geometry::Plane* surfRegister::realPtr(const int) const;
template Geometry::Cone* surfRegister::realPtr(const int) const;
template Geometry::Cylinder* surfRegister::realPtr(const int) const;
template Geometry::EllipticCyl* surfRegister::realPtr(const int) const;
template Geometry::Sphere* surfRegister::realPtr(const int) const;

template const Geometry::Plane* surfRegister::realPtr(const int) const;
template const Geometry::Cone* surfRegister::realPtr(const int) const;
template const Geometry::Cylinder* surfRegister::realPtr(const int) const;
template const Geometry::EllipticCyl* surfRegister::realPtr(const int) const;
template const Geometry::Sphere* surfRegister::realPtr(const int) const;
template const Geometry::Quadratic* surfRegister::realPtr(const int) const;

template const Geometry::Plane* surfRegister::findPtr<const Geometry::Plane>
(const Geometry::Vec3D&,const Geometry::Vec3D&,const Geometry::Vec3D&) const;
						     
///\endcond TEMPLATE


} // SimProcess
