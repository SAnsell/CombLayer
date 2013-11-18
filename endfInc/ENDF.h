/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   endfInc/ENDF.h
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
#ifndef ENDF_h
#define ENDF_h

/*!
  \namespace ENDF
  \brief ENDF reading functions
  \version 1.0
  \author S. Ansell
  \date May 2010
 */

namespace ENDF
{
  extern int lineCnt;           ///< Line count of read numbers 

  int getNumber(std::string&,double&);
  int getNumber(std::string&,const size_t,double&);

  std::pair<std::string,std::string> getLine(std::istream&);

  Triple<int> getMatIndex(const std::string&);

  std::string findMatMfMt(std::istream&,const int,const int,const int);

  void headRead(std::istream&,double&,double&,
		int&,int&,int&,int&);

  void listRead(std::istream&,double&,double&,
		int&,int&,int&,int&,
		std::vector<double>&);

  void table1Read(std::istream&,double&,double&,
		  int&,int&,int&,int&,
		  std::vector<int>&,std::vector<int>&,
		  std::vector<double>&,std::vector<double>&);
  
  void table2Read(std::istream&,double&,double&,
		  int&,int&,int&,int&,
		  std::vector<int>&,std::vector<int>&);

  // INTERPOLATION
  double loglinear(const double&,const double&,const double&,
		   const double&,const double&);

}

#endif
