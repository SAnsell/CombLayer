/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physicsInc/PhysImp.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef physicsSystem_PhysImp_h
#define physicsSystem_PhysImp_h

namespace physicsSystem
{
  template<typename T> class ZoneUnit;
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
  
  std::string fullName;                ///< Type vol,imp etc
  std::string type;                    ///< Type vol,imp etc
  bool defFlag;                        ///< default flag set
  double defValue;                     ///< default value
  std::set<int> particles;             ///< Particle list [mclp]
  std::map<int,double> impNum;         ///< Scale factors etc

 public:

  explicit PhysImp(const std::string&);
  PhysImp(std::string ,const std::string&);
  PhysImp(std::string ,const std::string&,const double);
  PhysImp(const PhysImp&);
  PhysImp& operator=(const PhysImp&);
  ~PhysImp();

  const std::string& getType() const { return type; }
  const std::string& getName() const { return fullName; }

  /// set default value
  void setDefValue(const double V) { defFlag=1; defValue=V; }
  void clear();

  bool isEmpty() const { return impNum.empty(); }
  size_t commonParticles(const PhysImp&) const;
  bool removeParticle(const PhysImp&);
  
  ///< Get particle count
  size_t particleCount() const { return particles.size(); }
  bool removeParticle(const std::string&);
  void setParticles(const std::string&);
  void addParticle(const std::string&);
  bool hasParticle(const std::string&) const;
  bool hasParticle(const int) const;
  std::string getParticles() const;

  void copyValues(const PhysImp&);
  
  double getValue(const int) const;
  void setValue(const int,const double);


  std::vector<int> getCellVector() const;

  void setCell(const int,const double);
  void updateCells(const ZoneUnit<double>&);
  void modifyCells(const std::vector<int>&,const double =1.0);
  void removeCell(const int);
  void renumberCell(const int,const int);

  void write(std::ostream&,const std::set<std::string>&,
	     const std::vector<int>&) const;
  
};

}

#endif
