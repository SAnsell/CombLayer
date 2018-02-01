/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/PortItemGenerator.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef setVariable_PortItemGenerator_h
#define setVariable_PortItemGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class PortItemGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief PortItemGenerator for variables
*/

class PortItemGenerator
{
 private:

  double length;          ///< length from outer
  double radius;          ///< Inner radius
  double wallThick;       ///< wall thickness
  
  double flangeLen;          ///< Flange length
  double flangeRadius;       ///< Flange radius
    
 public:

  PortItemGenerator();
  PortItemGenerator(const PortItemGenerator&);
  PortItemGenerator& operator=(const PortItemGenerator&);
  ~PortItemGenerator();
  

  template<typename CF> void setCF(const double);
  // set L,R,WT
  void setPort(const double,const double,const double);
  // set flangeLen,flangeRad
  void setFlange(const double,const double);

  void generatePort(FuncDataBase&,const std::string&,
		    const Geometry::Vec3D&,
		    const Geometry::Vec3D&) const;

};

}

#endif
 
