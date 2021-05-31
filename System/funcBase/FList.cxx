/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   funcBase/FList.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <climits>
#include <vector>
#include <map>
#include <typeinfo>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "Vec3D.h"
#include "Code.h"
#include "FItem.h"

//  ---------------------------------------
//                FVALUE
//  ---------------------------------------


template<typename T>
FList<T>::FList(varList* VA,const int I,const T& V) :
  FItem(VA,I),
  Vec({V})
  /*!
    Constructor 
    \param VA :: VarList pointer
    \param I :: Index value
    \param V :: First Vec to set
  */
{}

template<typename T> 
FList<T>::FList(const FList<T>& A) : 
  FItem(A),Vec(A.Vec)
  /*!
    Standard copy constructor
    \param A :: FList object to copy
  */
{}

template<typename T> 
FList<T>&
FList<T>::operator=(const FList<T>& A) 
  /*!
    Standard assignment operator
    \param A :: FList object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      FItem::operator=(A);
      Vec=A.Vec;
    }
  return *this;
}

template<typename T>
FList<T>*
FList<T>::clone() const
  /*!
    Clone class
    \return new Vec(*this)
  */
{
  return new FList<T>(*this);
}

template<typename T>
FList<T>::~FList()
  /*!
    Desturctor
  */
{}

// -----------------------------------------
//              SET VALUE
// -----------------------------------------


template<typename T>
void
FList<T>::setValue(const Geometry::Vec3D& V)
  /*!
    Sets the values 
    \param V :: Value to set
  */
{
  if constexpr (std::is_same<Geometry::Vec3D,T>::value)
     Vec=std::vector<T>({V});
  else
    throw ColErr::TypeMatch("Geometry::Vec3D",
			    typeid(T).name(),"FList::SetValue");
  return;
}

template<typename T>
void
FList<T>::setValue(const double& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  if constexpr( std::is_same<double,T>::value )
     Vec=std::vector<T>({V});
  else
    throw ColErr::TypeMatch("double",typeid(T).name(),
			    "FList::SetValue");

  return;
}

template<typename T>
void
FList<T>::setValue(const int& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  if constexpr( std::is_same<int,T>::value ||
		std::is_same<double,T>::value ||
		std::is_same<size_t,T>::value ||
		std::is_same<long int,T>::value)
     Vec=std::vector<T>({static_cast<T>(V)});
  else
    throw ColErr::TypeMatch("int",typeid(T).name(),
			    "FList::SetValue");

  return;
}

template<typename T>
void
FList<T>::setValue(const long int& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  if constexpr( !std::is_same<int,T>::value &&
		!std::is_same<double,T>::value &&
		!std::is_same<size_t,T>::value &&
		!std::is_same<long int,T>::value)
    {
      throw ColErr::TypeMatch("size_t",typeid(T).name(),
			    "FList::SetValue"+std::to_string(V));
    }

  else
    Vec=std::vector<T>({static_cast<T>(V)});

  return;
}


template<typename T>
void
FList<T>::setValue(const size_t& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  if constexpr( !std::is_same<int,T>::value &&
		!std::is_same<double,T>::value &&
		!std::is_same<size_t,T>::value &&
		!std::is_same<long int,T>::value)
    {
      throw ColErr::TypeMatch("size_t",typeid(T).name(),
			      "V="+std::to_string(V));
    }
  else
    Vec=std::vector<T>({static_cast<T>(V)});
  return;
}

template<typename T>
void
FList<T>::setValue(const std::string& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  ELog::RegMethod RegA("FList","setVec(string)");
  

  if constexpr( std::is_same<std::string,T>::value )
    {		
      Vec=std::vector<T>({V});
      return;
    }

  T Value;
  if (!StrFunc::convert(V,Value))
    throw ColErr::TypeMatch(V,typeid(T).name(),"FList::setValue");
  
  
  Vec=std::vector<T>({Value});  
  return;
}

template<>
void
FList<std::string>::setValue(const double& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  std::stringstream cx;
  cx<<V;
  Vec=std::vector<std::string>({cx.str()});
  return;
}

template<>
void
FList<std::string>::setValue(const int& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  std::stringstream cx;
  cx<<V;
  Vec=std::vector<std::string>({cx.str()});
  return;
}

template<>
void
FList<std::string>::setValue(const long int& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  std::stringstream cx;
  cx<<V;
  Vec=std::vector<std::string>({cx.str()});
  return;
}

template<>
void
FList<std::string>::setValue(const size_t& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  std::stringstream cx;
  cx<<V;
  Vec=std::vector<std::string>({cx.str()});
  return;
}

template<>
void
FList<std::string>::setValue(const std::string& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  Vec=std::vector<std::string>({V});
  return;
}

template<>
void
FList<Geometry::Vec3D>::setValue(const std::string& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  ELog::RegMethod RegA("FList<Vec3D>","setVec(string)");

  std::string X=V;
  Geometry::Vec3D A;
  if (!StrFunc::section(X,A[0]) ||
      !StrFunc::section(X,A[1]) ||
      !StrFunc::section(X,A[2]))
    throw ColErr::InvalidLine(V,"String to Vec3D",0);

  Vec=std::vector<Geometry::Vec3D>({A});
  return;
}


// -----------------------------------------
//              PUSH VALUE
// -----------------------------------------

template<typename T>
void
FList<T>::pushValue(const T& Value)
  /*!
    Simple back push of value 
    \param Value :: Value type
   */
{
  Vec.push_back(Value);
  return;
}


// -----------------------------------------
//              GET VECTOR
// -----------------------------------------


template<typename T>
int
FList<T>::getVector(std::vector<Geometry::Vec3D>& V) const
  /*!
    Gets the value 
    \param V :: Return place
    \return 1 if valid / 0 on fail
  */
{
  V.clear();
  if constexpr (std::is_same<Geometry::Vec3D,T>::value)
    {
      V=Vec;
      const_cast<int&>(active)++;
      return 1;
    }

  return 0;
}

template<typename T>
int
FList<T>::getVector(std::vector<double>& V) const
  /*!
    Sets the values 
    \param V :: New value 
    \return 1 (always possible)
  */
{
  V.clear(); 
  if constexpr (std::is_same<double,T>::value ||
		std::is_same<long int,T>::value ||
		std::is_same<size_t,T>::value ||
		std::is_same<int,T>::value)
    {
      for(const T& VItem : Vec)
	V.push_back(static_cast<double>(VItem));
      const_cast<int&>(active)++;
      return 1;
    }
  if constexpr (std::is_same<std::string,T>::value)
    {
      double DItem;
      for(const std::string& VItem : Vec)
	{
	  if (!StrFunc::convert(VItem,DItem))      
	    return 0; 
	  V.push_back(DItem);
	}
      
      const_cast<int&>(active)++;
      return 1;
    }
  return 0;
}

template<typename T>
int
FList<T>::getVector(std::vector<int>& V) const
  /*!
    Sets the values 
    \param V :: New value 
    \return 1 (always possible)
  */
{
  V.clear();

  if constexpr (std::is_same<double,T>::value ||
		std::is_same<long int,T>::value ||
		std::is_same<size_t,T>::value ||
		std::is_same<int,T>::value)
    {
      for(const T& VItem : Vec)
	V.push_back(static_cast<int>(VItem));
      const_cast<int&>(active)++;
      return 1;
    }
  
  if constexpr (std::is_same<std::string,T>::value)
    {
      int DItem;
      for(const std::string& VItem : Vec)
	{
	  if (!StrFunc::convert(VItem,DItem))      
	    return 0; 
	  V.push_back(DItem);
	}
      
      const_cast<int&>(active)++;
      return 1;
    }
  return 0;
}

template<typename T>
int
FList<T>::getVector(std::vector<long int>& V) const
  /*!
    Sets the values 
    \param V :: New value 
    \return 1 (always possible)
  */
{
  V.clear();
      
  if constexpr (std::is_same<double,T>::value ||
		std::is_same<long int,T>::value ||
		std::is_same<size_t,T>::value ||
		std::is_same<int,T>::value)
    {
      for(const T& VItem : Vec)
	V.push_back(static_cast<long int>(VItem));
      const_cast<int&>(active)++;
      return 1;
    }
  
  if constexpr (std::is_same<std::string,T>::value)
    {
      long int DItem;
      for(const std::string& VItem : Vec)
	{
	  if (!StrFunc::convert(VItem,DItem))      
	    return 0; 
	  V.push_back(DItem);
	}
      
      const_cast<int&>(active)++;
      return 1;
    }
  return 0;
}

template<typename T>
int
FList<T>::getVector(std::vector<size_t>& V) const
  /*!
    Sets the values 
    \param V :: New value 
    \return 1 (always possible)
  */
{
  V.clear();
      
  if constexpr (std::is_same<double,T>::value ||
		std::is_same<long int,T>::value ||
		std::is_same<size_t,T>::value ||
		std::is_same<int,T>::value)
    {
      for(const T& VItem : Vec)
	V.push_back(static_cast<size_t>(VItem));
      const_cast<int&>(active)++;
      return 1;
    }
  
  if constexpr (std::is_same<std::string,T>::value)
    {
      size_t DItem;
      for(const std::string& VItem : Vec)
	{
	  if (!StrFunc::convert(VItem,DItem))      
	    return 0; 
	  V.push_back(DItem);
	}
      
      const_cast<int&>(active)++;
      return 1;
    }
  return 0;
}

template<typename T>
int
FList<T>::getVector(std::vector<std::string>& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 0 if not possbile  (1 if valid)
  */
{

  V.clear();
  for(const T& VItem : Vec)
    {
      std::stringstream cx;
      cx<<VItem;
      V.push_back(cx.str());
    }
  const_cast<int&>(active)++;
  return 1;
}

template<>
int
FList<std::string>::getVector(std::vector<Geometry::Vec3D>& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 0 if not possbile  (1 if valid)
  */
{
  ELog::RegMethod RegA("FList","getVector(Vec3D)");

  V.clear();
  for(const std::string& VItem : Vec)
    {
      Geometry::Vec3D DItem;
      if (!StrFunc::convert(VItem,DItem))
	return 0;
      V.push_back(DItem);
    }
  const_cast<int&>(active)++;
  return 1;
}



// -----------------------------------------
//              GET VALUE
// -----------------------------------------


template<typename T>
int
FList<T>::getValue(Geometry::Vec3D& V) const
  /*!
    Gets the value 
    \param V :: Return place
    \return 1 if valid / 0 on fail
  */
{
  if constexpr (std::is_same<Geometry::Vec3D,T>::value)
    {
      if (!Vec.empty())
	{
	  V=Vec.front();
	  const_cast<int&>(active)++;
	  return 1;
	}
    }
  return 0;
}

template<typename T>
int
FList<T>::getValue(double& V) const
  /*!
    Sets the values 
    \param V :: New value 
    \return 1 (always possible)
  */
{
 
  if constexpr (std::is_same<double,T>::value ||
		std::is_same<long int,T>::value ||
		std::is_same<size_t,T>::value ||
		std::is_same<int,T>::value)
    {
      if (!Vec.empty())
	{
	  V=static_cast<double>(Vec.front());
	  const_cast<int&>(active)++;
	  return 1;
	}
    }
  if constexpr (std::is_same<std::string,T>::value)
    {
      if (!Vec.empty() &&
	  StrFunc::convert(Vec.front(),V))
	{
	  const_cast<int&>(active)++;
	  return 1;
	}
    }
  V=0.0;
  return 0;
}

template<typename T>
int
FList<T>::getValue(int& V) const
  /*!
    Sets the values 
    \param V :: New value 
    \return 1 (always possible)
  */
{
 
  if constexpr (std::is_same<double,T>::value ||
		std::is_same<long int,T>::value ||
		std::is_same<size_t,T>::value ||
		std::is_same<int,T>::value)
    {
      if (!Vec.empty())
	{
	  V=static_cast<int>(Vec.front());
	  const_cast<int&>(active)++;
	  return 1;
	}
    }
  if constexpr (std::is_same<std::string,T>::value)
    {
      if (!Vec.empty() &&
	  StrFunc::convert(Vec.front(),V))
	{
	  const_cast<int&>(active)++;
	  return 1;
	}
    }
  V=0;
  return 0;
}

template<typename T>
int
FList<T>::getValue(long int& V) const
  /*!
    Sets the values 
    \param V :: New value 
    \return 1 (always possible)
  */
{
 
  if constexpr (std::is_same<double,T>::value ||
		std::is_same<long int,T>::value ||
		std::is_same<size_t,T>::value ||
		std::is_same<int,T>::value)
    {
      if (!Vec.empty())
	{
	  V=static_cast<long int>(Vec.front());
	  const_cast<int&>(active)++;
	  return 1;
	}
    }
  if constexpr (std::is_same<std::string,T>::value)
    {
      if (!Vec.empty() &&
	  StrFunc::convert(Vec.front(),V))
	{
	  const_cast<int&>(active)++;
	  return 1;
	}
    }
  V=0;
  return 0;
}

template<typename T>
int
FList<T>::getValue(size_t& V) const
  /*!
    Sets the values 
    \param V :: New value 
    \return 1 (always possible)
  */
{
 
  if constexpr (std::is_same<double,T>::value ||
		std::is_same<long int,T>::value ||
		std::is_same<size_t,T>::value ||
		std::is_same<int,T>::value)
    {
      if (!Vec.empty())
	{
	  V=(Vec.front()<0) ? ULONG_MAX  :
	    static_cast<size_t>(Vec.front());
	  const_cast<int&>(active)++;
	  return 1;
	}
    }
  if constexpr (std::is_same<std::string,T>::value)
    {
      if (!Vec.empty() &&
	  StrFunc::convert(Vec.front(),V))
	{
	  const_cast<int&>(active)++;
	  return 1;
	}
    }
  V=0;
  return 0;
}


template<typename T>
int
FList<T>::getValue(std::string& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 0 if not possbile  (1 if valid)
  */
{
  std::stringstream cx;
  if (!Vec.empty())
    {
      cx<<Vec.front();
      V=cx.str();
      const_cast<int&>(active)++;
      return 1;
    }
  return 0;
}

template<>
int
FList<std::string>::getValue(Geometry::Vec3D& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 0 if not possbile  (1 if valid)
  */
{
  ELog::RegMethod RegA("FList","getVec(Vec3D)");

  if (Vec.empty() ||
      !StrFunc::convert(Vec.front(),V))
    return 0;
  
  const_cast<int&>(active)++;
  return 1;
}

template<>
int
FList<std::string>::getValue(double& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 0 if not possbile  (1 if valid)
  */
{
  ELog::RegMethod RegA("FList","getVec(double)");

  if (Vec.empty() || !StrFunc::convert(Vec.front(),V))
    return 0;
  const_cast<int&>(active)++;
  return 1;
}

template<>
int
FList<std::string>::getValue(long int& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 0 if not possbile  (1 if valid)
  */
{
  ELog::RegMethod RegA("FList","getVec(long int)");
  
  if (Vec.empty() || !StrFunc::convert(Vec.front(),V))
    return 0;
  
  const_cast<int&>(active)++;
  return 1;
}

template<>
int
FList<std::string>::getValue(size_t& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 0 if not possbile  (1 if valid)
  */
{
  if (Vec.empty() || !StrFunc::convert(Vec.front(),V))
    return 0;
  const_cast<int&>(active)++;
  return 1;
}

template<>
int
FList<std::string>::getValue(int& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 0 if not possbile  (1 if valid)
  */
{
  if (Vec.empty() || !StrFunc::convert(Vec.front(),V))
    return 0;
  const_cast<int&>(active)++;
  return 1;
}


template<>
int
FList<std::string>::getValue(std::string& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 1 as convertable (always)
  */
{
  if (Vec.empty())
    return 0;
  
  V=Vec.front();
  const_cast<int&>(active)++;
  return 1;
}

template<>
void
FList<Geometry::Vec3D>::write(std::ostream& OX) const
  /*!
    Write out the variable
    \param OX :: Output stream
   */
{
  for(const Geometry::Vec3D& V : Vec)
    OX<<"Vec3D("<<V[0]<<","<<V[1]<<","<<V[2]<<") ";
  return;
}

template<typename T>
void
FList<T>::write(std::ostream& OX) const
  /*!
    Write out the variable
    \param OX :: Output stream
   */
{
  for(const T& V : Vec)
    OX<<V<<" ";
  return;
}

/// \cond simpleTemplate

template<>
std::string FList<Geometry::Vec3D>::typeKey()  const
{  return "Vector(Geometry::Vec3D)"; }

template<>
std::string FList<double>::typeKey()  const
{  return "Vector(double)"; }

template<>
std::string FList<int>::typeKey()  const
{  return "Vector(int)"; }

template<>
std::string FList<size_t>::typeKey()  const
{  return "Vector(size_t)"; }

template<>
std::string FList<long int>::typeKey()  const
{  return "Vector(long int)"; }

template<>
std::string FList<std::string>::typeKey()  const
{  return "Vector(std::string)"; }

/// \endcond simpleTemplate

/// \cond TEMPLATE

template class FList<Geometry::Vec3D>;
template class FList<double>;
template class FList<int>;
template class FList<long int>;
template class FList<size_t>;
template class FList<std::string>;

/// \endcond TEMPLATE
