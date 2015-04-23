/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   lensModelInc/makeLens.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef lensSystem_makeLens_h
#define lensSystem_makeLens_h

namespace lensSystem
{
  class siModerator;
  class candleStick;
  class layers;
  
  /*!
    \class makeLens
    \version 1.0
    \author S. Ansell
    \date September 2011
    \brief Contains and builds the lens primary model
  */

class makeLens
{
 private:

  std::shared_ptr<siModerator> SiModObj;            ///< Inner Silicon moderator
  std::shared_ptr<candleStick> candleObj;           ///< candlestick holder
  std::shared_ptr<layers> layerObj;                 ///< Layers of shielding

  void setMaterials(const mainSystem::inputParam& IParam);

 public:
  
  makeLens();
  makeLens(const makeLens&);
  makeLens& operator=(const makeLens&);
  ~makeLens();
  
  void build(Simulation* SimPtr);
  void createTally(Simulation&,const mainSystem::inputParam&);
  
  const FlightCluster& getFC() const;
};

}

#endif
