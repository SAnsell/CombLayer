/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/surfDIter.h
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
#ifndef surfDIter_h
#define surfDIter_h

class FuncDataBase;

namespace ModelSupport
{

  
void populateDivideLen(const FuncDataBase&,const size_t,
		       const std::string&,const double,
		       std::vector<double>&);

void populateDivide(const FuncDataBase&,const size_t,
		    const std::string&,std::vector<double>&);


 void populateDivide(const FuncDataBase&,const size_t,
		     const std::string&,const std::string&,
		     std::vector<std::string>&);

void populateDivide(const FuncDataBase&,const size_t,
		    const std::string&,const int,std::vector<int>&);

 void populateAddRange(const FuncDataBase&,const size_t,
		    const std::string&,const double,
		    const double,std::vector<double>&);
 void populateRange(const FuncDataBase&,const size_t,
		    const std::string&,const double,
		    const double,std::vector<double>&);
 void populateQuadRange(const FuncDataBase&,const size_t,
			const std::string&,const double,
			const double,const double,
			std::vector<double>&);


void populateVecDivide(const FuncDataBase&,const std::string&,
		       const std::vector<int>&,std::vector<int>&);
void populateVecDivide(const FuncDataBase&,const std::string&,
		       const std::vector<std::string>&,std::vector<std::string>&);
 
}


#endif
