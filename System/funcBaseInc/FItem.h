/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   funcBaseInc/FItem.h
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
#ifndef FItem_h
#define FItem_h

/*!
  \class FItem
  \brief Variable base item
  \author S. Ansell 
  \date April 2006
  \version 1.0
  Holds the index point (which needs to be unique)

*/

class FItem
{
 private:
  
  int index;          ///< Identifier

 protected:

  int active;         ///< Active [read] 
  varList* VListPtr;  ///< Current varList item
  
  
 public:

  FItem(varList*,const int);
  FItem(const FItem&);
  FItem& operator=(const FItem&);
  virtual FItem* clone() const=0;          ///< Abstract clone 
  virtual ~FItem();

  int getIndex() const { return index; }   ///< Return ID
  void setIndex(const int A) { index=A; }  ///< Set ID
  void setVList(varList* VA) { VListPtr=VA; }  ///< Set varList pointer

  virtual void setValue(const Geometry::Vec3D&);
  virtual void setValue(const double&);
  virtual void setValue(const int&);
  virtual void setValue(const size_t&);
  virtual void setValue(const long int&);
  virtual void setValue(const std::string&);
  virtual void setValue(const Code&);

  /// Accessor to active
  int isActive() const { return active; }
  /// reset active
  void resetActive() { active=0; }
  ///\cond ABSTRACT

  virtual int getValue(Geometry::Vec3D&) const= 0;
  virtual int getValue(size_t&) const= 0;
  virtual int getValue(int&) const= 0; 
  virtual int getValue(long int&) const= 0; 
  virtual int getValue(double&) const= 0;
  virtual int getValue(std::string&) const= 0; 
  
  virtual std::string typeKey() const =0;
  virtual void write(std::ostream&) const=0;  
  ///\endcond ABSTRACT
};

/*!
  \class FValue 
  \brief Single type numbe
  \author S. Ansell 
  \date April 2006
  \version 1.0
*/
template<typename T>
class FValue : public FItem
{
 private:

  T Value;             ///< Value of an item (fixed)

 public:

  typedef T type;          ///< Storage type 

  FValue(varList*,const int,const T&);
  FValue(const FValue<T>&);
  FValue<T>& operator=(const FValue<T>&);
  virtual FValue<T>* clone() const;
  virtual ~FValue();

  virtual void setValue(const Geometry::Vec3D&);
  virtual void setValue(const double&);
  virtual void setValue(const size_t&); 
  virtual void setValue(const int&);
  virtual void setValue(const long int&);
  virtual void setValue(const std::string&);
  
  virtual int getValue(Geometry::Vec3D&) const;
  virtual int getValue(int&) const;     
  virtual int getValue(long int&) const;     
  virtual int getValue(size_t&) const;     
  virtual int getValue(double&) const;     
  virtual int getValue(std::string&) const;

  virtual std::string typeKey() const;
  void write(std::ostream&) const;  
};


/*!
  \class FFunc
  \brief Holds a parser object 
  \author S. Ansell 
  \date April 2006
  \version 1.0
  Holds just the code item of the parser (the only bit that
  is really needed)
*/

class FFunc : public FItem
{
 private:

  Code BaseUnit;    ///< Code unit of a compile Function

 public:

  FFunc(varList*,const int,const Code&);
  FFunc(const FFunc&);
  FFunc& operator=(const FFunc&);
  virtual FFunc* clone() const;
  virtual ~FFunc();

  void setValue(const Code&);

  virtual int getValue(Geometry::Vec3D&) const;  
  virtual int getValue(int&) const;     
  virtual int getValue(long int&) const;     
  virtual int getValue(size_t&) const;     
  virtual int getValue(double&) const;     
  virtual int getValue(std::string&) const;

  virtual std::string typeKey() const;
  void write(std::ostream&) const;

};



#endif
