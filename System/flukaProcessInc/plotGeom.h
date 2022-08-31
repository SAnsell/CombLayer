/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaProcessInc/plotGeom.h
*
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef plotGeom_h
#define plotGeom_h

class Simulation;

namespace flukaSystem
{

/*!
  \class plotGeom
  \brief Hold VTK output format 
  \date August 2010
  \author S. Ansell
  \version 1.0
  \todo Move to a binary/xml output schema

  This allows comparison of the vector for removing non-unique
  Vec3D from a list
*/
						
class plotGeom
{
 private:

  std::string title;             ///< Title
  
  Geometry::Vec3D APoint;        ///< Low unit
  Geometry::Vec3D BPoint;        ///< Top unitxs
  Geometry::Vec3D xUnit;         ///< X-unit
  Geometry::Vec3D yUnit;         ///< Y-unit

  int matRegion;                   ///< region flag
  
 public:

  plotGeom();
  plotGeom(const plotGeom&);
  plotGeom& operator=(const plotGeom&);
  ~plotGeom();

  void setTitle(const std::string&);
  /// make an average over the line from beginning to end
  void setBox(const Geometry::Vec3D&,const Geometry::Vec3D&);

  /// set flag to write regions and not materials
  void setRegion() { matRegion=0; }
  void write(std::ostream&) const;
};

}

#endif

  
