/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1BuildInc/MerlinModerator.h
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
#ifndef t1System_MerlinModerator_h
#define t1System_MerlinModerator_h

class Simulation;


namespace ts1System
{

/*!
  \class MerlinModerator
  \version 1.0
  \author G Skoro/S. Ansell
  \date September 2012
  \brief TS1 t1MerlinModerator [insert object]
*/

class MerlinModerator : public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:
  
  double width;                 ///< Left/Right size 
  double depth;                 ///< length down target
  double height;                ///< Vertical size 
  double innerThick;            ///< Al thickness (out)
  double vacThick;              ///< Outer void

  size_t nPoison;                  ///< Number of poison layers
  long int vaneSide;               ///< Side for vanes 0 -- none :P 
  std::vector<double> poisonYStep; ///< Poison Offset to origin
  std::vector<double> poisonThick; ///< Poison (Gadolinium) thickness

  int alMat;                    ///< Al
  int waterMat;                   ///< water
  int poisonMat;                  ///< Poison (Gadolinium)  

  std::map<size_t,double> modLayer;  ///< Surface modification layer
  int mainCell;                      ///< Main water cell

  void applyModification();
  Geometry::Vec3D getSurfacePoint(const size_t,const long int) const;
  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void createVanes(Simulation&);

 public:

  MerlinModerator(const std::string&);
  MerlinModerator(const MerlinModerator&);
  MerlinModerator& operator=(const MerlinModerator&);
  virtual ~MerlinModerator();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
