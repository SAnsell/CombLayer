/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/VCell.h
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
#ifndef ModelSystem_VCell_h
#define ModelSystem_VCell_h


namespace ModelSupport
{

/*!
  \class VCell
  \author S. Ansell
  \date April 2010
  \version 1.0
  \brief Holds a virtual cell component
*/

class VCell
{ 
 private:

  int flag;          ///< Active flag  [<0 :: inactive (mat:cell:count) ]
  int mat;           ///< On material 
  int count;         ///< count number
  int cell;          ///< cell number

  Rule* outerSurf;   ///< Outer surface

 public:
  
  VCell(const int);
  VCell(const int,const int,const int);
  VCell(const VCell&);
  VCell& operator=(const VCell&);
  ~VCell() {}        ///< Destructor

  bool isValid(const int,const int,const int) const;
  bool isCell(const int) const;
  bool isCount(const int) const;
  bool isMat(const int) const;

  int getVNum() const { return vNum; }    ///< Virtual number

  std::string getCellStr(const int,const int,const int) const;

};

}

#endif
