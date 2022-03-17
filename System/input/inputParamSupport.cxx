/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   input/inputParamSupport.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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

#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "support.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "BaseMap.h"
#include "PointMap.h"
#include "SurfMap.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Cone.h"
#include "Cylinder.h"
#include "Plane.h"


namespace mainSystem
{

Geometry::Vec3D
getNamedPoint(const Simulation& System,
	      const inputParam& IParam,
	      const std::string& keyItem,
	      const size_t setIndex,
	      size_t& index,
	      const std::string& errStr)
  /*!
    Generate a named point : One of
    - Vec3D(x,y,z)
    - FixedComp:Index
    - PointMap:Index
  */
{
  ELog::RegMethod RegA("inputParamSupport[F]","getNamedPoint");  

  const std::string objName=
    IParam.getValueError<std::string>
    (keyItem,setIndex,index,errStr+"[Object Name/Point]");

  index++;
  Geometry::Vec3D point;
  if (StrFunc::convert(objName,point))
    return point;

  const std::string::size_type pos=objName.find(':');
  if (pos!=std::string::npos)
    {
      const std::string unitFC=objName.substr(0,pos);
      std::string indexName=objName.substr(pos+1);
      
      // unitFC MUST work and indexName can be number/name
      const attachSystem::FixedComp* FCptr=
	System.getObjectThrow<attachSystem::FixedComp>(unitFC,errStr);
      // PointMap
      const attachSystem::PointMap* PMptr=
	dynamic_cast<const attachSystem::PointMap*>(FCptr);
      if (PMptr)
	{
	  const std::string::size_type posB=indexName.find(':');
	  size_t itemIndex(0);
	  if (posB!=std::string::npos &&
	      StrFunc::convert(indexName.substr(posB+1),itemIndex))
	    {
	      indexName.erase(posB,std::string::npos);
	    }
	  if (PMptr->hasPoint(indexName,itemIndex))
	    return PMptr->getPoint(indexName,itemIndex);
	}
      // FixedComp
      if (FCptr->hasLinkPt(indexName))
	return FCptr->getLinkPt(indexName);
    }
  
  // Everything failed
  index--;
  throw ColErr::InContainerError<std::string>(objName,errStr);
}

Geometry::Vec3D
getDefNamedPoint(const Simulation& System,
		 const inputParam& IParam,
		 const std::string& keyItem,
		 const size_t setIndex,
		 size_t& index,
		 const Geometry::Vec3D& defValue)
  /*!
    Generate a named point : One of
    - Vec3D(x,y,z)
    - FixedComp:Index
    - PointMap:Index
    \param defValue :: Returned value
  */
{
  ELog::RegMethod RegA("inputParamSupport[F]","getDefNamedPoint");  

  try
    {
      return getNamedPoint(System,IParam,keyItem,setIndex,index,"");
    }
  catch (const ColErr::ExBase&)
    {
      return defValue;
    }
}

Geometry::Vec3D
getNamedAxis(const Simulation& System,
	     const inputParam& IParam,
	     const std::string& keyItem,
	     const size_t setIndex,
	     size_t& index,
	     const std::string& errStr)
/*!
  Generate a named Axis : One of:
  - Vec3D(x,y,z).norm()
  - FixedComp:Index
  - SurfMap:Index. plane.normal/cyclinder.axis/cone.axis
*/
{
  ELog::RegMethod RegA("inputParamSupport[F]","getNamedAxis");  

    const std::string objName=
      IParam.getValueError<std::string>
      (keyItem,setIndex,index,errStr+"[Object Name/Point]");

    index++;      // add one -- subtract if faile

    Geometry::Vec3D point;
    if (StrFunc::convert(objName,point))
      return point.unit();

    const std::string::size_type pos=objName.find(':');
    if (pos!=std::string::npos)
      {
	const std::string unitFC=objName.substr(0,pos);
	std::string indexName=objName.substr(pos+1);
      
	// unitFC MUST work and indexName can be number/name
	const attachSystem::FixedComp* FCptr=
	  System.getObjectThrow<attachSystem::FixedComp>(unitFC,errStr);
	// SurfMap
	const attachSystem::SurfMap* SMptr=
	  dynamic_cast<const attachSystem::SurfMap*>(FCptr);
	if (SMptr)
	  {
	    const std::string::size_type posB=indexName.find(':');
	    size_t itemIndex(0);
	    if (posB!=std::string::npos &&
		StrFunc::convert(indexName.substr(posB+1),itemIndex))
	      {
		indexName.erase(posB,std::string::npos);
	      }
	    if (SMptr->hasSurf(indexName,itemIndex))
	      {
		const Geometry::Surface* SPtr=
		  SMptr->getSurfPtr(indexName,itemIndex);
		if (const Geometry::Plane* PPtr=
		    dynamic_cast<const Geometry::Plane*>(SPtr))
		  return PPtr->getNormal();
		// cylinder
		const Geometry::Cylinder* CPtr=
		  dynamic_cast<const Geometry::Cylinder*>(SPtr);
		if (CPtr)
		  return CPtr->getNormal();
		// cone
		const Geometry::Cone* CnPtr=
		  dynamic_cast<const Geometry::Cone*>(SPtr);
		if (CnPtr)
		  return CnPtr->getNormal();
	      }
	  }
	// FixedComp:Axis
	if (FCptr->hasLinkPt(indexName))
	  return FCptr->getLinkAxis(indexName);
      }
  
    // Everything failed: BUT need to reset index
    index--;
    throw ColErr::InContainerError<std::string>(objName,errStr);
  }

Geometry::Vec3D
getDefNamedAxis(const Simulation& System,
		const inputParam& IParam,
		const std::string& keyItem,
		const size_t setIndex,
		size_t& index,
		const Geometry::Vec3D& defValue)
/*!
  Generate a named point : One of
  - Vec3D(x,y,z)
  - FixedComp:Index
  - PointMap:Index
  \param defValue :: Returned value
*/
{
  ELog::RegMethod RegA("inputParamSupport[F]","getDefNamedPoint");  

  try
    {
      return getNamedAxis(System,IParam,keyItem,setIndex,index,"");
    }
  catch (const ColErr::ExBase&)
    {
      return defValue;
    }
}


std::tuple<Geometry::Vec3D,Geometry::Vec3D,Geometry::Vec3D>
getNamedOriginAxis(const Simulation& System,
		   const inputParam& IParam,
		   const std::string& keyItem,
		   const size_t setIndex,
		   size_t& index,
		   const std::string& errStr)
/*!
    Generate a set of point + axis directions based on
    free point or FixedComp or PointMap
    \return Point : Y axis : Z axis
  */
{
  ELog::RegMethod RegA("inputParamSupport[F]","getNamedOrginAxis");  

  const std::string objName=
    IParam.getValueError<std::string>
    (keyItem,setIndex,index++,errStr+"[Object Name/Point]");

  int nFound(0);
  Geometry::Vec3D Org;
  Geometry::Vec3D YAxis;
  Geometry::Vec3D ZAxis;
  
  if (!StrFunc::convert(objName,Org))
    {
      const std::string::size_type pos=objName.find(':');
      if (pos!=std::string::npos)
	{
	  const std::string unitFC=objName.substr(0,pos);
	  std::string indexName=objName.substr(pos+1);
	  
	  // unitFC MUST work and indexName can be number/name
	  const attachSystem::FixedComp* FCptr=
	    System.getObjectThrow<attachSystem::FixedComp>(unitFC,errStr);
	  // PointMap
	  const attachSystem::PointMap* PMptr=
	    dynamic_cast<const attachSystem::PointMap*>(FCptr);
	  if (PMptr)
	    {
	      const std::string::size_type posB=indexName.find(':');
	      size_t itemIndex(0);
	      if (posB!=std::string::npos &&
		  StrFunc::convert(indexName.substr(posB+1),itemIndex))
		{
		  indexName.erase(posB,std::string::npos);
		}
	      if (PMptr->hasPoint(indexName,itemIndex))
		{
		  nFound=1;
		  Org=PMptr->getPoint(indexName,itemIndex);
		}
	    }
	  // FixedComp
	  if (FCptr->hasLinkPt(indexName))
	    {
	      nFound=3;
	      const long int linkIndex=FCptr->getSideIndex(indexName);
	      Org=FCptr->getLinkPt(linkIndex);
	      YAxis=FCptr->getLinkAxis(linkIndex);
	      ZAxis=FCptr->getLinkZAxis(linkIndex);
	    }
	}
    }
  else
    nFound=1;
  // could have found point
  if (nFound==1)
    {
      YAxis=IParam.getCntVec3D(keyItem,setIndex,index,errStr+":YAxis");
      ZAxis=IParam.getCntVec3D(keyItem,setIndex,index,errStr+":ZAxis");    
    }
  
  // Everything failed
  if (nFound!=3)
    throw ColErr::InContainerError<std::string>(objName,errStr);

  return std::tuple<Geometry::Vec3D,Geometry::Vec3D,Geometry::Vec3D>
    (Org,YAxis,ZAxis);
}


std::vector<int>
getNamedCells(const Simulation& System,
	      const inputParam& IParam,
	      const std::string& keyItem,
	      const long int setIndex,
	      const long int index,
	      const std::string& errStr)
  /*!
    Calculate the objects based on a name e.g. a FixedComp.
    \param System :: Main simulation
    \param IParam :: Input parameters
    \param keyItem :: key Item to search fore
    \param setIndex :: input set index
    \param index :: item index
    \param errStr :: base of error string
  */  
{
  ELog::RegMethod RegA("inputParamSupport[F]","getNamedCells");

  const std::string objName=
    IParam.getValueError<std::string>
    (keyItem,setIndex,index,errStr+"[Object Name]");

  const std::vector<int> Cells=System.getObjectRange(objName);
  if (Cells.empty())
    throw ColErr::InContainerError<std::string>
      (objName,errStr+" [Empty cell]");

  return Cells;
}

std::set<MonteCarlo::Object*>
getNamedObjects(const Simulation& System,
		const inputParam& IParam,
		const std::string& keyItem,
		const long int setIndex,
		const long int index,
		const std::string& errStr)
  /*!
    Calculate the objects based on a name e.g. a FixedComp.
    \param System :: Main simulation
    \param IParam :: Input parameters
    \param keyItem :: key Item to search for
    \param setIndex :: input set index
    \param index :: item index
    \param errStr :: base of error string
  */  
{
  ELog::RegMethod RegA("inputParamSupport[F]","getNamedObjects");

  const std::vector<int> Cells=
    getNamedCells(System,IParam,keyItem,setIndex,index,errStr);

  std::set<MonteCarlo::Object*> outObjects;
    

  const Simulation::OTYPE& CellObjects=System.getCells();
  // Special to set cells in OBJECT  [REMOVE]
  for(const int CN : Cells)
    {
      Simulation::OTYPE::const_iterator mc=
	CellObjects.find(CN);
      if (mc!=CellObjects.end())
	outObjects.emplace(mc->second);
    }
  
  
  return outObjects;
}

} // NAMESPACE mainSystem
