#ifndef cycIterator_h
#define cycIterator_h

/*!
  \class cycIterator
  \tparam  IT :: Iterator type 
  \brief 
 */
template<typename T,typename Iterator> 
class cycIterator : 
  public std::iterator
    <std::bidirectional_iterator_tag, T,std::ptrdiff_t>
{
 private:

  int loopIndex;   ///< loop count
  Iterator beginIter;   ///< Begin point
  Iterator endIter;     ///< End point 
  Iterator vc;          ///< Current position  

 public:

  cycIterator(const Iterator&,const Iterator&);
  cycIterator(const cycIterator<T,Iterator>&);
  /// iterator assignment operator
  cycIterator<T,Iterator>& operator=(const Iterator& A)
    { loopIndex=0; vc=A; return *this; }
  cycIterator<T,Iterator>& operator=(const cycIterator<T,Iterator>&); 
  ~cycIterator() {}   ///< Destructor

  /// access base object
  const T& operator*() const { return *vc; }
  cycIterator<T,Iterator>& operator++();
  cycIterator<T,Iterator> operator++(int);
  /// access loop counter
  int getLoop() const { return loopIndex; }

};




#endif
