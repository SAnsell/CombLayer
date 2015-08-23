/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   physicsInc/PStandard.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef physicsSystem_SBias_h
#define physicsSystem_SBias_h

namespace physicsSystem
{

/*!
  \class SBais
  \version 1.0
  \date Jully 2010
  \version 1.0
  \author S. Ansell
  \brief Holds cut cards
  
  Holds any card which indexes.
  Has a particle list ie imp:n,h nad
  a cell mapping to number. The map is ordered
  prior to being written.
*/

class SBias : public PhysCard
{
 private:

  std::string particle;              ///< Primary partile to produce
  std::list<std::string> particles;  ///< Particle generators

  std::vector<double> eBound;        ///< Energy boundary [0 to B]
  std::vector<double> sVal;          ///< split value [<1.0 for roulette]
  
 public:

  SBias(const std::string&);
  SBias(const SBias&);
  SBias& operator=(const SBias&);
  SBias* clone() const;
  virtual ~SBias();

  // NOP function from PhysCard
  virtual void setEnergyCut(const double);
  virtual void clear();
  /// Access key
  virtual std::string getKey() const { return KeyName; }   
  virtual bool hasElm(const std::string&) const;
  virtual void addElm(const std::string&);  

  /// Get particle count
  size_t particleCount() const { return particles.size(); } 
  int removeParticle(const std::string&);

  double getValue(const size_t) const;
  void setValue(const size_t,const double);
  void setValues(const size_t,const double =0.0,const double =0.0,
		 const double =0.0,const double =0.0,const double =0.0);
  void setValues(const std::string&);
  void setDef(const size_t);


  void addElmList(const std::string&);

  virtual void write(std::ostream&) const;
  
};

std::ostream&
operator<<(std::ostream&,const SBias&);

}

#endif
