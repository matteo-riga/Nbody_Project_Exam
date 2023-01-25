#include "Vector.h"
#include "Particle.h"
#include <vector>
#include <array>

#define OCTREE 8


/// <summary>
/// Cell class : represents a cluster of particles
/// and is a node of the octree
/// </summary>

template <unsigned int dim>
class Cell
{
    public:

    // Constructors

    /// <summary>
    /// Cell constructor with parameters
    /// </summary>
    Cell(Vector<dim> ctr, 
        Vector<dim> size,
        std::vector<Particle<dim>*> bodies) : 
        center(ctr), 
        size(size)
        //, particles(bodies)
    {}

    /// <summary>
    /// Cell constructor without parameters
    /// everything is initialized to 0 and nullptr
    /// </summary>
    Cell()
    {
        center = Vector<dim>();
        size = Vector<dim>();

        for (unsigned int i = 0; i < OCTREE; ++i)
        {
            children[i] = nullptr;
        }
        
    }


    /// <summary>
    /// Method that returns true if the current cell is a leaf of the octree
    /// </summary>
    bool isLeaf() const
    {
        return children[0] == nullptr;
    }


    /// <summary>
    /// method that updates the force interactions among particles in a cluster
    /// </summary>
    void updateForce(Particle<dim>* p, Cell &c, double theta)
    {
        
        if(c.isLeaf())
        {
            for (Particle<dim>* q : c.particles)
            {
                if(p->pos != q->pos)
                {
                    Vector<dim> r = q->pos - p->pos;
                    const double r_norm = r.euNorm();
                    p->accel = p->accel + (r *(q->mass / (r_norm*r_norm*r_norm))) / p->mass;
                }
            }
        }

        else
        {
            const double size_norm = c.size.euNorm();
            Vector<dim> r = c.center - p->pos;
            const double r_norm = r.euNorm();

            if(size_norm/r_norm < theta)
            {
                p->accel = p->accel + (r * (c.particles.size() * c.particles[0] -> mass / (r_norm * r_norm * r_norm))) / p->mass;
            }
            else
            {
                for (unsigned int i = 0; i < OCTREE; ++i)
                {
                    if(c.children[i])
                    {
                        updateForce(p, *(c.children[i]), theta);
                    }
                }   
            }
        }
    }


    /// <summary>
    /// Method that builds the octree recursively given a starting cell
    /// </summary>
    void buildTree(Cell &c, unsigned int depth, double color)
    {
        

        if (c.particles.size() > 1 && c.particles.empty())
        {
            
            Vector<dim> h = c.size / 2;
            std::vector<Particle<dim>*> temp;

            for (unsigned int i = 0; i < OCTREE; ++i)
            {
                unsigned int x(i&1), y((i>>1)&1), z((i >> 2)&1);
                
                Vector<dim> offset = Vector<dim>({x * h[0] - 2000, y * h[1] - 2000, z * h[2] - 2000});
                c.children[i] = new Cell((center + offset), h, temp); 
            }

            for (Particle<dim> *p : c.particles)
            {
                // 3D version
                /*
                for (unsigned int i = 0; i < OCTREE; ++i)
                {
                    unsigned int x = (p -> pos[0]) > (c.children[i] -> center[0]);
                    unsigned int y = (p -> pos[1]) > (c.children[i] -> center[1]);
                    unsigned int z = (p -> pos[2]) > (c.children[i] -> center[2]);

                    unsigned int j = x + (y << 1) + (z << 2);
                    c.children[j] -> particles.emplace_back(p);
                }
                */

                // dimension independent version
                
                for (unsigned int i = 0; i < OCTREE; ++i)
                {
                    unsigned int j = 0;
                    for(unsigned int d = 0; d < DIM; ++d)
                    {
                        unsigned int x = (p -> pos[d]) > (c.children[i] -> center[d]);
                        j += x << d;
                    }
                    
                    c.children[j] -> particles.emplace_back(p);
                }
                
            }
            
            
            c.particles.clear();

            for(unsigned int i = 0; i < OCTREE; ++i)
            {
                buildTree(*(c.children[i]), depth+1, color);   
            }
        }
    }


    /// <summary>
    /// Method that updates the tree after having computed the interactions
    /// </summary>
    void updateTree(Cell &c)
    {
        c.center = Vector<dim>();
        c.particles.clear();

        for (unsigned int i = 0; i < OCTREE; ++i)
        {
            if(c.children[i])
            {
                updateTree(*c.children[i]);
                c.center = c.center + c.children[i] ->center * c.children[i] -> particles.size();
                c.particles.insert(c.particles.end(), c.children[i] -> particles.begin(), c.children[i] -> particles.end());
            }
        }
        
        if(!c.isLeaf())
        {
            c.center = c.center / c.particles.size();
        }
    }


    //private:
    // Variables
    // center represents the spatial center of the cell, size represents its size
    Vector<dim> center, size;
    // particles is a vector of pointers to particles
    std::vector<Particle<dim>*> particles;
    // children is a vector of pointers to cells
    std::array<Cell<dim>*, OCTREE> children;
};