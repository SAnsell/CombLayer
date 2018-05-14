/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/WWG.h
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
#ifndef WeightSystem_WWG_h
#define WeightSystem_WWG_h

namespace attachSystem
{
  class FixedComp;
}

class Simulation;

namespace WeightSystem
{
  class WWGWeight;
  /*!
    \class WWG 
    \author S. Ansell
    \version 1.0
    \date Jan 2016
    \brief Storage for WWG output
  */
  
class WWG 
{
 private:

  std::set<std::string> pType;   ///< Particle type
  double wupn;         ///< Max weight before upsplitting
  double wsurv;        ///< survival possiblitiy
  int maxsp;           ///< max split
  int mwhere;          ///< Check weight -1:col 0:all 1:surf
  int mtime;           ///< Flag to inditace energy(0)/time(1)
  int switchn;         ///< read from wwinp file
  
  std::vector<double> EBin;      ///< Energy bins
  Geometry::Mesh3D Grid;         ///< Mesh Grid

  /// linearized centre point [x,y,z order]
  std::vector<Geometry::Vec3D> GridMidPt;
  /// final output weight mesh
  WWGWeight WMesh;
    
  void writeHead(std::ostream&) const;
  
 public:

  
  WWG();
  WWG(const WWG&);
  WWG& operator=(const WWG&);
  ~WWG() {}
  
  /// access to grid
  Geometry::Mesh3D& getGrid() { return Grid; }
  /// access to grid
  const Geometry::Mesh3D& getGrid() const { return Grid; }
  /// get grid mid point
  const std::vector<Geometry::Vec3D>& getMidPoints() const
    { return GridMidPt; }

  void setParticles(const std::set<std::string>&);
  /// Access to EBin
  const std::vector<double>& getEBin() const { return EBin; }
  void setEnergyBin(const std::vector<double>&,
		    const std::vector<double>&);
  void resetMesh(const std::vector<double>&);


  void setRefPoint(const Geometry::Vec3D&);
  void scaleMeshItem(const size_t,const size_t,const size_t,
		     const size_t,const double);
  void calcGridMidPoints();
  void updateWM(const WWGWeight&,const double);
  void scaleRange(const double,const double,const double);
  void powerRange(const double);

  void write(std::ostream&) const;
  void writeWWINP(const std::string&) const;
  void writeVTK(const std::string&,const long int =0) const;


  
};
 

}


#endif
 
