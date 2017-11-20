/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/tmeshTally.h
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
#ifndef tallySystem_tmeshTally_h
#define tallySystem_tmeshTally_h


class pairRange;

namespace tallySystem
{

  
/*!
  \class tmeshTally
  \version 1.0
  \date July 2010
  \author S. Ansell
  \brief Mesh tally 
*/

class tmeshTally : public Tally
{
 private:
  
  int typeID;                    ///< type of tally [1/2/3]
  std::string keyWords;          ///< KeyWord list
  std::vector<double> kIndex;    ///< KeyIndex values

  int activeMSHMF;               ///< Flag for active mshmf
  pairRange mshmf;               ///< Mesh MF card

  int requireRotation;           ///< rotation to the mesh
  
  std::array<size_t,3> Pts;      ///< Points
  Geometry::Vec3D minCoord;      ///< Min coordinate
  Geometry::Vec3D maxCoord;      ///< Max coordinate
  
 public:

  explicit tmeshTally(const int);
  tmeshTally(const tmeshTally&);
  virtual tmeshTally* clone() const; 
  tmeshTally& operator=(const tmeshTally&);
  virtual ~tmeshTally();

  virtual std::string className() const
    { return "TMeshTally"; }

  void setType(const int);
  void setKeyWords(const std::string&);
  void setIndexLine(std::string);
  void setRot() { requireRotation=1; }   ///< Set rotations
  void setIndex(const std::array<size_t,3>&);
  void setCoordinates(const Geometry::Vec3D&,const Geometry::Vec3D&);
  void setResponse(const std::string&);

  /// Access the centre
  Geometry::Vec3D getCentre() const { return (minCoord+maxCoord)/2.0; }
  /// access min/max point
  const Geometry::Vec3D& getMinPt() const { return minCoord; }
  /// access min/max point
  const Geometry::Vec3D& getMaxPt() const { return maxCoord; }
  /// access min/max point
  const std::array<size_t,3>& getNPt() const { return Pts; }
  int hasActiveMSHMF() { return activeMSHMF; }
  void setActiveMSHMF(const int A) { activeMSHMF=A; }
  /// accessor to pairRange
  const pairRange& getMSHMF() const { return mshmf; }
  
  virtual void rotateMaster();
  int addLine(const std::string&);
  void writeCoordinates(std::ostream&) const;
  virtual void write(std::ostream&) const;
  
};

}

#endif
