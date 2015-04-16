/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1UpgradeInc/makeGamma.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef t1System_makeGamma_h
#define t1System_makeGamma_h

namespace constructSystem
{
  class ModBase;
  class BlockMod;
  class voidCylVolume;
}


/*!
  \namespace gammaSystem
  \brief General Gamma spectroscopy beamline
  \version 1.0
  \date November 2014
  \author S. Ansell
*/

namespace gammaSystem
{
  class NordBall;
  /*!
    \class makeGamma
    \version 1.0
    \author S. Ansell
    \date November 2014
    \brief General constructor for gamma beamline
  */

class makeGamma
{
 private:

  std::shared_ptr<constructSystem::BlockMod> brick;       ///< Brick
  size_t NDet;                                            ///< Number of dets
  std::vector<std::shared_ptr<NordBall> > Dets;           ///< Detectors
  std::shared_ptr<constructSystem::voidCylVolume> tallyVol;  ///< tallying void

  void makeDetectors(Simulation&);

 public:
  
  makeGamma();
  makeGamma(const makeGamma&);
  makeGamma& operator=(const makeGamma&);
  ~makeGamma();
  
  void build(Simulation*,const mainSystem::inputParam&);

};

}

#endif
