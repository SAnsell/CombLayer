/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/fmeshTally.h
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
#ifndef tallySystem_fmeshTally_h
#define tallySystem_fmeshTally_h

namespace tallySystem
{
/*!
  \class fmeshTally
  \version 1.0
  \date May 2016
  \author S. Ansell
  \brief fmesh tally 
*/

class fmeshTally : public Tally
{
 private:

  std::set<std::string> keyWords;  ///< active-keywords

  std::string geomType;            ///< Geometry type [GType]
  Geometry::Vec3D Origin;          ///< Origin position [manditory]
  Geometry::Vec3D Axis;            ///< Centre Axis of Cyl mesh
  Geometry::Vec3D theta0;          ///< Theta-0 Axis for Cyl mesh
  
  // MESH
  std::vector<double> X;     ///< X/R coordinates [+origin]
  std::vector<double> Y;     ///< Y/Z/phi coordinates [+origin]
  std::vector<double> Z;     ///< Z/theta coordinates [+origin]

  std::vector<size_t> XFine;    ///< Number of fine X bins 
  std::vector<size_t> YFine;    ///< Number of fine Y bins 
  std::vector<size_t> ZFine;    ///< Number of fine z bins 

  size_t NX;                    ///< Total number of X
  size_t NY;                    ///< Total number of Y
  size_t NZ;                    ///< Total number of Z

  int requireRotation;           ///< rotation to the mesh
  
  void writeMesh(std::ostream&) const;
  
 public:

  explicit fmeshTally(const int);
  fmeshTally(const fmeshTally&);
  virtual fmeshTally* clone() const; 
  fmeshTally& operator=(const fmeshTally&);
  virtual ~fmeshTally();
  
  void setType(const int);
  void setKeyWords(const std::string&);
  void setIndexLine(std::string);
  void setRot() { requireRotation=1; }   ///< Set rotations
  void setIndex(const size_t*);
  void setCoordinates(const Geometry::Vec3D&,const Geometry::Vec3D&);
  void setResponse(const std::string&);

 
  virtual void rotateMaster();
  int addLine(const std::string&);
  void writeCoordinates(std::ostream&) const;
  virtual void write(std::ostream&) const;
  
};

}

#endif
