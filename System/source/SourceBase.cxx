/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/SourceBase.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "doubleErr.h"
#include "Source.h"
#include "SrcData.h"
#include "WorkData.h"
#include "writeSupport.h"

#include "inputSupport.h"
#include "particleConv.h"
#include "SourceBase.h"

namespace SDef
{

SourceBase::SourceBase() : 
  particleType("neutron"),cutEnergy(0.0),
  Energy({14}),EWeight({1.0}),weight(1.0),
  TransPtr(0),polarFrac(0.0)
  /*!
    Constructor 
  */
{}

SourceBase::SourceBase(const SourceBase& A) : 
  particleType(A.particleType),cutEnergy(A.cutEnergy),
  Energy(A.Energy),EWeight(A.EWeight),
  weight(A.weight),
  TransPtr((A.TransPtr) ? new Geometry::Transform(*A.TransPtr) : 0),
  polarVec(A.polarVec),polarFrac(A.polarFrac)
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
      delete TransPtr;
      TransPtr=(A.TransPtr) ? new Geometry::Transform(*A.TransPtr) : 0;
      polarVec=A.polarVec;
      polarFrac=A.polarFrac;
    }
  return *this;
}
  
  
int
SourceBase::setEnergyFile(const std::string& FName,
			  const int colE,const int colP,
			  const double energyMin,const double energyMax)
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
  ELog::RegMethod RegA("SourceBase","setEnergyFile");

    
  WorkData A;
  if (FName.empty() || A.load(FName,colE,colP,0))
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

  EWeight.clear();
  EWeight.push_back(0.0);
  for(const DError::doubleErr& DE : Yvec)
    EWeight.push_back(DE.getVal());
  EWeight.push_back(0.0);
  
  return 1;
}

int
SourceBase::setEnergy(std::string EPts,std::string EProb)
  /*!
    Read two strings that are the Energy points and the 
    \param EPts :: Energy Points string 
    \param EProb :: Energy Prob String
    \return 1 on success success
   */
{
  ELog::RegMethod RegA("SourceBase","setEnergy");

  if (!StrFunc::isEmpty(EPts) || !StrFunc::isEmpty(EProb))
    {
      Energy.clear();
      EWeight.clear();

      double eB(-1.0),eP;
      
      while(StrFunc::section(EPts,eB) &&
	    StrFunc::section(EProb,eP))
	{
	  if (!Energy.empty() && eB<=Energy.back())
	    throw ColErr::IndexError<double>
	      (eB,Energy.back(),"Energy point not in sequence");
	  
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
    }
  return (EWeight.empty()) ? 0 : 1;
}

void
SourceBase::setParticle(const int T)
  /*!
    Get particle type
    \param T :: index
  */
{
  particleType=particleConv::Instance().mcnpToFLUKA(T);
  return;
}
  
void
SourceBase::populate(const mainSystem::MITYPE& inputMap)
  /*!
    Populate Varaibles
    inputMap is vector of size>=1
    \param inputMap :: Map keyanme : string of sdefMod
  */
{
  ELog::RegMethod RegA("SourceBase","populate");

  mainSystem::MITYPE::const_iterator mc,mcB;

  int eFlag(0);
  if (!mainSystem::findInput(inputMap,"particle",0,particleType))
    mainSystem::findInput(inputMap,"particleType",0,particleType);

  std::string EFile;
  if (mainSystem::findInput(inputMap,"energyFile",0,EFile) ||
      mainSystem::findInput(inputMap,"EFile",0,EFile) )
    {
      eFlag=setEnergyFile(EFile,1,11,0.0,2e9);
    }
  
  if ( ((mc=inputMap.find("energyProb"))!=inputMap.end() ||
	(mc=inputMap.find("EProb"))!=inputMap.end()) &&
       ((mcB=inputMap.find("energy"))!=inputMap.end() ||
	(mcB=inputMap.find("Energy"))!=inputMap.end())  )
    {
      const std::string EProb=mc->second.front();
      const std::string EList=mcB->second.front();
      eFlag=setEnergy(EList,EProb);
    }

  if ( !eFlag && mainSystem::hasInput(inputMap,"energyRange"))
    {      
      Energy.clear();
      EWeight.clear();

      double EInit=
	mainSystem::getDefInput<double>(inputMap,"energyRange",0,1.0);
      double EFinal=
	mainSystem::getDefInput<double>(inputMap,"energyRange",1,10.0);
      const size_t nE=
	mainSystem::getDefInput<size_t>(inputMap,"energyRange",2,0);
      

      if (EFinal<EInit)
	std::swap(EInit,EFinal);

      const double EStep((EFinal-EInit)/static_cast<double>(nE+1));
      double E(EInit);
      for(size_t i=0;i<nE;i++)
	{
	  Energy.push_back(E);
	  EWeight.push_back(1.0);
	  E+=EStep;
	}
      eFlag=1;
    }
  if (!eFlag &&
      mainSystem::hasInput(inputMap,"energy"))  // only single
    {
      Energy.clear();
      EWeight.clear();
      
      const double E=
	mainSystem::getInput<double>(inputMap,"energy",0);
      Energy.push_back(E);
      EWeight.push_back(1.0);
    }

  if (!eFlag &&
      mainSystem::hasInput(inputMap,"momentum"))  // only single momentum
    {
      
    }
      
  return;
}

void
SourceBase::createTransform(const Geometry::Vec3D& Origin,
			    const Geometry::Vec3D& X,
			    const Geometry::Vec3D& Y,
			    const Geometry::Vec3D& Z)
  /*!
    Construct a transform based on x,y,z on othogonatilty
    \param Origin :: Origin
    \param X :: X axis
    \param Y :: Y Axis
    \param Z :: Z axis
  */
{
  ELog::RegMethod RegA("SourceBase","constructTransform");

  if (!TransPtr)
    TransPtr=new Geometry::Transform;

  TransPtr->setName(1);

  Geometry::Matrix<double> A(3,3);
  for(size_t j=0;j<3;j++)
    {
      A[0][j]=X[j];
      A[1][j]=Y[j];
      A[2][j]=Z[j];
    }

  TransPtr->setTransform(Origin,A);
  return;
}


void
SourceBase::setEnergy(const double E)
  /*!
    Set a signle energy set
    \param E :: energy
  */
{
  ELog::RegMethod RegA("SourceBase","setEnergy");

  Energy={E};
  EWeight={1.0};
  ELog::EM<<"Single Energy Source == "<<E<<ELog::endDiag;
  return;
}

void
SourceBase::setEnergy(const std::vector<double>& EBin,
		      const std::vector<double>& EProb)
  /*!
    Set a range of energies
    \param EBin :: energy bins
    \param EProb :: energy probability 
  */
{
  ELog::RegMethod RegA("SourceBase","setEnergy(Vec,Vec)");

  
  if (EBin.empty() || EProb.empty())
    throw ColErr::EmptyContainer("EBin/EProb empty");
  
  if (EBin.size()==EProb.size())
    {
      Energy=EBin;
      EWeight=EProb;
    }
  else if (EBin.size()==EProb.size()+1)
    {
      Energy=EBin;
      EWeight.clear();
      EWeight.push_back(0.0);
      EWeight.insert(EWeight.end(),EProb.begin(),EProb.end());
    }
  else
    {
      throw ColErr::MisMatch<size_t>(EBin.size(),EProb.size(),
				     "Energy Bin mismatch");
    }
  return;
}

void
SourceBase::createEnergySource(SDef::Source& sourceCard) const
  /*!
    Creates the energy part of an MCNP source
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("SourceBase","createEnergySource");

  const particleConv& pConv=particleConv::Instance();
  const int mcnpPIndex=pConv.mcnpITYP(particleType);
  sourceCard.setComp("par",mcnpPIndex);   // neutron (1)/photon(2)
  
  // Energy:
  if (Energy.size()>1)
    {
      const size_t ND=sourceCard.getFreeDataIndex();
      SDef::SrcData D2(ND);
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
SourceBase::setPolarization(const Geometry::Vec3D& PValue,
			    const double PFrac)
  /*!
    Set the polarization vector
    \param PValue :: Polarization vector
    \param PFrac :: Fraction which is polarized
  */
{
  polarVec=PValue.unit();
  polarFrac=PFrac;
  return;
}


void
SourceBase::writePHITS(std::ostream& OX) const
  /*!
    Write out common part of PHITS source
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SourceBase","writePHITS");

  const particleConv& partCV=particleConv::Instance();
 
  OX<<"  proj = "<<particleType<<std::endl;
  OX<<"  wgt  = "<<weight<<std::endl;
  if (Energy.size()==1)
    OX<<"  e0  = "<<Energy.front()<<std::endl;
  else if (!Energy.empty())
    {
      OX<<"  e-type  = 1"<<std::endl;
      OX<<"  ne      = "<<Energy.size()<<std::endl;
      double eStart=0.0;
      OX<<"    0.0 ";
      for(size_t i=0;i<Energy.size();i++)
	{
	  OX<<"    "<<eStart<<"  "<<EWeight[i]<<std::endl;
	  eStart=Energy[i];
	}
      OX<<"    "<<eStart<<std::endl;
    }
  return;
}

void
SourceBase::writeFLUKA(std::ostream& OX) const
  /*!
    Write out common part of FLUKA source
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SourceBase","writeFLUKA");

  
  if (polarFrac>Geometry::zeroTol &&
      polarVec.abs()>Geometry::zeroTol)
    {
      std::ostringstream cx;
      cx<<"POLARIZA ";  
      cx<<polarVec<<" 0 "<<polarFrac<<" 0.0 ";
      StrFunc::writeFLUKA(cx.str(),OX);
    }
  return;
}
  
  
} // NAMESPACE SDef
