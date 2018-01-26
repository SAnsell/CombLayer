/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attach/FrontBackCut.cxx
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
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "SurInter.h"
#include "FrontBackCut.h"

namespace attachSystem
{

FrontBackCut::FrontBackCut() :
  activeFront(0),activeBack(0)
  /*!
    Constructor [default]
  */
{}

FrontBackCut::FrontBackCut(const FrontBackCut& A) : 
  activeFront(A.activeFront),frontCut(A.frontCut),
  frontDivider(A.frontDivider),activeBack(A.activeBack),
  backCut(A.backCut),backDivider(A.backDivider)
  /*!
    Copy constructor
    \param A :: FrontBackCut to copy
  */
{}

FrontBackCut&
FrontBackCut::operator=(const FrontBackCut& A)
  /*!
    Assignment operator
    \param A :: FrontBackCut to copy
    \return *this
  */
{
  if (this!=&A)
    {
      activeFront=A.activeFront;
      frontCut=A.frontCut;
      frontDivider=A.frontDivider;
      activeBack=A.activeBack;
      backCut=A.backCut;
      backDivider=A.backDivider;
    }
  return *this;
}
  

FrontBackCut::~FrontBackCut()
  /*!
    Destructor
  */
{}

void
FrontBackCut::setFront(const int FSurf)
  /*!
    Set a front wall
    \param FSurf :: Front surface [signed]
  */
{
  ELog::RegMethod RegA("FrontBackCut","setFront(int)");

  frontCut.procSurfNum(FSurf);
  frontCut.populateSurf();
  frontDivider.reset();
  activeFront=1;
  return;
}

void
FrontBackCut::setBack(const int BSurf)
  /*!
    Set a back wall
    \param BSurf :: Back surface [signed]
  */
{
  ELog::RegMethod RegA("FrontBackCut","setBack(int)");

  backCut.procSurfNum(BSurf);
  backCut.populateSurf();
  backDivider.reset();
  activeBack=1;
  return;
}

void
FrontBackCut::setFront(const std::string& FRule)
  /*!
    Set a front wall
    \param FRule :: Front rule string
  */
{
  ELog::RegMethod RegA("FrontBackCut","setFront(string)");


  if (!frontCut.procString(FRule))
    throw ColErr::InvalidLine(FRule,"FRule failed");
  frontCut.populateSurf();
  frontDivider.reset();
  activeFront=1;
  return;
}

void
FrontBackCut::setBack(const std::string& FRule)
  /*!
    Set a back wall
    \param FRule :: Back rule string
  */
{
  ELog::RegMethod RegA("FrontBackCut","setBack(string)");

  if (!backCut.procString(FRule))
    throw ColErr::InvalidLine(FRule,"FRule failed");
  backCut.populateSurf();
  backDivider.reset();
  activeBack=1;
  return;
}

void
FrontBackCut::setFront(const attachSystem::FixedComp& WFC,
                       const long int sideIndex)
  /*!
    Set a front wall
    \param WFC :: Front line
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("FrontBackCut","setFront");

  // FixedComp::setLinkSignedCopy(0,FC,sideIndex);
  frontCut=WFC.getMainRule(sideIndex);
  frontDivider=WFC.getCommonRule(sideIndex);
  frontCut.populateSurf();
  frontDivider.populateSurf();
  activeFront=1;
  return;
}

void
FrontBackCut::setBack(const attachSystem::FixedComp& WFC,
                      const long int sideIndex)
  /*!
    Set a back wall
    \param WFC :: Back component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("FrontBackCut","setBack");

  // FixedComp::setLinkSignedCopy(0,FC,sideIndex);
  backCut=WFC.getMainRule(sideIndex);
  backDivider=WFC.getCommonRule(sideIndex);
  backCut.populateSurf();
  backDivider.populateSurf();
  activeBack=1;
  return;
}

void
FrontBackCut::setFrontDivider(const std::string& FDRule)
  /*!
    Set the front divider
    \param FDRule :: Front divider rule
  */
{
  if (!frontDivider.procString(FDRule))
    throw ColErr::InvalidLine(FDRule,"FDRule failed");
  frontDivider.populateSurf();
  return;
}

  
void
FrontBackCut::setFrontDivider(const HeadRule& HR)
  /*!
    Set the divider
    \param HR :: Divider surface
   */
{
  frontDivider=HR;
  return;
}

void
FrontBackCut::setBackDivider(const std::string& BDRule)
  /*!
    Set the back divider
    \param BDRule :: Back divider rule
  */
{
  if (!backDivider.procString(BDRule))
    throw ColErr::InvalidLine(BDRule,"BDRule failed");
  backDivider.populateSurf();
  return;
}

void
FrontBackCut::setBackDivider(const HeadRule& HR)
  /*!
    Set the divider
    \param HR :: Divider surface
   */
{
  backDivider=HR;
  return;
}

std::string
FrontBackCut::frontRule() const
  /*!
    Accessor  to front rule
    \return frontRule with divider
  */
{
  return (activeFront) ?
    frontCut.display()+frontDivider.display() : "" ;    
}

std::string
FrontBackCut::frontComplement() const
  /*!
    Accessor to front rule (complement)
    \return frontRule.cmp with divider
  */
{
  return (activeFront) ?
    frontCut.complement().display()+frontDivider.display() : "" ;    
}

std::string
FrontBackCut::backRule() const
  /*!
    Accessor to back rule
    \return backRule with divider
  */
{
  return (activeBack) ?
    backCut.display()+backDivider.display() : "";    
}

std::string
FrontBackCut::backComplement() const
  /*!
    Accessor to back rule
    \return backRule with divider
  */
{
  return (activeBack) ?
    backCut.complement().display()+backDivider.display() : "";    
}

std::string
FrontBackCut::frontBridgeRule() const
  /*!
    Accessor to front bridge rule
    \return frontBridgeRule 
  */
{
  return (activeFront) ?
    frontDivider.display() : "";
}

std::string
FrontBackCut::backBridgeRule() const
  /*!
    Accessor to back bridge rule
    \return backRule divider
  */
{
  return (activeBack) ?
    backDivider.display() : "";    
}

void
FrontBackCut::createLinks(attachSystem::FixedComp& FC,
			  const Geometry::Vec3D& Org,
			  const Geometry::Vec3D& YAxis)
  /*!
    Generate the front/back links if active
    \param FC :: Fixed component [most likely this]
    \param Org :: Origin
    \param YAxis :: YAxis
   */
{
  ELog::RegMethod RegA("FrontBackCut","createLinks");
  createFrontLinks(FC,Org,YAxis);
  createBackLinks(FC,Org,YAxis);
  return;
}
  
void
FrontBackCut::createFrontLinks(attachSystem::FixedComp& FC,
                               const Geometry::Vec3D& Org,
                               const Geometry::Vec3D& YAxis)
  /*!
    Generate the front links if active
    \param FC :: Fixed component [most likely this]
    \param Org :: Origin
    \param YAxis :: YAxis
  */
{
  ELog::RegMethod RegA("FrontBackCut","createFrontLinks");

  if (activeFront)
    {
      FC.setLinkSurf(0,frontCut.complement());
      FC.setBridgeSurf(0,frontDivider);
      FC.setConnect
	(0,SurInter::getLinePoint(Org,YAxis,frontCut,frontDivider),
	 -YAxis);
    }
  return;
}

void
FrontBackCut::createBackLinks(attachSystem::FixedComp& FC,
                              const Geometry::Vec3D& Org,
                              const Geometry::Vec3D& YAxis)
  /*!
    Generate the back links if active
    \param FC :: Fixed component [most likely this]
    \param Org :: Origin
    \param YAxis :: YAxis
  */
{
  ELog::RegMethod RegA("FrontBackCut","createBackLinks");
  
  if (activeBack)
    {
      FC.setLinkSurf(1,backCut.complement());
      FC.setBridgeSurf(1,backDivider);
      FC.setConnect
        (1,SurInter::getLinePoint(Org,YAxis,backCut,backDivider),
	 YAxis);
    }
  return;
}

void
FrontBackCut::getShiftedFront(ModelSupport::surfRegister& SMap,
			      const int surfIndex,
			      const int dFlag,
			      const Geometry::Vec3D& YAxis,
			      const double length) const
  /*!
    Support function to calculate the shifted surface fo the front
    \param SMap :: Surface register
    \param index :: offset index
    \param dFlag :: direction flag
    \param YAxis :: Axid for shift of sphere/cylinder
    \param length :: length to shift by
  */
{
  ELog::RegMethod RegA("FrontBackCut","getShiftedBack");
  getShiftedSurf(SMap,frontCut,surfIndex,dFlag,YAxis,length);
  return;
}

void
FrontBackCut::getShiftedBack(ModelSupport::surfRegister& SMap,
			     const int surfIndex,
			     const int dFlag,
			     const Geometry::Vec3D& YAxis,
			     const double length) const
  /*!
    Support function to calculate the shifted surface fo the back
    \param SMap :: Surface register
    \param index :: offset index
    \param dFlag :: direction flag
    \param YAxis :: Axid for shift of sphere/cylinder
    \param length :: length to shift by
  */
{
  ELog::RegMethod RegA("FrontBackCut","getShiftedBack");
  
  getShiftedSurf(SMap,backCut,surfIndex,dFlag,YAxis,length);
  return;
}


void
FrontBackCut::getShiftedSurf(ModelSupport::surfRegister& SMap,
			     const HeadRule& HR,
			     const int index,
			     const int dFlag,
			     const Geometry::Vec3D& YAxis,
			     const double length)
  /*!
    Support function to calculate the shifted surface
    \param HR :: HeadRule to extract plane surf
    \param index :: offset index
    \param dFlag :: direction of surface axis (relative to HR.Plane)
    \param YAxis :: Direction of cylindical shift
    \param length :: length to shift by
  */
{
  ELog::RegMethod RegA("FrontBackCut","getShiftedSurf");
  
  std::set<int> FS=HR.getSurfSet();
  for(const int& SN : FS)
    {
      const Geometry::Surface* SPtr=SMap.realSurfPtr(SN);

      const Geometry::Plane* PPtr=
	dynamic_cast<const Geometry::Plane*>(SPtr);
      if (PPtr)
	{
	  if (SN*dFlag>0)
	    ModelSupport::buildShiftedPlane(SMap,index,PPtr,dFlag*length);
	  else
	    ModelSupport::buildShiftedPlaneReversed(SMap,index,PPtr,dFlag*length);
	  
	  return;
	}
      
      const Geometry::Cylinder* CPtr=
	dynamic_cast<const Geometry::Cylinder*>(SPtr);
      // Cylinder case:
      if (CPtr)
	{
	  if (SN>0)
	    ModelSupport::buildCylinder
	      (SMap,index,CPtr->getCentre()+YAxis*length,
	       CPtr->getNormal(),CPtr->getRadius());
	  else
	    ModelSupport::buildCylinder
	      (SMap,index,CPtr->getCentre()-YAxis*length,
	       CPtr->getNormal(),CPtr->getRadius());
	  return;
	}
    }
  
  throw ColErr::EmptyValue<int>("HeadRule contains no planes/cylinder");
} 
  

  
}  // NAMESPACE attachSystem
