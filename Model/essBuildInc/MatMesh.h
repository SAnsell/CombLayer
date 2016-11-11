
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
    void Dump(const std::vector<double>& vx,
	      const std::vector<double>& vy,
	      const std::vector<double>& vz,
	      const char *fname) const;
    void Dump(const Geometry::Vec3D &startPt,
	      const Geometry::Vec3D &endPt,
	      const size_t nX, const size_t nY, const size_t nZ,
	      const char *fname) const;
    std::vector<int> getMaterials(const Geometry::Vec3D &center,
				  double *stepXYZ, size_t N) const;
    std::string getMaterials(std::vector<int>) const;

  };
}
