/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   work/BinData.cxx
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
#include <sstream>
#include <cmath>
#include <climits>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <iterator>

#include "Exception.h"
#include "GTKreport.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "doubleErr.h"
#include "mathSupport.h"
#include "BUnit.h"
#include "Boundary.h"
#include "BinData.h"

BinData::BinData() 
  /*!
    Constructor
  */
{}

BinData::BinData(const BinData& A) :
  Yvec(A.Yvec)
  /*!
    Copy Constructor (Deep Copy)
    \param A :: BinData to copy
  */
{}

BinData&
BinData::operator=(const BinData& A) 
  /*!
    Standard copy constructor ::
    Note the cleverness to avoid a memory copy
    \param A :: Object top copy
    \returns *this
  */
{
  if (this!=&A)
    {
      Yvec=A.Yvec;
    }
  return *this;
}


BinData::~BinData()
  /*!
    Standard Destructor
  */
{}

void
BinData::addData(const double& xA,const double& xB,
		 const DError::doubleErr& YD)
  /*!
    Adds a component to the data
    \param xA :: Initial x value
    \param xB :: Final x value
    \param YD :: ydata  points
  */
{
  Yvec.push_back(BUnit(xA,xB,YD));
  return;
}

void
BinData::addData(const double& W,
		 const double& xA,const double& xB,
		 const DError::doubleErr& YD)
  /*!
    Adds a component to the data
    \param W :: Weight
    \param xA :: Initial x value
    \param xB :: Final x value
    \param YD :: ydata  points
  */
{
  ELog::RegMethod RegA("BinData","addData(4)");
  Yvec.push_back(BUnit(W,xA,xB,YD));
  return;
}

BinData&
BinData::operator+=(const BinData& A)
  /*!
    Add two BinData object together. Howver, it is
    done in master form, i.e. do not rebin "this"
    \param A :: BinData object to add
    \return This + A
   */
{
  ELog::RegMethod RegA("BinData","operator+=");
  
  addFactor(A,1.0);
  return *this;
}

BinData&
BinData::operator-=(const BinData& A)
  /*!
    Subtract two BinData object together. However, it is
    done in master form, i.e. do not rebin "this"
    \param A :: BinData object to subtract
    \return This - A
   */
{
  return addFactor(A,-1.0);
}

BinData&
BinData::operator*=(const BinData& A)
  /*!
    Multiply two BinData object together. However, it is
    done in master form, i.e. do not rebin "this"
    \param A :: BinData object to multiply
    \return This * A
   */
{
  Boundary XComp;
  XComp.setBoundary(A.Yvec,Yvec);
  // Now XComp 
  Boundary::BTYPE::const_iterator xc;
  BItems::FTYPE::const_iterator axc;
  
  size_t xCoordinate(XComp.getIndex());             // Get first index
  for(xc=XComp.begin();xc!=XComp.end();xc++,xCoordinate++)
    {
      // Now loop over components and add fraction
      DError::doubleErr sum;
      for(axc=xc->begin();axc!=xc->end();axc++)
	sum+=axc->second*A.Yvec[axc->first].Y;
      Yvec[xCoordinate]*=sum;
    }

  return *this;
}

BinData&
BinData::operator/=(const BinData& A)
  /*!
    Multiply two BinData object together. However, it is
    done in master form, i.e. do not rebin "this"
    \param A :: BinData object to multiply
    \return This * A
   */
{
  Boundary XComp;
  XComp.setBoundary(A.Yvec,Yvec);
  // Now XComp 
  Boundary::BTYPE::const_iterator xc;
  BItems::FTYPE::const_iterator axc;

  size_t xCoordinate(XComp.getIndex());             // Get first index
  for(xc=XComp.begin();xc!=XComp.end();xc++,xCoordinate++)
    {
      // Now loop over components and add fraction
      DError::doubleErr sum;
      for(axc=xc->begin();axc!=xc->end();axc++)
	sum+=axc->second*A.Yvec[axc->first].Y;
      if (sum!=0) Yvec[xCoordinate]/=sum;
    }
  
  return *this;
}


BinData&
BinData::operator+=(const double V)
  /*!
    Add a value to the BinData
    \param V :: Value to add
    \return This + V
   */
{
  DataTYPE::iterator vc;
  for(vc=Yvec.begin();vc!=Yvec.end();vc++)
    vc->Y+=V;
  return *this;
}

BinData&
BinData::operator-=(const double V)
/*!
  Subtract a value to the BinData
  \param V :: Value to subtract
  \return This - V
*/
{
  DataTYPE::iterator vc;
  for(vc=Yvec.begin();vc!=Yvec.end();vc++)
    vc->Y-=V;
  return *this;
}

BinData&
BinData::operator*=(const double V)
/*!
  Multiply a value
  \param V :: Value to subtract
  \return this * V
*/
{
  DataTYPE::iterator vc;
  for(vc=Yvec.begin();vc!=Yvec.end();vc++)
    vc->Y*=V;
  return *this;
}

BinData&
BinData::operator/=(const double V)
/*!
  Divide a value
  \param V :: Value to divide
  \return this / V
*/
{
  if (V!=0.0)
    {
      DataTYPE::iterator vc;
      for(vc=Yvec.begin();vc!=Yvec.end();vc++)
	vc->Y/=V;
    }
  return *this;
}


BinData
BinData::operator+(const BinData& A)  const
/*!
  Adds two dataLine Objects
  \param A :: BinData to add
  \return this+A;
*/
{
  BinData Out=BinData(*this);
  Out+=A;
  return Out;
}

BinData
BinData::operator-(const BinData& A)  const
/*!
  Subtracts two dataLine Objects
  \param A :: BinData to subtract
  \return this-A;
*/
{
  BinData Out=BinData(*this);
  Out-=A;
  return Out;
}

BinData
BinData::operator*(const BinData& A)  const
/*!
  Multiplies two dataLine Objects
  \param A :: BinData to multiply
  \return this*A;
*/
{
  BinData Out=BinData(*this);
  Out*=A;
  return Out;
}

BinData
BinData::operator/(const BinData& A)  const
/*!
  Divide two dataLine Objects (with limited
  overflow protection)
  \param A :: BinData to divide
  \return this/A;
*/
{
  BinData Out=BinData(*this);
  Out/=A;
  return Out;
}

BinData
BinData::operator+(const double V)  const
/*!
  Adds a value to this object
  \param V :: BinData to add
  \return this+V;
*/
{
  BinData Out=BinData(*this);
  Out+=V;
  return Out;
}

BinData
BinData::operator-(const double V)  const
/*!
  Subtracts two dataLine Objects
  \param V :: BinData to subtract
  \return this-V;
*/
{
  BinData Out=BinData(*this);
  Out-=V;
  return Out;
}

BinData
BinData::operator*(const double V)  const
/*!
  Multiplies two dataLine Objects
  \param V :: Value to multiply by
  \return this*V;
*/
{
  BinData Out=BinData(*this);
  Out*=V;
  return Out;
}

BinData
BinData::operator/(const double V)  const
/*!
  Divide two dataLine Objects (with limited
  overflow protection)
  \param V :: BinData to divide
  \return this/V;
*/
{
  BinData Out=BinData(*this);
  Out/=V;
  return Out;
}

void
BinData::clear()
  /*!
    Clear the data
  */
{
  Yvec.clear();
  return;
}


BinData&
BinData::xScale(const double Scale)
 /*!
   Scale the X coordinate
   \param Scale :: Factor
   \return *this
 */
{
  DataTYPE::iterator vc;
  for(vc=Yvec.begin();vc!=Yvec.end();vc++)
    {
      vc->xA*=Scale;
      vc->xB*=Scale;
    }  
  return *this;
}


BinData&
BinData::addFactor(const BinData& A,const double Scale)
/*!
  True data map addition. This uses a Master:Slave
  addition algorithm. The group A is the Slave 
  and does not increase the size of the Master.

  This uses fractional addition.

  \param A :: BinData to add
  \param Scale :: Sign/Factor for adding (normally +/- 1)
  \return *this+ (scale)*A
*/
{
  // Conditions for this/A begin empty
  if (A.Yvec.empty())
    return *this;
  // IF this is empty carry out a copy
  if (Yvec.empty())
    {
      Yvec=A.Yvec;
      this->operator*=(Scale);
      return *this;
    }

  Boundary XComp;
  XComp.setBoundary(A.Yvec,Yvec);
  // Now XComp 
  Boundary::BTYPE::const_iterator xc;
  BItems::FTYPE::const_iterator axc;
  
  size_t xCoordinate(XComp.getIndex());             // Get first index
  for(xc=XComp.begin();xc!=XComp.end();xc++,xCoordinate++)
    {
      // Now loop over components and add fraction
      for(axc=xc->begin();axc!=xc->end();axc++)
	{
	  Yvec[xCoordinate]+=
	    A.Yvec[axc->first]*(Scale*axc->second);
	}
    }
  return *this;
}

BinData&
BinData::binDivide(const double power)
 /*!
   Divide the data by the bin width
   \param power :: raize bin width to the power
   \return *this
 */
{
  DataTYPE::iterator vc;
  for(vc=Yvec.begin();vc!=Yvec.end();vc++)
    {
      const double factor=pow(vc->xB - vc->xA,power);
      if (fabs(factor)>1e-20)
	vc->Y/=factor;
    }
  return *this;
}

BinData&
BinData::rebin(const BinData& A)
/*!
  Rebin the BinData based on the binning of 
  the incomming map.
  \param A :: BinData to take the new X-coordinate from
  \return *this
*/
{
  return rebin(A.Yvec);
}


BinData&
BinData::rebin(const std::vector<BUnit>& XOut)
 /*!
   Generate a new rebining base on user defined steps
   \param XOut :: Required Bin steps
   \return rebined(this)
 */
{
  ELog::RegMethod RegA("BinData","rebin");
  if (XOut.empty())
    throw ColErr::IndexError<size_t>(XOut.size(),0,"XOut");

  Boundary XComp;
  XComp.setBoundary(Yvec,XOut);
  // Now XComp 
  Boundary::BTYPE::const_iterator xc;
  BItems::FTYPE::const_iterator axc;

  // Copy [Including data]
  std::vector<BUnit> Ynew(XOut);
  DataTYPE::iterator vc;
  for(vc=Ynew.begin();vc!=Ynew.end();vc++)
    vc->Y=DError::doubleErr(0.0);

  // Now iterater over xcoordinates:
  size_t xCoordinate(XComp.getIndex());           // Get first index
  for(xc=XComp.begin();xc!=XComp.end();xc++,xCoordinate++)
    {
      // Now loop over components and add fraction 
      for(axc=xc->begin();axc!=xc->end();axc++)
	Ynew[xCoordinate]+=Yvec[axc->first]*axc->second;
    }
  Yvec=Ynew;
  return *this;
}

size_t
BinData::getIndex(const double X,const double Y) const
 /*!
   Find the closest point to X,Y.
   Uses the mid point of (xA,xB)
   \param X :: X coordinate
   \param Y :: Y coordinate
   \return index of closest point
   \todo Add method of only iterating over the points close to X.
 */
{
  ELog::RegMethod RegA("BinData","getIndex");

  double D(-1.0);          // Initialization to avoid g++ Warn
  size_t index=ULONG_MAX;
  for(size_t i=0;i<Yvec.size();i++)
    {
      const double MX=(Yvec[i].xA+Yvec[i].xB)/2.0;
      const double xdiff=MX-X;
      const double ydiff=Yvec[i].Y - Y;
      const double Dist=xdiff*xdiff+ydiff*ydiff;
      if (D<0 || D>Dist)
        {
	  D=Dist;
	  index=i;
	}
    }
  return index;
}

size_t
BinData::getMaxPoint() const
/*!
  Determine the highest valued point
  \retval -1 (no data)
  \retval index of maximum point
*/
{
  if (Yvec.empty()) return ULONG_MAX;


  double MaxV(Yvec[0].Y);
  size_t index(0);
  for(size_t i=1;i<Yvec.size();i++)
    {
      if (Yvec[i].Y>MaxV)
	{
	  MaxV=Yvec[i].Y;
	  index=i;
	}
    }

  return index;
}

int
BinData::load(const std::string& Fname,const int xColA,
	      const int xColB,const int yCol,const int eCol)
/*!
  Load the data from a simple column file
  \param Fname :: Filename 
  \param xColA :: Xcolumn       [0 for number ]
  \param xColB :: Xcolumn       [0 for number+1 ]
  \param yCol :: Ycolumn       [0 for just zero] 
  \param eCol :: Error coloumn [0 for no error]
  \return -ve on error / 0 on success
*/ 
{
  ELog::RegMethod REgA("BinData","load");

  std::ifstream IX;
  if (!Fname.empty())
    IX.open(Fname.c_str());
  if (!IX.is_open() || !IX.good())
    {
      ELog::EM<<"BinData::load Failed to open "
	      <<Fname<<ELog::endErr;
      return -1;
    }

  selectColumn(IX,xColA-1,xColB-1,yCol-1,eCol-1,0);
  IX.close();
  return 0;
}

int
BinData::selectColumn(std::istream& IX,const int xcolA,
		      const int xcolB,const int ycol,
		      const int ecol,const int offset)
/*! 
  Routine designed to reading columns from a file.
  The file can have any number of columns
  \param IX :: Input file stream
  \param xcolA :: xcolumn identifiers (-ve to skip)
  \param xcolB :: xcolumn identifiers (-ve to skip)
  \param ycol :: xcolumn identifiers (-ve to skip)
  \param ecol :: xcolumn identifiers (-ve to skip)
  \param offset :: Number lines to skip 
  \return Number of points successfully read
*/
{
  
  std::string CLine;    // current line
  std::string OverFlow; // overflow if needed
  
  const int maxCol(std::max(xcolA,std::max(ycol,ecol)));
  // Number of items to find:
  int ColCount(0);
  ColCount+=(xcolA<0) ? 0 : 1;
  ColCount+=(xcolB<0) ? 0 : 1;
  ColCount+=(ycol<0) ? 0 : 1;
  ColCount+=(ecol<0) ? 0 : 1;


  int dataCount(0);  // Data Count
  int index(0);      // Index in lookup
  int cnt(0);        // Number of objects found
  int deFlag(0);     // Did we find a DError::doubleErr component

  // storage
  std::vector<double> XA;
  std::vector<double> XB;
  DataTYPE Y;
  double xa,xb,y,e;
  DError::doubleErr YE;
  int extraFlag(1);
  int lN=StrFunc::getPartLine(IX,CLine,OverFlow,1024);
  while(lN>=0)
    {
      const int comment=StrFunc::stripComment(CLine);    // remove comment
      std::string Part,PartExtra;
      // LINE PROCESS LOOP:
      if (StrFunc::section(CLine,Part))
        {
	  for(;index<=maxCol && extraFlag;index++)
	    {
	      extraFlag=StrFunc::section(CLine,PartExtra);
	      // Note : Columns can be equal
	      if (index==xcolA && StrFunc::convert(Part,xa))
		cnt++;
	      if (index==xcolB && StrFunc::convert(Part,xb))
		cnt++;
	      if (index==ycol)
	        {
		  // reads y as doubleErr (y,E)
		  if (ecol<0 && extraFlag && 
		      StrFunc::convert(Part+" "+PartExtra,YE))
		    {
		      deFlag=1;
		      cnt++;
		    }
		  else if (StrFunc::convert(Part,y))
		    {
		      cnt++;
		    }
		}
	      if (index==ecol)
	        {
		  if (StrFunc::convert(Part,e))
		    cnt++;
		}
	      std::swap(Part,PartExtra);  
	    }
	  // NOW: (a) we have a everything 
	  if (cnt==ColCount)
	    {
	      dataCount++;
	      if (dataCount>offset)
	        {
		  if (!xcolA) 
		    {
		      xa=dataCount; xb=xa+1;
		    }
		  else if (!xcolB)
		    {
		      xb=xa+1.0;
		    }
		    
		  if (deFlag)
		    Y.push_back(BUnit(xa,xb,YE));
		  else if (ycol>=0 && ecol>=0)
		    Y.push_back(BUnit(xa,xb,DError::doubleErr(y,e)));
		  else if (ycol>=0)
		    Y.push_back(BUnit(xa,xb,DError::doubleErr(y,0.0)));
		  else if (ecol>=0)
		    Y.push_back(BUnit(xa,xb,DError::doubleErr(0.0,e)));
		  else
		    Y.push_back(BUnit(xa,xb,DError::doubleErr(0.0,0.0)));
		}
	      // clear line
	      if (lN) IX.ignore(512,'\n');
	      lN=0;
	    }
	  // (b) Insufficient found / Comment ends
	  else if (index>=maxCol || comment)   
	    {
	      if (lN) IX.ignore(512,'\n');
	      lN=0;
	    }
	  // (c) More line avaliable
	  else if (lN>0)
	    {
	      std::swap(CLine,OverFlow);
	    }
	}
      if (!lN)
        {
	  CLine.erase();
	  deFlag=0;
	  index=0;
	  cnt=0;
	  extraFlag=1;
	  lN=StrFunc::getPartLine(IX,CLine,OverFlow,1024);
	}
    }
  // All read/processed
  if (Y.size()>=1)
    { 
      Y.pop_back();
      Yvec=Y;
      return static_cast<int>(Y.size());
    }
  return 0;
}

void
BinData::write(std::ostream& OX) const
/*!
  Writes out information about the BinData
  \param OX :: Output stream
*/
{  
  const size_t MP(this->getMaxPoint());
  if (MP==ULONG_MAX)
    OX<<"No data"<<std::endl;
  else
    OX<<"Max ("<<MP<<")[<<"<<
      Yvec[MP].xA<<" "<<Yvec[MP].xB<<"] == "<<Yvec[MP].Y<<std::endl;
  
  return; 
}

DError::doubleErr
BinData::integrate(const double xMin,const double xMax) const
/*!
  Integrate the data from xMin to xMax : 
  Uses fractional analysis.
  \param xMin :: Xmin value
  \param xMax :: Xmax value
  \return integrated value
*/
{
  ELog::RegMethod RegA("BinData","integrate");

  DError::doubleErr sum;
  // Get first point
  size_t i;
  for(i=0;i<Yvec.size();i++)
    {
      const BUnit& BU(Yvec[i]);
      if (BU.xB>xMin && BU.xA<xMax)
	{
	  const double DV(BU.xB-BU.xA);
	  if (DV>0.0)
	    {
	      const double fracA=(xMin<BU.xA) ? 1.0 : (xMin-BU.xA)/DV;
	      const double fracB=(xMax>BU.xB) ? 0.0 : 1.0-(BU.xB-xMax)/DV;
	      const double frac=fracA+fracB;
	      sum+=Yvec[i].Y*frac;
	    }
	}
    }
  return sum;
}

void
BinData::normalize()
  /*!
    Remove the weight component from each bin
   */
{
  ELog::RegMethod RegA("BinData","normalize");
  std::vector<BUnit>::iterator vc;
  for(vc=Yvec.begin();vc!=Yvec.end();vc++)
    vc->norm();
  return;
}

void
BinData::write(const std::string& FName) const
/*!
  Writes out information about the DataMap
  \param FName :: Filename 
*/
{
  std::ofstream OX(FName.c_str());
  OX<<"# Pts "<<Yvec.size()<<std::endl;
  OX<<"# Title : "<<std::endl;
  for(unsigned int i=0;i<Yvec.size();i++)
    OX<<Yvec[i].xA<<" "<<Yvec[i].xB<<" "<<Yvec[i].Y<<std::endl;
  OX.close();
  return;
 
}
