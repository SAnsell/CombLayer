/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/BeamWindow.h
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
#ifndef ts1System_BeamWindow_h
#define ts1System_BeamWindow_h

class Simulation;


namespace ts1System
{

/*!
  \class BeamWindow
  \version 1.0
  \author G. Skoro
  \date September 2012
  \brief TS1 Beam Window [insert object]
*/

class BeamWindow : public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:
  
  const int bwIndex;            ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double incThick1;              ///< Inconel Thickness 1
  double waterThick;              ///< Light water Thickness
  double incThick2;            ///< Inconel Thickness 2

  int heMat;                    ///< Helium at atmospheric pressure
  int inconelMat;                   ///< Inconel
  int waterMat;                  ///< Light water

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
      
  void createObjects(Simulation&);
  void createLinks();

 public:

  BeamWindow(const std::string&);
  BeamWindow(const BeamWindow&);
  BeamWindow& operator=(const BeamWindow&);
  virtual ~BeamWindow();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
