/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVarInc/FocusGenerator.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef essSystem_FocusGenerator_h
#define essSystem_FocusGenerator_h

class Simulation;

namespace setVariable
{

/*!
  \class FocusGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief FocusGenerator for variables

  Part of the simiplification process to produce a simpler variable 
  initialisation.
*/

class FocusGenerator
{
 private:


  int yStepActive;         ///< Y step active
  int yBeamActive;         ///< Y beam step active
  double yStep;            ///< Y step
  double yBeam;            ///< Y Beam step
  double zStep;            ///< Z step

  /// layer thicknesses [inside out]
  std::vector<double> layerThick;
  /// layer materials [inside out]
  std::vector<std::string> layerMat;

  void writeLayers(FuncDataBase&,const std::string&,const double) const;
  
 public:

  FocusGenerator();
  FocusGenerator(const FocusGenerator&);
  FocusGenerator& operator=(const FocusGenerator&);
  ~FocusGenerator();

  void setLayer(const size_t,const double,const std::string&);
  
  /// set the y step / beam step relative to the centre of the guide
  void setYCentreOffset(const double D)
  { yStep=D; yStepActive=2; yBeam=D; yBeamActive=2; }

  /// set the y step / beam step
  void setYOffset(const double D)
  { yStep=D; yStepActive=1; yBeam=D; yBeamActive=1; }

  /// set the main y step 
  void setYMainOffset(const double D)
  { yStep=D; yStepActive=1; }

  /// set the beam step
  void setYBeamOffset(const double D)
  { yBeam=D; yBeamActive=1; }

  void setZOffset(const double D)
  { zStep=D; }
  /// clear the y step
  void clearYOffset()
  { yStepActive=0; yBeamActive=0; }
  /// clear the beam y-step
  void clearYBeam() { yBeamActive=0; }
  /// clear the main y step
  void clearYMain() { yStepActive=0; }

  void generateRectangle(FuncDataBase&,
			 const std::string&,const double,
			 const double,const double) const;

  void generateTaper(FuncDataBase&,
		     const std::string&,const double,
		     const double,const double,
		     const double,const double) const;

  void generateBender(FuncDataBase&,
		      const std::string&,const double,
		      const double,const double,
		      const double,const double,
		      const double,const double) const;
  
  void generateOctagon(FuncDataBase&,
		       const std::string&,const double,
		       const double,const double) const;
  
};

}

#endif
 
