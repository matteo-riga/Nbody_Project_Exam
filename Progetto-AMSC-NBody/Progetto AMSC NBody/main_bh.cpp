#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include "Particle.h"
#include "Vector.h"
#include "ForceMatrix.h"
#include "Constants.h"
#include "simulation_functions.h"
#include "json_parser.hpp"
#include "Profiling.h"

#include <memory>
#include <random>
#include <numeric>
#include <thread>

#ifdef USE_GRAPHICS
#include "OpenGLFunctions.h"
#endif

#include "Globals.h"

using namespace std;


long long int forceComp_mean_durations_per_tick = 0, posComp_mean_durations_per_tick = 0, matrixComp_mean_duration = 0;
void saveParticles(const std::vector<Particle<DIM>> &, const std::string &);
void saveParticlesPtr(const std::vector<Particle<DIM>*> particles, const std::string &filename);

extern long long int total_sim_duration = 0;
time_t programme_start;
std::string profiling_folder = "";
std::string profiling_file_name = "Profiler_.txt";

// vector of Particle objects
std::vector<Particle<DIM>> particles;
// create vector of pointers to Particle objects
std::vector<Particle<DIM>*> pointer_to_particles;

void loadParticles(std::vector<Particle<DIM>> &, const std::string &);

unsigned int total_particles = 1000;
unsigned long int max_ticks = 100;
double ticks_per_second = 10.0;
unsigned int save_status_interval = 10;
unsigned int screen_refresh_millis = 200;
unsigned int screenResX = 2048;
unsigned int screenResY = 2048;
bool use_graphics = true;

std::string save_filename = "particles_output.pt";
std::string save_initial_status_filename = "particles_initial_output.pt";
std::string load_filename;

bool load_particles_from_file = false;

int mainLoop()
{
	total_sim_duration = 0;

	if (load_particles_from_file)
	{
		std::cout << "Attempting to load particle file..." << std::endl;
		loadParticles(particles, load_filename);
		std::cout << "Particle file loaded." << std::endl;
	}
	else
	{
		std::cout << "Attempting to load JSON file..." << std::endl;
		JsonParser parser("");
		parser.parse();

		std::srand(std::time(NULL));

		programme_start = time(0);

		std::cout << "JSON file loaded." << std::endl;

		for (unsigned int i = 0; i < total_particles; i++)
		{
			Vector<DIM> position, speed, acceleration;
			// generate mass
			double mass(5000 * 1.0e7);
			//double mass(static_cast<double>((std::rand() % 10000) * 1.0e7 + 1.0e7));
			// generate new position, velocity and acceleration
			position = Vector<DIM>({static_cast<double>(std::rand() % 4000) - 2000.0, static_cast<double>(std::rand() % 4000) - 2000.0, static_cast<double>(std::rand() % 4000) - 2000.0});
			// position = Vector<DIM>({ (i==1)*1.0 + (i == 2) * 0.7, (i==2)*0.5, 0.0});
			speed = Vector<DIM>({0.0, 0.0, 0.0});
			acceleration = Vector<DIM>({0.0, 0.0, 0.0});

			Particle<DIM> temp = Particle<DIM>(i, position, speed, acceleration, mass);
			Particle<DIM>* temp_ptr = new Particle<DIM>(i, position, speed, acceleration, mass);

			// generate particle
			// particles.emplace_back(i, position, speed, acceleration, mass);
			particles.emplace_back(temp);
			pointer_to_particles.push_back(temp_ptr);
		}
		
	}

	Vector<DIM> temp;
	unsigned int time(0);

	auto simstart = chrono::high_resolution_clock::now();

	// UPDATE SECTION
	for (time = 0; time < max_ticks; ++time)
	{
		if (!(time % (max_ticks / 100)))
		{
			cout << "Iteration " << std::setw(6) << time << " (simulation seconds: " << std::setw(4) << (double)(time + 1) / ticks_per_second << ")";
		}

		std::chrono::microseconds matrixComp_duration_this_tick;
		auto matrixComp_start = chrono::high_resolution_clock::now();

		do_simulation_step_bh(pointer_to_particles, 1);

		// compute forces
		auto matrixComp_end = chrono::high_resolution_clock::now();
		matrixComp_duration_this_tick = chrono::duration_cast<chrono::microseconds>(matrixComp_end - matrixComp_start);
		matrixComp_mean_duration += matrixComp_duration_this_tick.count();

		if (!(time % (max_ticks / 100)))
		{
			cout << " --- Execution time: " << std::setw(15) << matrixComp_duration_this_tick.count() << " us";
			cout << endl;
		}

		if (!(time % save_status_interval))
		{
			if(time == 0)
			{
				saveParticlesPtr(pointer_to_particles, save_initial_status_filename);
			}
			//saveParticlesPtr(pointer_to_particles, save_filename);
			
			if (time == 9900)
			{
				saveParticlesPtr(pointer_to_particles, save_filename);
				// print final situation (for debugging)
				/*
				for(unsigned int i = 0; i < pointer_to_particles.size(); ++i)
				{
					pointer_to_particles[i]  -> print();
				}
				*/
			}
			
		}

	} 



	
	
	
	// For time
	// Calculate mean time
	matrixComp_mean_duration /= max_ticks;

	// Calculate total execution time
	auto simend = chrono::high_resolution_clock::now();
	auto simduration = chrono::duration_cast<chrono::microseconds>(simend - simstart);

	cout << "SIMULATION ENDED. Time taken: " << (double)simduration.count() / 1000000.0 << " s" << endl;
	total_sim_duration = simduration.count();

	save_profiler_data_text_file(profiling_folder + profiling_file_name);

	return 0;
}

int main(int argc, char **argv)
{
#ifdef USE_GRAPHICS
	use_graphics = true;
#else
	use_graphics = false;
#endif
	
#ifdef USE_GRAPHICS
	GLFWwindow* window = nullptr;
	if (use_graphics)
	{
		gl_init(&window);
	}
#endif

	if (!use_graphics)
	{
		cout << "Computing without graphics" << endl;
	}

	std::thread t0(mainLoop);

#ifdef USE_GRAPHICS
	if (use_graphics)
	{
		drawParticles(&window, &particles);
		// std::thread t1(&drawParticles<DIM>, &window, &particles);

		glfwTerminate();
	}
#endif

	// TODO
	// Stop the other thread gracefully...
	t0.join();
}

void saveParticles(const std::vector<Particle<DIM>> &particles, const std::string &filename)
{
	std::ofstream outfile(filename, std::ios::out | std::ios::binary);
	if (!outfile)
	{
		std::cout << "Cannot open file to write particles!" << std::endl;
		return;
	}

	outfile.write(reinterpret_cast<const char *>(&DIM), sizeof(DIM));
	outfile.write(reinterpret_cast<char *>(&total_particles), sizeof(total_particles));

	for (const Particle<DIM> &particle : particles)
	{
		particle.saveToFile(outfile);
	}
	outfile.close();
}

void saveParticlesPtr(const std::vector<Particle<DIM>*> particles, const std::string &filename)
{
	std::ofstream outfile(filename, std::ios::out | std::ios::binary);
	if (!outfile)
	{
		std::cout << "Cannot open file to write particles!" << std::endl;
		return;
	}

	outfile.write(reinterpret_cast<const char *>(&DIM), sizeof(DIM));
	outfile.write(reinterpret_cast<char *>(&total_particles), sizeof(total_particles));

	for (const auto & particle : particles)
	{
		particle->saveToFile(outfile);
	}
	outfile.close();
}
// Close OpenGL window and terminate GLFW

void loadParticles(std::vector<Particle<DIM>> &particles, const std::string &filename)
{
	std::ifstream infile(filename, std::ios::in | std::ios::binary);
	if (!infile)
	{
		std::cout << "Cannot open file to read particles!" << std::endl;
		return;
	}

	unsigned int dim;
	infile.read(reinterpret_cast<char *>(&dim), sizeof(unsigned int));
	if (dim != DIM)
	{
		std::cout << "File has a different particle DIM.\nCannot load file information!" << std::endl;
		return;
	}

	infile.read(reinterpret_cast<char *>(&total_particles), sizeof(unsigned int));
	for (unsigned int i = 0; i < total_particles; i++)
	{
		Particle<DIM> particle(i);
		particle.loadFromFile(infile);
		particles.emplace_back(particle);
	}
	infile.close();
}
