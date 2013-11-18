/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/NodePoint.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef NodePoint_h
#define NodePoint_h

/*!
 \class NodePoint
 \brief Single Point Node 
 \author S. Ansell
 \version 1.5
 \date Aug 2005

 The class holds a single coordinate node.
 which has two points associated with it.
 The orignal point and the morphed point, along
 with the cell name, cell id and material type 
*/

class NodePoint
{
 private:

  int NodeID;       //!< Node identification number
  Geometry::Vec3D Coord;      //!< Point originally assigned to the node
  Geometry::Vec3D Morph;      //!< Point that morphing has taken place
  double heat;      //!< Heat at point
  int cellID;       //!< current cell ID number (not name) 
  int cellName;     //!< current cell Name
  int matType;      //!< Material type
  double surfDist;  //!< distance to a good surface
  
 public:

  NodePoint();
  NodePoint(const Geometry::Vec3D&,int const=0,double const=0);
  NodePoint(const NodePoint&);
  NodePoint& operator=(const NodePoint&);
  ~NodePoint() {} 
  
  void setPoint(const Geometry::Vec3D&);           
  void setMorph(const Geometry::Vec3D&);
  void setHeat(const double H) { heat=H; }   ///< assessor func. to Heat
  void setCellID(const int N) { cellID=N; }     ///< assessor func. to cellID
  void setCellName(const int N) { cellName=N; } ///< assessor func. to cellName
  void setMatType(const int N) { matType=N; }   ///< assessor func. to MatType
  void scale(const double S) { Coord *= S; }   ///<  Apply scalare Scalar

  Geometry::Vec3D& getPoint() { return Coord; }        ///< Get Original Point (ref)
  Geometry::Vec3D& getMorph() { return Morph; }        ///< Get Morphed Point (ref)
  Geometry::Vec3D getPoint() const { return Coord; }   ///< Get Original Point 
  Geometry::Vec3D getMorph() const { return Morph; }   ///< Get Morphed Point
  int getNodeID() const { return NodeID; }   ///< Get ID
  double getHeat() const { return heat; }    ///< Get total heating
  int getMatType() const { return matType; } ///< Get material type
  int getCellID() const { return cellID; }   ///< Get cell ID number
  int getCellName() const { return cellName; }  ///< Get cell name


  void applyPressure(const Geometry::Vec3D&,double const);
  void applyTransform(const Geometry::Transform&);
  void write(std::ostream&) const;
};

#endif
