/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/MultiPipeGenerator.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef setVariable_MultiPipeGenerator_h
#define setVariable_MultiPipeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class MultiPipeGenerator
  \version 1.0
  \author S. Ansell
  \date June 2020
  \brief MultiPipeGenerator for variables
*/

class MultiPipeGenerator 
{
 private:

  double flangeRadius;          ///< Radius of front flange
  double flangeLength;          ///< Length of front flange

  std::string voidMat;        ///< void material
  std::string pipeMat;        ///< pipe material
  std::string flangeMat;      ///< Flange material


  /// pipe values
  std::vector<tdcSystem::subPipeUnit> pipes;

 public:

  MultiPipeGenerator();
  MultiPipeGenerator(const MultiPipeGenerator&);
  MultiPipeGenerator& operator=(const MultiPipeGenerator&);
  ~MultiPipeGenerator();


  
  template<typename CF> void setFlangeCF();

  template<typename CF> void setPipe(const Geometry::Vec3D&,
				     const double,const double,
				     const double);

  
  void generateMulti(FuncDataBase&,const std::string&) const;

};

}

#endif
 
