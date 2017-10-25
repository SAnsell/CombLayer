/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/version.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <iomanip>
#include <complex>
#include <fstream>
#include <vector>
#include <array>
#include <string>

#include "Exception.h"
#include "support.h"
#include "version.h"

version::version() :
  VFile("/home/stuartansell/Model_MCNPX.ver"),
  vNum(1)
 ///< Constructor
{
  setFile(VFile);
}

version::version(const version& A) :
  VFile(A.VFile),vNum(A.vNum)
  /*!
    Copy Constructor
    \param A :: version to copy
  */
{}

version&
version::operator=(const version& A) 
  /*!
    Assignment operator
    \param A :: Version to copy
    \return *this
  */
{
  if (this!=&A)
    {
      VFile=A.VFile;
      vNum=A.vNum;
    }
  return *this;
}

version&
version::Instance()
  /*!
    Create an instance
    \return Effective this
  */
{
  static version APtr;
  return APtr;
}

std::string
version::getBuildTag() const
  /*!
    Return build tag
   */
{
  return buildTag;
}

int
version::getIncrement()
  /*!
    Get and return with the increment
    \return Old number						
   */
{
  vNum++;
  write();
  return vNum-1;
}

int
version::setFile(const std::string& FName) 
  /*!
    Set the file (tests for the file)
    \param FName :: Filename (including path)
    \retval -1 :: No file
    \retval -2 :: File format wrong
    \retval 0 :: success
  */
{
  if (FName.empty()) return -1;
  std::ifstream OX;
  OX.open(FName.c_str());
  if (!OX.good()) return -1;

  int N;
  std::string LineA=StrFunc::getLine(OX,256);
  std::string LineB=StrFunc::getLine(OX,256);
  if (!StrFunc::section(LineB,N))
    {
      if (StrFunc::section(LineA,N))
        {
          vNum=N;
          buildTag="unknown";
          VFile=FName;
          return 0;
        }
      return -2;
    }
  
  if (!StrFunc::section(LineA,buildTag))
    buildTag="unknown";
  
  vNum=N;
  VFile=FName;
  return 0;
}

void
version::write() const
  /*!
    Write the file out
  */
{
  if (!VFile.empty())
    {
      std::ofstream OX;
      OX.open(VFile.c_str());
      if (OX.good())
        {
          OX<<buildTag<<std::endl;
          OX<<vNum<<std::endl;
        }
      OX.close();
    }
  return;
}

