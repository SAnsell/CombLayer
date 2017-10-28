/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/SourceBase.cxx
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
#include "FixedOffset.h"
#include "WorkData.h"
#include "World.h"

#include "SourceBase.h"

namespace SDef
{

SourceBase::SourceBase() : 
  particleType(1),cutEnergy(0.0),
  Energy({14}),EWeight({1.0}),weight(1.0)
  /*!
    Constructor 
  */
{}

SourceBase::SourceBase(const SourceBase& A) : 
  particleType(A.particleType),cutEnergy(A.cutEnergy),
  Energy(A.Energy),EWeight(A.EWeight),
  weight(A.weight)
  /*!
    Copy constructor
    \param A :: SourceBase to copy
  */
{}

SourceBase&
SourceBase::operator=(const SourceBase& A)
  /*!
    Assignment operator
    \param A :: SourceBase to copy
    \return *this
  */
{
  if (this!=&A)
    {
      particleType=A.particleType;
      cutEnergy=A.cutEnergy;
      Energy=A.Energy;
      EWeight=A.EWeight;
      weight=A.weight;
    }
  return *this;
}
  
  
int
SourceBase::populateEFile(const std::string& FName,
			   const int colE,const int colP)
  /*!
    Load a distribution table
    - Care is taken to add an extra energy with zero 
    - Scale weight  onto sum 1.0 in the table 
    \param FName :: filename 
    \param colE :: Energy column from data file
    \param colP :: Prob/Weight column from data file
    \return 0 on failure / 1 on success
  */
{
  ELog::RegMethod RegA("SourceBase","populateEFile");

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
SourceBase::populateEnergy(std::string EPts,std::string EProb)
  /*!
    Read two strings that are the Energy points and the 
    \param EPts :: Energy Points string 
    \param EProb :: Energy Prob String
    \return 1 on success success
   */
{
  ELog::RegMethod RegA("SourceBase","populateEnergy");

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

    // single entry:
  if (Energy.empty() && eB>0.0)
    {
      Energy.push_back(eB);
      return 1;
    }

  // // Normalize 
  // for(double& prob : EWeight)
  //   prob/=sum;
  return (EWeight.empty()) ? 0 : 1;
}
  
void
SourceBase::populate(const std::string& keyName,
		     const FuncDataBase& Control)
  /*!
    Populate Varaibles
    \param keyName :: Keyname for variables as this is not a FC
    \param Control :: Control variables
  */
{
  ELog::RegMethod RegA("SourceBase","populate");

  // default neutron
  particleType=Control.EvalDefVar<int>(keyName+"ParticleType",particleType);

  
  const std::string EList=
    Control.EvalDefVar<std::string>(keyName+"Energy","");
  const std::string EPList=
    Control.EvalDefVar<std::string>(keyName+"EProb","");
  const std::string EFile=
    Control.EvalDefVar<std::string>(keyName+"EFile","");

  if (!populateEFile(EFile,1,11) &&
      !populateEnergy(EList,EPList))
    {

      double defEnergy(1.0);
      if (Energy.empty() ||
	  Control.hasVariable(keyName+"EStart") ||
	  StrFunc::convert(EList,defEnergy))
	{
	  double E=Control.EvalDefVar<double>(keyName+"EStart",defEnergy); 
	  const size_t nE=Control.EvalDefVar<size_t>(keyName+"NE",1); 
	  const double EEnd=Control.EvalDefVar<double>(keyName+"EEnd",E); 
	  const double EStep((EEnd-E)/static_cast<double>(nE+1));
	  Energy.clear();
	  EWeight.clear();
	  for(size_t i=0;i<nE;i++)
	    {
	      Energy.push_back(E);
	      EWeight.push_back(1.0);
	      E+=EStep;
	    }
	}
    }
  return;
}


void
SourceBase::setEnergy(const double E)
{
  ELog::RegMethod RegA("SourceBase","setEnergy");


  Energy={E};
  EWeight={1.0};
  
  return;
}
    
void
SourceBase::createEnergySource(SDef::Source& sourceCard) const
  /*!
    Creates the energy part of an MCNP source
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("SourceBase","createSource");
  
  sourceCard.setActive();

  sourceCard.setComp("par",particleType);   // neutron (1)/photon(2)
    

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

} // NAMESPACE SDef
