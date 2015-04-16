/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   log/FileReport.cxx
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
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "Exception.h"
#include "FileReport.h"

namespace ELog
{

FileReport::FileReport() : flagNumber(1)
  /*!
    Constructor
  */
{
}

FileReport::FileReport(const std::string& FName) : 
  flagNumber(1),FileName(FName)
  /*!
    Constructor
    \param FName :: File to open
  */
{
  if (!FName.empty())
    DX.open(FName.c_str(),std::ios::out);
}

FileReport::FileReport(const FileReport& A) :
  flagNumber(A.flagNumber),FileName(A.FileName)
  /*!
    Copy Constructor
    \param A :: FileReport to copy
  */
{
  DX.close();
  if (!FileName.empty())
    DX.open(FileName.c_str(),std::ios::out);
}
  
FileReport& 
FileReport::operator=(const FileReport& A) 
  /*!
    Assignment operator
    \param A :: FileReport to copy
    \return *this 
  */
{
  if (this!=&A)
    {
      flagNumber=A.flagNumber;
      setFile(A.FileName);
    }
  return *this;
}


FileReport::~FileReport()
  /*!
    Destructor
  */
{}

void 
FileReport::setFile(const std::string& Fname) 
  /*!
    Set the file (and remove old copy)
    \param Fname :: File name to use
  */
{
  FileName=Fname;
  DX.close();
  if (!Fname.empty())
    DX.open(Fname.c_str(),std::ios::out);
  else
    throw ColErr::FileError(0,"Empty String",
			    "FileReport::setFile");
  return;
}
	  
void 
FileReport::process(const std::string& M,const int) 
  /*!
    Procress Line
    \param M :: Line/Lines to add
    \param  :: Error number [1/2/4/8/16]
  */
{
  if (flagNumber)
    {
      if (flagNumber & Fileflag)
        {
	  if (!DX.good())
	    std::cerr<<"FileReport:: Setting error:"<<M<<std::endl;
	  else
	    DX<<M<<std::endl;
	}
       if (flagNumber & Coutflag)
	 std::cout<<"LOG  == "<<M<<std::endl;
       if (flagNumber & Cerrflag)
	 std::cerr<<"LOG "<<M<<std::endl;
    }
  return;
}

}   // NAMESPACE ELog

