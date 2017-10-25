/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chipInc/InnerWall.h
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
#ifndef hutchSystem_InnerWall_h
#define hutchSystem_InnerWall_h

class Simulation;

namespace hutchSystem
{

/*!
  \class InnerWall
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief InnerWall [insert object]
*/

class InnerWall : 
  public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:
  
  const int innerIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index
 
  double height;            ///< Hole: Half Height
  double width;             ///< Hole: Half width
  double depth;             ///< Full depth

  int defMat;               ///< Material for roof
  
  size_t nLayers;                  ///< number of layers
  std::vector<double> cFrac;    ///< coll Layer thicknesss (fractions)
  std::vector<int> cMat;        ///< coll Layer materials

  std::vector<int> CDivideList;   ///< Cell divide List for 



  void populate(const FuncDataBase&);
  void createUnitVector(const FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void layerProcess(Simulation&);

 public:

  InnerWall(const std::string&);
  InnerWall(const InnerWall&);
  InnerWall& operator=(const InnerWall&);
  ~InnerWall();

  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D&) const;

  //  void createObj(Simulation&);
  void createOnlySurfaces(Simulation&,const attachSystem::FixedComp&,
			  const long int);
  void createOnlyObjects(Simulation&);
  //  void createObj(Simulation&);
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
