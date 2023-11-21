/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/NGroup.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef RangeUnit_NGroup_h
#define RangeUnit_NGroup_h

namespace RangeUnit
{
  /*!
    \class RUnit 
    \tparam T :: double/int storage type
    \brief 
  */

template<typename T>
bool identVal(const double,const T&,const T&);
template<typename T>
bool intervalVal(const double,const T&,const T&,const T&);
template<typename T>
bool logIntVal(const double,const T&,const T&,const T&);

template<typename T>
class RUnit
{
protected:

  bool isFirst;   ///< Flag if first
  
public:

  RUnit() : isFirst(0) {}
  virtual RUnit* clone() const =0;
  virtual ~RUnit()  {}

  /// flag setter
  void setFirstFlag(const bool F) { isFirst=F; }
  
  virtual T getItem(const int) const = 0;
  virtual int getSize() const =0;
  virtual void writeVector(std::vector<T>&) const =0;
  virtual void write(std::ostream&) const =0;
};

template<typename T>  
class  RSingle : public RUnit<T>
{
private:

  T value;  ///< Single value

public:
  
  RSingle(const T&);
  RSingle(const RSingle<T>&);
  RSingle& operator=(const RSingle<T>&);
  RSingle* clone() const override { return new RSingle(*this); }
  ~RSingle() override {}

  T getItem(const int) const override { return value; } 
  int getSize() const override { return 1; }
  void writeVector(std::vector<T>&) const override;
  void write(std::ostream&) const override;
};

/*!
  \class RRepeat
  \brief Repeated unit
*/
  
template<typename T>
class  RRepeat : public RUnit<T>
{
private:
  
  T value;          ///< Value 
  int count;        ///< Full index count [-1 on output]

public:
  
  RRepeat(const T&,const int);
  RRepeat(const RRepeat<T>&);
  RRepeat<T>& operator=(const RRepeat<T>&);
  RRepeat* clone() const override { return new RRepeat(*this); }
  ~RRepeat() override {}

  T getItem(const int) const override { return value; } 
  int getSize() const override { return count+RUnit<T>::isFirst; }
  void writeVector(std::vector<T>&) const override;
  void write(std::ostream&) const override;
};

template<typename T>
class  RInterval : public RUnit<T>
{
private:
  
  T aValue;
  T bValue;
  int count;

public:
  
  RInterval(const T&,const T&,const int);
  RInterval(const RInterval<T>&);
  RInterval<T>& operator=(const RInterval<T>&);
  RInterval* clone() const override { return new RInterval(*this); }
  ~RInterval() override {}

  T getItem(const int) const override;
  int getSize() const override { return count+RUnit<T>::isFirst; }
  void writeVector(std::vector<T>&) const override;
  void write(std::ostream&) const override ;
};

template<typename T>
class  RLog : public RUnit<T>
{
private:
  
  T aValue;
  T bValue;
  int count;

public:
  
  RLog(const T&,const T&,const int);
  RLog(const RLog<T>&);
  RLog<T>& operator=(const RLog<T>&);
  RLog* clone() const override { return new RLog(*this); }
  ~RLog() override {}

  T getItem(const int) const override;
  int getSize() const override { return count+RUnit<T>::isFirst; }
  void writeVector(std::vector<T>&) const override;
  void write(std::ostream&) const override;
};

/*!
  \class NGroup
  \brief Ranges of intervals for cell output
  \author S. Ansell
  \date December 2020
  \version 1.0
*/

template<typename T> 
class NGroup
{
 private:
  
  std::list< RangeUnit::RUnit<T>* > Items;
  
  void clearItems();
  
 public:

  NGroup();
  NGroup(const NGroup<T>&);  
  NGroup<T>& operator=(const NGroup<T>&);
  ~NGroup();

  T operator[](const int) const;
  NGroup<T>& operator+=(const NGroup<T>&);

  bool empty() const { return Items.empty(); }      ///< Range is stored
  void clear(); 
  int count() const;

  int processString(const std::string&);     

  void condense(const double);
  void condense(const double,const std::vector<T>&);     

  void writeVector(std::vector<T>&) const;
  
  void write(std::ostream&) const;      

};

template<typename T>
std::ostream&
operator<<(std::ostream&,const RUnit<T>&);

template<typename T>
std::ostream&
operator<<(std::ostream&,const NGroup<T>&);


}
#endif
