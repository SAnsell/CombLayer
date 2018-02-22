/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/MainInput.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef mainSystem_MainInput_h
#define mainSystem_MainInput_h

class Simulation;
class FuncDataBase;

/*!
  \namespace mainSystem
  \brief Adds a layer for main processing
  \author S. Ansell
  \version 1.0
  \date July 2009
*/

namespace mainSystem
{
  class inputParam;


  void createBilbauInputs(inputParam&);
  void createBNCTInputs(inputParam&);
  void createCuInputs(inputParam&);
  void createD4CInputs(inputParam&);
  void createDelftInputs(inputParam&);
  void createEPBInputs(inputParam&);
  void createESSInputs(inputParam&);
  void createFilterInputs(inputParam&);
  void createFullInputs(inputParam&);
  void createGammaInputs(inputParam&);
  void createLensInputs(inputParam&);
  void createLinacInputs(inputParam&);
  void createMuonInputs(inputParam&);
  void createPhotonInputs(inputParam&);
  void createPipeInputs(inputParam&);
  void createSiliconInputs(inputParam&);
  void createSinbadInputs(inputParam&);
  void createSingleItemInputs(inputParam&);
  void createSNSInputs(inputParam&);
  void createTS1Inputs(inputParam&);
  void createTS3ExptInputs(inputParam&);
  void createXrayInputs(inputParam&);

  void createInputs(inputParam&);
}


#endif
 
