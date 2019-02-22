/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/EPSeparatorGenerator.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef setVariable_EPSeparatorGenerator_h
#define setVariable_EPSeparatorGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class EPSeparatorGenerator
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief EPSeparatorGenerator for variables
*/

class EPSeparatorGenerator 
{
 private:

  double length;                ///< frame length
  double photonOuterGap;        ///< Gap to outer radius
  double photonRadius;          ///< photon inner radius
  double photonAGap;            ///< Initial photon gap
  double photonBGap;            ///< Final photon gap  
  double electronRadius;        ///< Electron radius

  double initEPSeparation;       ///< initial separation between e / p

  double wallPhoton;            ///< Extra on photon side 
  double wallElectron;          ///< Extra on elextron side
  double wallHeight;            ///< Extra above/below 

  double electronAngle;         ///< XY angle of electron beam to proton
  
  double portAPipeRadius;   ///< Front port pipe radius
  double portAFlangeRadius; ///< Front port flange radius
  double portAThick;        ///< In Port

  double portBPipeRadius;   ///< Out Port inner radius
  double portBWall;         ///< Out Port wall thickness
  double portBFlangeRadius; ///< Out Port inner radius
  double portBLen;          ///< Out Port in step length
  double portBThick;        ///< Out Port y-thick

  
  std::string voidMat;                     ///< void material
  std::string wallMat;                     ///< wall material
  std::string portMat;                     ///< Pipe/flange material
  
 public:

  EPSeparatorGenerator();
  EPSeparatorGenerator(const EPSeparatorGenerator&);
  EPSeparatorGenerator& operator=(const EPSeparatorGenerator&);
  virtual ~EPSeparatorGenerator();
  
  
  virtual void generatePipe(FuncDataBase&,const std::string&,
			    const double) const;

};

}

#endif
 
