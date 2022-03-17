/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/WWG.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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

  typedef std::map<std::string,WWGWeight*> MeshTYPE;
  
  double wupn;                   ///< Max weight before upsplitting
  double wsurv;                  ///< survival possiblitiy
  int maxsp;                     ///< max split
  int mwhere;                    ///< Check weight -1:col 0:all 1:surf
  int mtime;                     ///< Flag to inditace energy(0)/time(1)
  int switchn;                   ///< read from wwinp file
  Geometry::Vec3D refPt;         ///< Reference point

  std::string defUnit;           ///< Default unit
  MeshTYPE WMeshMap;             /// Map of meshes
  
  void writeHead(std::ostream&) const;
  
 public:

  WWG();
  WWG(const WWG&);
  WWG& operator=(const WWG&);
  ~WWG();

  bool hasMesh(const std::string&) const;
  
  WWGWeight& getMesh(const std::string&); 
  const WWGWeight& getMesh(const std::string&) const;

  /// accessor to default name
  const std::string& getDefUnit() const { return defUnit; }
  

  WWGWeight& createMesh(const std::string&);
  WWGWeight& copyMesh(const std::string&,const std::string&);
  
  void setEnergyBin(const std::string&,const std::vector<double>&);
  void setDefValue(const std::string&,const std::vector<double>&);
  void setRefPoint(const Geometry::Vec3D&);

  void powerRange(const std::string&,const double);
  void scaleMesh(const std::string&,const double);
  void scaleMeshItem(const std::string&,
		     const size_t,const size_t,const size_t,
		     const size_t,const double);
  
  void resetMesh(const std::vector<double>&);


  void calcGridMidPoints();



  void writeWWINP(const std::string&) const;
  void writeVTK(const std::string&,const bool,
		const std::string&,const long int =0) const;

  void write(std::ostream&) const;
  void writePHITS(std::ostream&) const;
  void writeFLUKA(std::ostream&) const;


  
};
 

}


#endif
 
