/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/BeamSource.cxx
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
#include "FixedOffset.h"
#include "WorkData.h"
#include "World.h"

#include "BeamSource.h"

namespace SDef
{

BeamSource::BeamSource(const std::string& keyName) : 
  FixedOffset(keyName,0),
  cutEnergy(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param keyName :: main name
  */
{}


BeamSource::~BeamSource() 
  /*!
    Destructor
  */
{}

int
BeamSource::populateEFile(const std::string& FName,
			   const int colE,const int colP)
  /*!
    Load a distribution table
    - Care is taken to add an extra energy with zero 
    weight onto the table since we are using a
    \param FName :: filename 
    return 0 on failure / 1 on success
  */
{
  ELog::RegMethod RegA("BeamSource","loadEnergy");

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
BeamSource::populateEnergy(std::string EPts,std::string EProb)
  /*!
    Read two strings that are the Energy points and the 
    \param EPts :: Energy Points string 
    \param EProb :: Energy Prob String
    \return 1 on success success
   */
{
  ELog::RegMethod RegA("BeamSource","populateEnergy");

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
BeamSource::populate(const FuncDataBase& Control)
  /*!
    Populate Varaibles
    \param Control :: Control variables
   */
{
  ELog::RegMethod RegA("BeamSource","populate");

  attachSystem::FixedOffset::populate(Control);

  // default photon
  particleType=Control.EvalDefVar<int>(keyName+"ParticleType",1); 
  radius=Control.EvalVar<double>(keyName+"Radius"); 
  angleSpread=Control.EvalVar<double>(keyName+"ASpread"); 

  const std::string EList=
    Control.EvalDefVar<std::string>(keyName+"Energy","");
  const std::string EPList=
    Control.EvalDefVar<std::string>(keyName+"EProb","");
  const std::string EFile=
    Control.EvalDefVar<std::string>(keyName+"EFile","");

  if (!populateEnergy(EList,EPList) &&
      !populateEFile(EFile,1,11))
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
BeamSource::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int linkIndex)
  /*!
    Create the unit vector
    \param FC :: Fixed Componenet
    \param linkIndex :: Link index [signed for opposite side]
   */
{
  ELog::RegMethod RegA("BeamSource","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,linkIndex);
  applyOffset();

  return;
}
  
void
BeamSource::createSource(SDef::Source& sourceCard) const
  /*!
    Creates a gamma bremstraual source
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("BeamSource","createSource");

  
  sourceCard.setActive();

  sourceCard.setComp("vec",Y);
  sourceCard.setComp("axs",Y);
  sourceCard.setComp("par",particleType);   // neutron (1)/photon(2)
  sourceCard.setComp("dir",cos(angleSpread*M_PI/180.0));         /// 
  sourceCard.setComp("pos",Origin);
  
  // RAD
  SDef::SrcData D1(1);
  SDef::SrcInfo SI1;
  SDef::SrcProb SP1;
  SI1.addData(0.0);
  SI1.addData(radius);
  SP1.setFminus(-21,1.0);
  D1.addUnit(SI1);
  D1.addUnit(SP1);
  sourceCard.setData("rad",D1);
  

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
BeamSource::createAll(const FuncDataBase& Control,
		       SDef::Source& sourceCard)
  /*!
    Create all the source
    \param Control :: DataBase for variables
    \param souceCard :: Source Term
   */
{
  ELog::RegMethod RegA("BeamSource","createAll");
  populate(Control);
  createUnitVector(World::masterOrigin(),0);
  createSource(sourceCard);
  return;
}

void
BeamSource::createAll(const FuncDataBase& Control,
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
  ELog::RegMethod RegA("BeamSource","createAll<FC,linkIndex>");
  populate(Control);
  createUnitVector(FC,linkIndex);
  createSource(sourceCard);
  return;
}


} // NAMESPACE SDef
