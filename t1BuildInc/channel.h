/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/channel.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef ts1System_channel_h
#define ts1System_channel_h

class FuncDataBase;

namespace ts1System
{

/*!
  \class channel
  \version 1.0
  \author S. Ansell
  \date September 2010
  \brief Blocks in shutter
  
  Holds the edge/centre 
*/

class channel  : public attachSystem::ContainedComp,
  public attachSystem::FixedComp
{
 private:

  const int blockIndex;         ///< Index of block

  const int cIndex;             ///< Index of surface offset
  const int surfOffset;         ///< Actual surface offset
  int cellIndex;                ///< Cell index
 
  double centX;         ///< Centre X offset
  double centZ;         ///< Centre Z offset
  double width;         ///< Full width
  double height;        ///< Full height
  double midGap;        ///< Z lift of the target centre
  double inStep;        ///< Distance from outer steel layer
  double inDepth;        ///< Thickness of the water layer

  int matN;             ///< Material number  

  void populate(const Simulation&,const channel*);
  void createUnitVector(const attachSystem::FixedComp&);
  
  void createSurfaces();
  void createObjects(Simulation&);

  void setVar(const FuncDataBase&,const size_t,
	      const std::string&);
  void setVar(const size_t,const channel&);
  double getVar(const size_t) const;

 public:

  channel(const int,const int,const std::string&);
  channel(const channel&);
  channel& operator=(const channel&);
  ~channel() {}   ///< Destructor


  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const channel*);

};

}  

#endif
