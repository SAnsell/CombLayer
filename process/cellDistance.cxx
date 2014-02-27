/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/cellDistance.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <set> 
#include <map> 
#include <string>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

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
#include "varList.h"
#include "Code.h"
#include "FItem.h"
#include "FuncDataBase.h"
#include "BnId.h"
#include "Rules.h"
#include "neutron.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "ObjSurfMap.h"
#include "objectRegister.h"
#include "Simulation.h"
#include "cellDistance.h"


namespace ModelSupport
{

cellDistance::cellDistance(const Simulation& System) :
  ASim(&System),initObj(0),aRange(0),bRange(0)
 /*!
   Constructor
   \param System :: simulation
 */
{}

cellDistance::cellDistance(const cellDistance& A) : 
  ASim(A.ASim),initObj(A.initObj),aRange(A.aRange),
  bRange(A.bRange)
  /*!
    Copy constructor
    \param A :: cellDistance to copy
  */
{}

cellDistance&
cellDistance::operator=(const cellDistance& A)
  /*!
    Assignment operator
    \param A :: cellDistance to copy
    \return *this
  */
{
  if (this!=&A)
    {
      initObj=A.initObj;
      aRange=A.aRange;
      bRange=A.bRange;
    }
  return *this;
}

void
cellDistance::setRange(const std::string& Item)
  /*!
    Set the aim range
    \param Item :: Fixed item to use
  */
{
  ELog::RegMethod RegA("cellDistance","setRange");

  ModelSupport::objectRegister& MR=
    ModelSupport::objectRegister::Instance();
  
  const int A=MR.getCell(Item);
  const int R=MR.getRange(Item);
  if (!A)
    throw ColErr::InContainerError<std::string>(Item,"Item");
      
  aRange=A;
  bRange=A+R;

  return;
}

Geometry::Vec3D
cellDistance::calcPoint(const Geometry::Vec3D& Pt,
			const Geometry::Vec3D& V,
			const std::string& Range)
  /*!
    Calculate the distance to the first point on the object
    \param Pt :: Point to start from
    \param V :: Direction
    \param Range :: Object name to get cell range from
  */
{
  ELog::RegMethod RegA("cellDistance","calcPoint");

  
  const Geometry::Surface* SPtr;          // Output surface
  double aDist;                           // Output distribution
  
  setRange(Range);

  MonteCarlo::neutron TNeut(MonteCarlo::neutron(10,Pt,V));
  const ModelSupport::ObjSurfMap* OSMPtr =ASim->getOSM();

  initObj=ASim->findCell(TNeut.Pos,initObj);  
  MonteCarlo::Object* OPtr=initObj;

  int flag(0);
  int SN(0);      // if on a surface boundary : surf not to be used
  while(OPtr)
    {
      SN= -OPtr->trackOutCell(TNeut,aDist,SPtr,-SN);
      TNeut.moveForward(aDist);
      flag = (SN>=aRange && SN<bRange) ? 0 : 1;

      OPtr=OSMPtr->findNextObject(SN,TNeut.Pos,OPtr->getName());
      if (OPtr->getName()>aRange && OPtr->getName()<bRange)
	return TNeut.Pos;
    }

  ELog::EM<<"Failed to find hit "<<ELog::endErr;
  return TNeut.Pos;
}

} // Namespace ModelSupport
