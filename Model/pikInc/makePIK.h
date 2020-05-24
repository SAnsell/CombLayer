/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   pikInc/makePIK.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#ifndef pikSystem_makePIK_h
#define pikSystem_makePIK_h

namespace pikSystem
{
  class PIKPool;
  class PIKReflector;
  /*!
    \class makePIK
    \version 1.0
    \author K. Batkov
    \date May 2020
    \brief PIK reactor model
  */

class makePIK
{
 private:
  std::shared_ptr<pikSystem::PIKPool> pool;         ///< Pool
  std::shared_ptr<pikSystem::PIKReflector> refl;    ///< Reflector

  bool buildFuelElements(Simulation&, const mainSystem::inputParam&);

 public:

  makePIK();
  makePIK(const makePIK&);
  makePIK& operator=(const makePIK&);
  ~makePIK();

  void build(Simulation&,const mainSystem::inputParam&);

};

}

#endif