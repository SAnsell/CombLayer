/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   logInc/OutputLog.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef Elog_OutputLog_h
#define Elog_OutputLog_h

/*!
  \namespace ELog
  \brief Holder of a single Error Message buffer
  \author S. Ansell
  \date February 2006
  \version 1.0
  
  Simply holds a static version of the Error buffer.
*/

namespace ELog
{

  class NameStack;

  enum { basic=1,warn=2,error=4,debug=8,diag=16,crit=32,trace=64 };

/*!
  \class OutputLog
  \brief A master error log
  \version 1.0
  \author S. Ansell
  \date January 2006
  
  Class holds error messages that are either displayed immediately
  or held for a later processing. 
  They can be cleared at will. It uses a reporting
  class which decides the policy for what to do
  with the Error data when it is recieved. 

  activeBits 
  - 1 : Basic 
  - 2 : Warning
  - 4 : Error
  - 8 : Debug
  - 16 : Diagnosntic
  - 32 : Critical
  - 64 : Trace
*/

template<typename RepClass>
class OutputLog
{
 private:
  
  std::ostringstream cx;            ///< Stream for processing

  int colourFlag;                   ///< Activate colour
  size_t activeBits;                ///< Activity bits
  size_t actionBits;                ///< Action bits [exit on text]
  size_t debugBits;                 ///< Debug bits
  int typeFlag;                     ///< Give type information
  int locFlag;                      ///< Write Location
  int storeFlag;                    ///< Wait to process
  
  NameStack* NBasePtr;              ///< Reg Class base  
  RepClass FOut;                    ///< Holder for the report class

  std::vector<std::string> EText;   ///< Storage buffer (text)
  std::vector<int> EType;           ///< Storage buffer (type)

  bool isActive(const int) const;
  std::string getColour(const int) const;
  void makeAction(const int);
  std::string locString() const;
  std::string eType(const int) const;
  std::string indent() const;
  size_t getIndentLength() const;

 public:
 
  OutputLog();
  OutputLog(const std::string&);
  OutputLog(const OutputLog<RepClass>&);
  OutputLog<RepClass>& operator=(const OutputLog<RepClass>&);
  ~OutputLog();

  void report(const std::string&,const int);
  void report(const int);

  std::ostringstream& Estream() { return cx; }   ///< Access stream

  /// Set Pointer
  void setNBasePtr(NameStack* Ptr) { NBasePtr=Ptr; } 
  void setLocFlag(const int I) { locFlag=I; }   ///< Set location output flag
  void setTypeFlag(const int I) { typeFlag=I; }  ///< Set type output

  /// Template specialization to get input
  template<typename InputType>
  OutputLog& operator<<(const InputType& A)
    { cx<<A; return *this; }
  
  /// Special to pick up modifications to the stream
  OutputLog& operator<<(std::ostream& (*f)(std::ostream&) )
    {
      f(cx);
      return *this;
    }

  
  /// function that takes a custom stream, and returns it
  typedef OutputLog<RepClass>& (*OutputLogManipulator)(OutputLog<RepClass>&);
  /// take in a function with the custom signature
  OutputLog<RepClass>& operator<<(OutputLogManipulator manip)
    {
      return manip(*this);
    }

  /// Set Colour
  void setColour() { colourFlag=1; }
  /// set the active bits:
  void setActive(const size_t F) { activeBits=F; }
  /// set the action bits:
  void setAction(const size_t F) { actionBits=F; }
  /// set the debug bits:
  void setDebug(const size_t F ) { debugBits=F; }

  RepClass& getReport() { return FOut; }   ///< Get report class

  void basic(const std::string&);
  void warning(const std::string&);
  void error(const std::string&); 
  void debug(const std::string&);
  void diagnostic(const std::string&);
  void critical(const std::string&);
  void trace(const std::string&);

  void basic() { report(0); }    ///< General
  void warning() { report(1); }  ///< Warning
  void error()  { report(2); }   ///< Error 
  void debug()  { report(4); }   ///< Debug
  void diagnostic()  { report(8); }   ///< Diag
  void critical()  { report(16); }   ///< Critical
  void trace()  { report(32); }   ///< Critical

  // Process hold:
  void lock() { storeFlag=1; }     ///< Set log
  void unlock() { storeFlag=0; }   ///< Set unlock
  void dispatch(const int);      
  void disLevel(const int);      
};

///\cond EXTERN
extern OutputLog<EReport> EM;         ///< Global Error log
extern OutputLog<FileReport> FM;      ///< Global Error to a file
extern OutputLog<FileReport> RN;      ///< Renumbering
extern OutputLog<StreamReport> CellM; ///< Global Stream Report
///\endcond EXTERN

template<typename RepClass>
OutputLog<RepClass>& endDiag(OutputLog<RepClass>&);

template<typename RepClass>
OutputLog<RepClass>& endBasic(OutputLog<RepClass>&);

template<typename RepClass>
OutputLog<RepClass>& endErr(OutputLog<RepClass>&);

template<typename RepClass>
OutputLog<RepClass>& endWarn(OutputLog<RepClass>&);

template<typename RepClass>
OutputLog<RepClass>& endDebug(OutputLog<RepClass>&);

template<typename RepClass>
OutputLog<RepClass>& endCrit(OutputLog<RepClass>&);

template<typename RepClass>
OutputLog<RepClass>& endTrace(OutputLog<RepClass>&);



}  // NAMESPACE Elog

#endif
