
namespace essSystem
{
  /*!
    \class MatMesh
    \version 1.0
    \author Konstantin Batkov
    \date Nov 2016
    \brief Dumps in ASCII file rectangular mesh with information of materials
           in each cell of the mesh.
           Calculates volume fractions of materials in each cell of the mesh.
   */
  class MatMesh
  {
  private:
    const Simulation *SimPtr;
  public:
    MatMesh(Simulation &);
    virtual ~MatMesh();
    void Dump(const std::string &f,
	      const double& xmin, const double& xmax,
	      const double& ymin, const double& ymax,
	      const double& zmin, const double& zmax,
	      const size_t& nX, const size_t& nY, const size_t& nZ) const;

  };
}
