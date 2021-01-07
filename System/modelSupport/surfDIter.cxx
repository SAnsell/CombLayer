/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/surfDIter.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "splineSupport.h"
#include "stringCombine.h"
#include "Element.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "FItem.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MaterialSupport.h"
#include "surfDIter.h"


namespace ModelSupport
{

// Support for populateQuadRange

  
void
populateDivideLen(const FuncDataBase& Control,const size_t N,
	       const std::string& Name,const double TLen,
	       std::vector<double>& Vec)
  /*!
    Function to populate an integer vector
    with a set of points bases on a name type :
    \param Control :: Function data base
    \param N :: Number of points to get
    \param Name :: BaseName of divide name
    \param TLen :: Total length
    \param Vec :: Vector to populate [and clear]
  */
{
  ELog::RegMethod RegA("surfDIter","populateDivideLen");
  if (N && TLen>Geometry::zeroTol)
    {
      double curLen(0.0);
      double frac=1.0/static_cast<double>(N);
      for(size_t i=1;i<N;i++)
	{
	  const std::string NName=std::to_string(i);
	  const double fA=Control.EvalDefVar<double>(Name+NName,frac);
	  Vec.push_back(fA);
	  if (fabs(fA-frac)>Geometry::zeroTol)
	    {
	      curLen+=std::fabs(fA);    // NOTE: vec.back is negative
              
	      if (curLen>TLen)
		ELog::EM<<"Warning: over length in fractions [unit("
                        <<i<<")]"<<curLen<<" "<<TLen<<ELog::endErr;
	      Vec.back()=curLen/TLen;
	    }
	  curLen=Vec.back()*TLen;
	  frac=((static_cast<double>(N-i)-1.0)*Vec.back()+1.0)/
	    static_cast<double>(N-i);
	}
    }
  return;
}

void
populateDivide(const FuncDataBase& Control,const size_t N,
	       const std::string& Name,std::vector<double>& Vec)
  /*!
    Function to populate an integer vector
    with a set of points bases on a name type :
    \param Control :: Function data base
    \param N :: Number of points to get
    \param Name :: BaseName of divide name
    \param Vec :: Vector to populate [and clear]
  */
{
  ELog::RegMethod RegA("surfDIter","populateDivide");
  if (N>0)
    {
      double frac=1.0/static_cast<double>(N);
      for(size_t i=1;i<N;i++)
	{
	  const std::string NName=StrFunc::makeString(i);
	  const double fA=Control.EvalDefVar<double>(Name+NName,frac);
	  Vec.push_back(fA);
	  frac=((static_cast<double>(N-i)-1.0)*Vec.back()+1.0)/
	    static_cast<double>(N-i);
	}
    }
  return;
}

void
populateDivide(const FuncDataBase& Control,const size_t N,
	       const std::string& Name,const int defValue,
	       std::vector<int>& Vec)
  /*!
    Function to populate an integer vector
    with a set of points bases on a name type 
    \param Control :: Function data base
    \param N :: Number of points to get
    \param Name :: BaseName of divide name
    \param defValue :: Default starting value
    \param Vec :: Vector to populate [and clear]
  */
{
  Vec.clear();
  int defV=defValue;
  for(size_t i=0;i<N;i++)
    {
      defV=ModelSupport::EvalDefMat<int>
	(Control,Name+std::to_string(i),defV);
      Vec.push_back(defV);
    }
  return;
}

void
populateDivide(const FuncDataBase& Control,const size_t N,
	       const std::string& Name,const std::string& defValue,
	       std::vector<std::string>& Vec)
  /*!
    Function to populate an integer vector
    with a set of points bases on a name type 
    \param Control :: Function data base
    \param N :: Number of points to get
    \param Name :: BaseName of divide name
    \param defValue :: Default starting value
    \param Vec :: Vector to populate [and clear]
  */
{
  Vec.clear();
  std::string defV=defValue;
  for(size_t i=0;i<N;i++)
    {
      defV=Control.EvalDefVar<std::string>
	(Name+StrFunc::makeString(i),defV);
      Vec.push_back(defV);
    }
  return;
}
  
void
populateDivide(const FuncDataBase& Control,const size_t N,
	       const std::string& Name,const double defValue,
	       std::vector<double>& Vec)
  /*!
    Function to populate a double vector with a set of points 
    bases on a name type 
    \param Control :: Function data base
    \param N :: Number of points to get
    \param Name :: BaseName of divide name
    \param defValue :: Default starting value
    \param Vec :: Vector to populate [and clear]
  */
{
  Vec.clear();
  if (N>0)
    {
      for(size_t i=0;i<N;i++)
	{
	  const std::string NName=StrFunc::makeString(i);
	  const double fA=Control.EvalDefVar<double>(Name+NName,defValue);
	  Vec.push_back(fA);
	}
    }
  return;
}

void
populateAddRange(const FuncDataBase& Control,const size_t N,
	      const std::string& Name,
	      const double ARange,
	      const double BRange,
	      std::vector<double>& Vec)
  /*!
    Function to populate a double vector with a set of points 
    bases on a name type. Note that wehn variables are found they are
    added to the main base
    \param Control :: Function data base
    \param N :: Number of points to get
    \param Name :: BaseName of divide name
    \param ARange :: Start value    
    \param BRange :: End value 
    \param Vec :: Vector to populate [and cleared]
  */
{
  ELog::RegMethod RegA("surfDIter","populateRange[flag]");
  
  Vec.clear();
  if (N>0)
    {
      // first get as many as possible
      double sum(0.0);
      size_t cnt(0);
      Vec.resize(N+1);
      std::vector<size_t> setValues;
      Vec[0]=ARange;        // To deal with the first point
      setValues.push_back(0);
      for(size_t i=1;i<N;i++)
	{
	  const std::string NName=Name+std::to_string(i);
	  if (Control.hasVariable(NName))
	    {
	      const double fA=Control.EvalVar<double>(NName);
	      Vec[i]=fA;
	      sum+=fA;
	      cnt++;
	      setValues.push_back(i);
	    }
	}      


      const double step=(BRange-ARange-sum)/static_cast<double>(N-cnt);
      setValues.push_back(N);
      Vec[N]=step;
	      
      size_t aPt(0);
      for(const size_t index : setValues)
	{
	  if (index)
	    {
	      for(size_t j=aPt+1;j<index;j++)
		Vec[j]=Vec[j-1]+step;
	      aPt=index;
	      Vec[index]+=Vec[index-1];
	    }
	}
    }
  return;
}


void
populateRange(const FuncDataBase& Control,const size_t N,
	      const std::string& Name,
	      const double ARange,
	      const double BRange,
	      std::vector<double>& Vec)
  /*!
    Function to populate a double vector with a set of points 
    bases on a name type. Note that wehn variables are found they are
    added to the main base
    \param Control :: Function data base
    \param N :: Number of points to get
    \param Name :: BaseName of divide name
    \param ARange :: Start value    
    \param BRange :: End value 
    \param Vec :: Vector to populate [and cleared]
  */
{
  ELog::RegMethod RegA("surfDIter","populateRange[flag]");
  
  Vec.clear();
  if (N>0)
    {
      // first get as many as possible
      Vec.resize(N+1);
      std::vector<size_t> setValues;
      Vec[0]=ARange;        // To deal with the first point
      setValues.push_back(0);
      for(size_t i=1;i<N;i++)
	{
	  const std::string NName=Name+StrFunc::makeString(i);
	  if (Control.hasVariable(NName))
	    {
	      const double fA=Control.EvalVar<double>(NName);
	      Vec[i]=fA;
	      setValues.push_back(i);
	    }
	}      
      Vec[N]=BRange;
      setValues.push_back(N);

      double aVal(ARange);
      size_t aPt(0);
      for(const size_t index : setValues)
	{
	  if (index)
	    {
	      const double step=(Vec[index]-aVal)
		/static_cast<double>(index-aPt);
	      for(size_t j=aPt+1;j<index;j++)
		Vec[j]=Vec[j-1]+step;
	      aPt=index;
	      aVal=Vec[index];
	    }
	}
    }
  return;
}
    
void
populateQuadRange(const FuncDataBase& Control,const size_t N,
		  const std::string& Name,
		  const double ARange,
		  const double MPoint,
		  const double BRange,
		  std::vector<double>& Vec)
  /*!
    Function to populate a double vector with a set of points 
    bases on a name type. Note that wehn variables are found they are
    added to the main base
    \param Control :: Function data base
    \param N :: Number of points to get
    \param Name :: BaseName of divide name
    \param ARange :: Start value    
    \param MPoint :: mid Point value
    \param BRange :: End value 
    \param Vec :: Vector to populate [and cleared]
  */
{
  ELog::RegMethod RegA("surfDIter","populateQuadRange[flag]");

  // Initial matrix
  std::vector<double> ABC;

  if (N>0)
    {
      Vec.resize(N+1);
      std::set<size_t> setValues;
      std::vector<double> X;     // For spline
      std::vector<double> Y;
      
      Vec[0]=ARange;        // To deal with the first point
      X.push_back(0.0);
      Y.push_back(ARange);
      for(size_t i=1;i<N;i++)
	{
	  const std::string NName=Name+StrFunc::makeString(i);
	  if (Control.hasVariable(NName))
	    {
	      const double fA=Control.EvalVar<double>(NName);
	      Vec[i]=fA;
	      X.push_back(static_cast<double>(i));
	      Y.push_back(fA);
	      setValues.insert(i);
	    }
	  else if (i==N/2)
	    {
	      Vec[i]=MPoint;
	      setValues.insert(i);
	      X.push_back(static_cast<double>(i));
	      Y.push_back(MPoint);
	    }
	}      
      Vec[N]=BRange;
      Y.push_back(BRange);
      X.push_back(static_cast<double>(N));

      // Here : setValues acts as spline X
      // Vec  : acts as spline Y
      std::vector< mathSupport::SplinePt<double> > SP;
      mathSupport::calcSpline(X,Y,SP);
      
      for(size_t i=1;i<N;i++)
	{
	  if (setValues.find(i)==setValues.end())
	    Vec[i]=mathSupport::evalSpline
	      (X,SP,static_cast<double>(i));
	}
    }
  return;
}

void
populateVecDivide(const FuncDataBase& Control,
		  const std::string& Name,
		  const std::vector<std::string>& VecDef,
		  std::vector<std::string>& VecOut)
  /*!
    Function to populate an integer vector
    with a set of points bases on a name type 
    \param Control :: Function data base
    \param Name :: BaseName of divide name
    \param VecDef :: Default starting value [as string
    \param VecOut :: Vector to populate [and clear]
  */
{

  VecOut.clear();

  for(size_t i=0;i<VecDef.size();i++)
    {
      const std::string defV=Control.EvalDefVar<std::string>
	(Name+std::to_string(i),VecDef[i]);
      
      VecOut.push_back(defV);
    }
  return;
}

void
populateVecDivide(const FuncDataBase& Control,
		  const std::string& Name,
		  const std::vector<int>& VecDef,
		  std::vector<int>& VecOut)
  /*!
    Function to populate an integer vector
    with a set of points bases on a name type 
    \param Control :: Function data base
    \param Name :: BaseName of divide name
    \param VecDef :: Default starting value
    \param VecOut :: Vector to populate [and clear]
  */
{
  ELog::RegMethod RegA("surfDIter[F]","populateVecDivide");
  VecOut.clear();

  for(size_t i=0;i<VecDef.size();i++)
    {
      const int defV=ModelSupport::EvalDefMat<int>
	(Control,Name+std::to_string(i),VecDef[i]);
      
      VecOut.push_back(defV);
    }
  return;
}

} // NAMESPACE ModelSupport
