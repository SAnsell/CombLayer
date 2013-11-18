/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/Lh2Moderator.h
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
#ifndef ts1System_Lh2Moderator_h
#define ts1System_Lh2Moderator_h

class Simulation;


namespace ts1System
{

/*!
  \class Lh2Moderator
  \version 1.0
  \author G Skoro
  \date May 2012
  \brief TS1 Lh2Moderator [insert object]
*/

class Lh2Moderator : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int lh2Index;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var,2:axis,4:cent,8:face,16:cell
  
  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on Y to Target [+ve forward]
  double zStep;                 ///< Offset on Z top Target

  double xyAngle;               ///< Angle [degrees]
  double xOutSize;              ///< Left/Right size (out)
  double yOutSize;              ///< length down target (out)
  double zOutSize;              ///< Vertical size (out)

  double alThickOut;            ///< Al thickness (out)
  double alOffsetMid;           ///< Al position (mid)
  double alThickMid;            ///< Al thickness (mid)
  double alOffsetIn;            ///< Al position (in)
  double alThickIn;             ///< Al thickness (in)

  int alMat;                    ///< Al
  int lh2Mat;                   ///< LH2
  int voidMat;                  ///< void

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
//  void createObjects(Simulation&);
  void createObjects(Simulation&);
  void createLinks();

 public:

  Lh2Moderator(const std::string&);
  Lh2Moderator(const Lh2Moderator&);
  Lh2Moderator& operator=(const Lh2Moderator&);
  virtual ~Lh2Moderator();
//
  std::string getComposite(const std::string&) const;
  virtual void addToInsertChain(attachSystem::ContainedComp&) const;
//  void createAll(Simulation&,const attachSystem::FixedComp&);

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
