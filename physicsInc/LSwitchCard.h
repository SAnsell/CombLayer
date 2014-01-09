/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   physicsInc/LSwitchCard.h
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
#ifndef physicsSystem_LSwitchCard_h
#define physicsSystem_LSwitchCard_h

namespace physicsSystem
{

/*!
  \class LSwitchCard 
  \version 1.0
  \date July 2010
  \version 1.0
  \author S. Ansell
  \brief Holds cut cards
  
  Holds any card which indexes.
  Has a particle list ie imp:n,h nad
  a cell mapping to number. The map is ordered
  prior to being written.
*/

class LSwitchCard
{
 private:

  size_t lcaActive;         ///< Number active for lca
  size_t leaActive;         ///< Number active for lea
  size_t lcbActive;         ///< Number active for lcb

  std::vector<int> lcaVal;      ///< Values for lca
  std::vector<int> leaVal;      ///< Values for lea
  std::vector<int> lcbVal;      ///< Values for lcb

  size_t& getActive(const std::string&);
  std::vector<int>& getVec(const std::string&);

  size_t getActive(const std::string&) const;
  const std::vector<int>& getVec(const std::string&) const;

 public:

  LSwitchCard();
  LSwitchCard(const LSwitchCard&);
  LSwitchCard& operator=(const LSwitchCard&);
  ~LSwitchCard();

  /// Access key
  int getValue(const std::string&,const size_t) const;
  void setValue(const std::string&,const size_t,const int);

  void setValues(const std::string&,const std::string&);

  void write(std::ostream&) const;
  
};

std::ostream&
operator<<(std::ostream&,const LSwitchCard&);

}

#endif
