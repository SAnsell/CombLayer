/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   testInclude/testBoost.h
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
#ifndef testBoost_h
#define testBoost_h 

/*!
  \class Report
  \brief Support class for sharePts
  \version 1.0
  \date February 2006
  \author S.Ansell
  
  This class reports that is it destroyed and constructed.
*/

class Report
{
 private:

  static int cnt;       ///< count of the class
  static int Dcnt;      ///< count of the class destructors
  int num;              ///< parameter identifier of the class

 public:

  Report();
  Report(const int);
  Report(const size_t);
  Report(const Report&);
  Report& operator=(const Report&);
  /// Virtual copy constuctor
  virtual Report* clone() const  { return new Report(*this); }
  virtual ~Report();

  /// access stats to avoid writing
  virtual Triple<int> getStatus() const 
    { return Triple<int>(cnt,Dcnt,num); }

  int getNum() const { return num; }  ///< Return the identifier
  void setNum(const int);
  void setNum(const size_t);
  virtual void write(std::ostream&) const;

};

/*!
  \class ReportRef
  \version 1.0
  \date May 2006
  \author S. Ansell
  \brief Test class that requires a constant 
  reference 

  This class is extended from Report and requires a
  constant reference. This was to test the case when
  a default constructor could not be used.
*/

class ReportRef : public Report
{
 private:

  const double&  Param;  ///< Parametr

 public:

  ReportRef(const double&);
  ReportRef(const double&,const int);
  ReportRef(const ReportRef&);
  ReportRef& operator=(const ReportRef&);
  /// Virtual copy constructor
  virtual ReportRef* clone() const  { return new ReportRef(*this); }
  virtual ~ReportRef();

  double getParam() const { return Param; }  ///< value of param
  virtual void write(std::ostream&) const;

};

std::ostream&
operator<<(std::ostream&,const Report&);

/*!
  \class testBoost
  \brief test of Boost Share_Ptr and Share_Array
  \version 1.0
  \date February 2006
  \author S.Ansell
  
  This also needs a support class to report construction etc.
*/

class testBoost
{
private:

  //Tests 

  int testRepPtr();
  int testRepRefPtr();  
  int testSharePtr();
  int testSharePtrMap();
  int testVecPtr();
  int testVecRef();

public:

  testBoost();
  ~testBoost();

  int applyTest(const int);  
};


#endif


