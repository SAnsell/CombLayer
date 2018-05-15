/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/refPlate.cxx
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
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <complex>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <memory>

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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
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
#include "generateSurf.h"
#include "objectRegister.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"

#include "refPlate.h"

namespace ts1System
{

refPlate::refPlate(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  pIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(pIndex+1),planeFlag(0),matN(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

size_t
refPlate::dirType(const std::string& DN)
  /*!
    Determine the direction based on the +/- and x,y,z value
    in the string
    \param DN :: direction [+/- or xyz]
    \return 0-5 based no the side
   */
{
  ELog::RegMethod RegA("refPlate","dirType");

  if (DN.empty())
    throw ColErr::EmptyValue<std::string>("DN");
    
  const int plus=(DN.size()<2 || DN[0]=='+') ? 1 : 0;
  const int dirX=(DN.size()<2) ? tolower(DN[0]) : tolower(DN[1]);
  const int retVal=2*(dirX-static_cast<int>('x'))+plus;

  if (retVal<0 || retVal>5)
    throw ColErr::IndexError<std::string>(DN,"xyz","DN");

  return static_cast<size_t>(retVal);
}

size_t
refPlate::dirOppositeType(const std::string& DN)
  /*!
    Determine the otherside- direction based on the +/- and x,y,z value
    in the string 
    \param DN :: direction [+/- or xyz]
    \return 0-5 based on the side
   */
{
  ELog::RegMethod RegA("refPlate","dirOppositeType");
  size_t out=dirType(DN);
  return (out % 2) ? out-1 : out+1;
}

void
refPlate::setOrigin(const std::string& Name,
		    const long int Index)
  /*!
    Takes a named object (from the object-register), used the
    link surface given by Index as the origin and the axis is
    the Y-axis [+ve].
    \param Name :: FixedComp keyname
    \param Index :: Link point coordinate [\todo move to long int]
  */
{
  ELog::RegMethod RegA("refPlate","setOrigin(Name,Index)");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const attachSystem::FixedComp* OPtr=
    OR.getObjectThrow<attachSystem::FixedComp>(Name,"FixedComp");
  
  setOrigin(*OPtr,Index);

  return;
}

void
refPlate::setOrigin(const attachSystem::FixedComp& FC,
		    const long int Index)
  /*!
    Takes a named object (from the object-register), used the
    link surface given by Index as the origin and the axis is
    the Y-axis [+ve].
    \param FC :: FixedComp 
    \param Index :: Surface size [not-signed]
  */
{
  ELog::RegMethod RegA("refPlate","setOrigin(FC,size_t)");
  
  Origin= FC.getLinkPt(Index);
  Y= FC.getLinkAxis(-Index);
  SN[2]= FC.getLinkSurf(Index);

  FixedComp::setLinkSurf(2,-SN[2]);
  FixedComp::setConnect(2,Origin,-Y);
  planeFlag |= 4;

  return;
}

void
refPlate::setPlane(const std::string& dirName,
		   const std::string& Name,
		   const long int LIndex)
  /*!
    Takes a named object (from the object-register), used the
    link surface given by Index as the origin and the axis is
    the Y-axis [+ve].
    \param dirName :: Direction name
    \param Name :: FixedComp keyname
    \param LIndex :: Link surface index 
  */
{
  ELog::RegMethod RegA("refPlate","setPlane(string,index)");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const attachSystem::FixedComp* OPtr=
    OR.getObjectThrow<attachSystem::FixedComp>(Name,"FixedComp not found");

  setPlane(dirName,*OPtr,LIndex);
  return;
}

void
refPlate::setPlane(const std::string& dirName,
		   const FixedComp& FC,
		   const long int LIndex)
  /*!
    Takes a named object (from the object-register), used the
    link surface given by Index as the origin and the axis is
    the Y-axis [+ve].

    \param dirName :: Direction name
    \param Name :: FixedComp keyname
    \param LIndex :: Link surface index 
  */
{
  ELog::RegMethod RegA("refPlate","setPlane(string,FC)");

  // Calculate the sense of the surface:
  const size_t DIndex=dirType(dirName);


  SN[DIndex]=FC.getLinkSurf(LIndex);
  FixedComp::setLinkSignedCopy(DIndex,FC,LIndex);

  planeFlag |= (DIndex) ? 2 << (DIndex-1) : 1;            
    
  return;
}

void
refPlate::setPlane(const std::string& dirName,
		   const Geometry::Vec3D& Cent,
		   const Geometry::Vec3D& Axis)
  /*!
    Takes a named object (from the object-register), used the
    link surface given by Index as the origin and the axis is
    the Y-axis [+ve].

    If index is -ve then it is -(index+1) and the surface sense 
    is reversed.

    \param dirName :: Direction name
    \param Axis :: Axis on surface
    \param Cent :: Centre point
  */
{
  ELog::RegMethod RegA("refPlate","setPlane(string,Vec3D,Vec3D)");

  const int DIndex=static_cast<int>(dirType(dirName));
  FixedComp::setConnect(static_cast<size_t>(DIndex),Cent,Axis);
  ModelSupport::buildPlane(SMap,pIndex+DIndex,Cent,Axis);
  
  SN[DIndex]= -(pIndex+DIndex);
  FixedComp::setLinkSurf(static_cast<size_t>(DIndex),pIndex+DIndex);
  planeFlag |= (DIndex) ? 2 << (DIndex-1) : 1;            
  return;
}

void
refPlate::setPlane(const std::string& dirName,
		   const double D)
  /*!
    Takes a named object (from the object-register), used the
    link surface given by Index as the origin and the axis is
    the Y-axis [+ve].

    \param dirName :: Direction name
    \param D :: distance
  */
{
  ELog::RegMethod RegA("refPlate","setPlane(string,double)");

  const size_t DIndex=dirType(dirName);
  const size_t AltIndex=dirOppositeType(dirName);

  Geometry::Vec3D Pt=
    this->getLinkPt(static_cast<long int>(AltIndex+1));
  Geometry::Vec3D Axis=
    this->getLinkAxis(static_cast<long int>(AltIndex+1));

  FixedComp::setConnect(DIndex,Pt-Axis*D,-Axis);
  ModelSupport::buildPlane(SMap,pIndex+static_cast<int>(DIndex),
			   Pt-Axis*D,-Axis);

  SN[DIndex]= -SMap.realSurf(pIndex+static_cast<int>(DIndex));
  FixedComp::setLinkSurf(DIndex,pIndex+static_cast<int>(DIndex));

  planeFlag |= (DIndex) ? 2 << (DIndex-1) : 1;
  return;
}

void
refPlate::createObjects(Simulation& System)
  /*!
    Create the objects
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("refPlate","createObjects");

  std::ostringstream cx;
  for(size_t i=0;i<6;i++)
    cx<<SN[i]<<" ";

  std::string Out=cx.str();
  addOuterSurf(Out);

  System.addCell(MonteCarlo::Qhull(cellIndex++,matN,0.0,Out));
  
  return;
}

void
refPlate::createAll(Simulation& System)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
  */
{
  ELog::RegMethod RegA("refPlate","createAll");

  createObjects(System);
  insertObjects(System);       
  return;
}

} // NAMESPACE ts1System
