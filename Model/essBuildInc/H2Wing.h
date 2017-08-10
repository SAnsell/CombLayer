/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/H2Wing.h
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
#ifndef essSystem_H2Wing_h
#define essSystem_H2Wing_h

class Simulation;


namespace essSystem
{
  class H2FlowGuide;
  
/*!
  \class H2Wing
  \version 1.0
  \author S. Ansell
  \date April 2015 
  \brief H2Wing component in the butterfly moderator
*/

class H2Wing : 
  public attachSystem::ContainedComp,
  public attachSystem::LayerComp,
  public attachSystem::FixedComp,
  public attachSystem::CellMap
{
 private:

  const std::string baseName; ///< Basename
  const int wingIndex;       ///< Index of surface offset
  int cellIndex;             ///< Cell index

  int engActive;             ///< Engineering active
  std::shared_ptr<H2FlowGuide> InnerComp;    ///< Inner flow components
  
  double xStep;                 ///< Step across proton beam direction
  double yStep;                 ///< Step along proton beam direction
  double xyOffset;              ///< xy-Angle offset

  std::array<Geometry::Vec3D,3> Pts;    ///< Corner Points
  std::array<double,3> radius;  ///< corner radii
  double height;                ///< height of moderator cell
  double totalHeight;           ///< total height moderator
  
  int modMat;                   ///< LH2
  double modTemp;               ///< LH2 temperature [K]

  std::vector<double> Thick;        ///< Layer thickness
  std::vector<double> layerHeight;  ///< Layer veritcal 
  std::vector<double> layerDepth;   ///< layer depth
  std::vector<int> mat;             ///< Layer material
  std::vector<double> temp;         ///< Layer temperature


  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  Geometry::Vec3D realPt(const Geometry::Vec3D&) const;
  Geometry::Vec3D realAxis(const Geometry::Vec3D&) const;
  Geometry::Vec3D midNorm(const size_t) const;

  void cornerSet(const double,std::array<Geometry::Vec3D,3>&,
		 std::array<Geometry::Vec3D,3>&) const;
  

 public:

  H2Wing(const std::string&,const std::string&,const double);
  H2Wing(const H2Wing&);
  H2Wing& operator=(const H2Wing&);
  virtual H2Wing* clone() const;
  virtual ~H2Wing();

  virtual Geometry::Vec3D getSurfacePoint(const size_t,const long int) const;
  virtual std::string getLayerString(const size_t,const long int) const;
  virtual int getLayerSurf(const size_t,const long int) const;
  void createAll(Simulation&,const attachSystem::FixedComp&);
};

}

#endif
 
