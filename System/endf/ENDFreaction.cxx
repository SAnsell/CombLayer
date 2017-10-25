/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   endf/ENDFreaction.cxx
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
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <algorithm>

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
#include "ENDF.h"
#include "SEtable.h"
#include "doubleErr.h"
#include "ENDFreaction.h"

namespace ENDF
{

ENDFreaction::ENDFreaction() :
  mat(0),mf(0),mt(0),tmpIndex(0),tempActual(300.0),
  ZA(0),AWR(0.0),LAT(0),LLN(0)
  /*!
    Constructor
  */
{}

ENDFreaction::ENDFreaction(const std::string& FName,
			   const int matNum,
			   const int mfNum,
			   const int mtNum) :
  mat(matNum),mf(mfNum),mt(mtNum),tmpIndex(0),tempActual(300.0),
  ZA(0),AWR(0.0),LAT(0),LLN(0)
  /*!
    Constructor for values
    \param FName :: Endf file
  */
{
  ENDFfile(FName,matNum,mfNum,mtNum);
}

ENDFreaction::ENDFreaction(const ENDFreaction& A) : 
  mat(A.mat),mf(A.mf),mt(A.mt),tmpIndex(A.tmpIndex),
  tempActual(A.tempActual),ZA(A.ZA),AWR(A.AWR),
  LAT(A.LAT),LLN(A.LLN),XData(A.XData),YData(A.YData)
  /*!
    Copy constructor
    \param A :: ENDFreaction to copy
  */
{}

ENDFreaction&
ENDFreaction::operator=(const ENDFreaction& A)
  /*!
    Assignment operator
    \param A :: ENDFreaction to copy
    \return *this
  */
{
  if (this!=&A)
    {
      mat=A.mat;
      mf=A.mf;
      mt=A.mt;
      tmpIndex=A.tmpIndex;
      tempActual=A.tempActual;
      ZA=A.ZA;
      AWR=A.AWR;
      LAT=A.LAT;
      LLN=A.LLN;
      XData=A.XData;
      YData=A.YData;
    }
  return *this;
}

ENDFreaction*
ENDFreaction::clone() const
  /*!
    Clone method
    \return new (this)
  */
{
  return new ENDFreaction(*this);
}


ENDFreaction::~ENDFreaction() 
  /*!
    Destructor
  */
{}
 
void
ENDFreaction::procZaid(std::string& Line)
  /*!
    Process Zaid
    \param Line :: Line to process
  */
{
  ELog::RegMethod RegA("ENDFreaction","procZaid");
  
  double DZ;
  int N;
  if (!getNumber(Line,DZ) || !getNumber(Line,AWR) ||
      !StrFunc::section(Line,N) || !StrFunc::section(Line,LAT) )
    throw ColErr::InvalidLine(Line,"ENDFreaction::procZaid");
  
  ZA=static_cast<int>(DZ);
  return;
}



int
ENDFreaction::ENDFfile(const std::string& FName,
		       const int matNum,const int mfNum,
		       const int mtNum)
  /*!
    Process the whole file
    \param FName :: file
    \param matNum :: MAterial number
    \param mfNum :: mfNumber [typically 3]
    \param mtNum :: mt number [1-900 typically]
    \retval 0 on success 
    \retval -1 on exeception
   */
{
  ELog::RegMethod RegA("ENDFreaction","ENDFfile");
  
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
	} while(IX.good() && !Index.second && mat!=Index.first);
      if (!IX.good())
	throw ColErr::FileError(mat,FName,"File failed to read matIndex");

      // Get Zaid + Symmetry
      std::string Line=findMatMfMt(IX,matNum,mfNum,mtNum);
      // Get Zaid + Symmetry
      ELog::EM<<"Zaid Line ["<<lineCnt<<"]== "<<Line<<"::"<<ELog::endDebug;
      
      procZaid(Line);

      double qm,qi;
      int l1,l2,nr,np;
      std::vector<int> NBT;
      std::vector<int> INT;
      std::vector<double> XData;
      std::vector<double> YData;
      table1Read(IX,qm,qi,l1,l2,nr,np,NBT,INT,XData,YData);
      ELog::EM<<"NR == "<<nr<<" "<<np<<ELog::endDebug;
      ELog::EM<<"Size == "<<XData.size()<<" "<<YData.size()<<ELog::endDebug;
      // Get Bs:
      //      procB(IX);
      // Beta 
      //      procBeta(IX);
      // Alpha
      //      procAlpha(IX);
      // Teff
      //      procTeff(IX);

      // Populate SEtable:
      //      populateSETable();
    }
  catch (ColErr::ExBase& A)
    {
      ELog::EM<<"Exception "<<A.what()<<ELog::endErr;
      return -1;
    }
  // Now populate

  return 0;
}

void
ENDFreaction::exportWork(const std::vector<double>& EVal,
			 std::vector<DError::doubleErr>& OutY) const
  /*!
    Create a Energy / Cross section table
    \param EVal :: Energy Grid to use
    \param OutY :: Value [error] values 
   */
{
  ELog::RegMethod RegA("ENDFreaction","exportWork");

  if (EVal.empty() )
    {
      ELog::EM<<"No energy points "<<ELog::endErr;
      return;
    }

  std::vector<double>::const_iterator vc;
  double previous=EVal.front();
  size_t aIndex=0;
  //  size_t bIndex=0;
  OutY.clear();
  for(vc=EVal.begin()+1;vc!=EVal.end();vc++)
    {
      const double midX=(*vc+previous)/2.0;
      for(;aIndex<XData.size() && XData[aIndex]<midX;aIndex++) ;
      if (aIndex) aIndex--;
      OutY.push_back(DError::doubleErr(YData[aIndex],0.0));
    }
  return;
}

void
ENDFreaction::write(std::ostream& OX) const
  /*!
    Output into a matrix form 
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("ENDFreaction","write");


  return;
}

} // NAMESPACE ENDF

