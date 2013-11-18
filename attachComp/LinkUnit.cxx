/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachComp/LinkUnit.cxx
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
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <deque>
#include <string>
#include <algorithm>
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Triple.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"
#include "Surface.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "BnId.h"
#include "AttachSupport.h"
#include "LinkUnit.h"

namespace attachSystem
{

LinkUnit::LinkUnit() :
  populated(0),linkSurf(0)
  /*!
    Constructor 
  */
{}

LinkUnit::LinkUnit(const LinkUnit& A) : 
  populated(A.populated),Axis(A.Axis),
  ConnectPt(A.ConnectPt),linkSurf(A.linkSurf),
  bridgeSurf(A.bridgeSurf)
  /*!
    Copy constructor
    \param A :: LinkUnit to copy
  */
{}

LinkUnit&
LinkUnit::operator=(const LinkUnit& A)
  /*!
    Assignment operator
    \param A :: LinkUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      populated=A.populated;
      Axis=A.Axis;
      ConnectPt=A.ConnectPt;
      bridgeSurf=A.bridgeSurf;
    }
  return *this;
}

LinkUnit::~LinkUnit()
  /*!
    Deletion operator
  */
{}

void
LinkUnit::complement()
  /*!
    Convert the object into complement form
   */
{
  ELog::RegMethod RegA("LinkUnit","complement");
  Axis*= -1.0;  // reverse axis
  linkSurf*=-1;
  bridgeSurf.makeComplement();
  return;
}

const Geometry::Vec3D& 
LinkUnit::getConnectPt() const
  /*!
    Get the centre value if set
    \throw EmptyValue<void> if not populated
    \return ConnectPt unit 
   */
{
  ELog::RegMethod RegA("LinkUnit","getConnectPt");
  if (!(populated & 2))
    throw ColErr::EmptyValue<void>("Unpopulated ConnectPt");

  return ConnectPt;
}

const Geometry::Vec3D& 
LinkUnit::getAxis() const
  /*!
    Get the Axis value if set
    \throw EmptyValue<void> if not populated
    \return ConnectPt unit 
   */
{
  ELog::RegMethod RegA("LinkUnit","getAxis");
  if (!(populated & 1))
    throw ColErr::EmptyValue<void>("populated & 1");
  return Axis;
}


void
LinkUnit::setAxis(const Geometry::Vec3D& A)
  /*!
    Set the Axis
    \param A :: Direction axis
  */
{
  Axis=A.unit();
  populated |= 1;
  return;
}

void
LinkUnit::setConnectPt(const Geometry::Vec3D& C)
  /*!
    Set the ConnectPt
    \param C :: ConnectPt coordinate
   */
{
  ConnectPt=C;
  populated |= 2;
  return;
}

void
LinkUnit::setBridgeSurf(const int SN) 
  /*!
    Add a surface to the output
    \param SN :: Surface number [inward looking]
  */
{
  ELog::RegMethod RegA("LinkUnit","setBridgeSurf");
  bridgeSurf.reset();
  linkSurf=SN;
  bridgeSurf.addIntersection(SN);
  return;
}

void
LinkUnit::setBridgeSurf(const std::string& SList) 
  /*!
    Add a surface to the output
    \param SList :: String set to add
  */
{
  ELog::RegMethod RegA("LinkUnit","setBridgeSurf");
  bridgeSurf.reset();
  linkSurf=0;
  addBridgeSurf(SList);
  return;
}

void
LinkUnit::addBridgeSurf(const int SN) 
  /*!
    Add a surface to the output
    \param SN :: Surface number [inward looking]
  */
{
  ELog::RegMethod RegA("LinkUnit","addBridgeSurf");
  if (!bridgeSurf.hasRule()) 
    linkSurf=SN;
  bridgeSurf.addIntersection(SN);
  return;
}

void
LinkUnit::addBridgeSurf(const std::string& SList) 
  /*!
    Add a set of surfaces to the output
    \param SList ::  Surface string [fully decomposed]
  */
{
  ELog::RegMethod RegA("LinkUnit","addInterSurf(std::string)");
  if (!bridgeSurf.hasRule()) 
    StrFunc::convert(SList,linkSurf); 

  bridgeSurf.addIntersection(SList);
  return;
}


std::string
LinkUnit::getCommon() const
  /*!
    Calculate the write out the excluded surface
    \return Exclude string [union]
  */
{
  ELog::RegMethod RegA("LinkUnit","getCommon");

  return bridgeSurf.display();
}

int 
LinkUnit::getLinkSurf() const 
  /*!
    Get the linked surface [if one and only one exists]
    \throw EmptyValue if linkSurf not set
    \return linked surface number
  */
{
  ELog::RegMethod RegA("LinkUnit","getLinkSurf");
  if (!linkSurf)
    throw ColErr::EmptyValue<int>("Bridge surf ="+getCommon());
      
  return linkSurf;
}

std::string
LinkUnit::getLinkString() const 
  /*!
    Get the linked string
    \return linked surface number
  */
{
  ELog::RegMethod RegA("LinkUnit","getLinkSurf");
  if (bridgeSurf.hasRule())
    return " "+bridgeSurf.display()+" ";

  std::ostringstream cx;
  cx<<" "<<linkSurf<<" ";

  return cx.str();
}

}  // NAMESPACE attachSystem
