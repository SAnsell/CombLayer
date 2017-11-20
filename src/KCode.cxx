/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/KCode.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <complex>
#include <cmath>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <functional>
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MapSupport.h"
#include "mathSupport.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Transform.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "localRotate.h"
#include "SourceBase.h"
#include "KCode.h"

namespace SDef
{

KCode::KCode() : SourceBase(),
  rkk(1.0),defFlag(8,0),vals(8)
  /*!
    Constructor with default list
  */
{}

KCode::KCode(const KCode& A) :
  SourceBase(A),
  rkk(A.rkk),defFlag(A.defFlag),
  vals(A.vals),ksrc(A.ksrc)
  /*!
    Copy constructor
    \param A :: KCode to copy
  */
{}

KCode&
KCode::operator=(const KCode& A)
  /*!
    Assignment operator
    \param A :: KCode to copy
    \return *this
  */
{
  if (this!=&A)
    {
      SourceBase::operator=(A);
      rkk=A.rkk;
      defFlag=A.defFlag;
      vals=A.vals;
      ksrc=A.ksrc;
    }
  return *this;
}

KCode::~KCode()
  /*!
    Destructor
  */
{}

KCode*
KCode::clone() const
  /*!
    Clone constructor
    \return copy of this
  */
{
  return new KCode(*this);
}
  
void
KCode::setKSRC(const std::vector<Geometry::Vec3D>& Pts)
  /*!
    Set the ksrc terms
    \param Pts :: Points to use
   */
{
  ksrc=Pts;
  return;
}

void
KCode::setLine(const std::string& KLine)
  /*!
    Sets the kcode line
    \param KLine :: Line to process
  */
{
  ELog::RegMethod RegA("KCode","setLine");

  std::string Line=KLine;
  
  double R;
  int N;
  for(size_t index=0;index<8 && !StrFunc::isEmpty(Line);index++)
    {
      if (index==1 && StrFunc::section(Line,R))
	{
	  defFlag[index]=1;
	  rkk=R;
	}
      else if (index!=1 && StrFunc::section(Line,N))
	{

	  defFlag[index]=1;
	  vals[index]=N;
	}
      else
	{
	  ELog::EM<<"Failed to understand kcode line:"<<KLine<<" "
	    "at point "<<index+1<<ELog::endErr;
	}
    }
  return;
  
}


void
KCode::writePHITS(std::ostream&) const
  /*!
    Write out as a PHITS source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("KCode","write");

  ELog::EM<<"NOT YET WRITTEN "<<ELog::endCrit;
  return;
}


void
KCode::write(std::ostream& OX) const
  /*!
    Basic MCNPX writout function
    \param OX :: Output stream
   */
{
  ELog::RegMethod RegA("KCode","write");

  std::ostringstream cx;
  cx<<"kcode";
  size_t i;
  for(i=8;i>0 && !defFlag[i-1];i--) ;
  for(size_t j=0;j<i;j++)
    {
      if (defFlag[j])
	cx<<" "<<((j==1) ? rkk : vals[j]);
      else
	cx<<" j";
    }
  StrFunc::writeMCNPX(cx.str(),OX);
  if (!ksrc.empty())
    {
      cx.str("");
      cx<<"ksrc ";
      for(size_t i=0;i<ksrc.size();i++)
	{
	  cx<<"      "<<ksrc[i++];
	  if (i!=ksrc.size()) cx<<"   "<<ksrc[i];
	  if (i>1)
	    StrFunc::writeMCNPXcont(cx.str(),OX);
	  else
	    StrFunc::writeMCNPX(cx.str(),OX);
	  cx.str("");
	}
    }
  return;
}
  
}  // NAMESPACE SDef


