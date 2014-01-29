/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/surfDIter.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "InputControl.h"
#include "Element.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "SimProcess.h"
#include "MaterialSupport.h"
#include "surfDIter.h"

namespace ModelSupport
{


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
  if (N)
    {
      double curLen(0.0);
      double frac=1.0/N;
      for(size_t i=1;i<N;i++)
	{
	  Vec.push_back
	    (SimProcess::getDefIndexVar<double>(Control,Name,"",
						static_cast<int>(i),frac));
	  if (Vec.back()<0)
	    {
	      curLen-=Vec.back();    // NOTE: vec.back is negative
	      if (curLen>TLen)
		ELog::EM<<"Warning over length in fractions"<<ELog::endErr;
	      Vec.back()=curLen/TLen;
	    }
	  curLen=Vec.back()*TLen;
	  frac=((N-i-1.0)*Vec.back()+1.0)/(N-i);
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
      double frac=1.0/N;
      for(size_t i=1;i<N;i++)
	{
	  Vec.push_back
	    (SimProcess::getDefIndexVar<double>(Control,Name,"",
						static_cast<int>(i),frac));
 	  frac=((N-i-1.0)*Vec.back()+1.0)/(N-i);
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
	(Control,Name+StrFunc::makeString(i),defV);
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
  double defV=defValue;
  if (N>0)
    {
      for(size_t i=0;i<N;i++)
	{
	  defV=SimProcess::getDefIndexVar<double>(Control,Name,"",
						  static_cast<int>(i),defValue);
	  Vec.push_back(defV);
	}
    }
  return;
}


} // NAMESPACE ModelSupport
