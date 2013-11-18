/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   source/ChipIRSource.cxx
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
#include <string>
#include <algorithm>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "SecondTrack.h"
#include "TwinComp.h"
#include "LinearComp.h"
#include "InsertComp.h"
#include "WorkData.h"
#include "ChipIRSource.h"

namespace SDef
{

ChipIRSource::ChipIRSource() : 
  cutEnergy(0.0),weight(1.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
  */
{}

ChipIRSource::ChipIRSource(const ChipIRSource& A) : 
  cutEnergy(A.cutEnergy),weight(A.weight),
  CentPoint(A.CentPoint),Direction(A.Direction),
  angleSpread(A.angleSpread),radialSpread(A.radialSpread),
  Energy(A.Energy),EWeight(A.EWeight)
  /*!
    Copy constructor
    \param A :: ChipIRSource to copy
  */
{}

ChipIRSource&
ChipIRSource::operator=(const ChipIRSource& A)
  /*!
    Assignment operator
    \param A :: ChipIRSource to copy
    \return *this
  */
{
  if (this!=&A)
    {
      cutEnergy=A.cutEnergy;
      weight=A.weight;
      CentPoint=A.CentPoint;
      Direction=A.Direction;
      angleSpread=A.angleSpread;
      radialSpread=A.radialSpread;
      Energy=A.Energy;
      EWeight=A.EWeight;
    }
  return *this;
}

ChipIRSource::~ChipIRSource() 
  /*!
    Destructor
  */
{}

void
ChipIRSource::loadEnergy(const std::string& FName)
  /*!
    Load a distribution table
    - Care is taken to add an extra energy with zero 
    weight onto the table since we are using a
    \param FName :: filename 
  */
{
  ELog::RegMethod RegA("ChipIRSource","loadEnergy");
  const double current(60.0);
  const int eCol(1);
  const int iCol(11);
  
  Energy.clear();
  EWeight.clear();
  
  WorkData A;
  if (A.load(FName,eCol,iCol,0))
    {
      ELog::EM<<"Failed to read file:"<<FName<<ELog::endErr;
      return;
    }  

  A.xScale(1e-6);
  A.binDivide(1.0);
  //  DError::doubleErr IV=A.integrate(cutEnergy,1e38);
  DError::doubleErr IV=A.integrate(cutEnergy,1e38);
  weight=IV.getVal()/(current*6.24150636309e12);
  // Normalize A:
  A/=IV;

  Energy=A.getXdata();
  if (Energy.size()<2 || weight<1e-12)
    {
      ELog::EM<<"Failed to read energy/data from file:"<<FName<<ELog::endErr;
      return;
    }
  Energy.push_back(2.0*Energy.back()-Energy[Energy.size()-2]);
  const std::vector<DError::doubleErr>& Yvec=A.getYdata();

  std::vector<DError::doubleErr>::const_iterator vc;
  EWeight.push_back(0.0);
  for(vc=Yvec.begin();vc!=Yvec.end();vc++)
    EWeight.push_back(vc->getVal());
  EWeight.push_back(0.0);

  return;
}

void
ChipIRSource::createAll(const std::string& EFile,
			const Geometry::Vec3D& DAxis,
			const Geometry::Vec3D& point,const double angle,
			const double radius,SDef::Source& sourceCard) 
  /*!
    Generic function to create everything
    \param EFile :: File to get source data from
    \param DAxis :: Centre Direction axis
    \param point :: Start point [In ROTATED coordinates]
    \param angle :: Angle spread
    \param radius :: radial space
    \param sourceCard :: Card to update
  */
{
  ELog::RegMethod RegA("ChipIRSource","createAll");
  
  Direction=DAxis.unit();
  loadEnergy(EFile);
  angleSpread=angle;
  radialSpread=radius;
  CentPoint=point;
  createSource(sourceCard);
  return;
}

void
ChipIRSource::createAll(const std::string& EFile,
			const attachSystem::LinearComp& LC,
			const double angle,const double radius,
			SDef::Source& sourceCard) 
  /*!
    Generic function to create everything
    \param EFile :: File to get source data from
    \param LC :: Linear system to build along
    \param angle :: Angle spread
    \param radius :: radial space
    \param sourceCard :: Card to update
  */
{
  ELog::RegMethod RegA("ChipIRSource","createAll(LC)");
  createAll(EFile,LC.getBeamAxis(),LC.getCentre(),
	    angle,radius,sourceCard);
  return;
}
  
void
ChipIRSource::createSource(SDef::Source& sourceCard) const
  /*!
    Creates a target 2 neutron source 
    for chipIR
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("ChipIRSource","createSource");

  sourceCard.setActive();
  sourceCard.setComp("vec",Direction);
  sourceCard.setComp("par",1);            /// Neutron
  sourceCard.setComp("pos",CentPoint);

  // Direction:
  
  SDef::SrcData D1(1);
  SDef::SrcInfo SI1;
  SI1.addData(-1.0);
  SI1.addData(cos(M_PI*angleSpread/180.0));
  SI1.addData(1.0);

  SDef::SrcProb SP1;
  SP1.addData(0.0);
  SP1.addData(0.0);
  SP1.addData(1.0);
  D1.addUnit(SI1);  
  D1.addUnit(SP1);  
  sourceCard.setData("dir",D1);  

  // Energy:  
  SDef::SrcData D2(2);
  SDef::SrcInfo SI2('A');
  SDef::SrcProb SP2;
  SP2.setData(EWeight);
  SI2.setData(Energy);
  D2.addUnit(SI2);
  D2.addUnit(SP2);
  sourceCard.setData("erg",D2);

  // Radial
  if (radialSpread>0.0)
    {
      SDef::SrcData R1(3);
      SDef::SrcInfo* SIR3=R1.getInfo();
      SDef::SrcProb* SPR3=R1.getProb();
      SIR3->addData(0.0);
      SIR3->addData(radialSpread);
      SPR3->setFminus(-21,1.0);
      sourceCard.setData("rad",R1);
      sourceCard.setComp("axs",Direction);
    }
  if (fabs(weight)<1e-20)
    {
      if (radialSpread>0.0)
	{
	  ELog::EM<<"Normalization Weight == "
		  <<weight*radialSpread*radialSpread*M_PI<<ELog::endCrit;
	  //	  sourceCard.setComp("wgt",weight*radialSpread*
	  //		     radialSpread*M_PI);
	}
    }

  return;
}  


} // NAMESPACE SDef
