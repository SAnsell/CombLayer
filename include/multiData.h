#ifndef multiData_h
#define multiData_h

/*!
  \class multiData
  \version 2.0
  \author Stuart Ansell
  \date May 2023
  \beif
  
  Note: based on
  https://stackoverflow.com/questions/74031279/
  generic-slicing-views-of-multidimensional-array-in-c20-using-ranges
*/

template<typename T>
class multiData
{
 private:

  std::vector<size_t> index;     ///< Index sizes of array
  std::vector<size_t> strides;   ///< strides through data
  std::vector<T> flatData;       ///< Data vector

  size_t getRangeSize(std::vector<sRange>&) const;

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
  multiData(const size_t,std::vector<T>);
  multiData(const size_t,const size_t,std::vector<T>);
  multiData(const size_t,const size_t,const size_t,std::vector<T>);
  multiData(const multiData&);
  multiData(multiData&&);
  multiData& operator=(const multiData&);
  multiData& operator=(multiData&&);
  ~multiData();

  
  multiData<T>& operator+=(const T&);
  multiData<T>& operator-=(const T&);
  multiData<T>& operator*=(const T&);
  multiData<T>& operator/=(const T&);
  multiData<T>& operator+=(const multiData<T>&);
  multiData<T>& operator-=(const multiData<T>&);
  multiData<T>& operator*=(const multiData<T>&);
  multiData<T>& operator/=(const multiData<T>&);

  size_t getDim() const { return index.size(); }
  size_t size() const { return flatData.size(); }

  void resize(std::vector<size_t>);
  // special resize for 3D
  void resize(const size_t,const size_t,const size_t);
  // special resize for 2D
  void resize(const size_t,const size_t);

  void setData(std::vector<T>);
  void setData(const size_t,const size_t,std::vector<T>);
  void setData(const size_t,const size_t,const size_t,std::vector<T>);
  std::vector<T> getFlatRange(std::vector<sRange>) const;
  std::vector<T> getAxisRange(const size_t,const size_t) const;
  std::vector<T> getAxisIntegral(const size_t) const;
  std::vector<T> getAxisProjection(const size_t) const;
  multiData<T> getRange(std::vector<sRange>) const;
  multiData<T> reduceMap(const size_t) const;
  multiData<T> projectMap(const size_t,std::vector<sRange>) const;
  multiData<T> integrateMap(size_t,std::vector<sRange>) const;
    
  sliceUnit<T> get() { return sliceUnit<T>(flatData.data(), strides.data()); }
  sliceUnit<const T> get() const
   { return sliceUnit<const T>(flatData.data(), strides.data()); }

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
operator<<(std::ostream&,const multiData<T>&);

#endif
