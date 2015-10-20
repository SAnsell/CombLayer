/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/SurfNormSource.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "World.h"
#include "SurfNormSource.h"

namespace SDef
{

SurfNormSource::SurfNormSource(const std::string& K) :
  attachSystem::FixedComp(K,0),
  particleType(1),angleSpread(0.0),surfNum(0),
  cutEnergy(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param K :: main keyname 
  */
{}



SurfNormSource::~SurfNormSource() 
  /*!
    Destructor
  */
{}

void
SurfNormSource::loadEnergy(const std::string& FName)
  /*!
    Load a distribution table
    - Care is taken to add an extra energy with zero 
    weight onto the table since we are using a
    \param FName :: filename 
  */
{
  ELog::RegMethod RegA("SurfNormSource","loadEnergy");

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

int
SurfNormSource::populateEnergy(std::string EPts,std::string EProb)
  /*!
    Read two strings that are the Energy points and the 
    \param EPts :: Energy Points string 
    \param EProb :: Energy Prob String
    \return 1 on success success
   */
{
  ELog::RegMethod RegA("SurfNormSource","populateEnergy");

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
SurfNormSource::populate(const FuncDataBase& Control)
  /*!
    Populate Varaibles
    \param Control :: Control variables
   */
{
  ELog::RegMethod RegA("SurfNormSource","populate");

  // default photon
  particleType=Control.EvalDefVar<int>(keyName+"ParticleType",2); 
  angleSpread=Control.EvalVar<double>(keyName+"ASpread");
  height=Control.EvalVar<double>(keyName+"Height"); 

  const std::string EList=
    Control.EvalDefVar<std::string>(keyName+"Energy","");
  const std::string EPList=
    Control.EvalDefVar<std::string>(keyName+"EProb","");

  if (!populateEnergy(EList,EPList))
    {
      double E=Control.EvalVar<double>(keyName+"EStart"); 
      const size_t nE=Control.EvalVar<size_t>(keyName+"NE"); 
      const double EEnd=Control.EvalVar<double>(keyName+"EEnd"); 
      const double EStep((EEnd-E)/(nE+1));
      for(size_t i=0;i<nE;i++)
	{
	  Energy.push_back(E);
	  EWeight.push_back(1.0);
	  E+=EStep;
	}
    }
  return;
}

void
SurfNormSource::setSurf(const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Set the surface number. Also set centre of the system
    and determine the Z axis [to be done]
    \param FCPtr :: FixedComponent [can be zero]
    \param sideIndex :: surface index
  */
{
  ELog::RegMethod RegA("SurfNormSource","setSurf");
  ELog::EM<<"Surface == "<<FC.getKeyName()<<" "<<sideIndex<<ELog::endDiag;
  ELog::EM<<"STR == "<<FC.getSignedLinkString(sideIndex)<<ELog::endDiag;
  surfNum=FC.getSignedLinkSurf(sideIndex);
  ELog::EM<<"Surface == "<<FC.getKeyName()<<ELog::endDiag;
  return;
}
  
  
void
SurfNormSource::createSource(SDef::Source& sourceCard) const
  /*!
    Creates a gamma bremstraual source
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("SurfNormSource","createSource");
  
  sourceCard.setActive();
  sourceCard.setComp("sur",std::abs(surfNum));
  sourceCard.setComp("par",particleType);            /// photon (2)

  // Direction:

  if(angleSpread>Geometry::zeroTol)
    {
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
    }
  else
    {
      sourceCard.setComp("dir",1.0);
    }
      

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
SurfNormSource::createAll(const FuncDataBase& Control,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex,
			  SDef::Source& sourceCard)

  /*!
    Create all the source
    \param Control :: DataBase for variables
    \param souceCard :: Source Term
   */
{
  ELog::RegMethod RegA("SurfNormSource","createAll<FC,linkIndex>");

  populate(Control);
  setSurf(FC,sideIndex);
  createSource(sourceCard);
  return;
}


} // NAMESPACE SDef
