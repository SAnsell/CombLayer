/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsSupportInc/phitsWriteSupport.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef StrFunc_phitsWriteSupport_h
#define StrFunc_phitsWriteSupport_h


namespace StrFunc
{
template<typename T>
void writePHITSItems(std::ostream& OX,const T Item)
{
  OX<<std::left<<std::setw(14)<<Item<<" ";
  return;
}

template<typename T,typename... Other>
void writePHITSItems(std::ostream& OX,
		     const T AItem,const Other... ListItems)
  /*!
    Function to allow multiple items to be written as a table line
    \parma AItem :: first item
    \param 
   */
{
  writePHITSItems(OX,AItem);
  writePHITSItems(OX,ListItems...);
  return;
}

template<typename... Other>
void writePHITSTable(std::ostream& OX,const size_t depth,
		     const Other... ListItems)
/*!
    Function to allow multiple items to be written as a table line
    \param OX :: Output stream
    \param ListItems :: items to write
   */
{
  OX<<std::string((depth+1)*2,' ');
  writePHITSItems(OX,ListItems...);
  OX<<std::endl;
  return;
}

void writePHITSComment(std::ostream&,const size_t,const std::string&);
void writePHITSOpen(std::ostream&,const size_t,const std::string&);
void writePHITSTableHead(std::ostream&,const size_t,
			 const std::vector<std::string>&);
		  

template<typename T>
void writePHITS(std::ostream&,const size_t,
		const std::string&,const T,const std::string& ="");

template<typename T>
void writePHITSCont(std::ostream&,const size_t,const size_t,const T&);
 
}  

#endif
