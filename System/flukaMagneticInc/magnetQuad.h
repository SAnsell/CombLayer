/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaMagnetInc/magnetQuad.h
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
#ifndef flukaSystem_magnetQuad_h
#define flukaSystem_magnetQuad_h

namespace flukaSystem
{
  
/*!
  \class magnetQuad
  \version 1.0
  \date February 2019
  \author S.Ansell
  \brief Holds an external magnetic system
*/

class magnetQuad :
  public magnetUnit
{
 private:

  double KFactor;        ///< K factor [T/cm]

  virtual void populate(const FuncDataBase&);
  
 public:

  magnetQuad(const std::string&,const size_t);
  magnetQuad(const magnetQuad&);
  magnetQuad& operator=(const magnetQuad&);
  virtual ~magnetQuad();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  void createAll(Simulation&,const Geometry::Vec3D&,
		 const Geometry::Vec3D&,const Geometry::Vec3D&,
		 const Geometry::Vec3D&,const double);

  virtual void writeFLUKA(std::ostream&) const;

};

}

#endif
