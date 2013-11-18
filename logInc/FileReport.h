/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   logInc/FileReport.h
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
#ifndef ELog_FileReport_h
#define ELog_FileReport_h

namespace ELog
{

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
  int process(const std::string&,const int) const
    { return 0; }

  /// Dispatch delayed process 
  int process(std::ostream&,const std::vector<std::string>&,
	      const std::vector<int>&) const;
};

/*!
  \class EReport
  \brief A function class to process Error reports 
  \version 1.0
  \author S. Ansell
  \date January 2006
  
  Class processes Error reports by writting them
  to std::cerr
*/
class EReport
{
 private:

 public:
  
  EReport() {}  ///< Constructor
  EReport(const EReport&) {}  ///< Copy constructor
  EReport& operator=(const EReport&) { return *this; } ///< Assignment operator
  ~EReport() {}     ///< Destructor

  void process(const std::string&,const int);
};

/*!
  \class FileReport
  \version 1.0
  \brief Processes information into a file
  \author S. Ansell
  \date April 2007
*/    

class FileReport
{
 private:

  /// Enumeration of the type of output
  enum outENUM { Nullflag=0, Fileflag=1, 
		 Coutflag=2,Cerrflag=4 };

  /// Flags to write 
  int flagNumber;  
  std::string FileName;    ///< Name of the file
  std::ofstream DX;        ///< Output stream
  
 public:

  FileReport();
  FileReport(const std::string&);
  FileReport(const FileReport&);
  FileReport& operator=(const FileReport&);
  ~FileReport();

  void setFile(const std::string&);

  /// Dispatch process 
  void process(const std::string&,const int);
  /// Access to name
  std::string getName() const { return FileName; }  
};

} // NAMESPACE ELog

#endif
