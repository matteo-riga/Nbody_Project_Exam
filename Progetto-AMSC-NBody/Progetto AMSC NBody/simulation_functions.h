#pragma once

#include <vector>
#include <iostream>

#include "Particle.h"
#include "Cell.h"


#define THETA 0.5 // barnes hut parameter

template <unsigned int dim>
int do_simulation_step(std::vector<Particle<dim>> &particles, const unsigned int &ticks_step)
{
#pragma omp parallel for
	for (int i = 0; i < particles.size(); ++i)
	{
		Vector<dim> temp;
		/*for (unsigned int component = 0; component < dim; ++component)
		{
			double component_force_on_particle = 0; // Automatically initialised to 0
//#pragma omp parallel for reduction(+: component_force_on_particle)
			for (int j = 1; j < particles.size(); ++j)
			{
				component_force_on_particle += particles[i].calcForce(particles[(i + j) % particles.size()])[component];
			}

			temp[component] = component_force_on_particle;
		}*/
		for (int j = 1; j < particles.size(); ++j)
		{
			temp += particles[i].calcForce(particles[(i + j) % particles.size()]);
		}

		particles[i].updateResultingForce(temp);
	}
#pragma omp parallel for
	for (int i = 0; i < particles.size(); ++i)
	{
		particles[i].calcNewPosition(ticks_step);
	}
	return 0;
}


template <unsigned int dim>
int do_simulation_step_bh(std::vector<Particle<dim>*> particles, const unsigned int &ticks_step)
{

	unsigned int total_particles = particles.size();
	Vector<dim> center = Vector<dim>();
	Vector<dim> size = Vector<dim>({4000.0,4000.0,4000.0}); // come iniziallizzo ???
	Cell<dim> root = Cell<dim>(center, size, particles);

	//std::cout << "n of total particles : " << total_particles << std::endl;
	//std::cout << "sizeof root particles : " << particles.size() << std::endl;

	// calculate forces
	//	1. build tree
	for (auto p : particles)
	{
		root.particles.push_back(p);
	}
	
	root.buildTree(root, 0, red);

	
	//	2. calculate interactions
	//root.updateForce(root, particles, THETA);
	
	for(auto particle : particles)
	{
		root.updateForce(particle, root, THETA);
	}
	

	// update positions
	#pragma omp parallel for
	for (int i = 0; i < particles.size(); ++i)
	{
		particles[i] -> calcNewPosition(ticks_step);
	}

	//root.updateTree(root);

	return 0;
}
