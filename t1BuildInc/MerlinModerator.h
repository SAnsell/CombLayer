/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/MerlinModerator.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef moderatorSystem_MerlinModerator_h
#define moderatorSystem_MerlinModerator_h

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
    public attachSystem::FixedComp
{
 private:
  
  const int merlinIndex;        ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on Y to Target [+ve forward]
  double zStep;                 ///< Offset on Z top Target

  double xyAngle;               ///< Angle [degrees]
  double width;                 ///< Left/Right size 
  double depth;                 ///< length down target
  double height;                ///< Vertical size 
  double innerThick;            ///< Al thickness (out)
  double vacThick;              ///< Outer void

  size_t nPoison;                  ///< Number of poison layers
  size_t vaneSide;                 ///< Side for vanes 0 -- none :P 
  std::vector<double> poisonYStep; ///< Poison Offset to origin
  std::vector<double> poisonThick; ///< Poison (Gadolinium) thickness

  int alMat;                    ///< Al
  int waterMat;                   ///< water
  int poisonMat;                  ///< Poison (Gadolinium)  

  std::map<size_t,double> modLayer;  ///< Surface modification layer
  int mainCell;                      ///< Main water cell

  void applyModification();
  Geometry::Vec3D getSurfacePoint(const size_t,const size_t) const;
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void createVanes(Simulation&);

 public:

  MerlinModerator(const std::string&);
  MerlinModerator(const MerlinModerator&);
  MerlinModerator& operator=(const MerlinModerator&);
  virtual ~MerlinModerator();

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
