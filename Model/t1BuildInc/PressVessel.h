/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1BuildInc/PressVessel.h
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
#ifndef moderatorSystem_PressVessel_h
#define moderatorSystem_PressVessel_h

class Simulation;

namespace ts1System
{

/*!
  \class PressVessel
  \version 1.0
  \author S. Ansell
  \date April 2011
  \brief PressVessel [insert object]
*/

class PressVessel : public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:
  
  const int pvIndex;            ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int outerWallCell;            ///< outer wall cell
  int IVoidCell;                ///< Inner void cell

  double width;                 ///< Width tank
  double height;                ///< Height tank
  double length;                ///< Length of main tank
  double frontLen;              ///< Length of main tank
  double sideWallThick;         ///< Wall thickness
  double topWallThick;          ///< Wall thickness
  double frontWallThick;          ///< Wall thickness

  double cutX;                  ///< Front cut X 
  double cutY;                  ///< Front cut Y

  double viewRadius;            ///< View radius 
  double viewSteelRadius;       ///< View steel radius
  double viewThickness;         ///< View thickness

  double endYStep1;             ///
  double endXOutSize1;          ///
  double endZOutSize1;          ///
  double endYStep2;             ///
  double endXOutSize2;          ///
  double endZOutSize2;          ///
  double steelBulkThick;        ///< Main steel thickness
  double steelBulkRadius;       ///< Radius outlet point

  double watInThick;
  double watInRad;    

  size_t nBwch;                ///< number of big water channels at the end
  std::vector<double> begXstep;   ///< Vector of X offsets (start)
  std::vector<double> endXstep;   ///< Vector of X offsets (end)
  double bigWchbegThick;
  double bigWchbegZstep; 
  double bigWchbegHeight; 
  double bigWchbegWidth;     
  double bigWchendThick;
  double bigWchendZstep; 
  double bigWchendHeight; 
  double bigWchendWidth;         
  
  double winHouseOutRad;         ///
  double winHouseThick;          ///
  
  std::vector<channel> CItem;   ///< Set of channel objects


  int wallMat;                  ///< Material for walls
  int taMat;                    ///< Tantalum WINDOW HOUSING
  int waterMat;                 ///< Water [D2O]

  double sideZCenter;           ///< side water channels
  double sideXOffset;           ///< side water channels
  double sideHeight;
  double sideWidth;

  double targetLen;            ///< Target length [if set]

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);
  void buildChannels(Simulation&);

 public:

  PressVessel(const std::string&);
  PressVessel(const PressVessel&);
  PressVessel& operator=(const PressVessel&);
  ~PressVessel(); 

  /// set target length
  void setTargetLength(const double T) { targetLen=T; }
  /// Get Inner void cell
  int getInnerVoid() const { return IVoidCell; }
  /// Get Inner void cell
  int getOuterWall() const { return outerWallCell; }
  void buildFeedThrough(Simulation&);
  int addProtonLine(Simulation&,const std::string&);

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
