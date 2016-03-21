/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/RoofPillars.h
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
#ifndef essSystem_RoofPillars_h
#define essSystem_RoofPillars_h

class Simulation;

namespace essSystem
{
  class Bunker;
  
/*!
  \class RoofPillars
  \version 1.0
  \author S. Ansell
  \date November 2015
  \brief Roof pillar to hold up bunker roof
*/

class RoofPillars : public attachSystem::FixedComp,
  public attachSystem::CellMap
{
 private:
   
  const int rodIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  HeadRule TopSurf;             ///< Top surface  [roof]
  HeadRule BaseSurf;            ///< Base surface [floor]
  /// Relative point for each centre
  std::vector<Geometry::Vec3D> CentPoint;
  
  double radius;                ///< Radius for each support
  int mat;                      ///< Matieral
    
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);
  void insertPillars(Simulation&,const attachSystem::CellMap&);
  
 public:

  RoofPillars(const std::string&);
  RoofPillars(const RoofPillars&);
  RoofPillars& operator=(const RoofPillars&);
  virtual ~RoofPillars();
  
  void setSimpleSurf(const int,const int);
  void setTopSurf(const attachSystem::FixedComp&,const long int);
  void setBaseSurf(const attachSystem::FixedComp&,const long int);

  void createAll(Simulation&,const Bunker&);

};

}

#endif
 

