/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/NodeList.h
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
#ifndef NodeList_h
#define NodeList_h

/*!
  \class NodeList
  \version 1.5
  \date Aug 2005
  \author S. Ansell
  \brief Holds the collection of NodePoint 
  
  Holds the node point and associated transform.
  Carries out the morphing and general geometry
  modifications of the node.
  
*/

class NodeList
{
 private:
  
  Geometry::Transform TC;                    ///< Transform coordinate
        
  std::vector<NodePoint> Nodes;    ///< Nodes 

 public:

  NodeList();
  NodeList(const NodeList&);
  NodeList& operator=(const NodeList&);
  ~NodeList();
  
  int readFile(const std::string&);
  int cubeNodes(const Geometry::Vec3D&,const Geometry::Vec3D&,const double);  
  int vecNodes(const Geometry::Vec3D&,const Geometry::Vec3D&,
	       const Geometry::Vec3D&,const Geometry::Vec3D&,
	       const Geometry::Vec3D&); 

  void swapCoords(int const,int const);    
  void clear() { Nodes.clear(); }           ///< clear nodelist
  void setTransform(const Geometry::Transform&); 
  void processTrans();                      
  void write(const std::string&) const;
  double nodeScore(int const) const;
  void applyPressure(const Geometry::Vec3D&,const double);
  int eraseCellID(const std::vector<int>&);
  size_t size() const { return Nodes.size(); }     ///< Get Node count
  void scale(const double);

  // Helper functions.
  /// Begin iterator
  std::vector<NodePoint>::iterator begin() { return Nodes.begin(); }
  /// End iterator 
  std::vector<NodePoint>::iterator end() { return Nodes.end(); }   

  /// Begin iterator
  std::vector<NodePoint>::const_iterator begin() const { return Nodes.begin(); }
  /// End iterator
  std::vector<NodePoint>::const_iterator end() const { return Nodes.end(); }

};


#endif
