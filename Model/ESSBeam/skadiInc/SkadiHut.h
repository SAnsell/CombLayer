/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   skadiInc/SkadiHut.h
 *
 * Copyright (c) 2004-2022 by Tsitohaina Randiamalala & Stuart Ansell
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
#ifndef essSystem_SkadiHut_h
#define essSystem_SkadiHut_h

class Simulation;

namespace essSystem
{

  struct layerOffset
  {
    double front;               ///< Front extension
    double back;                ///< Back steel extension
    double leftWall;            ///< Left wall
    double rightWall;           ///< Right wall
    double roof;                ///< Roof
    double floor;               ///< Token floor depth

    int mat;
  };

/*!
  \class SkadiHut
  \version 1.0
  \author T. Randriamalala
  \date March 2017
  \brief SkadiHut unit  
*/
class SkadiHut:
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
{
 private:

  double voidLength;            ///< void length [total]
  double voidHeight;            ///< void height [top only]
  double voidWidth;             ///< void width [total]
  double voidDepth;             ///< void depth [low only]


  std::vector<layerOffset> layerV;  ///< layer values
  
  double pipeRadius;
  double pipeLength;
  double pipeL1Thickness;
  double pipeL2Thickness;
  
  int pipeL1Mat;
  int pipeL2Mat;  

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  SkadiHut(const std::string&);
  SkadiHut(const SkadiHut&);
  SkadiHut& operator=(const SkadiHut&);
  virtual ~SkadiHut();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}//End namespace
#endif

