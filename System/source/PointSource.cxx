/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/PointSource.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "FixedOffset.h"
#include "WorkData.h"
#include "World.h"

#include "PointSource.h"

namespace SDef
{

PointSource::PointSource(const std::string& keyName) : 
  FixedOffset(keyName,0),particleType(1),
  cutEnergy(0.0),angleSpread(0.0),weight(1.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param keyName :: main name
  */
{}

PointSource::PointSource(const PointSource& A) : 
  attachSystem::FixedOffset(A),
  particleType(A.particleType),cutEnergy(A.cutEnergy),
  angleSpread(A.angleSpread),
  weight(A.weight),Energy(A.Energy),EWeight(A.EWeight)
  /*!
    Copy constructor
    \param A :: PointSource to copy
  */
{}

PointSource&
PointSource::operator=(const PointSource& A)
  /*!
    Assignment operator
    \param A :: PointSource to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      particleType=A.particleType;
      cutEnergy=A.cutEnergy;
      angleSpread=A.angleSpread;
      weight=A.weight;
      Energy=A.Energy;
      EWeight=A.EWeight;
    }
  return *this;
}

PointSource::~PointSource() 
  /*!
    Destructor
  */
{}

int
PointSource::populateEFile(const std::string& FName,
			   const int colE,const int colP)
  /*!
    Load a distribution table
    - Care is taken to add an extra energy with zero 
    weight onto the table since we are using a
    \param FName :: filename 
    return 0 on failure / 1 on success
  */
{
  ELog::RegMethod RegA("PointSource","loadEnergy");

  const int eCol(colE);
  const int iCol(colP);
  
  Energy.clear();
  EWeight.clear();
  
  WorkData A;
  if (FName.empty() || A.load(FName,eCol,iCol,0))
    return 0;


  A.xScale(1e-6);   // convert to MeV
  A.binDivide(1.0);
  DError::doubleErr IV=A.integrate(cutEnergy,1e38);
  // Normalize A:
  A/=IV;

  Energy=A.getXdata();
  if (Energy.size()<2)
    {
      ELog::EM<<"Failed to read energy/data from file:"<<FName<<ELog::endErr;
      return 0;
    }
  Energy.push_back(2.0*Energy.back()-Energy[Energy.size()-2]);
  const std::vector<DError::doubleErr>& Yvec=A.getYdata();

  std::vector<DError::doubleErr>::const_iterator vc;
  EWeight.push_back(0.0);
  for(vc=Yvec.begin();vc!=Yvec.end();vc++)
    EWeight.push_back(vc->getVal());
  EWeight.push_back(0.0);
  return (EWeight.empty()) ? 0 : 1;
}

int
PointSource::populateEnergy(std::string EPts,std::string EProb)
  /*!
    Read two strings that are the Energy points and the 
    \param EPts :: Energy Points string 
    \param EProb :: Energy Prob String
    \return 1 on success success
   */
{
  ELog::RegMethod RegA("PointSource","populateEnergy");

  Energy.clear();
  EWeight.clear();

  double eB,eP;
  
  // if (!StrFunc::section(EPts,eA) || eA<0.0)
  //   return 0;
  while(StrFunc::section(EPts,eB) &&
	StrFunc::section(EProb,eP))
    {
      if (!Energy.empty() && eB<=Energy.back())
	throw ColErr::IndexError<double>(eB,Energy.back(),
					 "Energy point not in sequence");
      if (eP<0.0)
	throw ColErr::IndexError<double>(eP,0.0,"Probablity eP negative");
      Energy.push_back(eB);
      EWeight.push_back(eP);
    }
  if (!StrFunc::isEmpty(EPts) || !StrFunc::isEmpty(EProb))
    ELog::EM<<"Trailing line info \n"
	    <<"Energy : "<<EPts<<"\n"
  	    <<"Energy : "<<EProb<<ELog::endErr;

  // // Normalize 
  // for(double& prob : EWeight)
  //   prob/=sum;
  return (EWeight.empty()) ? 0 : 1;
}
  
void
PointSource::populate(const FuncDataBase& Control)
  /*!
    Populate Varaibles
    \param Control :: Control variables
   */
{
  ELog::RegMethod RegA("PointSource","populate");

  attachSystem::FixedOffset::populate(Control);

  // default photon
  particleType=Control.EvalDefVar<int>(keyName+"ParticleType",1); 
  angleSpread=Control.EvalDefVar<double>(keyName+"ASpread",0.0); 

  const std::string EList=
    Control.EvalDefVar<std::string>(keyName+"Energy","");
  const std::string EPList=
    Control.EvalDefVar<std::string>(keyName+"EProb","");
  const std::string EFile=
    Control.EvalDefVar<std::string>(keyName+"EFile","");

  if (!populateEnergy(EList,EPList) &&
      !populateEFile(EFile,1,11))
    {
      double E=Control.EvalDefVar<double>(keyName+"EStart",1.0); 
      const size_t nE=Control.EvalDefVar<size_t>(keyName+"NE",0); 
      const double EEnd=Control.EvalDefVar<double>(keyName+"EEnd",1.0); 
      const double EStep((EEnd-E)/(nE+1));
      for(size_t i=0;i<nE;i++)
	{
	  Energy.push_back(E);
	  EWeight.push_back(1.0);
	  E+=EStep;
	}
      if (Energy.empty())
	Energy.push_back(E);
    }
  return;
}

void
PointSource::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int linkIndex)
  /*!
    Create the unit vector
    \param FC :: Fixed Componenet
    \param linkIndex :: Link index [signed for opposite side]
   */
{
  ELog::RegMethod RegA("PointSource","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,linkIndex);
  applyOffset();

  return;
}
  
void
PointSource::createSource(SDef::Source& sourceCard) const
  /*!
    Creates a gamma bremstraual source
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("PointSource","createSource");

  
  sourceCard.setActive();

  if (angleSpread>Geometry::zeroTol &&
      angleSpread<180.0-Geometry::zeroTol)
    {
      sourceCard.setComp("vec",Y);
      sourceCard.setComp("axs",Y);
      sourceCard.setComp("dir",cos(angleSpread*M_PI/180.0));         ///
    }
  sourceCard.setComp("pos",Origin);
    

  // Energy:
  if (Energy.size()>1)
    {
      SDef::SrcData D2(2);
      SDef::SrcInfo SI2('A');
      SDef::SrcProb SP2;
      SP2.setData(EWeight);
      SI2.setData(Energy);
      D2.addUnit(SI2);
      D2.addUnit(SP2);
      sourceCard.setData("erg",D2);
    }
  else if (!Energy.empty())
    sourceCard.setComp("erg",Energy.front());

  return;
}  

void
PointSource::createAll(const FuncDataBase& Control,
		       SDef::Source& sourceCard)
  /*!
    Create all the source
    \param Control :: DataBase for variables
    \param souceCard :: Source Term
   */
{
  ELog::RegMethod RegA("PointSource","createAll");
  populate(Control);
  createUnitVector(World::masterOrigin(),0);
  createSource(sourceCard);
  return;
}

void
PointSource::createAll(const FuncDataBase& Control,
		       const attachSystem::FixedComp& FC,
		       const long int linkIndex,
		       SDef::Source& sourceCard)

  /*!
    Create all the source
    \param Control :: DataBase for variables
    \param souceCard :: Source Term
    \param linkIndex :: link Index						
   */
{
  ELog::RegMethod RegA("PointSource","createAll<FC,linkIndex>");
  populate(Control);
  createUnitVector(FC,linkIndex);
  createSource(sourceCard);
  return;
}


} // NAMESPACE SDef
