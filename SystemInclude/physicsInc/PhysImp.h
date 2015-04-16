/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   physicsInc/PhysImp.h
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
#ifndef PhysImp_h
#define PhysImp_h

namespace physicsSystem
{

/*!
  \class PhysImp 
  \version 1.0
  \date June 2006
  \version 1.0
  \author S. Ansell
  \brief Holds imp cards
  
  Holds any card which indexes.
  Has a particle list ie imp:n,h nad
  a cell mapping to number. The map is ordered
  prior to being written.
*/

class PhysImp
{
 private:
  
  std::string type;                    ///< Type vol,imp etc
  std::list<std::string> particles;    ///< Particle list (if any)
  std::map<int,double> impNum;         ///< Scale factors etc
  int nCutters;                        ///< number of cutter cells (0 at end)

 public:

  PhysImp();
  explicit PhysImp(const std::string&);
  PhysImp(const PhysImp&);
  PhysImp& operator=(const PhysImp&);
  ~PhysImp();

  void clear();

  int hasElm(const std::string&) const;
  int isType(const std::string&) const;
  ///< Get particle count
  size_t particleCount() const { return particles.size(); } 
  int removeParticle(const std::string&);

  double getValue(const int) const;
  void setValue(const int,const double);

  void addElm(const std::string&);
  void setAllCells(const double =1.0);
  void setAllCells(const std::vector<int>&,const std::vector<double>&);
  void setCells(const std::vector<int>&,const double =1.0);
  void modifyCells(const std::vector<int>&,const double =1.0);
  void removeCell(int const);
  void renumberCell(const int,const int);

  void write(std::ostream&,const std::vector<int>&) const;
  
};

}

#endif
