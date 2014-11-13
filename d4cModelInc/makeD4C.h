/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   d4cModelInc/makeD4C.h
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
#ifndef d4cSystem_makeD4C_h
#define d4cSystem_makeD4C_h

namespace instrumentSystem
{
  class CylSample;
}

namespace d4cSystem
{
  class BellJar;
  class DetectorArray;
  class DetectorBank;
  /*!
    \class makeD4C
    \version 1.0
    \author S. Ansell
    \date October 2012
    \brief main builder for d4c model
  */

class makeD4C
{
 private:

  typedef std::shared_ptr<DetectorBank> DTYPE;

  const size_t NDet;
  /// Outer bell jar
  std::shared_ptr<BellJar> BellObj;
  /// Cell object
  std::shared_ptr<instrumentSystem::CylSample> CellObj;
  /// Detector array object [9 objects]
  DTYPE DetObj[9];

  void setMaterials(const mainSystem::inputParam& IParam);

 public:
  
  makeD4C();
  makeD4C(const makeD4C&);
  makeD4C& operator=(const makeD4C&);
  ~makeD4C();
  
  void build(Simulation*,const mainSystem::inputParam&);
  
};

}

#endif
