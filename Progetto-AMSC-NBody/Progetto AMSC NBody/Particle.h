#pragma once

#include "Vector.h"
#include "Constants.h"

#include <iomanip>
#include <cmath>
#include <fstream>

//using namespace constants;


// colors of output to be read by python
double green = 65280;
double red = 16711680;
double blue = 255;
double yellow = 16776960;
double cyan = 65535;
double black = 0;
double pink = 16711935;
double gray = 11119017;
std::vector<double> sample_colors = {red, green, blue, yellow, cyan, pink, black, gray};


static const double mass_constant_k = 6.673e-11;//0.001;
static const double tol = 1e-3;

/// <summary>
/// Represents the basic particles that interact in the world
/// </summary>
/// <typeparam name="dim">Dimension (1D, 2D, 3D...).</typeparam>
template <unsigned int dim>
class Particle
{
public:

	/// <summary>
	/// Constructor
	/// </summary>
	/// <param name =
	Particle(unsigned int id,
		const Vector<dim> & position,
		const Vector<dim> & speed,
		const Vector<dim> & acceleration,
		double mass) :
		ID(id),
		pos(position),
		speed(speed),
		accel(acceleration),
		mass(mass),
		color(red)
	{}

	Particle(unsigned int id)
	{
		Particle(id, {}, {}, {}, 0.0);
	}

	/// <summary>
	/// Calculates the force exerted by the other particle on this particle.
	/// </summary>
	/// <param name="other">Other particle</param>
	Vector<dim> calcForce(const Particle<dim>& other) const;

	/// <summary>
	/// Calculates the absolute distance between this particle and the other.
	/// </summary>
	/// <param name="other">Other particle</param>
	Vector<dim> calcDistance(const Particle<dim>& other) const;

	/// <summary>
	/// Calculates the new particle position based on speed, acceleration and previous position
	/// </summary>
	/// <param name="delta_time">Delta time, expressed in unit time</param>
	/// <returns>The new position</returns>
	const Vector<dim>& calcNewPosition(const unsigned int& delta_ticks);

	/// <summary>
	/// Updates this particle's force from the global matrix.
	/// </summary>
	/// <param name="resulting_force">Resulting force from the matrix</param>
	void updateResultingForce(const Vector<dim>& resulting_force);

	void saveToFile(std::ofstream& outfile) const;

	void loadFromFile(std::ifstream& infile);

	/// <summary>
	/// Print particle information
	/// </summary>
	void print() const;

	/// <summary>
	/// Gets particle mass
	/// </summary>
	inline const double& getMass() const { return mass; }

	/// <summary>
	/// Gets particle position
	/// </summary>
	inline const Vector<dim>& get_position() const { return pos; }

	/// <summary>
	/// Gets particle speed
	/// </summary>
	inline const Vector<dim>& get_speed() const { return speed; }

	/// <summary>
	/// Gets particle acceleration
	/// </summary>
	inline const Vector<dim>& get_acc() const { return accel; }

	/// <summary>
	/// Gets particle ID
	/// </summary>
	inline const unsigned int& get_particle_id() const { return ID; }

//private:
	void _updateSpeed(const unsigned int& delta_ticks);
	void _updatePos(const unsigned int& delta_ticks);

	unsigned int ID; // particle id number

	Vector<dim> pos;
	Vector<dim> speed;
	Vector<dim> accel;
	double mass;

	double color; // color in html hexadecimal to be plotted with

	// TODO Set value
	// Mass constant k
	//static const double mass_constant_k = 6.673e-11;//0.001;

	//static const double tol = 1e-3;

	// TODO Electric constant

	// TODO Magnetic constant (?)
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<unsigned int dim>
void Particle<dim>::print() const
{
	constexpr char coord[3] = { 'X', 'Y', 'Z' };
	std::cout << "Particle ID: " << ID << std::endl;
	std::cout << "Mass : " << mass << std::endl;
	std::cout << "Position" << std::endl;
	for (int i = 0; i < dim; ++i)
	{
		std::cout << std::ios::right << std::setw(15) << coord[i] << ": " << pos[i] << std::endl;
	}
	std::cout << "Speed" << std::endl;
	for (int i = 0; i < dim; ++i)
	{
		std::cout << std::ios::right << std::setw(15) << coord[i] << ": " << speed[i] << std::endl;
	}
	std::cout << "Acceleration" << std::endl;
	for (int i = 0; i < dim; ++i)
	{
		std::cout << std::ios::right << std::setw(15) << coord[i] << ": " << speed[i] << std::endl;
	}
	std::cout << std::endl;
}

template<unsigned int dim>
const Vector<dim>& Particle<dim>::calcNewPosition(const unsigned int& delta_ticks)
{
	_updateSpeed(delta_ticks);
	_updatePos(delta_ticks);
	return pos;
}

template<unsigned int dim>
void Particle<dim>::updateResultingForce(const Vector<dim>& resulting_force)
{
	for (int i = 0; i < dim; ++i)
	{
		accel[i] = resulting_force[i] / mass;
	}
}

template<unsigned int dim>
void Particle<dim>::_updateSpeed(const unsigned int& delta_ticks)
{
	for (unsigned int i = 0; i < dim; ++i)
	{
		speed[i] += accel[i] * (static_cast<double>(delta_ticks) / ticks_per_second);
	}
}

template<unsigned int dim>
void Particle<dim>::_updatePos(const unsigned int& delta_ticks)
{
	for (unsigned int i = 0; i < dim; ++i)
	{
		pos[i] += speed[i] * (static_cast<double>(delta_ticks) / ticks_per_second);
	}
}

template<unsigned int dim>
Vector<dim> Particle<dim>::calcForce(const Particle<dim>& other) const
{
	Vector<dim> displacement = calcDistance(other);
	double distance = displacement.euNorm();
	if (distance <= tol)
	{
		return Vector<dim>();
	}
	return displacement * (/*-*/mass_constant_k * mass * other.getMass()) / (distance*distance*distance);
}

template<unsigned int dim>
Vector<dim> Particle<dim>::calcDistance(const Particle<dim>& other) const
{
	Vector<dim> displacement = Vector<dim>();
	for (int i = 0; i < dim; ++i)
	{
		displacement[i] = other.pos[i] - pos[i];
	}
	return displacement;
}

template<unsigned int dim>
void Particle<dim>::saveToFile(std::ofstream& outfile) const
{
	outfile << pos << speed << accel;
	outfile.write(reinterpret_cast<const char *>(&mass), sizeof(mass));
	outfile.write(reinterpret_cast<const char *>(&color), sizeof(color));
}

template<unsigned int dim>
void Particle<dim>::loadFromFile(std::ifstream& infile)
{
	infile >> pos >> speed >> accel >> mass;
}
