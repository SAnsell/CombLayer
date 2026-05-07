/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/WhiteBeamStopGenerator.h
 *
 * Copyright (c) 2004-2026 by Konstantin Batkov
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
#ifndef setVariable_WhiteBeamStopGenerator_h
#define setVariable_WhiteBeamStopGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class WhiteBeamStopGenerator
  \version 1.0
  \author Konstantin Batkov
  \date March 2026
  \brief WhiteBeamStopGenerator for variables
*/

class WhiteBeamStopGenerator
{
 private:

  double length;                ///< Total length including void
  double width;                 ///< Angle-invariant width
  double height;                ///< Height
  double angle;                 ///< Angle between the incident beam and the beam target surface (#3)

  std::string mat;              ///< Beam stop material

 public:

  WhiteBeamStopGenerator();
  WhiteBeamStopGenerator(const WhiteBeamStopGenerator&);
  WhiteBeamStopGenerator& operator=(const WhiteBeamStopGenerator&);
  virtual ~WhiteBeamStopGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
