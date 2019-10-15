/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/DoublePortItemGenerator.h
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
#ifndef setVariable_DoublePortItemGenerator_h
#define setVariable_DoublePortItemGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class DoublePortItemGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief DoublePortItemGenerator for variables
*/

class DoublePortItemGenerator : public PortItemGenerator
{
 private:

  double partLength;          ///< length from outer
  double radiusB;             ///< Inner radius
  
 public:

  DoublePortItemGenerator();
  DoublePortItemGenerator(const DoublePortItemGenerator&);
  DoublePortItemGenerator& operator=(const DoublePortItemGenerator&);
  ~DoublePortItemGenerator();
  
  virtual void generatePort(FuncDataBase&,const std::string&,
		    const Geometry::Vec3D&,const Geometry::Vec3D&) const;

};

}

#endif
 
