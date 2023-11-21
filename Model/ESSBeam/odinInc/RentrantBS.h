/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/odin/RentrantBS.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef essSystem_RentrantBS_h
#define essSystem_RentrantBS_h

class Simulation;

namespace essSystem
{
  
/*!
  \class RentrantBS
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief Rentrant beam stop
*/

class RentrantBS :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
  
{
 private:
  

  double width;                  ///< Total 
  double height;                 ///< Height [+ve Z]
  double depth;                  ///< Depht [-ve Z]
  double length;                 ///< Length 

  double b4cThick;               ///< b4c thickness
  
  double feRadius;               ///< Radius of cylinder
  double feLength;               ///< Length [From full cu]

  double outerRadius;             ///< First cut section [void]
  double outerFeRadius;           ///< First cut section [Steel[
  double outerFeStep;             ///< Step in [if radius bigger
  double outerCut;                ///< First cut length [void]

  double innerRadius;             ///< First inner radius [enterance]
  double innerFeRadius;           ///< Second inner radius 
  double innerFeStep;             ///< Step in [if radius bigger]
  double innerCut;                ///< Beam Axis step between first/second radii

  int b4cMat;                 ///< B4c layer material
  int feMat;                  ///< Fe layer material 
  int concMat;                ///< Second layer material

  
  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  RentrantBS(const std::string&);
  RentrantBS(const RentrantBS&);
  RentrantBS& operator=(const RentrantBS&);
  ~RentrantBS() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
 
