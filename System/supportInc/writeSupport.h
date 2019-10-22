/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/writeSupport.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef StrFunc_writeSupport_h
#define StrFunc_writeSupport_h



namespace StrFunc
{

std::string flukaNum(const int);
std::string flukaNum(const double);

std::vector<std::string> splitComandLine(std::string);
 
// Write file in standard MCNPX input form
void writeControl(const std::string&,std::ostream&,
		  const size_t,const int);

/// Write file in standard FLUKA input form 
void writeFLUKA(const std::string&,std::ostream&);

/// Write file in standard MCNPX input form 
void writeMCNPX(const std::string&,std::ostream&);
void writeMCNPXcont(const std::string&,std::ostream&);
void writeMCNPXcomment(const std::string&,std::ostream&,
			const std::string="c ");

void writeFLUKAhead(const std::string&,const std::string&,
		    const std::string&,std::ostream&);
 
}  // NAMESPACE StrFunc

#endif

