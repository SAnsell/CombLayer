/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   work/WorkData.cxx
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
#include <cmath>
#include <climits>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <iterator>

#include <boost/format.hpp>

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
#include "WorkData.h"

WorkData::WorkData() : 
  weight(-1.0)
  /*!
    Constructor
  */
{}

WorkData::WorkData(const WorkData& A) :
  weight(A.weight),XCoord(A.XCoord),Yvec(A.Yvec)
  /*!
    Copy Constructor (Deep Copy)
    \param A :: WorkData to copy
  */
{}

WorkData&
WorkData::operator=(const WorkData& A) 
  /*!
    Standard copy constructor ::
    Note the cleverness to avoid a memory copy
    \param A :: Object top copy
    \returns *this
  */
{
  if (this!=&A)
    {
      weight=A.weight;
      XCoord=A.XCoord;
      Yvec=A.Yvec;
    }
  return *this;
}


WorkData::~WorkData()
  /*!
    Standard Destructor
  */
{}

void
WorkData::setX(const std::vector<double>& Xpts)
  /*!
    Set the Xpts base on a vector:
    \param Xpts :: not checked to Yvec/Evec size
  */
{
  XCoord=Xpts;
  return;
}

void
WorkData::setX(const size_t Index,const double XPt)
  /*!
    Set the a single point in the Xcoordinates 
    \param Index :: Point to use
    \param XPt :: not checked to Yvec/Evec size
  */
{
  if (Index>=XCoord.size())
    {
      ELog::RegMethod RegA("WorkData","setX");
      throw ColErr::IndexError<size_t>(Index,XCoord.size(),
				       "Index");
    }

  XCoord[Index]=XPt;
  return;
}

void
WorkData::setLogX(const double XMin,const double XMax,const size_t NPts)
  /*!
    Set a log set of points for X
    \param XMin :: Low X value [+ve]
    \param XMax :: High X value [+ve]
    \param NPts :: number of points
   */
{
  ELog::RegMethod RegA("WorkData","setLogX");
  if (NPts<1) return;
  
  if (XMin<=0.0 || XMax<0.0 || XMax==XMin)
    throw ColErr::NumericalAbort("Xmin == "+std::to_string(XMin)+
                                 " Xmax == "+std::to_string(XMax));

  XCoord.resize(NPts);
  Yvec.clear();
  for(size_t i=0;i<NPts;i++)
    XCoord[i]=mathFunc::logFromLinear(XMin,XMax,NPts,i);
  Yvec.resize(NPts-1,DError::doubleErr(0.0,0.0));
  return;
}

void
WorkData::setData(const std::vector<DError::doubleErr>& X, 
		  const std::vector<DError::doubleErr>& YD)
  /*!
    Set the data given Y and error
    \param X :: X data points [Error discarded]
    \param YD :: ydata  points
  */
{
  if (X.size()!=YD.size()+1)
    {
      ELog::RegMethod RegA("WorkData","setData");
      throw ColErr::MisMatch<size_t>(X.size(),YD.size()+1,
				     "X / YD+1 ");
    }

  XCoord.resize(X.size());
  transform(X.begin(),X.end(),XCoord.begin(),
	    std::bind(&DError::doubleErr::getVal,
			std::placeholders::_1));
  Yvec=YD;
  return;
}

void
WorkData::setData(const std::vector<double>& X, 
		  const std::vector<DError::doubleErr>& YD)
  /*!
    Set the data given Y and error
    \param X :: X data points
    \param YD :: ydata  points
  */
{
  if (X.size()!=YD.size()+1)
    {
      ELog::RegMethod RegA("WorkData","setData");
      throw ColErr::MisMatch<size_t>(X.size(),YD.size()+1,
				     "X / Y");
    }

  XCoord=X;
  Yvec=YD;
  return;
}

void
WorkData::setData(const std::vector<double>& Y,
		  const std::vector<double>& E)
  /*!
    Set the data given Y and error.
    \param Y :: Y points
    \param E :: Error points
  */
{
  Yvec.clear();
  std::vector<double>::const_iterator yc=Y.begin();
  std::vector<double>::const_iterator ec=E.begin();
  for(;ec!=E.end() && yc!=Y.end();yc++,ec++)
    Yvec.push_back(DError::doubleErr(*yc,*ec));
  for(;yc!=Y.end();yc++)
   Yvec.push_back(DError::doubleErr(*yc,0.0));

  return;
}

void
WorkData::setData(const std::vector<double>& X,
		  const std::vector<double>& Y,
		  const std::vector<double>& E)
  /*!
    Set the data given Y and error.
    \param X :: X points [ysize+1]
    \param Y :: Y points
    \param E :: Error points
  */
{
  if (X.size()!=Y.size()+1)
    {
      ELog::RegMethod RegA("WorkData","setData(x,y,e)");
      throw ColErr::MisMatch<size_t>(X.size(),Y.size()+1,
				       "X / Y");
    }
  setX(X);
  Yvec.clear();
  std::vector<double>::const_iterator yc=Y.begin();
  std::vector<double>::const_iterator ec=E.begin();
  for(;ec!=E.end() && yc!=Y.end();yc++,ec++)
    Yvec.push_back(DError::doubleErr(*yc,*ec));
  for(;yc!=Y.end();yc++)
   Yvec.push_back(DError::doubleErr(*yc,0.0));

  return;
}

void
WorkData::setSize(const size_t S)
  /*!
    Set the size including refill the array
    with zero.
    \param S :: New size for Y 
   */
{
//  const int Nx(XCoord.size());
  XCoord.resize(S+1);
  Yvec.resize(S,DError::doubleErr(0.0,0.0));
  return;
}

void
WorkData::setData(const size_t Index,const double Ypt,const double Ept)
  /*!
    Set a data point Map[A][B]=Val : Error
    \param Index :: Point in map
    \param Ypt :: Y value
    \param Ept :: Error value
   */
{
  if (Index>=Yvec.size())
    {
      ELog::RegMethod RegA("WorkData","setData(I,ypt,ept)");      
      throw ColErr::IndexError<size_t>(Index,Yvec.size(),"Index");
    }
  Yvec[Index]=DError::doubleErr(Ypt,Ept);
  return;
}

void
WorkData::setData(const size_t Index,const double Xpt,
		  const double Ypt,const double Ept)
  /*!
    Set a data point Map[A][B]=Val : Error
    \param Index :: Point in map
    \param Xpt :: X value (+1 point)
    \param Ypt :: Y value
    \param Ept :: Error value
   */
{
  if (Index>=Yvec.size())
    {
      ELog::RegMethod RegA("WorkData","setData(I,xpt,ypt,ept)");      
      throw ColErr::IndexError<size_t>(Index,Yvec.size(),"Index");
    }
  
  XCoord[Index+1]=Xpt;
  Yvec[Index]=DError::doubleErr(Ypt,Ept);
  return;
}

void
WorkData::initX(const double Xpt)
  /*!
    Initialize and set first X point
    \param Xpt :: First X Point
   */
{
  XCoord.clear();
  Yvec.clear();
  XCoord.push_back(Xpt);
  return;
}
void
WorkData::pushData(const double Ypt,const double Ept)
  /*!
    Push back the Ypt and the Error.
    \param Ypt :: Y data point
    \param Ept :: Error to the Ypt [sqrt form]
   */
{
  Yvec.push_back(DError::doubleErr(Ypt,Ept));
  if (XCoord.size()<=Yvec.size())
    XCoord.push_back(static_cast<int>(Yvec.size()));
  return;
}

void
WorkData::pushData(const DError::doubleErr& Ypt)
  /*!
    Push back the Ypt and the Error.
    \param Ypt :: Y data point
   */
{
  Yvec.push_back(Ypt);
  if (XCoord.size()<=Yvec.size())
    XCoord.push_back(static_cast<int>(Yvec.size()));
  return;
}

void
WorkData::pushData(const double Xpt,const DError::doubleErr& Ypt)
  /*!
    Push back the Ypt and the Error.
    \param Xpt :: X data point [upper boundary]
    \param Ypt :: Y data point
  */
{
  // First point test
  if (XCoord.empty()) XCoord.push_back(0.0);
  XCoord.push_back(Xpt);
  Yvec.push_back(Ypt);
  return;
}

WorkData&
WorkData::operator+=(const WorkData& A)
  /*!
    Add two WorkData object together. Howver, it is
    done in master form, i.e. do not rebin "this"
    \param A :: WorkData object to add
    \return This + A
   */
{
  ELog::RegMethod RegA("WorkData","operator+=");
  if (A.weight>0 && weight>0)         // adding with quadrature
    {
      this->operator*=(weight);
      addFactor(A,A.weight);
      weight+=A.weight;
      this->operator/=(weight);
    }
  else 
    {
      addFactor(A,1.0);
      weight=A.weight;
    }
  return *this;
}

WorkData&
WorkData::operator-=(const WorkData& A)
  /*!
    Subtract two WorkData object together. However, it is
    done in master form, i.e. do not rebin "this"
    \param A :: WorkData object to subtract
    \return This - A
   */
{
  return addFactor(A,-1.0);
}

WorkData&
WorkData::operator*=(const WorkData& A)
  /*!
    Multiply two WorkData object together. However, it is
    done in master form, i.e. do not rebin "this"
    \param A :: WorkData object to multiply
    \return This * A
   */
{
  Boundary XComp;
  XComp.setBoundary(A.XCoord,XCoord);
  // Now XComp 
  Boundary::BTYPE::const_iterator xc;
  BItems::FTYPE::const_iterator axc;
  
  size_t xCoordinate(XComp.getIndex());             // Get first index
  for(xc=XComp.begin();xc!=XComp.end();xc++,xCoordinate++)
    {
      // Now loop over components and add fraction
      for(axc=xc->begin();axc!=xc->end();axc++)
	Yvec[xCoordinate]*=
	  axc->second*A.Yvec[axc->first];
    }
  return *this;
}

WorkData&
WorkData::operator/=(const WorkData& A)
  /*!
    Multiply two WorkData object together. However, it is
    done in master form, i.e. do not rebin "this"
    \param A :: WorkData object to multiply
    \return This * A
   */
{
  Boundary XComp;
  XComp.setBoundary(A.XCoord,XCoord);
  // Now XComp 
  Boundary::BTYPE::const_iterator xc;
  BItems::FTYPE::const_iterator axc;
  
  size_t xCoordinate(XComp.getIndex());             // Get first index
  for(xc=XComp.begin();xc!=XComp.end();xc++,xCoordinate++)
    {
      // Now loop over components and add fraction
      for(axc=xc->begin();axc!=xc->end();axc++)
        {
	  const double compVal=	axc->second*A.Yvec[axc->first];
	  if (compVal!=0)
	    Yvec[xCoordinate]/=compVal;
	  else
	    Yvec[xCoordinate]=0.0;
	}
    }
  return *this;
}


WorkData&
WorkData::operator+=(const double V)
  /*!
    Add a value to the WorkData
    \param V :: Value to add
    \return This + V
   */
{
  transform(Yvec.begin(),Yvec.end(),Yvec.begin(),
	    std::bind2nd(std::plus<DError::doubleErr>(),V));
  return *this;
}

WorkData&
WorkData::operator-=(const double V)
  /*!
    Subtract a value to the WorkData
    \param V :: Value to subtract
    \return This - V
   */
{
  transform(Yvec.begin(),Yvec.end(),Yvec.begin(),
	    std::bind2nd(std::minus<DError::doubleErr>(),V));
  return *this;
}

WorkData&
WorkData::operator*=(const double V)
  /*!
    Multiply a value
    \param V :: Value to subtract
    \return this * V
   */
{
  transform(Yvec.begin(),Yvec.end(),Yvec.begin(),
	    std::bind2nd(std::multiplies<DError::doubleErr>(),V));
  return *this;
}

WorkData&
WorkData::operator/=(const double V)
  /*!
    Divide a value
    \param V :: Value to divide
    \return this / V
   */
{
  if (V!=0.0)
    {
      transform(Yvec.begin(),Yvec.end(),Yvec.begin(),
		std::bind2nd(std::divides<DError::doubleErr>(),V));
    }
  return *this;
}


WorkData
WorkData::operator+(const WorkData& A)  const
  /*!
    Adds two dataLine Objects
    \param A :: WorkData to add
    \return this+A;
   */
{
  WorkData Out=WorkData(*this);
  Out+=A;
  return Out;
}

WorkData
WorkData::operator-(const WorkData& A)  const
  /*!
    Subtracts two dataLine Objects
    \param A :: WorkData to subtract
    \return this-A;
   */
{
  WorkData Out=WorkData(*this);
  Out-=A;
  return Out;
}

WorkData
WorkData::operator*(const WorkData& A)  const
  /*!
    Multiplies two dataLine Objects
    \param A :: WorkData to multiply
    \return this*A;
   */
{
  WorkData Out=WorkData(*this);
  Out*=A;
  return Out;
}

WorkData
WorkData::operator/(const WorkData& A)  const
  /*!
    Divide two dataLine Objects (with limited
    overflow protection)
    \param A :: WorkData to divide
    \return this/A;
   */
{
  WorkData Out=WorkData(*this);
  Out/=A;
  return Out;
}

WorkData
WorkData::operator+(const double V)  const
  /*!
    Adds a value to this object
    \param V :: WorkData to add
    \return this+V;
   */
{
  WorkData Out=WorkData(*this);
  Out+=V;
  return Out;
}

WorkData
WorkData::operator-(const double V)  const
  /*!
    Subtracts two dataLine Objects
    \param V :: WorkData to subtract
    \return this-V;
   */
{
  WorkData Out=WorkData(*this);
  Out-=V;
  return Out;
}

WorkData
WorkData::operator*(const double V)  const
  /*!
    Multiplies two dataLine Objects
    \param V :: Value to multiply by
    \return this*V;
  */
{
  WorkData Out=WorkData(*this);
  Out*=V;
  return Out;
}


WorkData
WorkData::operator/(const double V)  const
  /*!
    Divide two dataLine Objects (with limited
    overflow protection)
    \param V :: WorkData to divide
    \return this/V;
   */
{
  WorkData Out=WorkData(*this);
  Out/=V;
  return Out;
}

void
WorkData::normalize(const size_t NPts)
  /*!
    Normalize the data to N pts
    \param NPts :: Number of points [not zero]
  */
{
  ELog::RegMethod RegA("WorkData","normalize");

  if (NPts)
    {
      const double scale(1.0/static_cast<double>(NPts));
      weight*=scale;
  
      this->operator*=(scale);
    }
  return;
}

WorkData&
WorkData::xScale(const double Scale)
  /*!
    Scale the X coordinate
    \param Scale :: Factor
    \return *this
  */
{

  transform(XCoord.begin(),XCoord.end(),XCoord.begin(),
	    std::bind2nd(std::multiplies<double>(),Scale));
  
  return *this;
}

double
WorkData::XInverse(const double YValue) const
  /*!
    Given a value Y find the value X that corresponds to it
    Assumes that Ydata is ordered.
    \param YValue :: YValue
    \return X value [linear split]
   */
{
  ELog::RegMethod RegA("WorkData","XInverse");
  
  long int index=indexPos(Yvec,DataTYPE::value_type(YValue));
  const size_t IX=static_cast<size_t>(index);
  if (index<0 || IX>=Yvec.size())
    throw ColErr::RangeError<double>(YValue,Yvec.front().getVal(),
                                     Yvec.back().getVal(),"value out of range");

  const double yA=Yvec[IX].getVal();
  const double yB=Yvec[IX+1].getVal();
  const double frac=(YValue-yA)/(yB-yA);

  return XCoord[IX]+frac*(XCoord[IX+1]-XCoord[IX]);
}


WorkData&
WorkData::addFactor(const WorkData& A,const double Scale)
  /*!
    True data map addition. This uses a Master:Slave
    addition algorithm. The group A is the Slave 
    and does not increase the size of the Master.

    This uses fractional addition.

    \param A :: WorkData to add
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
      XCoord=A.XCoord;
      Yvec=A.Yvec;
      this->operator*=(Scale);
      return *this;
    }

  Boundary XComp;
  XComp.setBoundary(A.XCoord,XCoord);
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

WorkData&
WorkData::binDivide(const double power)
  /*!
    Divide the data by the bin width
    \param power :: raize bin width to the power
    \return *this
  */
{
  for(size_t i=0;i<Yvec.size();i++)
    {
      const double factor=pow(XCoord[i+1]-XCoord[i],power);
      if (fabs(factor)>1e-20)
	Yvec[i]/=factor;
    }
  return *this;
}


WorkData&
WorkData::rebin(const WorkData& A)
  /*!
    Rebin the WorkData based on the binning of 
    the incomming map.
    \param A :: WorkData to take the new X-coordinate from
    \return *this
  */
{
  return rebin(A.XCoord);
}


WorkData&
WorkData::rebin(const std::vector<double>& XOut)
  /*!
    Generate a new rebining base on user defined steps
    \param XOut :: Required Bin steps
    \return rebined(this)
  */
{
  ELog::RegMethod RegA("WorkData","rebin");
  if (XOut.size()<2)
    throw ColErr::IndexError<size_t>(XOut.size(),2,"XOut size");

  Boundary XComp;
  XComp.setBoundary(XCoord,XOut);
  // Now XComp 
  Boundary::BTYPE::const_iterator xc;
  BItems::FTYPE::const_iterator axc;

  std::vector<DError::doubleErr> Ynew(XOut.size()-1);
  fill(Ynew.begin(),Ynew.end(),0.0);

  size_t xCoordinate(XComp.getIndex());           // Get first index
  for(xc=XComp.begin();xc!=XComp.end();xc++,xCoordinate++)
    {
      // Now loop over components and add fraction 
      for(axc=xc->begin();axc!=xc->end();axc++)
	Ynew[xCoordinate]+=Yvec[axc->first]*axc->second;
    }
  Yvec=Ynew;
  XCoord=XOut;
  return *this;
}

size_t
WorkData::getIndex(const double X,const double Y) const
  /*!
    Find the closest point to X,Y.
    Uses the mid point of XCoord

    \todo Add method of only iterating over the points
    close to X.

    \param X :: X coordinate
    \param Y :: Y coordinate
    \return index of point [ULONG_MAX on unfound coordinate]
   */
{
  ELog::RegMethod RegA("workData","getIndex");

  double D(-1.0);         
  size_t index=ULONG_MAX;
  for(size_t i=1;i<XCoord.size();i++)
    {
      const double MX=(XCoord[i]+XCoord[i-1])/2.0;
      const double xdiff=MX-X;
      const double ydiff=Yvec[i]-Y;
      const double Dist=xdiff*xdiff+ydiff*ydiff;
      if (D>Dist || D<0)
        {
	  D=Dist;
	  index=i;
	}
    }
  return index;
}

size_t
WorkData::getXIndex(const double X) const
  /*!
    Find the appropiate X bin
    \param X :: X coordinate
    \return index of point [ULONG_MAX on unfound coordinate]
   */
{
  if (XCoord.empty() ||
      XCoord.front()>X ||
      XCoord.back()<X)
    return ULONG_MAX;

  std::vector<double>::const_iterator vc=
    lower_bound(XCoord.begin(),XCoord.end(),X);
  return static_cast<size_t>(vc-XCoord.begin());
}

void
WorkData::addPoint(const double XC,const double YVal)
  /*!
    Add a point to existing bin
    \param XC :: XValue
    \param YVal :: Data point ot add
  */
{
  const size_t Index=getXIndex(XC);
  if (Index<=XCoord.size())
    Yvec[Index]+=YVal;
  return;
}


size_t
WorkData::getMaxPoint() const
  /*!
    Determine the highest valued point
    \retval ULONG_MAX (no data)
    \retval index of maximum point
   */
{
  if (Yvec.empty()) return ULONG_MAX;
  DataTYPE::const_iterator vc=
    std::max_element(Yvec.begin(),Yvec.end());
  return static_cast<size_t>(distance(Yvec.begin(),vc));
}

int
WorkData::load(const std::string& Fname,const int xCol,
	       const int yCol,const int eCol)
  /*!
    Load the data from a simple column file
    \param Fname :: Filename 
    \param xCol :: Xcolumn       [0 for number ]
    \param yCol :: Ycolumn       [0 for just zero] 
    \param eCol :: Error coloumn [0 for no error]
    \return -ve on error / 0 on success
  */ 
{
  ELog::RegMethod RegA("WorkData","load");

  std::ifstream IX;
  if (!Fname.empty())
    IX.open(Fname.c_str());
  if (!IX.is_open() || !IX.good())
    {
      ELog::EM<<"WorkData::load Failed to open "
	      <<Fname<<ELog::endErr;
      return -1;
    }

  selectColumn(IX,xCol-1,yCol-1,eCol-1,0);
  IX.close();
  return 0;
}

int
WorkData::selectColumn(std::istream& IX,const int xcol,const int ycol,
		       const int ecol,const int offset)
 /*! 
   Routine designed to reading columns from a file.
   The file can have any number of columns
   \param IX :: Input file stream
   \param xcol :: xcolumn identifiers (-ve to skip)
   \param ycol :: xcolumn identifiers (-ve to skip)
   \param ecol :: xcolumn identifiers (-ve to skip)
   \param offset :: Number lines to skip 
   \return Number of points successfully read
 */
{
  
  std::string CLine;    // current line
  std::string OverFlow; // overflow if needed
  
  const int maxCol(std::max(xcol,std::max(ycol,ecol)));
  // Number of items to find:
  int ColCount(0);
  ColCount+=(xcol<0) ? 0 : 1;
  ColCount+=(ycol<0) ? 0 : 1;
  ColCount+=(ecol<0) ? 0 : 1;


  int dataCount(0);  // Data Count
  int index(0);      // Index in lookup
  int cnt(0);        // Number of objects found
  int deFlag(0);     // Did we find a DError::doubleErr component

  // storage
  std::vector<double> X;
  DataTYPE Y;
  double x,y,e;
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
	      if (index==xcol && StrFunc::convert(Part,x))
		cnt++;
	      if (index==ycol)
	        {
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
		  X.push_back((xcol<0) ? dataCount : x);
		  if (deFlag)
		    Y.push_back(YE);
		  else if (ycol>=0 && ecol>=0)
		    Y.push_back(DError::doubleErr(y,e));
		  else if (ycol>=0)
		    Y.push_back(DError::doubleErr(y,0.0));
		  else if (ecol>=0)
		    Y.push_back(DError::doubleErr(0.0,e));
		  else
		    Y.push_back(DError::doubleErr(0.0,0.0));
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
  if (X.size()>=2)
    { 
      Y.pop_back();
      XCoord=X;
      Yvec=Y;
      return static_cast<int>(Y.size());
    }
  return 0;
}

DError::doubleErr
WorkData::integrate(const double xMin,const double xMax) const
  /*!
    Integrate the data from xMin to xMax : 
    Only additions are allowed so that the error term is
    correct.
    Uses fractional analysis.
    \param xMin :: Xmin value
    \param xMax :: Xmax value
    \return integrated value
  */
{
  ELog::RegMethod RegA("WorkData","integrate");
  if (xMin>xMax)
    throw ColErr::MisMatch<double>(xMin,xMax,RegA.getBase());

  DError::doubleErr sum(0,0);
  // Get first point
  size_t i;
  for(i=0;i<Yvec.size() && XCoord[i+1]<xMin;i++) ;
  if (i<Yvec.size())   
    {
      // Only one bin:
      if (XCoord[i+1]>xMax)
	return Yvec[i]*((xMax-xMin)/(XCoord[i+1]-XCoord[i]));
      // Multiple bins:
      double frac=(XCoord[i+1]-xMin)/(XCoord[i+1]-XCoord[i]);
      sum+=Yvec[i]*frac;
      for(i++;i<Yvec.size() && XCoord[i+1]<xMax;i++)
	sum+=Yvec[i];  

      if (i<Yvec.size() && xMax<XCoord[i+1])
	{
	  frac=(xMax-XCoord[i])/(XCoord[i+1]-XCoord[i]);
	  sum+=Yvec[i]*frac;
	}
    }
  return sum;
}

DError::doubleErr
WorkData::integrateX(const double xMin,const double xMax) const
  /*!
    Integrate the data from xMin to xMax : 
    Uses fractional analysis.
    \param xMin :: Xmin value
    \param xMax :: Xmax value
    \return integrated value
  */
{
  ELog::RegMethod RegA("WorkData","integrate");
  if (xMin>xMax)
    throw ColErr::MisMatch<double>(xMin,xMax,RegA.getBase());

  DError::doubleErr sum;
  // Get first point
  size_t i;
  for(i=0;i<Yvec.size() && XCoord[i+1]<xMin;i++) ;
  if (i!=Yvec.size()) 
    {
      // Only one bin:
      if (XCoord[i+1]>xMax)
	return Yvec[i]*(xMax-xMin);

      sum+=Yvec[i]*(XCoord[i+1]-xMin);
      for(i++;i<Yvec.size() && XCoord[i+1]<xMax;i++)
	sum+=Yvec[i]*(XCoord[i+1]-XCoord[i]);  

      if (i<Yvec.size() && xMax<XCoord[i+1])
	sum+=Yvec[i]*(xMax-XCoord[i]);
    }
  return sum;
}
WorkData&
WorkData::sort()
  /*!
    Sort the data based on the X value
    \return WorkData [after sort]
  */
{
  ELog::RegMethod RegA("WorkData","sort");
  
  if (XCoord.size()>=2)
    {
      std::vector<size_t> Index(XCoord.size());
      std::generate(Index.begin(),Index.end(),IncSeq());
      indexSort(XCoord,Index);
      std::vector<double> XItems;
      std::vector<DError::doubleErr> YItems;
      std::vector<size_t>::const_iterator vc;
      for(vc=Index.begin();vc!=Index.end();vc++)
	{
	  XItems.push_back(XCoord[*vc]);
	  if (*vc)
	    YItems.push_back(Yvec[*vc-1]);
	}
      XCoord=XItems;
      Yvec=YItems;
    }
  
  return *this;
}

void
WorkData::stream(std::ostream& OX) const
  /*!
    Writes out information about the DataMap
    \param OX :: output file 
  */
{
  boost::format FMT("%1$=12.6g%|16t|%2$=12.6g%|16t|%3$=12.6g");

  OX<<"# Pts "<<XCoord.size()<<std::endl;
  for(unsigned int i=0;i<Yvec.size();i++)
      OX<<(FMT % XCoord[i+1] % Yvec[i].getVal() % Yvec[i].getErr())<<std::endl;

  return;
 
}

void
WorkData::write(std::ostream& OX) const
  /*!
    Writes out information about the WorkData
    \param OX :: Output stream
  */
{
  if (XCoord.size()>=1)
    OX<<"Xrange:"<<XCoord.front()<<" "<<XCoord.back()
      <<" ("<<XCoord.size()<<")"<<std::endl;
  
  const size_t MP(this->getMaxPoint());
  if (MP==ULONG_MAX)
    OX<<"No data"<<std::endl;
  else
    OX<<"Max ("<<MP<<")[<<"<<
      XCoord[MP]<<"] == "<<Yvec[MP]<<std::endl;
  
  return; 
}


void
WorkData::write(const std::string& FName) const
  /*!
    Writes out information about the DataMap
    \param FName :: Filename 
  */
{
  std::ofstream OX(FName.c_str());
  stream(OX);
  OX.close();
  return;
 
}
