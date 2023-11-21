/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physicsInc/PStandard.h
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
#ifndef physicsSystem_PStandard_h
#define physicsSystem_PStandard_h

namespace physicsSystem
{

/*!
  \class PStandard 
  \version 1.0
  \date Jully 2010
  \version 1.0
  \author S. Ansell
  \brief Holds cut cards
  
  Holds any card which indexes.
  Has a particle list ie cut:n,h nad
  a cell mapping to number. The map is ordered
  prior to being written.
*/

class PStandard : public PhysCard
{
 private:

  std::string KeyName;               ///< NameType e.g. cut/phys
  std::list<std::string> particles;  ///< Particle list (if any)
  std::array<int,5> defFlag;         ///< Default values
  std::array<double,5> vals;         ///< Values

 public:

  PStandard(std::string );
  PStandard(const PStandard&);
  PStandard& operator=(const PStandard&);
  PStandard* clone() const override;
  ~PStandard() override;

  // NOP function from PhysCard
  void setEnergyCut(const double) override;
  void clear() override;
  /// Access key
  std::string getKey() const override { return KeyName; }   
  bool hasElm(const std::string&) const override;
  void addElm(const std::string&) override;  

  /// Get particle count
  size_t particleCount() const { return particles.size(); } 
  /// Access to particles
  const std::list<std::string>& getParticles() const
    { return particles; }
  int removeParticle(const std::string&);

  double getValue(const size_t) const;
  void setValue(const size_t,const double);
  void setValues(const size_t,const double =0.0,const double =0.0,
		 const double =0.0,const double =0.0,const double =0.0);
  void setValues(const std::string&);
  void setDef(const size_t);

  void addElmList(const std::string&);

  void write(std::ostream&) const override;
  
};

std::ostream&
operator<<(std::ostream&,const PStandard&);

}

#endif
