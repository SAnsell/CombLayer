/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   physicsInc/ModeCard.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef ModeCard_h
#define ModeCard_h

namespace physicsSystem
{

/*!
  \class ModeCard 
  \version 1.0
  \date November 2010
  \version 1.0
  \author S. Ansell
  \brief Mode card [master particle list]

  Keeps the master particle list:
  - verification
  - addition
  
*/

class ModeCard
{
 private:

  std::list<std::string> particles;    ///< Particle list (if any)

 public:

  ModeCard();
  explicit ModeCard(const std::string&);
  ModeCard(const ModeCard&);
  ModeCard& operator=(const ModeCard&);
  ~ModeCard();

  void clear();

  /// Has been set
  bool isSet() const { return !particles.empty(); }
  /// Get particle count
  size_t particleCount() const { return particles.size(); } 

  int hasElm(const std::string&) const;
  int removeParticle(const std::string&);

  void addElm(const std::string&);

  void write(std::ostream&) const;  
};

}

#endif
