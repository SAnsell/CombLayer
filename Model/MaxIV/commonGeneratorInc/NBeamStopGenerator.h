/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/NBeamStopGenerator.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef setVariable_NBeamStopGenerator_h
#define setVariable_NBeamStopGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class NBeamStopGenerator
  \version 1.0
  \author Stuart Ansell
  \date June 2020
  \brief NBeamStopGenerator for variables
*/

class NBeamStopGenerator
{
 private:

  double fullLength;                           ///< Total length including void
  std::vector<double> radii;                   ///< radii
  std::vector<std::vector<double>> len;        ///< layer units
  std::vector<std::vector<std::string>> mat;   ///< material         units  

 public:

  NBeamStopGenerator();
  NBeamStopGenerator(const NBeamStopGenerator&);
  NBeamStopGenerator& operator=(const NBeamStopGenerator&);
  virtual ~NBeamStopGenerator();

  void setFullLen(const double L) { fullLength=L;}
  virtual void generateBStop(FuncDataBase&,const std::string&,
			     const double) const;

};

}

#endif
