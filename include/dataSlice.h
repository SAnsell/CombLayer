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
  
  // accessor to data index
  sliceUnit operator[](std::size_t index) const
    {return{ dataPtr+index * *stride, stride+1 };  }

  /// reference () operator (to get data member)
  operator T&() const { return *dataPtr; }

  sliceUnit&
  operator=(const sliceUnit& A)
  {
    if (this!=&A)
      {
	// notice the double inner else :
	// this gives speed optimization for const / non-const
	// <T> forms 
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
  
  // assignment system for values
  T& operator=(typename std::remove_const<T>::type in) const
    {
      *dataPtr = std::move(in);
      return *dataPtr;
    }
  
  const T* pointer() const
    {
      return dataPtr;
    }
   T* assignPointer() const
    {
      return dataPtr;
    }
};

#endif
