#ifndef StrFunc_fortranWrite_h
#define StrFunc_fortranWrite_h

namespace StrFunc
{

struct FmtID
{
  size_t type;        ///< Type [0 X, 1 I, 2F]
  size_t lenA;        ///< Primary length 
  size_t lenB;        ///< Secondary lenght
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

  std::vector<FmtID> FmtInfo;
  std::ostringstream outStr;

  void parse(const std::string&);
  
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
