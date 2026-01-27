#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include <biofvm/microenvironment.h>

using namespace physicore;
using namespace physicore::biofvm;

int main()
{
	const real_t output_interval = 100;
	const index_t oxygen_idx = 0;
	const index_t glucose_idx = 1;

	// Create microenvironment
	std::unique_ptr<microenvironment> m;
	{
		const std::filesystem::path config_file = "settings.xml";

		try
		{
			std::cout << "[diffuse] Loading configuration from: " << config_file << std::endl;
			m = microenvironment::create_from_config(config_file);
		}
		catch (const std::exception& e)
		{
			std::cerr << "[diffuse] Error: " << e.what() << std::endl;
			return 1;
		}
	}

	m->print_info(std::cout);

	real_t current_time = 0.0;
	real_t next_output_time = output_interval;
	std::chrono::duration<double> diffusion_runtime { 0.0 };
	std::chrono::duration<double> serialize_runtime { 0.0 };

	m->solver->initialize(*m);
	m->serialize_state(current_time);

	std::cout << "\n[diffuse] Running simulation for " << m->simulation_time << " time units..." << std::endl;

	while (current_time < m->simulation_time - 1e-12)
	{
		current_time += m->diffusion_timestep;

		auto run_start = std::chrono::steady_clock::now();
		m->solver->transfer_to_device(*m);
		m->run_single_timestep();
		m->solver->transfer_to_host(*m);
		diffusion_runtime += std::chrono::steady_clock::now() - run_start;

		if (current_time + 1e-12 >= next_output_time)
		{
			next_output_time += output_interval;

			auto serialize_start = std::chrono::steady_clock::now();
			m->serialize_state(current_time);
			serialize_runtime = std::chrono::steady_clock::now() - serialize_start;

			std::cout << "[diffuse] t=" << current_time << " diffusion runtime: " << diffusion_runtime.count() << " s"
					  << " serialization runtime: " << serialize_runtime.count() << " s" << std::endl;

			diffusion_runtime = std::chrono::duration<double> { 0.0 };
		}
	}

	std::cout << "\n[diffuse] Simulation completed successfully!" << std::endl;

	return 0;
}
