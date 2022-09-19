/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/TubeDetBox.h
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
#ifndef setVariable_TubeDetBoxGenerator_h
#define setVariable_TubeDetBoxGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class TubeDetBoxGenerator
  \version 1.0
  \author S. Ansell
  \date September 2022
  \brief TubeDetBox for variables
*/

class TubeDetBoxGenerator
{
 private:
  
  double centRadius;            ///< Radius of from centre
  double tubeRadius;            ///< Radius of detector
  double wallThick;             ///< Outer wall thickness
  double height;                ///< Height/depth

  double gap;                    ///< Gap thickness
  double outerThick;             ///< Wall thickness

  std::string detMat;                   ///< Detector material
  std::string wallMat;                  ///< Wall material
  std::string outerMat;                 ///< Outer material
  std::string filterMat;                ///< Filter on front window

 public:

  TubeDetBoxGenerator();
  TubeDetBoxGenerator(const TubeDetBoxGenerator&);
  TubeDetBoxGenerator& operator=(const TubeDetBoxGenerator&);
  ~TubeDetBoxGenerator();

  void generateBox(FuncDataBase&,const std::string&,
		   const Geometry::Vec3D&,const size_t) const;

};

}

#endif
 
