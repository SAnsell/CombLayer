/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/HeatAbsorberStepBellows.h
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
#ifndef xraySystem_HeatAbsorberStepBellows_h
#define xraySystem_HeatAbsorberStepBellows_h

class Simulation;

namespace xraySystem
{
/*!
  \class HeatAbsorberStepBellows
  \author U. Friman-Gayer
  \version 1.0
  \date May 2026
  \brief Bellows surrounding Heat Absorber in DanMAX/SINCRYS beamline

  This class has been derived from StepBellows to be able to accept the heat absorber's
  "Closed" and "InOutRange" variables.

  Version history:
  1.0   - 2026-05-07
    - Compatible with StepBellows v1.0
*/
class HeatAbsorberStepBellows final:
  public StepBellows
{
 protected:

  std::string heatAbsorberKey;

  double inOutRange;

  bool heatAbsorberClosed;

  void populate(const FuncDataBase&) override;

 public:

  HeatAbsorberStepBellows(const std::string&, const std::string&);
  HeatAbsorberStepBellows(const HeatAbsorberStepBellows&);
  HeatAbsorberStepBellows& operator=(const HeatAbsorberStepBellows&);
  ~HeatAbsorberStepBellows();
};
}

#endif
