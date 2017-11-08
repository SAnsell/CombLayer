/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/DXTConstructor.cxx
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
#include <string>
#include <algorithm>
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "MapRange.h"
#include "Triple.h"
#include "support.h"
#include "stringCombine.h"
#include "NRange.h"
#include "Transform.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MainProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinkSupport.h"
#include "Simulation.h"
#include "inputParam.h"

#include "ModeCard.h"
#include "PhysImp.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "NList.h"
#include "NRange.h"

#include "PhysicsCards.h"

#include "DXTControl.h"
#include "DXTConstructor.h" 

namespace physicsSystem
{

DXTConstructor::DXTConstructor() 
  /// Constructor
{}
  
void
DXTConstructor::processDD(Simulation& System,
			  const mainSystem::inputParam& IParam,
			  const size_t Index) 
  /*!
    Add a simple dd card
    \param System :: Simulation to get physics/fixed points
    \param IParam :: Main input parameters
    \param Index :: index of the -wDXT card
   */
{
  ELog::RegMethod RegA("DXTConstructor","processDD");

  const size_t NParam=IParam.itemCnt("wDD",Index);
  if (NParam<2)
    throw ColErr::IndexError<size_t>(NParam,2,"Insufficient items wDXT");

  // Get all values:
  DXTControl& DXT=System.getPC().getDXTCard();
  for(size_t j=1;j<NParam;j+=2)
    {
      
      const double DDk=IParam.getValue<double>("wDD",Index,j-1);
      const double DDm=IParam.getValue<double>("wDD",Index,j);
      DXT.setDD(DDk,DDm);
    }
  return;
}
 
void
DXTConstructor::processUnit(Simulation& System,
			    const mainSystem::inputParam& IParam,
			    const size_t Index) 
 /*!
   Add dxtran component 
   \param System :: Simulation to get physics/fixed points
   \param IParam :: Main input parameters
   \param Index :: index of the -wDXT card
 */
{
  ELog::RegMethod RegA("DXTConstructor","processPoint");

  const size_t NParam=IParam.itemCnt("wDXT",Index);

  if (NParam<1)
    throw ColErr::IndexError<size_t>(NParam,3,"Insufficient items wDXT");

  std::string dxtName=IParam.getValue<std::string>("wDXT",Index,0);
  dxtName[0]=static_cast<char>(std::tolower(dxtName[0]));
  if (dxtName=="help")
    {
      writeHelp(ELog::EM.Estream());
      ELog::EM<<ELog::endBasic;
      return;
    }
  DXTControl& DXT=System.getPC().getDXTCard();
  double RI,RO;
  if (dxtName=="object" || dxtName=="objOffset")
    {
      const std::string place=
	IParam.outputItem<std::string>("wDXT",Index,1,"position not given");
      const std::string linkPt=
	IParam.outputItem<std::string>("wDXT",Index,2,"position not given");
      Geometry::Vec3D PPoint,XAxis,YAxis,ZAxis;
      if (!attachSystem::getAttachPointWithXYZ
             (place,linkPt,PPoint,XAxis,YAxis,ZAxis) )        
	throw ColErr::InContainerError<std::string>
	  (place,"Fixed Object not found");

      size_t itemCnt(3);
      if (dxtName=="objOffset")
        {
          const Geometry::Vec3D DVec=
            IParam.getCntVec3D("wDXT",Index,itemCnt,"Offset");
          PPoint+=XAxis*DVec[0]+YAxis*DVec[1]+ZAxis*DVec[2];

        }
      
      RI=IParam.outputItem<double>("wDXT",Index,itemCnt,"radius not given");
      if (!IParam.checkItem("wDXT",Index,itemCnt+1,RO))
	RO=RI;
      ELog::EM<<"DXT Centre Point == "<<PPoint<<ELog::endDiag;
      DXT.setUnit(PPoint,RI,RO,0);
    }
  else if (dxtName=="free")
    {
      size_t itemIndex(1);
      const Geometry::Vec3D PPoint=
	IParam.getCntVec3D("wDXT",Index,itemIndex,"Point position not given");

      RI=IParam.outputItem<double>("wDXT",Index,itemIndex,"radius not given");
      if (!IParam.checkItem("wDXT",Index,itemIndex+1,RO))
	RO=RI;
      DXT.setUnit(PPoint,RI,RO,0);
    }
  
  return;
}

void
DXTConstructor::writeHelp(std::ostream& OX) const
  /*!
    Write out help
    \param OX :: Output stream
  */
{
    OX<<"-wDXT PointID radius\n"
      " :: \n"
      "   object [objectName] linkNumber radius {radiusOuter}\n"
      "   objOffset [objectName] linkNumber OffsetVector"
      " radius {radiusOuter} \n"
      "   free Vec3D radius \n";
    OX<<"-wDD [Kvalue Dvalue] \n";
    OX<<"-wDXT  \n";
  return;
}


}  // NAMESPACE physicsSystem

