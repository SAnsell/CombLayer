/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   visitInc/Visit.h
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
#ifndef Visit_h
#define Visit_h

class Simulation;
namespace MonteCarlo
{
class Object;
}

/*!
  \class Visit
  \brief Hold VTK output format 
  \date August 2010
  \author S. Ansell
  \version 1.0
  \todo Move to a binary/xml output schema

  This allows comparison of the vector for removing non-unique
  Vec3D from a list
*/
						
class Visit
{
 public:

  /// Types of information to plot
  enum class VISITenum : int { cellID=0,material=1,density=2,weight=3};

 private:
  
  VISITenum outType;          ///< Output type
  Geometry::Vec3D Origin;     ///< Origin
  Geometry::Vec3D XYZ;        ///< XYZ extent

  Triple<long int> nPts;        ///< Number x points
  boost::multi_array<double,3> mesh;  ///< results mesh

  double getResult(const MonteCarlo::Object*) const;

 public:

  Visit();
  Visit(const Visit&);
  Visit& operator=(const Visit&);
  ~Visit();

  void setType(const VISITenum&);
  void setBox(const Geometry::Vec3D&,
              const Geometry::Vec3D&);
  void setIndex(const size_t,const size_t,const size_t);

  void populate(const Simulation*);
  void populate(const Simulation*,const std::set<std::string>&);
  void writeVTK(const std::string&) const;
};


#endif
