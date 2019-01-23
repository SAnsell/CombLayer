/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   zoomInc/ZoomPrimary.h
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
#ifndef zoomSystem_ZoomPrimary_h
#define zoomSystem_ZoomPrimary_h

class Simulation;


namespace zoomSystem
{
  class ZoomChopper;

/*!
  \class ZoomPrimary
  \version 1.0
  \author S. Ansell
  \date September 2011
  \brief Zoom Collimator unit
  
  The guide is defined along a line that leads from the centre line
  of the shutter. The origin is the point that the shutter centre 
  line impacts the 40000 outer surface
*/

  class ZoomPrimary : public attachSystem::FixedGroup,
    public attachSystem::ContainedComp
{
 private:
  
  int populated;                ///< populated or not

  double length;               ///< Full length
  double height;               ///< Full height
  double depth;                ///< Full height
  double leftWidth;            ///< Full width
  double rightWidth;           ///< Right width

  double cutX;                 ///< Cut X position shift
  double cutZ;                 ///< Cut Z position shift
  double cutWidth;             ///< Inner void width
  double cutHeight;            ///< Cut Height

  size_t nLayers;                 ///< number of layers
  int feMat;                   ///< Iron material
  int innerVoid;               ///< Inner void cell

  std::vector<double> cFrac;  ///< Layer thicknesss (fractions)
  std::vector<int> cMat;      ///< Layer materials
  std::vector<int> CDivideList;   ///< Cell divide List for 


  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedGroup&);
  
  void createSurfaces(const attachSystem::FixedComp&);
  void createObjects(Simulation&);
  void createLinks();
  void layerProcess(Simulation&);

 public:

  ZoomPrimary(const std::string&);
  ZoomPrimary(const ZoomPrimary&);
  ZoomPrimary& operator=(const ZoomPrimary&);
  virtual ~ZoomPrimary();

  void createAll(Simulation&,const attachSystem::FixedGroup&);

  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D&) const;

};

}

#endif
 
