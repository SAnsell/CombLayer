#ifndef StrFunc_fortranWrite_h
#define StrFunc_fortranWrite_h

namespace StrFunc
{

struct FmtID
{
  size_t type;        ///< Type [0 X, 1 I, 2F]
  int lenA;        ///< Primary length 
  int lenB;        ///< Secondary length

  /// Constructor
  FmtID(const size_t T,const int A,const int B) :
    type(T),lenA(A),lenB(B) {}
};
 
/*!
  \class fortranWrite
  \brief Write out fortran data 
  \author S. Ansell
  \version 1.0
  \date April 2015

  Uses:  stream << FortranWrite("format") % data1 % data2 % data3
 */
class fortranWrite
{
 private:

  std::deque<FmtID> FmtInfo;
  std::ostringstream outStr;

  void parse(const std::string&);
  void consumeZeroType();
  
 public:

  explicit fortranWrite(const std::string&);

  template<typename T>
  fortranWrite& operator%(const T&);

  void write(std::ostream&) const;
};

std::ostream&
operator<<(std::ostream&,const fortranWrite&);

}  

#endif
