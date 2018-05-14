/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   endf/ENDF.cxx
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
#include "ENDF.h"

namespace ENDF
{

int lineCnt(0);       ///< Line count of read numbers 

std::pair<std::string,std::string>
getLine(std::istream& IX) 
 /*!
    Return split string because need to split
    from first 66 char of data and the last ide numbes
    \param IX :: File stream
    \return  string[0..65] :: string[66..80] 
 */
{
  ELog::RegMethod RegA("ENDF","getLine");
  
  const std::string FullLine=StrFunc::getAllLine(IX,255);
  if (IX.fail()) 
    throw ColErr::FileError(0,"ENDF","ReadLine");

  ENDF::lineCnt++;
  return std::pair<std::string,std::string>
    (FullLine.substr(0,66),FullLine.substr(66,80));
}

int
getNumber(std::string& Line,double& DF)
  /*!  Process a line of the types
    - x.yyyyyySz     : Number.decimal +/- Number
    - x.yyyyySzz     : Nubmer.decimal +/- Number   
    \param Line :: Line to process
    \param DF :: Number to extract
    \return 1 on success / 0 on failure
  */
{
  ELog::RegMethod RegA("ENDF","getNumber");
  double D;
  int inExp;
  if (StrFunc::sectPartNum(Line,D) &&
      StrFunc::sectPartNum(Line,inExp))
    {
      DF=D*pow(10.0,inExp);
      return 1;
    }
  return 0;
}

int
getNumber(std::string& Line,const size_t N,double& DF)
  /*!  Process a line of the types
    - x.yyyyyySz     : Number.decimal +/- Number
    - x.yyyyySzz     : Nubmer.decimal +/- Number   
    \param Line :: Line to process
    \param N :: Number of characters to extract
    \param DF :: Number to extract
    \return 1 on success / 0 on failure
  */
{
  ELog::RegMethod RegA("ENDF","getNumber");
  double D;
  int inExp;
  std::string LPart=Line.substr(0,N);
  if (StrFunc::sectPartNum(LPart,D) &&
      StrFunc::sectPartNum(LPart,inExp))
    {
      DF=D*pow(10.0,inExp);
    }
  else if (StrFunc::section(LPart,D))
    {
      DF=D;
    }
  else 
    return 0;
  
  Line.erase(0,N);
  return 1;
}

Triple<int>  
getMatIndex(const std::string& NStr)
  /*!
    Extracts the three components: mat, mf, mt
    \param NStr :: string to process
    \return (Mat,MF,MT)
   */
{
  ELog::RegMethod RegA("ENDF","getMatIndex");

  Triple<int> Out;
  if (!StrFunc::convert(NStr.substr(0,4),Out.first) ||
      !StrFunc::convert(NStr.substr(4,2),Out.second) ||
      !StrFunc::convert(NStr.substr(6,3),Out.third))  
    throw ColErr::InvalidLine(NStr,"NStr fail to convert [Mat,MF,MT]");

  return Out;
}

void
headRead(std::istream& IX,
	 double& c1,double& c2,
	 int &l1, int& l2, int& n1, int& n2)
  /*!
    Processes the list with the parameter. 
    \param IX :: File to read [at head position]
    \param c1 :: double number 
    \param c2 :: double number
    \param l1 :: int number
    \param l2 :: int number
    \param n1 :: int number 
    \param n2 :: int number
   */
{
  ELog::RegMethod RegA("ENDF","headRead");
  typedef std::pair<std::string,std::string> LTYPE;
  LTYPE IA=getLine(IX);
  if (!getNumber(IA.first,c1) || !getNumber(IA.first,c2) ||
      !StrFunc::section(IA.first,l1) || !StrFunc::section(IA.first,l2) ||
      !StrFunc::section(IA.first,n1) || !StrFunc::section(IA.first,n2))
    throw ColErr::InvalidLine(IA.first,"ENDF::headRead(A)");

  return;
}

std::string
findMatMfMt(std::istream& IX,const int aimMAT,
	    const int aimMF,const int aimMT)
  /*!
    Aim to find a material/MF/Mt card
    \param IX :: filestream
    \param aimMAT :: material number
    \param aimMF :: format
    \param aimMT :: table
    \return Main body string
  */
{
  ELog::RegMethod RegA("ENDFmaterial","findMatMfMt");
  Triple<int> Index;
  for(;;)
    {
      // this line thows if at end of file
      std::pair<std::string,std::string> Item=getLine(IX);
      Index=getMatIndex(Item.second);
      if ((Index.first==aimMAT || aimMAT==0) &&
	  (Index.second==aimMF || aimMF==0) &&
	  (Index.third==aimMT || aimMT==0))
	return Item.first;
    }
  // Never gets here
  return "";
}

void
listRead(std::istream& IX,
	 double& c1,double& c2,
	 int &l1, int& l2, int& npl, int& n2,
	 std::vector<double>& Data)
/*!
  Processes the list with the parameter
  - Format : 2e11.0,4I11,I4,i2,i3,i5
  - Format : 6e11.0
  \param IX :: File stream [already at place]
  \param c1 :: double number
  \param c2 :: double number
  \param l1 :: int number
  \param l2 :: int number
  \param npl :: number of points
  \param n2 :: int number
  \param Data :: Data vector
*/
{
  ELog::RegMethod RegA("ENDF","listRead");
  // Clear before we start
  Data.clear();

  typedef std::pair<std::string,std::string> LTYPE;
  LTYPE IA=getLine(IX);
  if (!getNumber(IA.first,11,c1) || !getNumber(IA.first,11,c2) ||
      !StrFunc::fortRead(IA.first,11,l1) || 
      !StrFunc::fortRead(IA.first,11,l2) ||
      !StrFunc::fortRead(IA.first,11,npl) ||
      !StrFunc::fortRead(IA.first,11,n2))
    throw ColErr::InvalidLine(IA.first+IA.second,
			      "ENDF::listRead(A)");

  double dX;  
  for(size_t i=0;i<static_cast<size_t>(npl);i++)
    {
      if (!(i % 6)) IA=getLine(IX);
      if (!getNumber(IA.first,11,dX))
	throw ColErr::InvalidLine(IA.first+IA.second,
				  "ENDF::listRead(B)",i);
      Data.push_back(dX);
    }
  return;
}


void
table1Read(std::istream& IX,
	   double& c1,double& c2,
	   int& l1, int& l2, int& nr, int& np,
	   std::vector<int>& NBT,
	   std::vector<int>& INT,
	   std::vector<double>& XData,
	   std::vector<double>& YData) 
  /*!
    Read and process a table of type 1.
    - format(2e11,4i11)
    - format(6i11)
    - format(6e11.0)
    \param IX :: input stream
    \param c1 :: double number
    \param c2 :: double number
    \param l1 :: int number
    \param l2 :: int number
    \param nr :: number of records
    \param np :: number of points
    \param NBT :: tables
    \param INT :: Interpolation type
    \param XData :: Data vector
    \param YData :: Data vector
   */
{
  ELog::RegMethod RegA("ENDF","table1Read");
  // Clear before we start
  NBT.clear();
  INT.clear();
  XData.clear();
  YData.clear();

  typedef std::pair<std::string,std::string> LTYPE;
  LTYPE IA=getLine(IX);
  if (!getNumber(IA.first,11,c1) || !getNumber(IA.first,11,c2) ||
      !StrFunc::fortRead(IA.first,11,l1) || 
      !StrFunc::fortRead(IA.first,11,l2) ||
      !StrFunc::fortRead(IA.first,11,nr) ||
      !StrFunc::fortRead(IA.first,11,np))
    throw ColErr::InvalidLine(IA.first+IA.second,
			      "ENDF::table1read(A)");

  int inpA,inpB;
  for(size_t i=0;i<static_cast<size_t>(nr);i++)
    {
      if (!(i % 3)) IA=getLine(IX);
      if (!StrFunc::fortRead(IA.first,11,inpA) || 
	  !StrFunc::fortRead(IA.first,11,inpB)) 
	throw ColErr::InvalidLine(IA.first+IA.second,
				  "ENDF::table1Read(B)",i);
      NBT.push_back(inpA);
      INT.push_back(inpB);
    }

  double dX,dY;
  for(size_t i=0;i<static_cast<size_t>(np);i++)
    {
      if (!(i % 3)) IA=getLine(IX);
      if (!getNumber(IA.first,11,dX) || !getNumber(IA.first,11,dY))
	throw ColErr::InvalidLine(IA.first+IA.second,
				  "ENDF::table1Read",i);
      XData.push_back(dX);
      YData.push_back(dY);
    }
  return;
}

void
table2Read(std::istream& IX,
	   double& c1,double& c2,
	   int &l1, int& l2, int& nr, int& nz,
	   std::vector<int>& NBT,
	   std::vector<int>& INT)
  /*!
    Processes the tab2 with the parameter
    - Format (2e11,4I11)
    - Format (6i11)

    \param IX :: File to read [at head position]    
    \param c1 :: double number
    \param c2 :: double number
    \param l1 :: int number
    \param l2 :: int number
    \param nr :: number of points
    \param nz :: int number
    \param NBT :: First data unit
    \param INT :: Second data unit
   */
{
  ELog::RegMethod RegA("ENDF","table2Read");
  // Clear before we start
  NBT.clear();
  INT.clear();

  typedef std::pair<std::string,std::string> LTYPE;
  LTYPE IA=getLine(IX);
  if (!getNumber(IA.first,11,c1) || !getNumber(IA.first,11,c2) ||
      !StrFunc::fortRead(IA.first,11,l1) || 
      !StrFunc::fortRead(IA.first,11,l2) ||
      !StrFunc::fortRead(IA.first,11,nr) ||
      !StrFunc::fortRead(IA.first,11,nz))
    throw ColErr::InvalidLine(IA.first+IA.second,
			      "ENDF::table2Read(A)");

  // Section 2:
  int inpA,inpB;
  for(size_t i=0;i<static_cast<size_t>(nr);i++)
    {
      if (!(i % 3)) IA=getLine(IX);
      if (!StrFunc::fortRead(IA.first,11,inpA) || 
	  !StrFunc::fortRead(IA.first,11,inpB))
	throw ColErr::InvalidLine(IA.first+IA.second,
				  "ENDF::table2Read(B)",i);
      NBT.push_back(inpA);
      INT.push_back(inpB);
    }
  return;
}

//---------------
// INTERPOLATION:
//---------------

double 
loglinear(const double& x1,const double& x2,const double& y1,
	  const double& y2,const double& aimX)
  /*!
    Interpolate a point in a log-linear graph
    \param x1 :: x1 point [low]
    \param x2 :: x2 point [high]
    \param y1 :: y1 point [at x1]
    \param y2 :: y2 point [at x2]
    \param aimX :: x point for aim
   */
{
  const double ly1=log(y1);
  const double ly2=log(y2);
  
  return exp(ly1+(ly2-ly1)*(aimX-x1)/(x2-x1));
}

}  // NAMESPACE ENDF
