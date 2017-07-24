/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   visit/MD5sum.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <complex>
#include <string>
#include <sstream>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <boost/format.hpp>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Triple.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "MatMD5.h"
#include "MD5sum.h"


std::ostream&
operator<<(std::ostream& OX,const MD5sum& A)
  /*!
    Write to a stream
    \param OX :: Output stream
    \param A :: MD5sum to write
    \return Stream state
  */
{
  A.write(OX);
  return OX;
}

MD5sum::MD5sum(const size_t MaxN) : 
  Results(MaxN)
  /*!
    Constructor
    \param MaxN :: Maximum number of materials
  */
{}

MD5sum::MD5sum(const MD5sum& A) : 
  Origin(A.Origin),XYZ(A.XYZ),nPts(A.nPts),
  Results(A.Results)
  /*!
    Copy constructor
    \param A :: MD5sum to copy
  */
{}

MD5sum&
MD5sum::operator=(const MD5sum& A)
  /*!
    Assignment operator
    \param A :: MD5sum to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Origin=A.Origin;
      XYZ=A.XYZ;
      nPts=A.nPts;
      Results=A.Results;
    }
  return *this;
}

MD5sum::~MD5sum()
  /*!
    Destructor
   */
{}

void 
MD5sum::setBox(const Geometry::Vec3D& startPt,
	      const Geometry::Vec3D& endPt)
  /*!
    Set the box start and end point
    \param startPt :: Point on near corner
    \param endPt :: Point on opposite corner
  */
{
  Origin=startPt;
  XYZ=endPt-Origin;
  return;
}


void
MD5sum::setIndex(const size_t A,const size_t B,const size_t C)
  /*!
    Set the index values , checks to ensure that 
    they are greater than zero
    \param A :: Xcoordinate division
    \param B :: Ycoordinate division
    \param C :: Zcoordinate division
  */
{
  nPts=Triple<size_t>((A>0) ? A : 1,
		      (B>0) ? B : 1,
		      (C>0) ? C : 1);
  return;
}

void
MD5sum::populate(const Simulation* SimPtr)
  /*!
    The big population call
    \param SimPtr :: Simulation system
   */
{
  ELog::RegMethod RegA("MD5sum","populate");
  const size_t RSize(Results.size());
  MonteCarlo::Object* ObjPtr(0);
  Geometry::Vec3D aVec;

  size_t percent(0);
  size_t cnt(0);
  size_t reportTime(nPts[0]*nPts[1]*nPts[2] / 100);
  
  std::vector<double> sizeXYZ(3);
  std::vector<size_t> index(3);
  for(size_t i=0;i<3;i++)
    {
      sizeXYZ[i]=std::abs(XYZ[i]/static_cast<double>(nPts[i]));
      index[i]=i;
    }
  indexSort(sizeXYZ,index);
  // lowest first
  const size_t a=index[2];  
  const size_t b=index[1];
  const size_t c=index[0];
  for(size_t i=0;i<nPts[a];i++)
    {
      aVec[a]=XYZ[a]*(static_cast<double>(i)+0.5)/
	static_cast<double>(nPts[a]);
      
      for(size_t j=0;j<nPts[b];j++)
        {
	  aVec[b]=XYZ[b]*(static_cast<double>(j)+0.5)/
			  static_cast<double>(nPts[b]);

	  for(size_t k=0;k<nPts[c];k++)
	    {
	      aVec[c]=XYZ[c]*(static_cast<double>(k)+0.5)/
		static_cast<double>(nPts[c]);
	      
	      const Geometry::Vec3D Pt=Origin+aVec;
	      ObjPtr=SimPtr->findCell(Pt,ObjPtr);
	      const size_t matN=static_cast<size_t>(ObjPtr->getMat());
	      if (matN>=RSize)
		{
		  throw ColErr::IndexError<size_t>
		    (matN,RSize,"RSize[point="+StrFunc::makeString(aVec)+"]");
		}
	      
	      Results[matN].addUnit(aVec);
	      
	      if (cnt>reportTime)
		{
		  percent++;
		  cnt=0;
		  ELog::EM<<"On section "<<percent<<" ["
			  <<reportTime*percent<<"]"<<ELog::endTrace;
		}
	      cnt++;
	    }
	}
    }
  return;
}


void 
MD5sum::write(std::ostream& OX) const
  /*!
    Write out the data				
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("MD5sum","write");
  
  for(size_t i=0;i<Results.size();i++)
    {
      if (!Results[i].isEmpty())
	OX<<"Mat "<<i<<" "<<Results[i]<<std::endl;
    }
  return;
}
