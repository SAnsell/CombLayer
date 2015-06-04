/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/SupplyPipe.h
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
#ifndef constructSystem_SupplyPipe_h
#define constructSystem_SupplyPipe_h

class Simulation;

namespace constructSystem
{

/*!
  \class SupplyPipe
  \version 1.0
  \author S. Ansell
  \date July 2011
  \brief SupplyPipe [insert object]
*/

class SupplyPipe : public attachSystem::FixedComp
{
 private:
  
  std::string optName;          ///< Option value
  const int pipeIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index
   
  size_t NSegIn;                      ///< Number of segments  
  size_t wallOffset;                  ///< Segments for inital wall

  std::vector<double> Radii;          ///< Inner to outer radii
  std::vector<int> Mat;               ///< Material 
  std::vector<double> Temp;           ///< Temperature

  std::vector<size_t> ActiveFlag;     ///< Active flag
  std::vector<size_t> layerSeq;       ///< Layer Sequence

  ModelSupport::PipeLine Coaxial;      ///< Global outer
  Geometry::Vec3D layerOffset;         ///< Offset of layer
  std::vector<Geometry::Vec3D> PPts;   ///< Pipe points
  size_t nAngle;                       ///< Number of angle segments

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&,const size_t,
			const size_t);
  void insertInlet(const attachSystem::FixedComp&,
		   const size_t);
  void addExtra(const attachSystem::LayerComp&,
		const size_t,const size_t);
  void addOuterPoints();
  void addExtraLayer(const attachSystem::LayerComp&,const size_t);

  void setActive();

 public:

  SupplyPipe(const std::string&);
  SupplyPipe(const SupplyPipe&);
  SupplyPipe& operator=(const SupplyPipe&);
  ~SupplyPipe();

  /// Set the option name
  void setOption(const std::string& ON) 
  {  optName=keyName+ON; }
  /// Set angle segments
  void setAngleSeg(const size_t N) { nAngle=N; }
  /// Set wallOffset
  void setWallOffset(const size_t N) { wallOffset=N; }
  /// SEt Layer sequ
  void setLayerSeq(const std::vector<size_t>& LS)
     { layerSeq=LS; }

  void addInsertCell(const size_t,const int);
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const size_t,const size_t,const size_t);

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const size_t,const size_t,const size_t,
		 const attachSystem::LayerComp&,const size_t);

};

}

#endif
 
