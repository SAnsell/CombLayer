/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   workInc/WorkData.h
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
#ifndef WorkData_h
#define WorkData_h

/*!
  \class WorkData
  \brief Base class x-y data
  \version 1.1
  \date July 2010
  \author S. Ansell
 
  Holds a list of all the spectra in a flat array.
  The class is a modified DataLine from LoqNSwig

*/

class WorkData
{
 private:
 
  /// Storeage Type
  typedef std::vector<DError::doubleErr> DataTYPE;

  double weight;                   ///< Scale/weight factor [for error]
  std::vector<double> XCoord;      ///< Xvalues : boundary values (Yvec.size+1)
  DataTYPE Yvec;                   ///< Yvalues 

  WorkData& addFactor(const WorkData&,const double);
  int selectColumn(std::istream&,const int,const int,const int,const int);

 public:
 
  WorkData();
  WorkData(const WorkData&);
  WorkData& operator=(const WorkData&);
  virtual ~WorkData();

  /// Effective typeid 
  virtual std::string className() const { return "WorkData"; }
  
  WorkData& operator+=(const WorkData&);
  WorkData& operator-=(const WorkData&);
  WorkData& operator*=(const WorkData&);
  WorkData& operator/=(const WorkData&);
  WorkData& operator+=(const double);
  WorkData& operator-=(const double);
  WorkData& operator*=(const double);
  WorkData& operator/=(const double);

  WorkData operator+(const double) const;
  WorkData operator-(const double) const;
  WorkData operator*(const double) const;
  WorkData operator/(const double) const;
  WorkData operator+(const WorkData&) const;
  WorkData operator-(const WorkData&) const;
  WorkData operator*(const WorkData&) const;
  WorkData operator/(const WorkData&) const;

  WorkData& rebin(const std::vector<double>&);
  WorkData& rebin(const WorkData&);
  WorkData& sort();
  WorkData& binDivide(const double);
  WorkData& xScale(const double);
  double XInverse(const double) const;

  /// set weight
  void setWeight(const double W) { weight=W; }
  void setSize(const size_t);
  void setX(const std::vector<double>&);
  void setX(const size_t,const double);
  void setLogX(const double,const double,const size_t);

  void setData(const size_t,const double,const double);
  void setData(const size_t,const double,const double,const double);
  void setData(const std::vector<double>&,const std::vector<double>&);
  void setData(const std::vector<double>&,const std::vector<double>&,
	       const std::vector<double>&);
  void setData(const std::vector<double>&,
	       const std::vector<DError::doubleErr>&);
  void setData(const std::vector<DError::doubleErr>&,
	       const std::vector<DError::doubleErr>&);

  /// X-Accessor 
  const std::vector<double>& getXdata() const { return XCoord; } 
  /// Data-Accessor 
  const std::vector<DError::doubleErr>& getYdata() const 
    { return Yvec; }

  /// Size accessor
  size_t getSize() const { return Yvec.size(); }   
  /// Access the weight
  double getWeight() const { return weight; }   
  size_t getXIndex(const double) const;
  size_t getIndex(const double,const double) const;
  size_t getMaxPoint() const;

  void initX(const double);
  void pushData(const double,const double);
  void pushData(const DError::doubleErr&);
  void pushData(const double,const DError::doubleErr&);

  void addPoint(const double,const double);
  void addPoint(const double,const DError::doubleErr&);

  void normalize(const size_t);
  
  int load(const std::string&,const int,const int,const int=0);

  DError::doubleErr integrate(const double,const double) const;
  DError::doubleErr integrateX(const double,const double) const;

  void stream(std::ostream&) const;
  void write(std::ostream&) const;
  void write(const std::string&) const;

};
 
#endif
