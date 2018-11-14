/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/ExtractCut.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "HeadRule.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "SurInter.h"
#include "ExternalCut.h"

namespace attachSystem
{

ExternalCut::ExternalCut() 
  /*!
    Constructor [default]
  */
{}

ExternalCut::ExternalCut(const ExternalCut& A) : 
  cutItems(A.cutItems)
  /*!
    Copy constructor
    \param A :: ExternalCut to copy
  */
{}

ExternalCut&
ExternalCut::operator=(const ExternalCut& A)
  /*!
    Assignment operator
    \param A :: ExternalCut to copy
    \return *this
  */
{
  if (this!=&A)
    {
      cutItems=A.cutItems;
    }
  return *this;
}

ExternalCut::~ExternalCut()
  /*!
    Destructor
  */
{}

bool
ExternalCut::isActive(const std::string& extName) const
  /*!
    Determine if we have unit
    \param extName :: Cut naem
    \return true if unit exists
  */
{
  return (cutItems.find(extName)!=cutItems.end());
}

const cutUnit*
ExternalCut::findUnit(const std::string& extName) const
  /*!
    Determine if we have unit by name
    \param extName :: Cut name
    \return Ptr / 0
   */
{
  cutTYPE::const_iterator mc=cutItems.find(extName);
  return (mc!=cutItems.end()) ? &(mc->second) : 0;
}
  
cutUnit&
ExternalCut::getUnit(const std::string& extName)
  /*!
    Creates or finds a cutUnit [throw on error]
    \param extName :: Cut name
    \return cutUnit
  */
{
  ELog::RegMethod RegA("ExternalCut","getUnit");
  
  if (extName.empty())
    throw ColErr::EmptyValue<std::string>("ExternalCut::extName");
  cutTYPE::iterator mc=
    cutItems.find(extName);
  if (mc!=cutItems.end())
    return mc->second;
  std::pair<cutTYPE::iterator,bool> mx=
    cutItems.emplace(extName,cutUnit());
  
  return mx.first->second;
}
  
void
ExternalCut::copyCutSurf(const std::string& extName,
			const ExternalCut& ESurf,
			const std::string& otherName)
  /*!
    Set a a surface on an existing/new cutUnit
    \param extName :: external-cut name
    \param ESurf :: object to copy
    \param outerName :: external-cut name
  */
{
  ELog::RegMethod RegA("ExternalCut","copyCutSurf(ExternalCut)");

  cutUnit& A=getUnit(extName);
  const cutUnit* BPtr=ESurf.findUnit(otherName);
  if (!BPtr)
    throw ColErr::InContainerError<std::string>
      (otherName,"Other exteralCut not found");

  A = *BPtr;
  return;
}

void
ExternalCut::setCutSurf(const std::string& extName,
			const int ESurf)
  /*!
    Set a a surface
    \param extName :: external-cut name
    \param ESurf :: surface number [signed]
  */
{
  ELog::RegMethod RegA("ExternalCut","setCutSurf(int)");

  cutUnit& A=getUnit(extName);
  A.main.procSurfNum(ESurf);
  A.main.populateSurf();
  A.divider.reset();
  return;
}

void
ExternalCut::setCutSurf(const std::string& extName,
			const HeadRule& ESurfHR)
  /*!
    Set a a surface
    \param extName :: external-cut name
    \param ESurfHR :: surface head rule
  */
{
  ELog::RegMethod RegA("ExternalCut","setCutSurf(HR)");

  cutUnit& A=getUnit(extName);
  A.main=ESurfHR;
  A.divider.reset();
  return;
}


void
ExternalCut::setCutSurf(const std::string& extName,
			const std::string& ERule)
  /*!
    Set a a surface
    \param extName :: external-cut name
    \param ESurf :: surface rule
  */
{
  ELog::RegMethod RegA("ExternalCut","setCutSurf(string)");

  cutUnit& A=getUnit(extName);
    
  if (!A.main.procString(ERule))
    throw ColErr::InvalidLine(ERule,"ERule failed");
  A.main.populateSurf();
  A.divider.reset();
  return;
}

void
ExternalCut::setCutSurf(const std::string& extName,
			const attachSystem::FixedComp& WFC,
			const std::string& sideName)
  /*!
    Set a surface from a linkpoint
    \param extName :: external-cut name
    \param WFC :: Fixedcomp
    \param sideName :: link point
  */
{
  ELog::RegMethod RegA("ExternalCut","setCutSurf(FC,name)");

  setCutSurf(extName,WFC,WFC.getSideIndex(sideName));
  return;
}

void
ExternalCut::setCutSurf(const std::string& extName,
		     const attachSystem::FixedComp& WFC,
		     const long int sideIndex)
  /*!
    Set a surface from a linkpoint
    \param extName :: external-cut name
    \param WFC :: Fixedcomp
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("ExternalCut","setCutSurf");

  // FixedComp::setLinkSignedCopy(0,FC,sideIndex);
  cutUnit& A=getUnit(extName);
  
  A.main=WFC.getMainRule(sideIndex);
  A.divider=WFC.getCommonRule(sideIndex);
  A.main.populateSurf();
  A.divider.populateSurf();
  return;
}


void
ExternalCut::setCutDivider(const std::string& extName,
			   const std::string& EDRule)
  /*!
    Set the divider
    \param extName :: external-cut name
    \param EDRule :: Divider rule
  */
{
  ELog::RegMethod RegA("ExternalCut","setCutDivider");
  
  cutUnit& A=getUnit(extName);
  if (!A.divider.procString(EDRule))
    throw ColErr::InvalidLine(EDRule,"EDRule failed");
  A.divider.populateSurf();
  return;
}

  
void
ExternalCut::setCutDivider(const std::string& extName,
			   const HeadRule& HR)
  /*!
    Set the divider
    \param extName :: external-cut name
    \param HR :: Divider surface
   */
{
  ELog::RegMethod RegA("ExternalCut","setCutDivider(HR)");
  
  cutUnit& A=getUnit(extName);
  A.divider=HR;
  return;
}

std::string
ExternalCut::getRuleStr(const std::string& extName) const
  /*!
    Accessor to main rule
    \param extName :: external-cut name
    \return frontRule with divider
  */
{
  ELog::RegMethod RegA("ExternalCut","getRuleStr");

  const cutUnit* CU=findUnit(extName);
  return (CU) ?
    CU->main.display()+CU->divider.display() : "" ;    
}

std::string
ExternalCut::getComplementStr(const std::string& extName) const
  /*!
    Accessor to front rule (complement)
    \param extName :: external-cut name
    \return frontRule.cmp with divider
  */
{
  ELog::RegMethod RegA("ExternalCut","getComplementStr");

  const cutUnit* CU=findUnit(extName);
  return (CU) ?
    CU->main.complement().display()+CU->divider.display() : "" ;    
}


std::string
ExternalCut::getBridgeStr(const std::string& extName) const
  /*!
    Accessor to bridge rule
    \param extName :: CutUnit name						
    \return Divider rule
  */
{
  const cutUnit* CU=findUnit(extName);
  return (CU) ?
    CU->divider.display() : "" ;    
}

const HeadRule&
ExternalCut::getRule(const std::string& extName) const
  /*!
    Accessor to main rule
    \param extName :: external-cut name
    \return frontRule with divider
  */
{
  ELog::RegMethod RegA("ExternalCut","getRule");
  static HeadRule nullOut;
  
  const cutUnit* CU=findUnit(extName);
  return (CU) ? CU->main :  nullOut;    
}

const HeadRule&
ExternalCut::getDivider(const std::string& extName) const
  /*!
    Accessor to main rule
    \param extName :: external-cut name
    \return frontRule with divider
  */
{
  ELog::RegMethod RegA("ExternalCut","getDivider");
  static HeadRule nullOut;
  
  const cutUnit* CU=findUnit(extName);
  return (CU) ? CU->divider :  nullOut;    
}

void
ExternalCut::createLink(const std::string& extName,
			attachSystem::FixedComp& FC,
			const size_t linkIndex,
			const Geometry::Vec3D& Org,
			const Geometry::Vec3D& YAxis) const
  /*!
    Generate the front/back links if active
    \param extName :: Cut Unit item
    \param FC :: Fixed component [most likely this]
    \param linkIndex :: link point to build
    \param Org :: Origin
    \param YAxis :: YAxis
   */
{
  ELog::RegMethod RegA("ExternalCut","createLinks");

  const cutUnit* CU=findUnit(extName);
  if (CU)  
    {
      FC.setLinkSurf(linkIndex,CU->main.complement());
      FC.setBridgeSurf(linkIndex,CU->divider);
      FC.setConnect(linkIndex,
	 SurInter::getLinePoint(Org-YAxis*10.0,YAxis,CU->main,CU->divider),-YAxis);
    }
  return;
}
  

void
ExternalCut::makeShiftedSurf(ModelSupport::surfRegister& SMap,
			    const HeadRule& HR,
			    const int index,
			    const int dFlag,
			    const Geometry::Vec3D& YAxis,
			    const double length)
  /*!
    Support function to calculate the shifted surface based
    on surface type and form
    \param SMap :: local surface register
    \param HR :: HeadRule to extract plane surf
    \param index :: offset index
    \param dFlag :: direction of surface axis (relative to HR.Plane)
    \param YAxis :: Direction of cylindical shift [NOT PLANE]
    \param length :: length to shift by
  */
{
  ELog::RegMethod RegA("ExternalCut","makeShiftedSurf");
  
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

void
ExternalCut::makeExpandedSurf(ModelSupport::surfRegister& SMap,
			     const HeadRule& HR,
			     const int index,
			     const Geometry::Vec3D& expandCentre,
			     const double dExtra) 
  /*!
    Support function to calculate the shifted surface based
    on surface type and form. Moves away from the center if dExtra
    positive.
    \param SMap :: local surface register
    \param HR :: HeadRule to extract plane surf
    \param index :: offset index
    \param expandCentre :: Centre for expansion
    \param dExtra :: displacement extra [cm]
  */
{
  ELog::RegMethod RegA("ExternalCut","makeExpandedSurf(HR)");
  
  std::set<int> FS=HR.getSurfSet();
  for(const int& SN : FS)
    {
      const Geometry::Surface* SPtr=SMap.realSurfPtr(SN);
      // plane case does not use distance
      const Geometry::Plane* PPtr=
	dynamic_cast<const Geometry::Plane*>(SPtr);
      if (PPtr)
	{
	  int sideFlag=PPtr->side(expandCentre);
	  if (sideFlag==0) sideFlag=1;
	  ModelSupport::buildShiftedPlane
	    (SMap,index,PPtr, -sideFlag*dExtra);
	  return;
	}
      
      const Geometry::Cylinder* CPtr=
	dynamic_cast<const Geometry::Cylinder*>(SPtr);
      // Cylinder case:
      if (CPtr)
	{
	  // // this may not alway be what we want:
	   Geometry::Vec3D NVec=(CPtr->getCentre()-expandCentre);
	   const Geometry::Vec3D CAxis=CPtr->getNormal();
	   // now must remove component in axis direction
	   NVec-= CAxis * NVec.dotProd(CAxis);
	   NVec.makeUnit();
	   //	  const Geometry::Vec3D NC=CPtr->getCentre()+NVec*dExtra;
	   const Geometry::Vec3D NC=CPtr->getCentre();
	  
	  ModelSupport::buildCylinder
	    (SMap,index,NC,CPtr->getNormal(),CPtr->getRadius()+dExtra);
	  return;
	}
    }  
  throw ColErr::EmptyValue<int>("HeadRule contains no planes/cylinder");
} 
  
Geometry::Vec3D
ExternalCut::interPoint(const std::string& extName,
			     const Geometry::Vec3D& Centre,
			     const Geometry::Vec3D& CAxis) const
  /*!
    Calculate the intersection point on the points
    \param extName :: cutUnit name
    \param Centre :: Centre point of line
    \param CAxis :: Axis of line
    \return intersection point
  */
{
  ELog::RegMethod RegA("ExternalCut","interPoint");

  const cutUnit* CU=findUnit(extName);

  if (!CU) 
    throw ColErr::InContainerError<std::string>(extName,"Unit not found");
  
  
  return SurInter::getLinePoint(Centre,CAxis,CU->main,CU->divider);
}



void
ExternalCut::makeShiftedSurf(ModelSupport::surfRegister& SMap,
			    const std::string& extName,
			    const int index,
			    const int dFlag,
			    const Geometry::Vec3D& YAxis,
			    const double length) const
  /*!
    Support function to calculate the shifted surface based
    on surface type and form
    \param SMap :: local surface register
    \param extName :: cut unit name
    \param index :: offset index
    \param YAxis :: Direction of cylindical shift [NOT PLANE]
    \param dExtra :: displacement extra [cm]
  */
{
  ELog::RegMethod RegA("ExternalCut","makeShiftedSurf(string)");

  const cutUnit* CU=findUnit(extName);
  if (!CU)
    throw ColErr::InContainerError<std::string>(extName,"Unit not named");
  
  makeShiftedSurf(SMap,CU->main,index,dFlag,YAxis,length);

  return;
}

void
ExternalCut::makeExpandedSurf(ModelSupport::surfRegister& SMap,
			      const std::string& extName,
			      const int index,
			      const Geometry::Vec3D& expandCentre,
			      const double dExtra) const
  /*!
    Support function to calculate the shifted surface based
    on surface type and form
    \param SMap :: local surface register
    \param extName :: cut unit name
    \param index :: offset index
    \param expandCentre :: Centre for expansion
    \param dExtra :: displacement extra [cm]
  */
{
  ELog::RegMethod RegA("ExternalCut","makeExpandedSurf(string)");

  const cutUnit* CU=findUnit(extName);
  if (!CU)
    throw ColErr::InContainerError<std::string>(extName,"Unit not named");
  
  makeExpandedSurf(SMap,CU->main,index,expandCentre,dExtra);

  return;
}

  
}  // NAMESPACE attachSystem
