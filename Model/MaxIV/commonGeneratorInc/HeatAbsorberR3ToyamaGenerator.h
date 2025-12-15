/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/HeatAbsorberR3ToyamaGenerator.h
 *
 * Copyright (c) 2025 by Udo Friman-Gayer
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
#ifndef setVariable_HeatAbsorberR3ToyamaGenerator_h
#define setVariable_HeatAbsorberR3ToyamaGenerator_h

class FuncDataBasae;

namespace setVariable
{

/*!
  \class HeatAbsorberR3ToyamaGenerator
  \version 1.0
  \author Udo Friman-Gayer
  \date February 2025
  \brief HeatAbsorberR3ToyamaGenerator for variables
*/

class HeatAbsorberR3ToyamaGenerator
{
 private:
  // Outer dimensions
  double length;
  double flangeRadius;

  // Connector dimensions
  double connectorInnerRadius;

  // Absorber dimensions
  double absorberLength;
  double absorberWidth;
  double absorberHeight;
  double absorberConnectorLength;

  // Inner dimensions
  double gapWidth;
  double gapMinHeight;
  double gapMaxHeight;

  double inOutRange;
  bool closed;                  ///< open/closed flag

  std::string mainMat;          ///< Main material
  std::string voidMat;          ///< Wall material
  std::string pipeMat;          ///< Pipe material

 public:

  HeatAbsorberR3ToyamaGenerator();
  HeatAbsorberR3ToyamaGenerator(const HeatAbsorberR3ToyamaGenerator&);
  HeatAbsorberR3ToyamaGenerator& operator=(const HeatAbsorberR3ToyamaGenerator&);
  virtual ~HeatAbsorberR3ToyamaGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
