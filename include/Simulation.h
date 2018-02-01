/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/Simulation.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef Simulation_h
#define Simulation_h

namespace Geometry
{
  class Transform;
}

namespace tallySystem
{
  class Tally;
  class sswTally;
}

namespace Surface 
{
  class Surface;
}

namespace physicsSystem
{
  class PhysicsCards;
}

class RemoveCell;

namespace ModelSupport
{
  class ObjSurfMap;
}

namespace WeightSystem
{
  class WeightMesh;
  class WeightControl;
}

namespace MonteCarlo
{
  class Object;
  class Material;
  class Qhull;
}

/*!
  \class Simulation
  \version 1.3
  \date Dec 2005
  \author S.Ansell
  \brief Containes all the information on from the simulations.

  Contains the running simulation information 
  Mainly list of maps and process information
  Is expected to be mainly a singleton class.
*/

class Simulation
{
 protected:

  typedef std::map<int,Geometry::Transform> TransTYPE; ///< Transform type
 
 public:

  // UGLY
  typedef std::map<int,MonteCarlo::Qhull*> OTYPE;      ///< Object type
  typedef std::map<int,tallySystem::Tally*> TallyTYPE; ///< Tally type

 protected:

  int mcnpVersion;                      ///< version of mcnp
  std::string inputFile;                ///< Input file
  std::string cmdLine;                  ///< Command line : historical recall 
  int CNum;                             ///< Number of complementary components
  FuncDataBase DB;                      ///< DataBase of variables
  ModelSupport::ObjSurfMap* OSMPtr;     ///< Object surface map [if required]

  TransTYPE TList;                      ///< Transforms List (key=Transform)

  OTYPE OList;   ///< List of objects  (allow to become hulls)
  std::vector<int> cellOutOrder;        ///< List of cells [output order]
  std::set<int> voidCells;              ///< List of void cells

  TallyTYPE TItem;                        ///< Tally Items
  physicsSystem::PhysicsCards* PhysPtr;   ///< Physics Cards

  std::string sourceName;                 ///< Source name
  
  // METHODS:

  void deleteObjects();
  void deleteTally();
  
  int readTransform(std::istream&);    
  int readTally(std::istream&);        

  // ALL THE sub-write stuff
  void writeCells(std::ostream&) const;
  void writeSurfaces(std::ostream&) const;
  void writeMaterial(std::ostream&) const;
  void writeWeights(std::ostream&) const;
  void writeTransform(std::ostream&) const;
  void writeTally(std::ostream&) const;
  void writeSource(std::ostream&) const;
  void writePhysics(std::ostream&) const;
  void writeVariables(std::ostream&,const char ='c') const;

  // The Cinder Write stuff
  void writeCinderMat() const;

  int checkInsert(const MonteCarlo::Qhull&);       ///< Inserts (and test) new hull into Olist map 
  int removeNullSurfaces();
  int removeComplement(MonteCarlo::Qhull&) const;
  void addObjSurfMap(MonteCarlo::Qhull*);

 public:

  Simulation();
  Simulation(const Simulation&);
  Simulation& operator=(const Simulation&);
  virtual ~Simulation();
  
  /// set the command line
  void setCmdLine(const std::string& S) { cmdLine=S; }
  void resetAll();
  void readMaster(const std::string&);   
  int applyTransforms();  
  int isValidCell(const int,const Geometry::Vec3D&) const;


  /// is the system MCNP6
  bool isMCNP6() const { return mcnpVersion!=10; }
  
  MonteCarlo::Qhull* findQhull(const int);         
  const MonteCarlo::Qhull* findQhull(const int) const; 
  MonteCarlo::Object* findCell(const Geometry::Vec3D&,
			       MonteCarlo::Object*) const;
  int findCellNumber(const Geometry::Vec3D&,const int) const;  

  int existCell(const int) const;              ///< check if cell exist
  int getCellMaterial(const int) const;        ///< return cell material
  int bindCell(const int,const int);
  int setMaterialDensity(OTYPE&);
  int setMaterialDensity(const int);

  /// Gets the data base
  FuncDataBase& getDataBase() { return DB; }  
  /// Gets the data base
  const FuncDataBase& getDataBase() const { return DB; }
  /// Get PhysicsCards
  physicsSystem::PhysicsCards& getPC() { return *PhysPtr; }

  /// set Source name
  void setSourceName(const std::string&);

  const OTYPE& getCells() const { return OList; } ///< Get cells(const)
  OTYPE& getCells() { return OList; } ///< Get cells

  int removeComplements(); 

  int populateCells();  
  int populateCells(const std::vector<int>&);  

  int calcVertex(const int); 
  void calcAllVertex();
  
  void masterRotation();
  void masterPhysicsRotation();

  // ADD Objects
  int addCell(const MonteCarlo::Qhull&);         
  int addCell(const int,const MonteCarlo::Qhull&);         
  int addCell(const int,const int,const std::string&);
  int addCell(const int,const int,const double,const std::string&);

  // LIST Stuff

  void reZeroFromVertex(const int,const unsigned int,const unsigned int,
			const unsigned int,const unsigned int,Geometry::Vec3D&,
			Geometry::Matrix<double>&);

  /// Get values

  std::vector<int> getCellVector() const;
  std::vector<int> getCellTempRange(const double,const double) const;
  std::vector<int> getCellInputVector() const;
  std::vector<int> getNonVoidCellVector() const;
  std::vector<int> getCellVectorRange(const int,const int) const;
  std::vector<int> getCellWithMaterial(const int) const;
  std::vector<int> getCellWithZaid(const size_t) const;

  void processCellsImp();           
  int makeVirtual(const int);

  int removeDeadSurfaces(const int); 
  int removeCells(const int,const int); 
  void removeCell(const int);
  int removeAllSurface(const int);
  int substituteAllSurface(const int,const int);
  void voidObject(const std::string&);
  void updateSurface(const int,const std::string&);

  void createObjSurfMap();
  void validateObjSurfMap();
  /// Access surface map
  const ModelSupport::ObjSurfMap* getOSM() const;

  // Tally processing

  void removeAllTally();
  int removeTally(const int);

  int addTally(const tallySystem::Tally&);
  tallySystem::Tally* getTally(const int) const;
  tallySystem::sswTally* getSSWTally() const;
  /// Access tally items
  TallyTYPE& getTallyMap() { return TItem; }
  /// Access constant
  const TallyTYPE& getTallyMap() const { return TItem; }
  void setForCinder();
  int nextTallyNum(int) const;

  void setEnergy(const double);
  void setENDF7();
  /// set MCNPversion
  void setMCNPversion(const int);
  
  void renumberAll();
  void renumberCells(const std::vector<int>&,const std::vector<int>&);
  void renumberSurfaces(const std::vector<int>&,const std::vector<int>&);
  void prepareWrite();
  void writeCinder() const;          

  virtual void write(const std::string&) const;  
    
  // Debug stuff
  
  void printVertex(const int) const;

};

#endif
