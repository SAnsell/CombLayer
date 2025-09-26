/********************************************************************* 
  C++Azint : 2D-Detector to Q-Data processor
 
 * File:   include/dataSlice.h
 *
 * Copyright (c) 2004-2025 by Stuart Ansell
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
#ifndef dataSlice_h
#define dataSlice_h

/*!
  \struct sRange
  \author S. Ansell
  \version 1.0
  \brief Holds a simple x - y range for an index
 */
struct sRange
{
  constexpr sRange() = default;
  /// Create a slice with a single index
  constexpr sRange(size_t i) : begin(i), end(i+1) {}
  /// Create a slice with a start and an end index
  constexpr sRange(size_t s, size_t e) : begin(s), end(e+1) { } 
  
  size_t begin {0};  ///< Start iterator position
  size_t end {0};    ///< end iterator posiition
}; 


template<class T>
struct sliceUnit
{
  T* dataPtr = 0;
  const std::size_t* stride = 0;

  /// constructor
  sliceUnit(T* dPtr,const size_t* sPtr) :
    dataPtr(dPtr),stride(sPtr) {}
  
  /// accessor to data index
  sliceUnit operator[](std::size_t index) const
    {return{ dataPtr+index * *stride, stride+1 };  }

  /// reference () operator (to get data member)
  operator const T& () const { return *dataPtr; }
  /// data member (non-const)
  operator T& () { return *dataPtr; }   
  /// point operator
  const T& operator->() const { return *dataPtr; }

  sliceUnit&
  operator=(const sliceUnit& A)
  {
    if (this!=&A)
      {
	// if the item is not constant then we can set it by value
	// only if both strides have hit zero
	if constexpr (!std::is_const<T>::value)
	  {
	    if (!(*stride) && !(*A.stride))
	      {
		*dataPtr=*A.dataPtr;
	      }
	    else
	      {
		dataPtr=A.dataPtr;
		stride=A.stride;
	      }
	  }
	else
	  {
	    dataPtr=A.dataPtr;
	    stride=A.stride;
	  }
      }
    return *this;
  }
  
  /// assignment system for values
  T& operator=(typename std::remove_const<T>::type in) const
    {
      *dataPtr = std::move(in);
      return *dataPtr;
    }

  T& get() { return *dataPtr; }
  const T* pointer() const { return dataPtr; }
  T* assignPointer() const { return dataPtr; }
};


/*!
  Specialization for string
  maybe replace with constexpr but difficult
 */

template<>
struct sliceUnit<std::string>
{
  std::string* dataPtr = 0;
  const std::size_t* stride = 0;

  /// constructor
  sliceUnit(std::string* dPtr,const size_t* sPtr) :
    dataPtr(dPtr),stride(sPtr) {}
  
  /// accessor to data index
  sliceUnit operator[](std::size_t index) const
    {return{ dataPtr+index * *stride, stride+1 };  }

  /// reference () operator (to get data member)
  operator std::string& () { return static_cast<std::string&>(*dataPtr); }

  /// concatination operator
  std::string operator+=(const std::string& A)
    { return *dataPtr += A;  }

  /// inequalty operator
  bool operator!=(const std::string& A)
  { return *dataPtr!=A;  }

  /// equally operator
  bool operator==(const std::string& A)
    { return *dataPtr!=A;  }

  /// assignement and equally operator
  bool operator==(const char* APtr)
  { return *dataPtr == std::string(APtr);  }

  /// equal opertor  for 
  sliceUnit&
  operator=(const sliceUnit& A)
  {
    if (this!=&A)
      {
	if (!(*stride) && !(*A.stride))
	  *dataPtr = *A.dataPtr;
	else
	  {
	    dataPtr=A.dataPtr;
	    stride=A.stride;
	  }
      }
    return *this;
  }
  
  // assignment system for values
  std::string& operator=(std::string in) const
    {
      *dataPtr = std::move(in);
      return *dataPtr;
    }

  /// direct access to underlying type
  const std::string* pointer() const
    { return dataPtr; }
  /// direct access to underlying type
  std::string* assignPointer() const
    {  return dataPtr; }
};

#endif
