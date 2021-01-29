/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physicsInc/FCLControl.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef physicsSystem_FCLControl_h
#define physicsSystem_FCLControl_h

namespace physicsSystem
{

  /*!
  \class FCLControl 
  \version 1.0
  \date December 2020
  \version 1.0
  \author S. Ansell
  \brief FC
  
  Holds any card which indexes.
  Has a particle list ie imp:n,h nad
  a cell mapping to number. The map is ordered
  prior to being written.
*/

class FCLControl
{
 private:

  bool wwgFlag;                        ///< If wwg use -ve fcl value
  std::set<int> particles;             ///< Particle list [mclp]
  std::set<int> cells;                 ///< Cells for FCL

 public:

  FCLControl();
  FCLControl(const FCLControl&);
  FCLControl& operator=(const FCLControl&);
  ~FCLControl();

  void clear();
  ///< Get particle count
  size_t particleCount() const { return particles.size(); }

  void setParticle(const std::string&);
  void addParticle(const std::string&);
  bool hasParticle(const std::string&) const;
  bool removeParticle(const std::string&);

  void setCells(const std::set<int>&,const double =1.0);
  void addCell(const int);
  void renumberCell(const int,const int);

  void write(std::ostream&,
	     const std::vector<int>&) const;
  
};

}

#endif
