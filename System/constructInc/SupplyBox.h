/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/SupplyBox.h
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
#ifndef constructSystem_SupplyBox_h
#define constructSystem_SupplyBox_h

class Simulation;

namespace constructSystem
{

/*!
  \class SupplyBox
  \version 1.0
  \author S. Ansell
  \date July 2011
  \brief SupplyBox [insert object]
*/

class SupplyBox : public attachSystem::FixedComp
{
 private:
  
  std::string optName;          ///< Option value
  const int pipeIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index
   
  size_t NSegIn;                      ///< Number of segments  
  size_t wallOffset;                  ///< Segments for inital wall

  std::vector<double> Width;          ///< Inner to outer radii
  std::vector<double> Depth;          ///< Inner to outer 
  std::vector<int> Mat;               ///< Material 
  std::vector<double> Temp;           ///< Temperature

  std::vector<size_t> ActiveFlag;     ///< Active flag
  std::vector<size_t> layerSeq;       ///< Layer Sequence

  ModelSupport::BoxLine Coaxial;      ///< Global outer
  Geometry::Vec3D layerOffset;         ///< Offset of layer
  std::vector<Geometry::Vec3D> PPts;   ///< Pipe points
  size_t nAngle;                       ///< Number of angle segments

  std::string startSurf;               ///< Start surfaces [if needed]
  
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&,const size_t,
			const long int);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void insertInlet(const attachSystem::FixedComp&,
		   const long int);
  void addExtra(const attachSystem::LayerComp&,
		const size_t,const size_t);
  void addOuterPoints();
  void addExtraLayer(const attachSystem::LayerComp&,const long int);

  void setActive();
  void createLinks();

 public:

  SupplyBox(const std::string&);
  SupplyBox(const SupplyBox&);
  SupplyBox& operator=(const SupplyBox&);
  ~SupplyBox();

  /// Set the option name
  void setOption(const std::string& ON) 
  {  optName=keyName+ON; }
  /// Set the start option
  void setStartSurf(const std::string& SS) 
  {  startSurf=SS; }
  /// Set angle segments
  void setAngleSeg(const size_t N) { nAngle=N; }
  /// Set wallOffset
  void setWallOffset(const size_t N) { wallOffset=N; }
  /// SEt Layer sequ
  void setLayerSeq(const std::vector<size_t>& LS)
     { layerSeq=LS; }

  void addInsertCell(const size_t,const int);
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const size_t,const long int,const long int);

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const size_t,const long int,const long int,
		 const attachSystem::LayerComp&,const long int);

};

}

#endif
 
