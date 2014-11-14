/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   endf/ENDFmaterial.cxx
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
#include <sstream>
#include <cmath>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Triple.h"
#include "support.h"
#include "RefCon.h"
#include "Simpson.h"
#include "neutMaterial.h"
#include "ENDF.h"
#include "SQWtable.h"
#include "SEtable.h"
#include "ENDFmaterial.h"
#include "ePrimeFunc.h"

namespace ENDF
{

ENDFmaterial::ENDFmaterial() :
  mat(0),tmpIndex(0),tempActual(300),
  ZA(0)
  /*!
    Constructor
  */
{}

ENDFmaterial::ENDFmaterial(const std::string& FName) :
  mat(0),tmpIndex(0),tempActual(300),
  ZA(0)
  /*!
    Constructor for values
    \param FName :: Endf file
  */
{
  ENDF7file(FName);
}

ENDFmaterial::ENDFmaterial(const ENDFmaterial& A) : 
  mat(A.mat),tmpIndex(A.tmpIndex),tempActual(A.tempActual),
  ZA(A.ZA),AWR(A.AWR),LAT(A.LAT),LASYM(A.LASYM),LLN(A.LLN),
  NS(A.NS),NI(A.NI),NT(A.NT),Sn(A.Sn),SE(A.SE),Teff(A.Teff),
  B(A.B)
  /*!
    Copy constructor
    \param A :: ENDFmaterial to copy
  */
{}

ENDFmaterial&
ENDFmaterial::operator=(const ENDFmaterial& A)
  /*!
    Assignment operator
    \param A :: ENDFmaterial to copy
    \return *this
  */
{
  if (this!=&A)
    {
      mat=A.mat;
      tmpIndex=A.tmpIndex;
      tempActual=A.tempActual;
      ZA=A.ZA;
      AWR=A.AWR;
      LAT=A.LAT;
      LASYM=A.LASYM;
      LLN=A.LLN;
      NS=A.NS;
      NI=A.NI;
      NT=A.NT;
      Sn=A.Sn;
      SE=A.SE;
      Teff=A.Teff;
      B=A.B;
    }
  return *this;
}

ENDFmaterial*
ENDFmaterial::clone() const
  /*!
    Clone method
    \return new (this)
  */
{
  return new ENDFmaterial(*this);
}


ENDFmaterial::~ENDFmaterial() 
  /*!
    Destructor
  */
{}
 
void
ENDFmaterial::procZaid(std::string& Line)
  /*!
    Process Zaid
    \param Line :: Line to process
  */
{
  ELog::RegMethod RegA("ENDFmaterial","procZaid");
  
  double DZ;
  int N;
  if (!getNumber(Line,DZ) || !getNumber(Line,AWR) ||
      !StrFunc::section(Line,N) || !StrFunc::section(Line,LAT) ||
      !StrFunc::section(Line,LASYM) )
    throw ColErr::InvalidLine(Line,"ENDFmaterial::procZaid");
  
  ZA=static_cast<int>(DZ);
  return;
}

void
ENDFmaterial::procB(std::istream& IX)
  /*!
    Process flags / Bpts
    \param IX :: File to process from
  */
{
  ELog::RegMethod RegA("ENDFmaterial","procB");
  double c1,c2;
  int b;
  B.clear();
  listRead(IX,c1,c2,LLN,b,NI,NS,B);
  
  ELog::EM<<"Diag c1 c2:: "<<c1<<" "<<c2<<ELog::endDiag;
  ELog::EM<<"Diag lln,b,ni,ns,(B) :: "<<LLN<<" "<<b<<" "<<NI<<" "<<NS
	  <<ELog::endDiag;
  copy(B.begin(),B.end(),std::ostream_iterator<double>(ELog::EM.Estream(),":"));
  ELog::EM<<ELog::endDiag;

  return;
}

void
ENDFmaterial::procBeta(std::istream& IX)
  /*!
    Process the Beta line
    \param IX :: File to process from
   */
{
  ELog::RegMethod RegA("ENDFmaterial","procBeta");
  
  double c1,c2;
  int a,b,nr,nb;
  table2Read(IX,c1,c2,a,b,nr,nb,
	     Sn.betaIBoundary,Sn.betaInterp);
  
  Sn.setNBeta(static_cast<size_t>(nb));
  ELog::EM<<"Number of beta == "<<nb<<ELog::endDebug;
  for(size_t i=0;i<Sn.betaIBoundary.size();i++)
    {
      ELog::EM<<"IB/IA["<<i<<"] =="<<Sn.betaIBoundary[i]<<":"
	      <<Sn.betaInterp[i]<<ELog::endDebug;
    }
  return;  
}

void
ENDFmaterial::procAlpha(std::istream& IX)
  /*!
    Process the alpha line
    \param IX :: input stream
   */
{
  ELog::RegMethod RegA("ENDFmaterial","procAlpha");
  double c1,c2;
  int a,b,nr,np,nt;
  int li;
  std::vector<double> XDATA;
  std::vector<double> YDATA;


  table1Read(IX,c1,c2,NT,b,nr,np,Sn.alphaIBoundary,
	     Sn.alphaInterp,XDATA,YDATA);
  NT++;        // Number of temperatures
  for(size_t j=0;j<static_cast<size_t>(NT);j++)
    {
      if (j) listRead(IX,c1,c2,li,a,np,b,YDATA);
      if (tmpIndex==j)
	{
	  Sn.setNAlpha(static_cast<size_t>(np));
	  tempActual=c1;
	  Sn.Beta[0]=c2;
	  Sn.setData(0,XDATA,YDATA);
	  ELog::EM<<"Number of alpha boundary == "<<Sn.alphaIBoundary.size()
		  <<ELog::endDebug;
	  for(size_t i=0;i<Sn.alphaIBoundary.size();i++)
	    {
	      ELog::EM<<"IB/IA["<<i<<"] == "<<Sn.alphaIBoundary[i]<<":"
		      <<Sn.alphaInterp[i]<<ELog::endDebug;
	    }

	}
    }

  ELog::EM<<"NBeta == "<<Sn.nBeta<<ELog::endDebug;
  // All other betas
  std::vector<int> II;
  std::vector<int> IB;
  
  for(size_t i=1;i<Sn.nBeta;i++)
    {
      table1Read(IX,c1,c2,nt,b,nr,np,IB,II,XDATA,YDATA);
      if (Sn.checkAlpha(IB,II,XDATA)) 
	throw ColErr::ExitAbort("SN.checkAlpha");
      for(size_t j=0;j<static_cast<size_t>(NT);j++)
	{
	  if (j) listRead(IX,c1,c2,li,a,np,b,YDATA);
	  if (tmpIndex==j)
	    {
	      Sn.Beta[i]=c2;
	      Sn.setData(i,XDATA,YDATA);
	    }
	}
    }
  return;  
}

void
ENDFmaterial::procTeff(std::istream& IX)
  /*!
    Process the Teff Line
    \param IX :: input stream
  */
{
  ELog::RegMethod RegA("ENDFmaterial","procTeff");

  double c1,c2;
  int a,b,nr,nb;
  std::vector<int> NBT;           /// Number in interpolation region
  std::vector<int> INT;           /// Interpolation type
  std::vector<double> XDATA;
  std::vector<double> YDATA;
  // Beta[0] READ:
  Teff.clear();
  table1Read(IX,c1,c2,a,b,nr,nb,NBT,INT,XDATA,YDATA);
  if (YDATA.size()<=tmpIndex)
    {
      ELog::EM<<"Error with number of Teff:"<<YDATA.size()<<ELog::endErr;
      exit(1);
    }
  Teff.push_back(YDATA[tmpIndex]);
  size_t index=6;
  while(B.size()>index)
    {
      if (B[index]==0.0)
	table1Read(IX,c1,c2,a,b,nr,nb,NBT,INT,XDATA,YDATA);
      if (YDATA.size()<=tmpIndex)
	{
	  ELog::EM<<"Error with number of Teff:"
		  <<YDATA.size()<<ELog::endErr;
	  throw ColErr::ExitAbort("YDATA");
	}
      Teff.push_back(YDATA[tmpIndex]);
      index+=6;
    }
  return;  
}

int
ENDFmaterial::ENDF7file(const std::string& FName)
  /*!
    Process the whole file
    \param FName :: file
    \retval 0 on success 
    \retval -1 on exeception
   */
{
  ELog::RegMethod RegA("ENDFmaterial","ENDF7file");
  
  typedef std::pair<std::string,std::string> LTYPE;

  std::ifstream IX;
  LTYPE IA;
  try
    {
      IX.open(FName.c_str());
      if (!IX.good())
	{
	  ELog::EM<<"Failed to to open file "<<FName<<ELog::endErr;
	  return -1;
	}      
      ENDF::lineCnt=0;

      
      // Determine the Mat number :
      Triple<int> Index;
      do 
	{
	  LTYPE IA=getLine(IX);
	  Index=getMatIndex(IA.second);
	} while(!Index.second);
      mat=Index.first;
      // Get Zaid + Symmetry
      std::string Line=findMatMfMt(IX,mat,7,4);
      // Get Zaid + Symmetry
      ELog::EM<<"Zaid Line ["<<lineCnt<<"]== "<<Line<<"::"<<ELog::endDebug;
      
      procZaid(Line);
      // Get Bs:
      procB(IX);
      // Beta 
      procBeta(IX);
      // Alpha
      procAlpha(IX);
      // Teff
      procTeff(IX);

      // Populate SEtable:
      populateSETable();
    }
  catch (ColErr::ExBase& A)
    {
      ELog::EM<<"Exception "<<A.what()<<ELog::endErr;
      return -1;
    }
  // Now populate

  return 0;
}


double
ENDFmaterial::Sab(const size_t atomIndex,const double E,
		  const double Eprime,const double mu) const
  /*!
    Calculate S(q,omega) for a neutron of energy E, for a given
    atom index [not primary]
    \param atomIndex :: Atom index
    \param E :: Energy of neutron [eV]
    \param Eprime :: Final Energy of neutron [eV]
    \param mu :: cos(angle)
    \return S(Q,w)
  */
{
  ELog::RegMethod RegA("ENDFmaterial","Sab");

  const double alpha=(Eprime+E-2*mu*sqrt(Eprime*E))/
    (AWR*RefCon::k_bev*tempActual);

  const double beta=(Eprime-E)/(tempActual*RefCon::k_bev);
  
  if (atomIndex==0) 
    return Sn.Sab(alpha,beta);
  
  if (B.size()<6*(atomIndex+1))
    return 0.0;

  // SCT approximation:
  if (B[6*atomIndex]<0.1 || fabs(B[6*atomIndex]-1.0)<1e-5)
    {
      const double sF=4*M_PI*alpha*Teff[atomIndex]/tempActual;
      double eP=pow((alpha-fabs(beta)),2.0)*tempActual/
	(Teff[atomIndex]*4*alpha);
      eP+=fabs(beta)/2.0;
      return exp(-eP)/sqrt(sF);
    }
  // CASE: FREE GAS:
  if (fabs(B[6*atomIndex]-1.0)<1e-5)
    return exp(-(alpha*alpha+beta*beta)/(4*alpha))/sqrt(4*M_PI*alpha);

  return 0.0;
}


double
ENDFmaterial::dSdOdE(const double E,const double Eprime,
		     const double mu) const
  /*!
    Calculate S(q,omega) for a neutron of energy E.
    \param E :: Energy of neutron [eV]
    \param Eprime :: Final Energy of neutron [eV]
    \param mu :: cos(angle)
    \return do/dOde=S(Q,w)  [barns/str/ev] 
  */
{
  ELog::RegMethod RegA("ENDFmaterial","dSdOdE");

  const double beta=(Eprime-E)/(tempActual*RefCon::k_bev);
  const double fact=sqrt(Eprime/E)/
    (4.0*M_PI*RefCon::k_bev*tempActual);
  const double symFactor=(LASYM) ? exp(-beta/2) : 1.0;
  double sigma(0.0);
  for(size_t i=0;i<=static_cast<size_t>(NS);i++)
    {
      const size_t bI(i*6);
      const double SAB=Sab(i,E,Eprime,mu);
      sigma+=SAB*B[bI]*pow((B[bI+2]+1.0)/B[bI+2],2.0);
    }
  return sigma*fact*symFactor;
}

void
ENDFmaterial::populateSETable()
  /*!
    Create table of sigma(E)
  */
{
  ELog::RegMethod RegA("ENDFmaterial","populateSETable");
  
  const double Eend(4.0);
  const double NSteps(500);
  SE.clear();
  for(int i=1;i<NSteps;i++)
    {
      const double E=(exp(i/NSteps)-1.0)*Eend/(exp(1)-1.0);
      double sigma(0.0);
      for(int i=-10;i<10;i++)
	{
	  const double mu(i*0.1);
	  ePrimeFunc XX=ePrimeFunc(*this,E,mu);
	  sigma+=Simpson::integrate<ePrimeFunc>(250,E/51,4*E,XX);
	}
      sigma*=0.1;     // step size:
      sigma*=2*M_PI;  // Integral of theta
      SE.addEnergy(E,sigma);
    }
  
  return;
}

double
ENDFmaterial::sigma(const double E) const
  /*!
    Returns the the total sigma at an energy
    - required SEtable to have been set
    \param E :: Energy
    \return total 
  */
{
  return SE.STotal(E);
}

void
ENDFmaterial::write(std::ostream& OX) const
  /*!
    Output into a matrix form 
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("ENDFmaterial","write");
  
  ELog::EM<<"Size == "<<Sn.Alpha.size()<<" "<<Sn.Beta.size()<<ELog::endDebug;
  ELog::EM<<"Temp(real/eff) == "<<tempActual<<" "<<Teff[0]<<ELog::endDebug;

  for(size_t a=0;a<Sn.nAlpha;a++)
    {
      for(size_t b=0;b<Sn.nBeta;b++)
	{
	  const long int AL(static_cast<long int>(a));
	  const long int BL(static_cast<long int>(b));
	  OX<<Sn.Alpha[a]<<" "
	    <<Sn.Beta[b]<<" "
	    <<Sn.SAB[AL][BL]<<std::endl;
	}
      OX<<std::endl;
    }
  return;
}



} // NAMESPACE MonteCarlo

