/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   workInc/BinData.h
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
#ifndef BinData_h
#define BinData_h

/*!
  \class BinData
  \brief Base class x-y data
  \version 1.1
  \date July 2010
  \author S. Ansell
 
  Holds a list of all the spectra in a flat array.
  The class is a modified DataLine from LoqNSwig

*/

class BinData
{

 public:

  /// Storeage Type
  typedef std::vector<BUnit> DataTYPE;

 private:
 

  DataTYPE Yvec;                   ///< Yvalues 

  BinData& addFactor(const BinData&,const double);
  int selectColumn(std::istream&,const int,const int,const int,
		   const int,const int);

 public:
 
  BinData();
  BinData(const BinData&);
  BinData& operator=(const BinData&);
  virtual ~BinData();
  /// Effective typeid
  virtual std::string className() const { return "BinData"; }
  
  BinData& operator+=(const BinData&);
  BinData& operator-=(const BinData&);
  BinData& operator*=(const BinData&);
  BinData& operator/=(const BinData&);
  BinData& operator+=(const double);
  BinData& operator-=(const double);
  BinData& operator*=(const double);
  BinData& operator/=(const double);

  BinData operator+(const double) const;
  BinData operator-(const double) const;
  BinData operator*(const double) const;
  BinData operator/(const double) const;
  BinData operator+(const BinData&) const;
  BinData operator-(const BinData&) const;
  BinData operator*(const BinData&) const;
  BinData operator/(const BinData&) const;

  BinData& rebin(const std::vector<BUnit>&);
  BinData& rebin(const BinData&);
  BinData& binDivide(const double);
  BinData& xScale(const double);

  void clear();
  void addData(const double&,const double&,
	       const double&,const DError::doubleErr&);
  void addData(const double&,const double&,const DError::doubleErr&);

  /// Data-Accessor 
  const std::vector<BUnit>& getData() const 
    { return Yvec; }

  /// Size accessor
  size_t getSize() const { return Yvec.size(); }   
  size_t getIndex(const double,const double) const;
  size_t getMaxPoint() const;

  /// Determine if the workspace contains data
  bool isEmpty() const { return Yvec.empty(); } 
  int load(const std::string&,const int,const int,const int,const int=0);

  DError::doubleErr integrate(const double,const double) const;
  void normalize();

  void write(std::ostream&) const;
  void write(const std::string&) const;

};
 
#endif
