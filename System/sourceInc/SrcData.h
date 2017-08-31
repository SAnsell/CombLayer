/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/SrcData.h
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
#ifndef SDef_SrcData_h
#define SDef_SrcData_h

namespace SDef
{

  class SrcInfo;
  class SrcBias;
  class SrcProb;
  class DSBase;
  template<typename T> class DSTerm;

/*!
  \class SrcData
  \version 1.0
  \date July 2009
  \author S.Ansell
  \brief Comprehensive holder for sdef dx option
  
  This is a basic form (d1 d2). Not the FPOS etc
*/

class SrcData
{
 private:

  size_t Index;                      ///< Index number
  SrcInfo* SI;                    ///< SI card if present
  SrcBias* SB;                    ///< SB card if present
  SrcProb* SP;                    ///< SP card if present
  DSBase* DS;                     ///< DS card if present
  void renormalize();

 public:
  
  explicit SrcData(const size_t);
  SrcData(const SrcData&);
  SrcData& operator=(const SrcData&);
  virtual ~SrcData();
  
  /// Is active
  virtual bool isActive() const { return 1; }
  /// Access Index
  size_t getIndex() const { return Index; }
  /// Is dependent
  int isDependent() const { return (DS) ? 1 : 0; }
  std::string getDep() const;
  

  SrcInfo* getInfo();
  SrcBias* getBias();
  SrcProb* getProb();
  template<typename T> DSTerm<T>* getDS();
  
  void addUnit(const SrcInfo&);
  void addUnit(const SrcBias&);
  void addUnit(const SrcProb&);  
  void addUnit(const DSBase*);

  void cutValue(const double);
  virtual void write(std::ostream&) const;

};

/*!
  \class SrcInfo
  \version 2.0
  \author S. Ansell
  \brief SI info card
  \date July 2009
  
  Modified to be part of SrcData no inheritted from
*/

class SrcInfo 
{
 private:
  
  char option;                          ///< Option [H/L/S/A]
  std::vector<double> Values;           ///< Individual values
  std::vector<std::string> Str;         ///< Individual values
  
 public:
  
  SrcInfo();
  explicit SrcInfo(const char);
  SrcInfo(const SrcInfo&);
  SrcInfo& operator=(const SrcInfo&);
  ~SrcInfo() {}          ///< Destructor
 
  void setData(const std::vector<double>&);
  void setData(const std::vector<std::string>&);
  void addData(const double&);
  void addData(const std::string&);
  /// Access data
  std::vector<double>& getData() { return Values; }
  /// Access string
  std::vector<std::string>& getStr() { return Str; }
  /// Access option
  char getOption() const { return option; }

  void write(const size_t,std::ostream&) const;

};

/*!
  \class SrcProb
  \version 1.0
  \author S. Ansell
  \brief SP info card
  \date July 2009
*/

class SrcProb 
{
 private:
  
  static int MFcount(const int); 

  char option;                        ///< Option [D/C/V]
  int minusF;                         ///< Minus F option
  std::vector<double> Values;         ///< Individual values

 public:
  
  SrcProb();
  explicit SrcProb(const char);
  SrcProb(const SrcProb&);
  SrcProb& operator=(const SrcProb&);
  ~SrcProb() {}          ///< Destructor
 
  void setFminus(const int,const double,const double);
  void setFminus(const int,const double);
  void setData(const std::vector<double>&);
  void addData(const double&);
  /// Access data
  int getFMinus() const { return minusF; }
  /// Access the data values
  std::vector<double>& getData() { return Values; }

  void write(const size_t,std::ostream&) const;

};


/*!
  \class SrcBias
  \version 1.0
  \author S. Ansell
  \brief SB info card
  \date August 2010
*/

class SrcBias 
{
 private:
 
  char option;                        ///< Option on output
  int minusF;                         ///< Fminus values
  std::vector<double> Values;         ///< Individual values
  
 public:
  
  explicit SrcBias();
  explicit SrcBias(const char);
  SrcBias(const SrcBias&);
  SrcBias& operator=(const SrcBias&);
  ~SrcBias() {}          ///< Destructor

  
  void setData(const std::vector<double>&);
  void addData(const double&);
  /// Access data
  std::vector<double>& getData() { return Values; }
  /// Access FMinus
  int getFMinus() const { return minusF; }
  
  void setFminus(const int,const double,const double);
  void write(const size_t,std::ostream&) const;

};

}

#endif
