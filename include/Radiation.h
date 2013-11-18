/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/Radiation.h
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
#ifndef Radiation_h
#define Radiation_h

/*!
  \class Radiation
  \version 1.0
  \date August 2005
  \author S.Ansell
  \brief Processes MCNPX grid files

  Reads the coordinate and heat in an 
  MCNPX grid.total file (the ascii version)
*/

class Radiation 
{
 private:
  
  int coordinate;                 ///< Type of coordinate system 0== rectangular 1=cylindrical
  double current;                 ///< Current to multiply heat by

  std::vector<double> Xpts;       ///<X coordinates of box
  std::vector<double> Ypts;       ///<Y coordinates of box
  std::vector<double> Zpts;       ///<Z coordinates of box
  
  Tensor<double> Upts;        ///< Data from radiation file (grid.total)

  void cycleUpdate(size_t&,size_t&,size_t&) const;
  int openMcnpx(const std::string&,std::ifstream&,unsigned int*);
  Geometry::Vec3D toCylinderCoord(const Geometry::Vec3D&) const;

 public:

  Radiation();
  Radiation(const Radiation&);
  Radiation& operator=(const Radiation&);
  ~Radiation();

  /// Set the beam current
  void setCurrent(const double C) { current=C; }
  int readFile(const std::string& Fname);
  double heat(const Geometry::Vec3D&) const;
  void setRange(double*,double*) const;
  double Volume() const;

};


#endif
