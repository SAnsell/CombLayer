/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   visitInc/FEM.h
*
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef FEM_h
#define FEM_h

class Simulation;
namespace MonteCarlo
{
class Object;
}

/*!
  \class FEM
  \brief Hold VTK output format 
  \date August 2010
  \author S. Ansell
  \version 1.0
  \todo Move to a binary/xml output schema

  This allows comparison of the vector for removing non-unique
  Vec3D from a list
*/
						
class FEM
{
 private:
  
  bool lineAverage;           ///< set line average
  
  Geometry::Vec3D Origin;     ///< Origin
  Geometry::Vec3D XYZ;        ///< XYZ extent

  std::set<int> matInventory;  ///< Material inventory
  std::array<size_t,3> nPts;        ///< Number x/y/z points
  multiData<int> matMesh;           ///< Matieral
  multiData<double> rhoCp;          ///< rhoCp
  multiData<double> K;              ///< conductivity
  

  int& getMeshUnit(const size_t,const size_t,const size_t,const size_t);
  double& getRhoUnit(const size_t,const size_t,const size_t,const size_t);
  double& getKUnit(const size_t,const size_t,const size_t,const size_t);
      
  size_t getMaxIndex() const;
  double getResult(const MonteCarlo::Object*) const;

  static size_t procPoint(double&,const double);
  void populateLine(const Simulation&);
  void populatePoint(const Simulation&);
  
 public:

  FEM();
  FEM(const FEM&);
  FEM& operator=(const FEM&);
  ~FEM();

  /// make an average over the line from beginning to end
  void setLineForm() { lineAverage=1; }
  void setBox(const Geometry::Vec3D&,
              const Geometry::Vec3D&);
  void setIndex(const size_t,const size_t,const size_t);

  void populate(const Simulation&);
  
  void writeFEM(const std::string&) const;

};


#endif
