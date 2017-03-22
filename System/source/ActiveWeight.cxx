/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/ActiveWeight.cxx
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
#include "doubleErr.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Source.h"
#include "SrcItem.h"
#include "SrcData.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "WorkData.h"
#include "MXcards.h"
#include "Zaid.h"
#include "Material.h"
#include "DBMaterial.h"
#include "ModeCard.h"
#include "Simulation.h"
#include "ActiveWeight.h"

namespace SDef
{

ActiveWeight::ActiveWeight() :
  distWeight(2.0),angleWeight(2.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
  */
{}

ActiveWeight::ActiveWeight(const ActiveWeight& A) : 
  nPoints(A.nPoints),Origin(A.Origin),Axis(A.Axis),
  distWeight(A.distWeight),angleWeight(A.angleWeight),
  ABoxPt(A.ABoxPt),BBoxPt(A.BBoxPt),matFlux(A.matFlux)
  /*!
    Copy constructor
    \param A :: ActiveWeight to copy
  */
{}

ActiveWeight&
ActiveWeight::operator=(const ActiveWeight& A)
  /*!
    Assignment operator
    \param A :: ActiveWeight to copy
    \return *this
  */
{
  if (this!=&A)
    {
      nPoints=A.nPoints;
      Origin=A.Origin;
      Axis=A.Axis;
      distWeight=A.distWeight;
      angleWeight=A.angleWeight;
      ABoxPt=A.ABoxPt;
      BBoxPt=A.BBoxPt;
      matFlux=A.matFlux;
    }
  return *this;
}

ActiveWeight::~ActiveWeight() 
  /*!
    Destructor
  */
{}

void
ActiveWeight::setBox(const Geometry::Vec3D& APt,
			 const Geometry::Vec3D& BPt)
  /*!
    Set the box coordinates
    \param APt :: Lower corner
    \param BPt :: Upper corner
   */

{
  ELog::RegMethod RegA("ActiveWeight","setBox");

  for(size_t i=0;i<3;i++)
    if (BPt[i]-APt[i]<Geometry::zeroTol)
      ELog::EM<<"Error with box coordinate: [Low - Hig] required :"
	      <<APt<<":"<<BPt<<ELog::endErr;
      
  ABoxPt=APt;
  BBoxPt=BPt;
  return;
}

void
ActiveWeight::setBiasConst(const Geometry::Vec3D& Cent,
                           const Geometry::Vec3D& AVec,
                           const double D,const double A)
  /*!
    Set the box coordinates
    \param Cent :: Origin Point
    \param AVec :: Axis direction
    \param D :: Distance weight
    \param A :: Angle weight
   */

{
  ELog::RegMethod RegA("ActiveWeight","setBiasConst");

  Origin=Cent;
  Axis=AVec;
  distWeight=D;
  angleWeight=A;

  return;
}
  
void
ActiveWeight::addMaterial(const std::string& matName,
			      const std::string& matFile)
  /*!
    Add a material to the system
    \param matName :: material name 
    \param matFile :: File to contain flux data
   */
{
  ELog::RegMethod RegA("ActiveWeight","addMaterial");

  WorkData Flux;

  if (!Flux.load(matFile,1,2,0))
    throw ColErr::FileError(0,matFile,"Not Loaded");

  const int matN=
    ModelSupport::DBMaterial::Instance().getIndex(matName);

  matFlux.emplace(matN,Flux);
  return;
}
  

void
ActiveWeight::createSource(Simulation& System,
                           const std::string& outputName)
  /*!
    Create all the source
    \param System :: Simulation to use
    \param outputName :: Output file
   */
{
  ELog::RegMethod RegA("ActiveWeight","createSource");

  for(size_t i=0;i<nPoints;i++)
    {
      // construct cone on axis
    }
  
  return;
}


} // NAMESPACE SDef
