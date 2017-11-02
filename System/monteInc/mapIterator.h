/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monteInc/mapIterator.h
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
#ifndef MapSupport_MapIterator_h
#define MapSupport_MapIterator_h

namespace MapSupport
{

/*!
  \class mapIterator
  \version 0.5
  \author S. Ansell
  \date May 2013
  \brief Holds an iteration of a groupd

  This constructs a set of values based on the idea of
  switches. E.g. initial state for 6 keys would be
   ------ then it goes through each pin +/- and run
  2^6 iterations in the cycle
 */

  class mapIterator
{
 private:

  typedef std::map<int,int> MTYPE;

  const size_t N;                ///< Max index number
  size_t index;                  ///< current index

  std::vector<int> keys;         ///< key values
  std::map<int,int> M;           ///< Map of values

  static size_t binaryToGray(const size_t);
  size_t nextGrayIndex() const;
  
  void initSet(const std::set<int>&);
  

 public:
  
  
  mapIterator(const std::set<int>&);
  mapIterator(const mapIterator&);  
  mapIterator& operator=(const mapIterator&);
  ~mapIterator() {}  ///< Destructor
  
  bool operator++();

  /// Accessor
  const std::map<int,int> getM() const { return M; }
  int getItem(const int) const;
  std::string status() const;
};


} // NAMESPACE MapSupport


#endif
