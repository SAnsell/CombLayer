/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVarInc/FlangePlateGenerator.h
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
#ifndef setVariable_FlangePlateGenerator_h
#define setVariable_FlangePlateGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class FlangePlateGenerator
  \version 1.0
  \author S. Ansell
  \date August 2020
  \brief FlangePlateGenerator for variables
*/

class FlangePlateGenerator
{
 private:

  double innerRadius;           ///< Inner window (void) if present
  double flangeRadius;          ///< void radius [inner]
  double flangeLength;          ///< Flange thickness

  std::string innerMat;         ///< inner (Void) material
  std::string flangeMat;        ///< Main material

 public:

  FlangePlateGenerator();
  FlangePlateGenerator(const FlangePlateGenerator&);
  FlangePlateGenerator& operator=(const FlangePlateGenerator&);
  ~FlangePlateGenerator();

  void setFlange(const double,const double);
  void setFlangeLen(const double);

  template<typename CF> void setCF(const double);
  template<typename CF> void setFlangeCF();

  /// setter for material name
  void setMat(const std::string& M, const std::string& IM="Void")
  { flangeMat = M; innerMat = IM;  }

  void generateFlangePlate(FuncDataBase&,const std::string&) const;

};

}

#endif
