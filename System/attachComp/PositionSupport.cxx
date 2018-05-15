/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/PositionSupport.cxx
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
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "surfRegister.h"
#include "support.h"

#include "LinkUnit.h"
#include "FixedComp.h"
#include "objectRegister.h"
#include "inputParam.h"
#include "LinkSupport.h"
#include "PositionSupport.h"


namespace attachSystem
{

void
applyZAxisRotate(const FixedComp& FC,const double xyAngle,
		 Geometry::Vec3D& V)
 /*!
   Apply a rotation to the vector around the z axis
   \param FC :: Object to get Z axis from 
   \param xyAngle :: Angle in degrees
   \param V :: Vector to rotate
 */
{
  ELog::RegMethod RegA("PositionSupport[F]","applyZAxisRotate");

  if (fabs(xyAngle)>Geometry::zeroTol)
    {
      const Geometry::Quaternion Qxy=
	Geometry::Quaternion::calcQRotDeg(xyAngle,FC.getZ());
      Qxy.rotate(V);
    }
  return;
}

Geometry::Vec3D
getCntVec3D(const mainSystem::inputParam& IParam,
            const std::string& K,const size_t setIndex,
            size_t& itemIndex) 
  /*!
    Get a Vec3D [consuming a number of itemIndex]
    \param K :: keyName
    \param setIndex :: Index
    \param itemIndex :: item count
    \return Vecter point
  */ 
{ 
  ELog::RegMethod RegA("PositionSuppport[F]","getCntVec3D");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const std::vector<std::string>& DItems=
    IParam.getObjectItems(K,setIndex);

  const size_t NItems=DItems.size();
  if (itemIndex>=NItems)
    throw ColErr::IndexError<size_t>
      (itemIndex,NItems,K+":itemIndex");

  Geometry::Vec3D Value;
  // Simple Vec3D
  if (StrFunc::convert(DItems[itemIndex],Value))
    {
      itemIndex++;
      return Value;
    }

  // Flat numbers:
  if (itemIndex+3<=NItems &&
      StrFunc::convert(DItems[itemIndex],Value[0]) &&
      StrFunc::convert(DItems[itemIndex+1],Value[1]) &&
      StrFunc::convert(DItems[itemIndex+2],Value[2]) )
    {
      itemIndex+=3;
      return Value;
    }
  // object name 
  if (itemIndex+3<=NItems && DItems[itemIndex]=="object")
    {
      const attachSystem::FixedComp* TPtr=
	OR.getObjectThrow<attachSystem::FixedComp>
	(DItems[itemIndex+1],"FixedComp");
      
      const long int linkNumber=
        attachSystem::getLinkIndex(DItems[itemIndex+2]);
      Value=TPtr->getLinkPt(linkNumber);
      itemIndex+=3;
      return Value;
    }

  // objOffset name 
  if (itemIndex+4<=NItems && DItems[itemIndex]=="objOffset")
    {
      const attachSystem::FixedComp* TPtr=
        OR.getObjectThrow<attachSystem::FixedComp>(DItems[itemIndex+1],
						   "FixedComp");
      const long int linkNumber=attachSystem::getLinkIndex(DItems[itemIndex+2]);
      Value=TPtr->getLinkPt(linkNumber);

      Geometry::Vec3D DVec;
      if (StrFunc::convert(DItems[itemIndex+3],DVec))
        {
          itemIndex+=4;
        }
      else if (itemIndex+6<NItems &&
               StrFunc::convert(DItems[itemIndex+3],DVec[0]) &&
               StrFunc::convert(DItems[itemIndex+4],DVec[1]) &&
               StrFunc::convert(DItems[itemIndex+5],DVec[2]) )
        {
          itemIndex+=6;
        }
      else
        throw ColErr::InContainerError<std::string>
          (DItems[4],"None-converted Offset (objOffset)");      
      
      Geometry::Vec3D XDir,YDir,ZDir;
      TPtr->calcLinkAxis(linkNumber,XDir,YDir,ZDir);
      Value+=XDir*DVec[0]+YDir*DVec[1]+ZDir*DVec[2];
      return Value;
    }
  // implied object name 
  if (itemIndex+2<=NItems)
    {
      ELog::EM<<"Item == "<<DItems[itemIndex]<<ELog::endDiag;
      const attachSystem::FixedComp* TPtr=
        OR.getObjectThrow<attachSystem::FixedComp>(DItems[itemIndex],
						   "FixedComp");

      const long int linkNumber=attachSystem::getLinkIndex(DItems[itemIndex+1]);
      Value=TPtr->getLinkPt(linkNumber);
      ELog::EM<<"Item == "<<DItems[itemIndex]<<" "<<Value<<ELog::endDiag;
      itemIndex+=2;
      return Value;
    }
  
  throw ColErr::TypeMatch(DItems[itemIndex],
                          "Geomtery::Vec3D",K+":convert error");

}

  
  

}  // NAMESPACE attachSystem
