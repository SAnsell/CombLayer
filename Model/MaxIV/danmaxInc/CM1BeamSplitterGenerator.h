/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/CM1BeamSplitterGenerator.h
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
#ifndef setVariable_CM1BeamSplitterGenerator_h
#define setVariable_CM1BeamSplitterGenerator_h

class FuncDataBase;

namespace setVariable
{
/*!
  \class CM1BeamSplitterGenerator
  \version 0.1
  \author U. Friman-Gayer
  \date March 2026
  \brief CM1BeamSplitterGenerator for variables
*/
class CM1BeamSplitterGenerator
{
 private:

  double bodyAngle;
  double width;

  double bottomChamferWidth;
  double bottomWidth;

  double filterHoleOffset;

  double splitterHoleToFilterHole;

  double topOverhangWidth;

  int mode;

 public:

  CM1BeamSplitterGenerator();
  ~CM1BeamSplitterGenerator() = default;

  void setMode(const int m){ mode = m; }

  void generate(
    FuncDataBase&,const std::string&) const;
  
};

}

#endif
 
