/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/Importance.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef MonteCarlo_Importance_h
#define MonteCarlo_Importance_h

namespace MonteCarlo
{

/*!
  \class Importance
  \brief Importance table for cells
  \version 1.0
  \date December 2020
  \author S. Ansell

  An object is a collection of Rules and
  surface object
*/

class Importance
{
 private:

  bool zeroImp;      ///< True if all zero 
  bool allSame;      ///< All values same
  double defValue;   ///< Default all value

  std::set<int> particles;   ///< particle list [not = all]
  /// Map of particles (mcplNumber) / importance 
  std::map<int,double> impMap;

 public:

  Importance();
  Importance(const Importance&);
  Importance& operator=(const Importance&);
  ~Importance() {}

  
  void setImp(const double);
  void setImp(const std::string&,const double);

  const std::set<int>& getParticles() const { return particles; }
  std::tuple<bool,double> getAllPair() const;
  double getAllImp() const;
  double getImp(const int) const;
  double getImp(const std::string&) const;
  bool isZeroImp() const { return zeroImp; }
  
  /// Check is a null object
  bool isZero() const { return zeroImp; }
  void write(std::ostream&) const;
};
 
std::ostream& operator<<(std::ostream&,const Importance&);

}

#endif
