/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVarInc/FlangeDomeGenerator.h
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
#ifndef setVariable_FlangeDomeGenerator_h
#define setVariable_FlangeDomeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class FlangeDomeGenerator
  \version 1.0
  \author S. Ansell
  \date August 2020
  \brief FlangeDomeGenerator for variables
*/

class FlangeDomeGenerator
{
 private:

  double curveRadius;           ///< inner Radius of dome (extent)
  double curveStep;             ///< Step from flat start plane
  double plateThick;            ///< Thickness of plate
  double flangeRadius;          ///< Flange radius 

  std::string voidMat;         ///< inner (Void) material
  std::string mainMat;        ///< Main material

 public:

  FlangeDomeGenerator();
  FlangeDomeGenerator(const FlangeDomeGenerator&);
  FlangeDomeGenerator& operator=(const FlangeDomeGenerator&);
  ~FlangeDomeGenerator();

  /// set main dome sphere:
  void setSphere(const double R,const double S)
  { curveRadius=R; curveStep=S; }
  void setSphere(const double R,const double F,const double S)
  { curveRadius=R; flangeRadius=F,curveStep=S; }
  
  template<typename CF> void setFlangeCF();

  /// setter for material name
  void setMat(const std::string& M, const std::string& IM="Void")
  { mainMat = M; voidMat = IM;  }

  void generateDome(FuncDataBase&,const std::string&) const;

};

}

#endif
