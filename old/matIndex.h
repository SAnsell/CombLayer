/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/matIndex.h
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
#ifndef ModelSupport_matIndex_h
#define ModelSupport_matIndex_h

namespace MonteCarlo
{
  class Material;
}

namespace ModelSupport
{

/*!
  \class matIndex 
  \version 1.0
  \author S. Ansell
  \date December 2009
  \brief Control surfaces for rotation / touches
*/

class matIndex
{
 public:

  /// Storage type
  typedef std::map<std::string,MonteCarlo::Material*> MTYPE;
  
 private:
 
  MTYPE MMap;              ///< Index of kept surfaces

  matIndex();
  ///\cond SINGLETON
  matIndex(const matIndex&);
  matIndex& operator=(const matIndex&);
  ///\endcond SINGLETON

 public:
  
  static matIndex& Instance();
  
  ~matIndex();

  MonteCarlo::Material* addMaterial(MonteCarlo::Material*);
  MonteCarlo::Material* cloneMaterial(const MonteCarlo::Material*);
  MonteCarlo::Material* createMaterial(const std::string&);
  
  MonteCarlo::Material* getMat(const std::string&) const; 

  /// access to map
  const MTYPE& matMap() const { return MMap; }

  void createUniqID();
  void writeMCNPX(std::ostream&) const;

};

}

#endif
