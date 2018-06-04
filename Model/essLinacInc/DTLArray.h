/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/DTLArray.h
 *
 * Copyright (c) 2018 by Konstantin Batkov
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
#ifndef essSystem_DTLArray_h
#define essSystem_DTLArray_h

class Simulation;

namespace essSystem
{
  class DTL;
/*!
  \class DTLArray
  \version 1.0
  \author Konstantin Batkov
  \date 4 Jun 2018
  \brief Array of DTL tanks
*/

class DTLArray : public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:
  const std::string baseName;    ///< Base name
  int nDTL;                ///< Number of DTL tanks
  std::vector<std::shared_ptr<DTL> > dtl; ///< array of DTL sections
  
  void populate(const FuncDataBase&);
  void createLinks();

 public:

  DTLArray(const std::string&,const std::string&);
  DTLArray(const DTLArray&);
  DTLArray& operator=(const DTLArray&);
  virtual DTLArray* clone() const;
  virtual ~DTLArray();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
 

