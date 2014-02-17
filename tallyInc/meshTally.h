/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tallyInc/meshTally.h
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
#ifndef tallySystem_meshTally_h
#define tallySystem_meshTally_h

namespace tallySystem
{
/*!
  \class meshTally
  \version 1.0
  \date July 2010
  \author S. Ansell
  \brief Mesh tally 
*/

class meshTally : public Tally
{
 private:
  
  int typeID;                    ///< type of tally [1/2/3]
  std::string keyWords;          ///< KeyWord list
  std::vector<double> kIndex;    ///< KeyIndex values
  pairRange mshmf;               ///< Mesh MF card

  int requireRotation;           ///< rotation to the mesh
  
  Triple<int> Pts;               ///< Points
  Geometry::Vec3D minCoord;      ///< Min coordinate
  Geometry::Vec3D maxCoord;      ///< Max coordinate
  
 public:

  explicit meshTally(const int);
  meshTally(const meshTally&);
  virtual meshTally* clone() const; 
  meshTally& operator=(const meshTally&);
  virtual ~meshTally();
  
  void setType(const int);
  void setKeyWords(const std::string&);
  void setIndexLine(std::string);
  void setRot() { requireRotation=1; }   ///< Set rotations
  void setIndex(const int*);
  void setCoordinates(const Geometry::Vec3D&,const Geometry::Vec3D&);
  void setResponse(const std::string&);

  /// Access the centre
  Geometry::Vec3D getCentre() const { return (minCoord+maxCoord)/2.0; }
  
  virtual void rotateMaster();
  int addLine(const std::string&);
  virtual void write(std::ostream&) const;
  
};

}

#endif
