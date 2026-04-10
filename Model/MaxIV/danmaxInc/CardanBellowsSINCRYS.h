/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   danmax/CardanBellowsSINCRYS.h
 *
 * Copyright (c) 2026 by U. Friman-Gayer
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
#ifndef xraySystem_CardanBellowsSINCRYS_h
#define xraySystem_CardanBellowsSINCRYS_h

class Simulation;

namespace xraySystem
{
/*!
  \class CM1BeamSplitter
  \author U. Friman-Gayer
  \version 1.0
  \date April 2026
  \brief Cardan bellows in SINCRYS beamline

  This class has been derived from SmallAngleBellows to be able to support some CombLayer 
  variables that are specific to the DanMAX/SINCRYS model.

  Version history:
  1.0   - 2026-04-10
*/
class CardanBellowsSINCRYS final:
  public SmallAngleBellows
{
 protected:

  double CM1ToCardanBellowsUpstream;
  double CM2PortLength;
  double SINCRYSAngle;
  double SINCRYSBranchShift;
  double SINCRYSCenterAngle;


  void populate(const FuncDataBase&) override;

 public:

  CardanBellowsSINCRYS(const std::string&);
  CardanBellowsSINCRYS(const CardanBellowsSINCRYS&);
  CardanBellowsSINCRYS& operator=(const CardanBellowsSINCRYS&);
  ~CardanBellowsSINCRYS();
};
}

#endif
