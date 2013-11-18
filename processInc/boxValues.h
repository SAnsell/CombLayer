/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/boxValues.h
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
#ifndef ModelSupport_boxValues_h
#define ModelSupport_boxValues_h

namespace ModelSupport
{
 
 /*!
   \class boxValues 
   \version 1.0
   \author S. Ansell
   \date October 2011
   \brief Holds simple things about the boxSides
 */
class boxValues
{
 private:

  size_t NSides;                         ///< Number of sides
  int setFlag;                           ///< Dist/Axis set 
  std::vector<double> SDist;             ///< Distances from centre line
  std::vector<Geometry::Vec3D> SAxis;    ///< Axises [Towards centre]

  int MatN;         ///< Material number
  double Temp;      ///< Temperature

 public:  
  
  /// Basic constructor
  boxValues(const size_t,const int&,const double&);
  boxValues(const boxValues&);
  boxValues& operator=(const boxValues&);
  ~boxValues() {} 

  void symPlanes(const double&,const double&);
  void setMaterial(const int,const double);

  /// Get number of sides
  size_t getSides() const { return NSides; }
  /// Access Mat
  int getMat() const { return MatN; }
  /// Access Temp
  double getTemp() const { return Temp; }
  double getExtent() const;
  Geometry::Vec3D getDatum(const size_t,
			   const Geometry::Vec3D&,
			   const Geometry::Vec3D&,
			   const Geometry::Vec3D&) const;
  Geometry::Vec3D getAxis(const size_t,
			  const Geometry::Vec3D&,
			  const Geometry::Vec3D&) const;

 };

}

#endif
