/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/PreModWing.h
 *
 * Copyright (c) 2015-2022 by Konstantin Batkov/Stuart Ansell
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
#ifndef essSystem_PreModWing_h
#define essSystem_PreModWing_h

class Simulation;

namespace essSystem
{

/*!
  \class PreModWing
  \author S. Ansell / K. Batkov
  \version 2.0
  \date August 2016
  \brief Premoderator wing :fills space below pre-mod and H2 start.
*/

class PreModWing : 
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:
  
  double innerHeight;             ///< Mid step [up]
  double outerHeight;             ///< outer step [up]
  double innerDepth;              ///< Mid set [down]
  double outerDepth;              ///< Outer step [down]
  double wallThick;               ///< wall thickness

  double innerRadius;            ///< Start of down curve [-ve if not used]
  double outerRadius;            ///< End of down curve [-ve if not used]
  double innerYCut;              ///< Start from inner shape [makeing overstep]
  
  int mat;                        ///< (water) material
  int wallMat;                    ///< wall material

  size_t nLayers;                 ///< Number of layers
  std::vector<double> layerRadii; ///< Radii for change in material
  std::vector<int> innerMat;      ///< Inner materials [water]
  std::vector<int> surfMat;       ///< Surface material [al]
  
  HeadRule outerSurf;            ///< Outer surface(s)
  
  HeadRule getLayerZone(const size_t) const;
  
  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  PreModWing(const std::string&);
  PreModWing(const PreModWing&);
  PreModWing& operator=(const PreModWing&);
  virtual PreModWing* clone() const;
  ~PreModWing() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
 
