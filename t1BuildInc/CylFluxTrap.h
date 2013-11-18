/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/CylFluxTrap.h
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
#ifndef TMRSystem_CylFluxTrap_h
#define TMRSystem_CylFluxTrap_h

class Simulation;

namespace ts1System
{
  class ProtonVoid;
  class BeamWindow;
}

namespace TMRSystem
{
/*!
  \class CylFluxTrap
  \version 1.0
  \author S. Ansell
  \date June 2013
  \brief Extension of the Cylindertartet

  Provides linkage to its outside on FixedComp[0]
*/

class CylFluxTrap :  public TS2target
{
 private:
  
  const int fluxIndex;          ///< Index of surface offset
  const std::string molyKey;    ///< moly key name
  int cellIndex;                ///< Cell index

  // PLATES:
  std::vector<double> plateYDist;    ///< Simple plates Distances
  std::vector<double> plateYThick;   ///< Simple plate thicknesses
  std::vector<int> plateMat;         ///< Plate material

  double centLen;                    ///< Centre length
  double forwardLen;                 ///< Centre length

  
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);
  void createSurfaces();
  void createObjects(Simulation&);

 public:

  CylFluxTrap(const std::string&,const std::string&);
  CylFluxTrap(const CylFluxTrap&);
  CylFluxTrap& operator=(const CylFluxTrap&);
  CylFluxTrap* clone() const;
  virtual ~CylFluxTrap();

  virtual void createAll(Simulation&,const attachSystem::FixedComp&);
};

}

#endif
 
