/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/Rabbit.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef delftSystem_Rabbit_h
#define delftSystem_Rabbit_h

class Simulation;

namespace delftSystem
{

  class ReactorGrid;
/*!
  \class Rabbit
  \version 1.0
  \author S. Ansell
  \date May 2014
  \brief Rabbit for reactor  
*/

class Rabbit : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:

  const std::string& baseName;  ///< Base name
  
  std::string objName;          ///< FC object

  double sampleRadius;          ///< Sample radius [mid capsual]

  double capsuleRadius;         ///< Capsule cyclinder radius [inner]
  double capsuleLen;            ///< Capsule length
  double capsuleWall;           ///< Capsule wall thick  
  double capsuleZShift;             ///< Distance up from end

  int sampleMat;                ///< Sample material
  int capsuleMat;               ///< Capsule material

  size_t nLayer;                ///< Nubmer of layer
  std::vector<double> Radii;    ///< Radii 
  std::vector<int> Mat;         ///< Material 
  
  double length;                ///< Length to stop
  double capThick;              ///< Cap thickness
  int capMat;                   ///< Cap material
 
  int innerVoid;                ///< Inner void cell

  void populate(const FuncDataBase&);
  void createUnitVector(const ReactorGrid&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Rabbit(const std::string&,const int);
  Rabbit(const Rabbit&);
  Rabbit& operator=(const Rabbit&);
  virtual ~Rabbit();

  /// Accessor to inner void cell
  int getInnerVoid() const { return innerVoid; }

  int build(Simulation&,const ReactorGrid&);
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);
  

};

}

#endif
 
