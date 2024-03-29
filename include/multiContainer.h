#ifndef multiContainer_h
#define multiContainer_h

/*!
  \class multiContainer
   \version 1.0
   \author Stuart Ansell
   \date November 2023
   \brief Container for multiIndex requirements
   
   Note: based on:
   https://stackoverflow.com/questions/74031279/
   generic-slicing-views-of-multidimensional-array-in-c20-using-ranges

   This is the version for objects like strings that have not
   arithmetric system and only 

*/

template<typename T>
class multiContainer
{
 private:

  std::vector<size_t> index;     ///< Index sizes of array
  std::vector<size_t> strides;   ///< strides through data [+1 size for zero]
  /// Data vector but allows random sizing of data
  std::vector<T> flatData;

  void getRangeImpl(std::vector<T>&,size_t&,
		    const std::vector<sRange>&,
		    const size_t,const size_t) const; 

  void throwMatchCheck(const multiContainer<T>&,const std::string&) const;
  
 public:

  explicit multiContainer();
  multiContainer(std::vector<size_t>,std::vector<T>);
  explicit multiContainer(std::vector<size_t>);
  explicit multiContainer(const size_t);

  multiContainer(std::vector<size_t>,std::map<size_t,T>);

  multiContainer(const size_t,const size_t);
  multiContainer(const size_t,const size_t,const size_t);
  multiContainer(const size_t,const size_t,const size_t,const size_t);

  multiContainer(const size_t,std::vector<T>);
  multiContainer(const size_t,const size_t,std::vector<T>);
  multiContainer(const size_t,const size_t,const size_t,std::vector<T>);

  multiContainer(const multiContainer&);
  multiContainer(multiContainer&&);
  multiContainer& operator=(const multiContainer&);
  multiContainer& operator=(multiContainer&&);
  ~multiContainer();

  
  multiContainer<T>& operator+=(const T&);
  multiContainer<T>& operator-=(const T&);
  multiContainer<T>& operator*=(const T&);
  multiContainer<T>& operator/=(const T&);
  multiContainer<T>& operator+=(const multiContainer<T>&);
  multiContainer<T>& operator-=(const multiContainer<T>&);
  multiContainer<T>& operator*=(const multiContainer<T>&);
  multiContainer<T>& operator/=(const multiContainer<T>&);

  bool isEmpty() const { return index.empty(); } 
  size_t getDim() const { return index.size(); }
  size_t size() const { return (strides[0]) ? index[0]*strides[0] : 0; }

  size_t getRangeSize(std::vector<sRange>&) const;
  
  void resize(std::vector<size_t>);
  // special resize for 4D
  void resize(const size_t,const size_t,const size_t,const size_t);
  // special resize for 3D
  void resize(const size_t,const size_t,const size_t);
  // special resize for 2D
  void resize(const size_t,const size_t);

  void setData(std::vector<T>);
  void setData(const size_t,const size_t,std::vector<T>);
  void setData(const size_t,const size_t,const size_t,std::vector<T>);

  multiContainer<T> getRange(std::vector<sRange>) const;
  std::vector<T> getFlatRange(std::vector<sRange>) const;
  std::vector<T> getAxisRange(const size_t,const size_t) const;
  
  sliceMap get() const
  { return sliceMap(strides.data()); }


  T& value(const std::vector<size_t>&);
  const T& value(const std::vector<size_t>&) const;
  
  // /// accessor to pointer
  // T& getDAta() { return flatData.data(); }
  // const T* getPtr() const { return flatData.data(); }
  
  const std::vector<size_t>& shape() const
    { return index; }
  const std::vector<size_t>& sVec() const
  { return strides; }
  std::vector<size_t> reduceAxis(const size_t) const;
  
  size_t offset(const size_t,const size_t) const;
  size_t offset(const size_t,const size_t,const size_t) const;
  size_t offset(const std::vector<size_t>&) const;
  void fill(const T&);
  
  using iterator=typename std::vector<T>::iterator;
  using const_iterator=typename std::vector<T>::const_iterator;
  
  iterator begin() { return flatData.begin(); }
  iterator end() { return flatData.end(); }
  const_iterator begin() const { return flatData.begin(); }
  const_iterator end() const { return flatData.end(); }

  void write(std::ostream&) const;
};

template<typename T> 
std::ostream&
operator<<(std::ostream&,const multiContainer<T>&);

#endif
