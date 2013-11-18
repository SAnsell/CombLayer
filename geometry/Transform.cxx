/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geometry/Transform.cxx
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
#include <iomanip>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <boost/regex.hpp>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "regexSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "masterWrite.h"
#include "Transform.h"

namespace Geometry
{

std::ostream&
operator<<(std::ostream& OX,const Transform& A)
  /*!
    Output stream
    \param OX :: output stream
    \param A :: Transform to write
    \return Output stream
  */
{
  A.write(OX);
  return OX;
}

Transform::Transform() :
  Nmb(-1),Rot(3,3)
  /*!
    Constructor
  */
{
  Rot.identityMatrix();
}

Transform::Transform(const Transform& A) :
  Nmb(A.Nmb),Rot(A.Rot),Shift(A.Shift)
  /*!
    Copy Constructor
    \param A :: Transform to copy
  */
{}

Transform&
Transform::operator=(const Transform& A)
  /*!
    Assignment operator
    \param A :: Transform to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Nmb=A.Nmb;
      Rot=A.Rot;
      Shift=A.Shift;
    }
  return *this;
}

Transform::~Transform()
{}

int
Transform::setTransform(const std::string& DisP,const std::string& MatStr)
  /*!
    Creates in transform object from string forms of the 
    displacement and rotation matrix
    \param DisP :: Displacment string of three numbers + tr parts
    \param MatStr :: Matrix string of 9 numbers (no line breaks)
    \retval -1 :: Incorrect number of numbers in DisP
    \retval -2 :: DisP must start "tr" 
    \retval -3 :: transform number is not availiable
    \retval -4 :: Numbers are wrong in DisP
    \retval -5 :: Numbers are wrong in MatStr
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("Transform","setTransform(string,string)");

  boost::regex divSea("\\s*(\\S+)");
  
  std::vector<std::string> Items=StrFunc::StrParts(DisP,divSea);
  if (Items.size()<4)           //indecyferable line
    {
      ELog::EM<<"Transform Parts == "<<Items.size()<<ELog::endCrit;
      ELog::EM<<"Transform Dis =="<<DisP<<ELog::endErr;
      return -1;
    }
  
  if (tolower(Items[0][0])!='t' &&
      tolower(Items[0][1])!='r')
    return -2;

  Items[0].erase(0,2);      //Remove 'tr'
  int N;
  if (!StrFunc::convert(Items[0],N))
    {
      ELog::EM<<"Unable to convert "<<Items[0]<<ELog::endErr;
      return -3;              // no N
    }
  Nmb=N;
  
  double xyz[9];   // get shift vector
  size_t ii;
  for(ii=0;ii<3 && StrFunc::convert(Items[ii+1],xyz[ii]);ii++) ;
  if (ii!=3)
    return -4;
  Shift=Geometry::Vec3D(xyz);
  
  //get matrix
  std::string Mtest=MatStr;
  for(ii=0;ii<9 && StrFunc::section(Mtest,xyz[ii]);ii++) ;
  if (ii<9)
    return -5;
  Rot.setMem(3,3);
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      Rot[i][j]=xyz[i*3+j];

  // Invert matrix 
  Rot.Invert();
  return 0;
}

int
Transform::setTransform(const std::string& TLine)
  /*!
    Creates in transform object from string forms of the 
    displacement and rotation matrix
    \param TLine :: Displacment string of three numbers + tr part
    \retval 1 :: Incorrect number of numbers in TLine (To low cont possiltbe)
    \retval -2 :: DisP does not start "tr"
    \retval -3 :: transform number is not availiable
    \retval -4 :: Numbers are wrong in DisP
    \retval -5 :: Numbers are wrong in MatStr
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("Transform","setTransform");
  
  std::string Line=TLine;
  std::string ItemName;
  if (!StrFunc::section(Line,ItemName))
    return 1;

  double xyz[12];
  int cnt(0);
  for(cnt=0;cnt<12 && StrFunc::section(Line,xyz[cnt]);cnt++) ;
  if (cnt!=12)
    {
      if (StrFunc::isEmpty(Line))
	return 1;
      ELog::EM<<"Failed to understand line components:\n"
	      <<TLine<<ELog::endErr;
      return -4;
    }
  
  size_t type(0);
  if (ItemName.substr(0,2)=="tr" || ItemName.substr(0,2)=="TR")
    type=1;
  else if (ItemName.substr(0,3)=="*tr" || ItemName.substr(0,3)=="*TR")
    type=2;
  else 
    {
      ELog::EM<<"Transform:Failed on line "<<TLine<<ELog::endErr;
      return -2;
    }

  ItemName.erase(0,1+type);      //Remove 'tr' / '*tr'
  int N;
  if (!StrFunc::convert(ItemName,N))
    {
      ELog::EM<<"Number unavailable "<<ItemName<<ELog::endErr;
      return -3;              // no N
    }
  Nmb=N;
  
  Shift=Geometry::Vec3D(xyz[0],xyz[1],xyz[2]);
  
  Rot.setMem(3,3);
  if (type==1)
    {
      for(size_t i=0;i<3;i++)
	for(size_t j=0;j<3;j++)
	  Rot[i][j]=xyz[3+i*3+j];
    }
  else if (type==2)
    {
      for(size_t i=0;i<3;i++)
	for(size_t j=0;j<3;j++)
	  Rot[i][j]=cos(M_PI*xyz[3+i*3+j]/180.0);
    }
  // Invert matrix 
  Rot.Invert();
  return 0;
}

int
Transform::setTransform(const Vec3D& DisV,
			const Matrix<double>& Mat)
  /*!
    Creates in transform object from string forms of the 
    displacement and rotation matrix
    \param DisV :: Displacment Vector
    \param Mat :: Matrix of 3 items
    \retval -1 :: Incorrect number of numbers in DisP
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("Transform","setTransform(Vec,Mat)");
  Shift=DisV;
  std::pair<size_t,size_t> SizePair=Mat.size();
  if (SizePair.first!=3 || SizePair.second!=3)
    {
      ELog::EM<<"Size incorrect !(3x3) "
	      <<SizePair.first<<" "<<SizePair.second<<ELog::endErr;
      return -1;
    }
  Rot=Mat;
  Shift=DisV;
  return 0;
}

void
Transform::print() const
  /*!
    Print out the string in typical MCNPX output form 
    to the std::cout
    \todo (add precision)
  */
{
  std::cout<<"tr"<<Nmb<<" "<<Shift<<std::endl;
  std::cout<<Rot.str()<<std::endl;
  return;
}

void
Transform::write(std::ostream& OX) const
  /*!
    Print out the string in MCNPX output form 
    \param OX :: output stream
  */
{
  std::ostringstream cx;
  masterWrite& MW=masterWrite::Instance();

  cx<<"tr"<<Nmb<<" "<<MW.Num(Shift);
  StrFunc::writeMCNPX(cx.str(),OX);
  cx.str("");
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      cx<<MW.Num(Rot.item(i,j))<<" ";
  StrFunc::writeMCNPXcont(cx.str(),OX);
  return;
}

}  // NAMESPACE Geometry
