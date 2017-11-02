/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chipInc/makeChipIR.h
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
#ifndef hutchSystem_makeChipIR_h
#define hutchSystem_makeChipIR_h

namespace shutterSystem
{
  class BulkShield;
}

namespace hutchSystem
{

  class FeedThrough;
  class FBBlock;
  class ChipIRGuide;
  class ChipIRHutch;

  /*!
    \class makeChipIR 
    \version 1.0
    \author S. Ansell
    \date June 2011
  */

class makeChipIR
{
 private:

  std::shared_ptr<ChipIRGuide> GObj;   ///< Guide 
  std::shared_ptr<ChipIRHutch> HObj;   ///< Hutch object
  std::vector<FeedThrough> FeedVec;   ///< Feed though if used
  FBBlock FB;                         ///< FeedBlock

 public:
  
  makeChipIR();
  makeChipIR(const makeChipIR&);
  makeChipIR& operator=(const makeChipIR&);
  ~makeChipIR();
  
  void build(Simulation*,const mainSystem::inputParam&,
	     const shutterSystem::BulkShield&);
  void buildIsolated(Simulation&,const mainSystem::inputParam&);

  /// Guide object accessor
  const ChipIRGuide& getGuide() const { return *GObj; }
  /// Hutch object accessor
  const ChipIRHutch& getHutch() const { return *HObj; }

};

}

#endif
