/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/RadNodes.h
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
#ifndef RadNodes_h
#define RadNodes_h

/*!
  \class RadNodes
  \version 1.0
  \date September 2008
  \author S.Ansell
  \brief Contains the information on the radiation Nodes
*/


class RadNodes : public Simulation
{
 private:

  static MTRand RNG;                    ///< Random number generator

  int aimMat;                           ///< aim Material
  Radiation Grid;                       ///< Radiation Grid
  NodeList ND;                          ///< List of nodes

  double pScale;                        ///< pressure scale
  double bestScore;                     ///< bestScore
  double curScore;                      ///< currentScore
  double temp;                          ///< temperature  (of bath)

  int applyMorph(const double);        ///< Allow cell points to move
  int acceptNewPos(const double);      
  
 public:

  RadNodes();
  RadNodes(const RadNodes&);
  RadNodes& operator=(const RadNodes&);
  ~RadNodes();

  /// Set the current Grid temperature ??
  void setCurrent(const double C) { Grid.setCurrent(C); }

  void readRadiation(const std::string&);
  void readNodes(const std::string&);
  void setNodes(const std::vector<int>&,const Geometry::Vec3D&,const Geometry::Vec3D&,const double);
  void setNodes(const int,const double);            
  void scaleNodes(const double);          ///< Scale the node points 
  /// Swap coordinates in the note
  void swapNodeCoords(const int A,const int B) { ND.swapCoords(A,B); }

  /// carry out transform on all nodes 
  void processNodes(const std::string&,       
		    const std::string&);

  int calcVertex(const int);           ///< calc the vertexes for the cell

  void setupMorphFit(const double,const double);
  void setAimMat(const int A) { aimMat=A; }    ///< sets the aim material
  void makeNodeType();                         ///< determine material of nodes
  double NodeScore() const;           ///< determine the score for the missing nodes            
  std::pair<Geometry::Vec3D,Geometry::Vec3D> getMaxNodeSize() const;
  int processMorph(const int);         ///< Iterate the pressure point steps


  void reZeroFromVertex(const int,const unsigned int,const unsigned int,
			const unsigned int,const unsigned int,
			Geometry::Vec3D&,Geometry::Matrix<double>&);


  void writeNodes(std::ostream&,const Geometry::Vec3D&,
		  const Geometry::Matrix<double>&);
  // Debug stuff
  
  void printVertex(const int) const;
  void printNodes();
  double getHeat(const Geometry::Vec3D&) const;
  void sumCells(const int,const int);             
  double heatSum() const;              ///< Sum the heat in the cell (not by volume)
  double sumNodes() const;

};

#endif
