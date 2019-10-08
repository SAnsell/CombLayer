/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/FrontBackCut.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "HeadRule.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "SurInter.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

namespace attachSystem
{

FrontBackCut::FrontBackCut() :
  ExternalCut()
  /*!
    Constructor [default]
  */
{}

FrontBackCut::FrontBackCut(const FrontBackCut& A) :
  ExternalCut(A)
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
      ExternalCut::operator=(A);
    }
  return *this;
}
  

FrontBackCut::~FrontBackCut()
  /*!
    Destructor
  */
{}

void
FrontBackCut::setFront(const ExternalCut& FSurf)
  /*!
    Set a front wall
    \param FSurf :: Front object
  */
{
  ELog::RegMethod RegA("FrontBackCut","setFront(FrontBackCut)");

  ExternalCut::copyCutSurf("front",FSurf,"front");
  return;
}

void
FrontBackCut::setBack(const ExternalCut& BSurf)
  /*!
    Set a back wall
    \param BSurf :: back object
  */
{
  ELog::RegMethod RegA("FrontBackCut","setBack(FrontBackCut)");

  ExternalCut::copyCutSurf("back",BSurf,"back");
  return;
}

void
FrontBackCut::setFront(const int FSurf)
  /*!
    Set a front wall
    \param FSurf :: Front surface [signed]
  */
{
  ELog::RegMethod RegA("FrontBackCut","setFront(int)");

  ExternalCut::setCutSurf("front",FSurf);
  
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
  ExternalCut::setCutSurf("back",BSurf);
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

  ExternalCut::setCutSurf("front",FRule);
  return;
}

void
FrontBackCut::setBack(const std::string& BRule)
  /*!
    Set a back wall
    \param BRule :: Back rule string
  */
{
  ELog::RegMethod RegA("FrontBackCut","setBack(string)");

  ExternalCut::setCutSurf("back",BRule);
  return;
}

void
FrontBackCut::setFront(const attachSystem::FixedComp& WFC,
                       const std::string& sideName)
  /*!
    Set a front wall
    \param WFC :: Front line
    \param sideNAme :: link point
  */
{
  ELog::RegMethod RegA("FrontBackCut","setFront(FC,index)");

  ExternalCut::setCutSurf("front",WFC,sideName);
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
  ELog::RegMethod RegA("FrontBackCut","setFront(FC,index)");

  ExternalCut::setCutSurf("front",WFC,sideIndex);
  return;
}

void
FrontBackCut::setBack(const attachSystem::FixedComp& WFC,
                      const std::string& sideName)
  /*!
    Set a back wall
    \param WFC :: Back component
    \param sideName :: link point
  */
{
  ELog::RegMethod RegA("FrontBackCut","setBack");

  ExternalCut::setCutSurf("back",WFC,sideName);
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

  ExternalCut::setCutSurf("back",WFC,sideIndex);
  return;
}

void
FrontBackCut::setFrontDivider(const std::string& FDRule)
  /*!
    Set the front divider
    \param FDRule :: Front divider rule
  */
{
  ELog::RegMethod RegA("FrontBackCut","setFrontDivider");

  ExternalCut::setCutDivider("front",FDRule);
  return;
}

  
void
FrontBackCut::setFrontDivider(const HeadRule& HR)
  /*!
    Set the divider
    \param HR :: Divider surface
   */
{
  ExternalCut::setCutDivider("front",HR);
  return;
}

void
FrontBackCut::setBackDivider(const std::string& BDRule)
  /*!
    Set the back divider
    \param BDRule :: Back divider rule
  */
{
  ELog::RegMethod RegA("FrontBackCut","setBackDivider");

  ExternalCut::setCutDivider("back",BDRule);
  return;
}

void
FrontBackCut::setBackDivider(const HeadRule& HR)
  /*!
    Set the divider
    \param HR :: Divider surface
   */
{
  ExternalCut::setCutDivider("back",HR);
  return;
}

std::string
FrontBackCut::frontRule() const
  /*!
    Accessor  to front rule
    \return frontRule with divider
  */
{
  return ExternalCut::getRuleStr("front");  
}

std::string
FrontBackCut::frontComplement() const
  /*!
    Accessor to front rule (complement)
    \return frontRule.cmp with divider
  */
{
  return ExternalCut::getComplementStr("front");  
}

std::string
FrontBackCut::backRule() const
  /*!
    Accessor to back rule
    \return backRule with divider
  */
{
  return ExternalCut::getRuleStr("back");
}

std::string
FrontBackCut::backComplement() const
  /*!
    Accessor to back rule
    \return backRule with divider
  */
{
  return ExternalCut::getComplementStr("back");  
}

std::string
FrontBackCut::frontBridgeRule() const
  /*!
    Accessor to front bridge rule
    \return frontBridgeRule 
  */
{
  return ExternalCut::getBridgeStr("front");  
}

std::string
FrontBackCut::backBridgeRule() const
  /*!
    Accessor to back bridge rule
    \return backRule divider
  */
{
  return ExternalCut::getBridgeStr("back");  
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

  ExternalCut::createLink("front",FC,0,Org,-YAxis);
  ExternalCut::createLink("back",FC,1,Org,YAxis);
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

  ExternalCut::createLink("front",FC,0,Org,-YAxis);
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

  ExternalCut::createLink("back",FC,1,Org,YAxis);
  return;
}

void
FrontBackCut::getShiftedFront(ModelSupport::surfRegister& SMap,
			      const int surfIndex,
			      const int dFlag,
			      const Geometry::Vec3D& YAxis,
			      const double length) const
  /*!
    Support function to calculate the shifted surface for the front
    \param SMap :: Surface register
    \param surfIndex :: offset index [new]
    \param dFlag :: direction flag
    \param YAxis :: Axis for shift of sphere/cylinder
    \param length :: length to shift by
  */
{
  ELog::RegMethod RegA("FrontBackCut","getShiftedFront");

  ExternalCut::makeShiftedSurf(SMap,"front",surfIndex,dFlag,YAxis,length);
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
    \param surfIndex :: offset index [new]
    \param dFlag :: direction flag
    \param YAxis :: Axid for shift of sphere/cylinder
    \param length :: length to shift by
  */
{
  ELog::RegMethod RegA("FrontBackCut","getShiftedBack");

  ExternalCut::makeShiftedSurf(SMap,"back",surfIndex,dFlag,YAxis,length);
  return;
}

  
Geometry::Vec3D
FrontBackCut::frontInterPoint(const Geometry::Vec3D& Centre,
			      const Geometry::Vec3D& CAxis) const
  /*!
    Calculate the intersection point on the front  points
    \param Centre :: Centre point of line
    \param CAxis :: Axis of line
    \return intersection point
  */
{
  ELog::RegMethod RegA("FrontBackCut","frontInterPoint");

  return ExternalCut::interPoint("front",Centre,CAxis);  
}

Geometry::Vec3D
FrontBackCut::backInterPoint(const Geometry::Vec3D& Centre,
			      const Geometry::Vec3D& CAxis) const
  /*!
    Calculate the intersection point on the  back points
    \param Centre :: Centre point of line
    \param CAxis :: Axis of line
    \return intersection point
  */
{
  ELog::RegMethod RegA("FrontBackCut","backInterPoint");

  return ExternalCut::interPoint("back",Centre,CAxis);  
}
  
}  // NAMESPACE attachSystem
