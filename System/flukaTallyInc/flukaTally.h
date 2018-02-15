/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/flukaTally.h
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
#ifndef flukaSystem_flukaTally_h
#define flukaSystem_flukaTally_h

namespace flukaSystem
{

/*!
  \class flukaTally
  \version 1.0
  \author S. Ansell
  \date Febryar 2018
  \brief Holds a tally object as a base class
  
*/

class flukaTally
{
 private:
  
  int active;                            ///< Is the tally active
    
 public:
  
  explicit flukaTally(const int);
  flukaTally(const flukaTally&);
  flukaTally& operator=(const flukaTally&);
  virtual flukaTally* clone() const; 
  virtual ~flukaTally();
  
  /// Set active value
  void setActive(const int Flag) { active=(Flag) ? 1 : 0; }
  /// Check is active
  int isActive() const { return active; }
  
  
};

std::ostream&
operator<<(std::ostream&,const flukaTally&);
  
}  // NAMESPACE flukaSystem

#endif
