/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/hexUnit.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef constructSystem_hexUnit_h
#define constructSystem_hexUnit_h

namespace constructSystem
{

/*!
  \class hexUnit
  \version 1.0
  \author S. Ansell
  \date May 2013
  \brief Deals with a number of tubes in a plate
 */

class hexUnit : public gridUnit
{
 public:


  hexUnit(const long int,const long int,const Geometry::Vec3D&);  
  hexUnit(const long int,const long int,const bool,const Geometry::Vec3D&);
  hexUnit(const hexUnit&);
  hexUnit& operator=(const hexUnit&); 
  ~hexUnit() override {}  ///< Destructor

  bool isConnected(const gridUnit&) const override;
  int gridIndex(const size_t) const override;

};

std::ostream&
operator<<(std::ostream&,const hexUnit&);

}

#endif
 
