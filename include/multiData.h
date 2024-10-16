/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/multiDAta.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#ifndef multiData_h
#define multiData_h

/*!
  \class multiData
   \version 2.0
   \author Stuart Ansell
   \date May 2023
   \brief multiData code
   
   Note: based on:
   https://stackoverflow.com/questions/74031279/
   generic-slicing-views-of-multidimensional-array-in-c20-using-ranges

   Has been improved for better performance by allowing map/integration
   and axis reduction.

   It is an effective a replacement for boost::multi_array. But
   is faster / and with sensible copy semantics e.g. A=B works for
   ALL sizes and means that A is a copy of B. Also a multiData can be
   resizes to new number of axis e.g. going from A[][][] to A[][] and
   allows exchange of axis: e.g. A[i][j][k] -> exchange(0,1) -> A[j][i][k]
*/

template<typename T>
class multiData
{
 private:

  std::vector<size_t> index;     ///< Index sizes of array
  std::vector<size_t> strides;   ///< strides through data [+1 size for zero]
  std::vector<T> flatData;       ///< Data vector

  void getRangeSumImpl(T&,
		    const std::vector<sRange>&,
		    const size_t,const size_t) const; 

  void getRangeImpl(std::vector<T>&,size_t&,
		    const std::vector<sRange>&,
		    const size_t,const size_t) const; 

  void throwMatchCheck(const multiData<T>&,const std::string&) const;
  
 public:

  explicit multiData();
  multiData(std::vector<size_t>,std::vector<T>);
  explicit multiData(std::vector<size_t>);
  explicit multiData(const size_t);

  multiData(const size_t,const size_t);
  multiData(const size_t,const size_t,const size_t);
  multiData(const size_t,const size_t,const size_t,const size_t);

  multiData(const size_t,std::vector<T>);
  multiData(const size_t,const size_t,std::vector<T>);
  multiData(const size_t,const size_t,const size_t,std::vector<T>);

  multiData(const std::vector<std::vector<T>>&);
  multiData(const std::vector<std::vector<std::vector<T>>>&);

  template<typename U>
  multiData(const size_t,const std::vector<U>&);

  multiData(const multiData&);
  multiData(multiData&&);
  multiData& operator=(const multiData&);
  multiData& operator=(multiData&&);
  template<typename U> multiData(const multiData<U>&);
  ~multiData();

  void clear(); 
  
  multiData<T>& operator+=(const T&);
  multiData<T>& operator-=(const T&);
  multiData<T>& operator*=(const T&);
  multiData<T>& operator/=(const T&);
  multiData<T>& operator+=(const multiData<T>&);
  multiData<T>& operator-=(const multiData<T>&);
  multiData<T>& operator*=(const multiData<T>&);
  multiData<T>& operator/=(const multiData<T>&);

  bool isEmpty() const { return flatData.empty(); } 
  size_t getDim() const { return index.size(); }
  size_t size() const { return flatData.size(); }

  size_t getRangeSize(std::vector<sRange>&) const;

  void resize(std::vector<long long unsigned int>);
  void resize(std::vector<size_t>);
  // special resize for 4D
  void resize(const size_t,const size_t,const size_t,const size_t);
  // special resize for 3D
  void resize(const size_t,const size_t,const size_t);
  // special resize for 2D
  void resize(const size_t,const size_t);

  
  void combine(const size_t,const size_t);
  
  void setData(std::vector<T>);
  void setData(const size_t,const size_t,std::vector<T>);
  void setData(const size_t,const size_t,const size_t,std::vector<T>);

  std::vector<T> getFlatRange(std::vector<sRange>) const;
  T getFlatIntegration(std::vector<sRange>) const;
  std::vector<T> getAxisRange(const size_t,const size_t) const;
  std::vector<T> getAxisIntegral(const size_t,sRange sUnit={}) const;
  std::vector<T> getAxisProjection(const size_t) const;
  multiData<T> getRange(std::vector<sRange>) const;
  multiData<T> reduceMap(const size_t) const;
  multiData<T> projectMap(const size_t,std::vector<sRange>) const;
  multiData<T> integrateMap(size_t,std::vector<sRange>) const;
  T integrateValue(std::vector<sRange>) const;

  void setSubMap(const size_t,const size_t,const multiData<T>&);
  
  void fill(const T&);
  multiData<T> exchange(size_t,size_t) const;
  
  sliceUnit<T> get() { return sliceUnit<T>(flatData.data(), strides.data()); }
  sliceUnit<const T> get() const
    { return sliceUnit<const T>(flatData.data(), strides.data()); }

  T& value(const std::vector<size_t>&);
  const T& value(const std::vector<size_t>&) const;
  
  /// accessor to pointer
  T* getPtr() { return flatData.data(); }
  const T* getPtr() const { return flatData.data(); }
  
  /// accessor to vector
  const std::vector<T>& getVector() const
     { return flatData; }
  /// accessor to vector
  std::vector<T>& getVector() { return flatData; }

  const std::vector<size_t>& shape() const
    { return index; }
  const std::vector<size_t>& sVec() const
  { return strides; }
  std::vector<size_t> reduceAxis(const size_t) const;
  
  size_t offset(const size_t,const size_t) const;
  size_t offset(const size_t,const size_t,const size_t) const;
  size_t offset(const std::vector<size_t>&) const;
  
  using iterator=typename std::vector<T>::iterator;
  using const_iterator=typename std::vector<T>::const_iterator;
  
  iterator begin() { return flatData.begin(); }
  iterator end() { return flatData.end(); }
  const_iterator begin() const { return flatData.begin(); }
  const_iterator end() const { return flatData.end(); }

  void write(std::ostream&) const;
  std::string simpleStr() const;
};

template<typename T> 
std::ostream&
operator<<(std::ostream&,const multiData<T>&);

#endif
