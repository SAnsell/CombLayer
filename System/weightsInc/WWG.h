/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/WWG.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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

  
class WWG 
{
 public:

  static void writeLine(std::ostream&,const double,size_t&);
  static void writeLine(std::ostream&,const int,size_t&);

 private:

  char ptype;          ///< Particle type
  double wupn;         ///< Max weight before upsplitting
  double wsurv;        ///< survival possiblitiy
  int maxsp;           ///< max split
  int mwhere;          ///< Check weight -1:col 0:all 1:surf
  int mtime;           ///< Flag to inditace energy(0)/time(1)
  int switchn;         ///< read from wwinp file
  
  std::vector<double> EBin;      ///< Energy bins
  WeightMesh Grid;               ///< Mesh Grid
  std::vector<double> WMesh;     ///< linearized weight mesh

  void writeHead(std::ostream&) const;
  
 public:

  
  WWG();
  WWG(const WWG&);
  WWG& operator=(const WWG&);

  /// access to grid
  WeightMesh& getGrid() { return Grid; }
  /// access to grid
  const WeightMesh& getGrid() const { return Grid; }

  void setEnergyBin(const std::vector<double>&);
  
  void write(std::ostream&) const;
  void writeWWINP(const std::string&) const;
  
};
 

}


#endif
 
