/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   support/fileSupport.cxx
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
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cmath>
#include <complex>
#include <map>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "doubleErr.h"
#include "mathSupport.h"
#include "support.h"
#include "fileSupport.h"

/*! 
  \file fileSupport.cxx
*/

namespace  StrFunc
{

template<template<typename T,typename A> class V,typename T,typename A> 
int
writeFile(const std::string& Fname,const T step,
	  const V<T,A>& Y)
  /*!
    Write out the three vectors into a file of type dc 9
    \param step :: parameter to control x-step (starts from zero)
    \param Y :: Y column
    \param Fname :: Name of the file
    \returns 0 on success and -ve on failure
  */
{
  V<T,A> Ex;   // Empty vector
  V<T,A> X;    // Empty vector
  for(unsigned int i=0;i<Y.size();i++)
    X.push_back(i*step);

  return writeFile(Fname,X,Y,Ex);
}


template<template<typename T,typename A> class V,typename T,typename A> 
int
writeFile(const std::string& Fname,const V<T,A>& X,
	  const V<T,A>& Y)
  /*!
    Write out the three vectors into a file of type dc 9
    \param X :: X column
    \param Y :: Y column
    \param Fname :: Name of the file
    \returns 0 on success and -ve on failure
  */
{
  V<T,A> Ex;   // Empty vector/list
  return writeFile(Fname,X,Y,Ex);  // don't need to specific ??
}

template<template<typename T,typename A> class V,typename T,typename A,
	 template<typename Tx,typename Ax> class W,typename Tx,typename Ax> 
int
writeFile(const std::string& Fname,const V<T,A>& X,
	  const W<Tx,Ax>& Y)
  /*!
    Write out the three vectors into a file of type dc 9
    \param X :: X column
    \param Y :: Y column
    \param Fname :: Name of the file
    \returns 0 on success and -ve on failure
  */
{
  const size_t Npts(X.size()>Y.size() ? Y.size() : X.size());
  std::ofstream FX;

  FX.open(Fname.c_str());
  if (!FX.good())
    return -1;

  FX<<"# "<<Npts<<std::endl;
  FX.precision(10);
  FX.setf(std::ios::scientific,std::ios::floatfield);
  typename V<T,A>::const_iterator xPt=X.begin();
  typename W<Tx,Ax>::const_iterator yPt=Y.begin();
  
  // Double loop to include/exclude a short error stack
  size_t eCount=0;
  for(;eCount<Npts;eCount++)
    {
      FX<<(*xPt)<<" "<<(*yPt)<<std::endl;
      xPt++;
      yPt++;
    }
  FX.close();
  return 0;
}

template<template<typename T,typename A> class V,typename T,typename A> 
int
writeFile(const std::string& Fname,const V<T,A>& X,
	  const V<T,A>& Y,const V<T,A>& Err)
  /*!
    Write out the three container into a file of type dc 9
    \param X :: X column
    \param Y :: Y column
    \param Err :: Err column
    \param Fname :: Name of the file
    \returns 0 on success and -ve on failure
  */
{
  const size_t Npts(X.size()>Y.size() ? Y.size() : X.size());
  const size_t Epts(Npts>Err.size() ? Err.size() : Npts);

  std::ofstream FX;

  FX.open(Fname.c_str());
  if (!FX.good())
    return -1;

  FX<<"# "<<Npts<<" "<<Epts<<std::endl;
  FX.precision(10);
  FX.setf(std::ios::scientific,std::ios::floatfield);
  typename V<T,A>::const_iterator xPt=X.begin();
  typename V<T,A>::const_iterator yPt=Y.begin();
  typename V<T,A>::const_iterator ePt=(Epts ? Err.begin() : Y.begin());
  
  // Double loop to include/exclude a short error stack
  size_t eCount=0;
  for(;eCount<Epts;eCount++)
    {
      FX<<(*xPt)<<" "<<(*yPt)<<" "<<(*ePt)<<std::endl;
      xPt++;
      yPt++;
      ePt++;
    }
  for(;eCount<Npts;eCount++)
    {
      FX<<(*xPt)<<" "<<(*yPt)<<" 0.0"<<std::endl;
      xPt++;
      yPt++;
    }
  FX.close();
  return 0;
}

float
getVAXnum(const float A) 
  /*!
    Converts a vax number into a standard unix number
    \param A :: float number as read from a VAX file
    \returns float A in IEEE little eindian format
  */
{
  union 
   {
     float f;
     unsigned int ival;
   } Bd;

  unsigned int fmask;
  int expt;
  float frac,sign;
  double onum;

  Bd.f=A;
  sign  = (Bd.ival & 0x8000) ? -1.0f : 1.0f;
  expt = ((Bd.ival & 0x7f80) >> 7);   //reveresed ? 
  if (!expt) 
    return 0.0;

  fmask = ((Bd.ival & 0x7f) << 16) | ((Bd.ival & 0xffff0000) >> 16);
  expt-=128;
  fmask |=  0x800000;
  frac = (float) fmask  / 0x1000000;
  onum= frac * sign * pow(2.0,expt);
  return (float) onum;
}

template<typename T> 
int
setValues(const std::string& Line,const std::vector<int>& Index,
	  std::vector<T>& Out)
  /*!  
    Call to read in various values in position x1,x2,x3 from the
    line.
    \param Line :: string to read
    \param Index :: Indexes to read
    \param Out :: OutValues [unchanged if not read]
    \retval 0 :: success
    \retval -ve on failure.
  */
{
  if (Index.empty())
    return 0;
  
  if(Out.size()!=Index.size())
    throw ColErr::MisMatch<size_t>(Index.size(),Out.size(),
				"StrFunc::setValues");

  std::string modLine=Line;
  std::vector<int> sIndex(Index);     // Copy for sorting
  std::vector<size_t> OPt(Index.size());
  for(size_t i=0;i<Index.size();i++)
    OPt[i]=i;
  mathFunc::crossSort(sIndex,OPt);
  
  typedef std::vector<int>::const_iterator iVecIter;
  std::vector<int>::const_iterator sc=sIndex.begin();
  std::vector<size_t>::const_iterator oc=OPt.begin();
  int cnt(0);
  T value;
  std::string dump;
  while(sc!=sIndex.end() && *sc<0)
    {
      sc++;
      oc++;
    }
  
  while(sc!=sIndex.end())
    {
      if (*sc==cnt)
        {
	  if (!section(modLine,value))
	    return -1-static_cast<int>(
	      distance(static_cast<iVecIter>(sIndex.begin()),sc));  
	  // this loop handles repeat units
	  do
	    {
	      Out[*oc]=value;
	      sc++;
	      oc++;
	    } while (sc!=sIndex.end() && *sc==cnt); 
	}
      else
        {
	  if (!section(modLine,dump))
	    return -1-static_cast<int>(
	      distance(static_cast<iVecIter>(sIndex.begin()),sc));  
	}
      cnt++;         // Add only to cnt [sc/oc in while loop]
    }
  // Success since loop only gets here if sc is exhaused.
  return 0;       
}

template<template<typename T,typename A> class V,typename T,typename A> 
int
writeColumn(const std::string& Fname,const V<T,A>& Y,const V<T,A>& E)
  /*!
    Write out an extra column in file
    \param Fname :: Name of the file
    \param Y :: Y values Extra column
    \param E :: Error Extra column
    \returns 0 on success and -ve on failure
  */
{
  const long int Npts(Y.size());
  const long int Epts(E.size());

  std::ifstream IX;
  std::list<std::string> Lines;
  IX.open(Fname.c_str());
  while(IX.good())
    {
      std::string line=StrFunc::getLine(IX,512);
      line=StrFunc::removeSpace(line);
      if (!line.empty())
	Lines.push_back(line);
    }

  std::ofstream FX;
  FX.open(Fname.c_str());
  if (!FX.good())
    return -2;
  if (!Lines.empty() || Lines.front()[0]=='#')
    {
      FX<<Lines.front()<<" "<<Npts<<std::endl;
      Lines.pop_front();
    }
  else
      FX<<"# "<<Npts<<std::endl;
  
  FX.precision(10);
  FX.setf(std::ios::scientific,std::ios::floatfield);
  
  int cnt(0);
  typename V<T,A>::const_iterator yPt=Y.begin();
  typename V<T,A>::const_iterator ePt=(Epts ? E.begin() : Y.begin());
  for(yPt=Y.begin();yPt!=Y.end();yPt++)
    {
      while(!Lines.empty() && 
	    (Lines.front()[0]=='#' || Lines.front()[0]=='!'))
        {
	  FX<<Lines.front()<<std::endl;
	  Lines.pop_front();
	}
      if (!Lines.empty())
        {
	  FX<<Lines.front()<<" ";
	  Lines.pop_front();
	}
      FX<<*yPt++;
      if (Epts>cnt)
	FX<<" "<<*ePt++;
    }
  if (!Lines.empty())
    {
      while(!Lines.empty()) 
        {
	  FX<<Lines.front()<<std::endl;
	  Lines.pop_front();
	}
    }
  FX.close();
  return 0;
}

template<typename T>
int
writeStream(const std::string& Fname,const std::string& Title,
	    const std::vector<T>& X,const std::vector<T>& Y,
	    const std::vector<T>& Err,const int append)
  /*!
    Write out the three vectors into a file of type dc 2
    \param Fname :: Name of the file
    \param Title :: second line title
    \param X :: X column
    \param Y :: Y column
    \param Err :: Error column [can be empty]
    \param append :: True if appending to file / else overwriting

    \returns 0 on success and -ve on failure
  */
{
  const size_t Npts(X.size()>Y.size() ? Y.size() : X.size());
  const size_t Epts(Npts>Err.size() ? Err.size() : Npts);

  std::ofstream FX;
  if (append)
    FX.open(Fname.c_str(),std::ios::app);
  else
    FX.open(Fname.c_str());
  if (!FX.good())
    return -1;

  FX<<"3 "<<Npts<<" "<<Epts<<std::endl;
  FX<<"# "<<Title<<std::endl;
  FX.precision(10);
  FX.setf(std::ios::scientific,std::ios::floatfield);
  
  size_t i;
  for(i=0;i<Epts;i++)
    FX<<X[i]<<" "<<Y[i]<<" "<<Err[i]<<std::endl;
  for(;i<Npts;i++)
    FX<<X[i]<<" "<<Y[i]<<" "<<0.0<<std::endl;
  return 0;
}

template<typename T,typename U>
int
writeStream(const std::string& Fname,const std::string& Title,
	    const std::vector<T>& X,const std::vector<U>& Y,
	    const int append)
  /*!
    Write out the three vectors into a file of type dc 2
    \param Fname :: Name of the file
    \param Title :: second line title
    \param X :: X column
    \param Y :: Y column
    \param append :: True if appending to file / else overwriting
    \returns 0 on success and -ve on failure
  */
{
  ELog::RegMethod RegA("fileSupport","writeStream<U,T>");
  const size_t Npts(X.size()>Y.size() ? Y.size() : X.size());

  std::ofstream FX;
  if (append)
    FX.open(Fname.c_str(),std::ios::app);
  else
    FX.open(Fname.c_str());
  if (!FX.good())
    return -1;

  FX<<"3 "<<Npts<<std::endl;
  FX<<"# "<<Title<<std::endl;
  FX.precision(10);
  FX.setf(std::ios::scientific,std::ios::floatfield);
  
  for(size_t i=0;i<Npts;i++)
    FX<<X[i]<<" "<<Y[i]<<std::endl;

  return 0;
}


/// \cond TEMPLATE 


template int writeFile(const std::string&,const double,
		       const std::vector<double>&);

template int writeFile(const std::string&,const std::vector<double>&,
		       const std::vector<double>&,const std::vector<double>&);

template int writeFile(const std::string&,
		       const std::vector<double>&,const std::vector<double>&);

template int writeFile(const std::string&,const std::vector<float>&,const std::vector<float>&);

template int writeFile(const std::string&,const std::vector<float>&,const std::vector<float>&,const std::vector<float>&);
template int writeFile(const std::string&,
		       const std::vector<double>&,
		       const std::vector<DError::doubleErr>&);

template int writeStream(const std::string&,const std::string&,
			 const std::vector<double>&,const std::vector<double>&,
			 const std::vector<double>&,const int);

template int writeStream(const std::string&,const std::string&,
			 const std::vector<double>&,const std::vector<double>&,
			 const int);

template int writeStream(const std::string&,const std::string&,
			 const std::vector<double>&,
			 const std::vector<DError::doubleErr>&,
			 const int);


/// \endcond TEMPLATE 

}  // NAMESPACE StrFunc
