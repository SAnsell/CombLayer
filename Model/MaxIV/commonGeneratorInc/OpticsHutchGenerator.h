/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGeneratorInc/OpticsHutchGenerator.h
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
#ifndef setVariable_OpticsHutchGenerator_h
#define setVariable_OpticsHutchGenerator_h

class FuncDataBase;

namespace setVariable
{
  class XRayHutchBaseGenerator;
/*!
  \class OpticsHutchGenerator
  \version 1.0
  \author S. Ansell
  \date July 2021
  \brief Standard (exept length/chicane) optics hutch for Max IV
*/

class OpticsHutchGenerator :
    public XRayHutchBaseGenerator
{
 protected:
  double backPlateThick;        ///< Back plate thick
  double backPlateWidth;        ///< Back plate width
  double backPlateHeight;       ///< back plate height

  double wallShineThick;        ///< wall shine thickness
  double wallShineLength;       ///< wall shine lenght
  double wallShineOutThick;     ///< thickness of wall shine outside of REW
  double wallShineOutLength;    ///< length of wall shine outside of REW
  double roofShineLength;       ///< Roof shine length
  double roofShineThick;        ///< Roof shine thickness

public:

  OpticsHutchGenerator();
  OpticsHutchGenerator(const OpticsHutchGenerator&) =default;
  OpticsHutchGenerator& operator=(const OpticsHutchGenerator&) =default;
  ~OpticsHutchGenerator() =default;

  /// Set back plate [must be smaller than back wall]
  void setBackPlateSize(const double W,const double H,const double T)
  { backPlateWidth=W;backPlateHeight=H;backPlateThick=T; }

  void generateHut(FuncDataBase&,const std::string&,
		   const double) const;

};

}

#endif
