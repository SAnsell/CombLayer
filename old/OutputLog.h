/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/OutputLog.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef OutputLog_h
#define OutputLog_h

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

/*!
  \class EReport
  \brief A function class to process Error reports 
  \version 1.0
  \author S. Ansell
  \date January 2006
  
  Class processes Error reports by writting them
  to std::cerr. Cant mask report  and it is written to 
  std::cout
*/
class EReport
{
  public:
  
  /// Dispatch process 
  void process(const std::string& M,const int T) const
    {
      if (T & 2)
	std::cerr<<"Error("<<T<<") :: "<<M<<std::endl;
      else if (T & 1)
	std::cerr<<"Diagnostic("<<T<<") :: "<<M<<std::endl;
      else if (T & 4)
	std::cerr<<"Debug("<<T<<") :: "<<M<<std::endl;
      else 
	std::cout<<"Report ("<<T<<") :: "<<M<<std::endl;
    }

};

/*!
  \class FileReport
  \verion 1.0
  \date January 2006
  \author S. Ansell
  \brief Allow a Error log to a file 
*/

class FileReport
{
 private:

  std::string FileName;  
  std::ofstream DX;
  
 public:

  FileReport();
  FileReport(const std::string&);
  FileReport(const FileReport&);
  FileReport& operator=(const FileReport&);
  ~FileReport();

  void setFile(const std::string&);
	  
  /// Dispatch process 
  void process(const std::string&,int const);
  std::string getName() const { return FileName; } 
};

/*!
  \class StreamReport
  \brief A function class to output the log in one chunk
  \version 1.0
  \author S. Ansell
  \date May 2006
  
  Class processes Error reports by writting them
  to std::cerr
*/
class StreamReport
{
  public:
  
  /// Dispatch process 
  int process(const std::string&,int const) const
    { return 0; }


  /// Dispatch delayed process 
  int process(std::ostream&,
	      const std::vector<std::string>&,const std::vector<int>&) const;
};


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
  - 0 : Basic 
  - 1 : Warning
  - 2 : Error
  - 4 : Debug

*/

template<typename RepClass>
class OutputLog
{
 private:
  
  std::ostringstream cx;
  int newError;                     ///< flag to say if new errors have been found
  std::vector<std::string> Etext;   ///< Buffer for the Error messages
  std::vector<int> Etype;           ///< Buffer for the Error types
  unsigned int activeBits;          ///< Activity bits

  RepClass FOut;        ///< Holder for the report class (since needs state)

  int isActive(int const) const;

 public:
 
  OutputLog();
  OutputLog(const std::string&);
  OutputLog(const OutputLog<RepClass>&);
  OutputLog<RepClass>& operator=(const OutputLog<RepClass>&);
  ~OutputLog();

  void report(const std::string&,int const);
  void report(int const);
  std::ostringstream& Estream() { return cx; }   ///< Access stream
  
  std::string& getText(int const = -1);
  const std::string& getText(int const = -1) const;
  int getType(int const =-1) const; 

  /// set the active bits:
  void setActive(const unsigned int F) { activeBits=F; }

  void clear();         ///< clear the error log

  RepClass& getReport() { return FOut; }     ///< Get report class
  void processReport(std::ostream&);  
  int check();          ///< return newError and clear flag

  
  void basic(const std::string&);
  void warning(const std::string&);
  void diagnostic(const std::string&);
  void error(const std::string&); 
  void debug(const std::string&);

  void basic() { report(0); }    ///< General
  void warning() { report(1); }  ///< Warning  
  void diagnostic() { report(1); }  ///< diagnostic 
  void error() { report(2); }   ///< Error 
  void debug() { report(4); }   ///< Debug

};

extern OutputLog<EReport> EMessages;      ///< Global Error log
extern OutputLog<FileReport> FMessages;   ///< Global Error to a file
extern OutputLog<StreamReport> CellMessage; ///< Global Stream Report

} // NAMESPACE ELog

#endif
