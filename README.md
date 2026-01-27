# Requirements

- Python 3.11+
- Python venv module
- make
- git
- A C++ compiler with C++11 support (e.g. gcc, clang)

# Repository setup instructions

1. Clone the PhysiCell repository

   ```bash
   git clone --recurse-submodules --depth 1
   ```

2. Create and activate a Python virtual environment

   ```bash
    python3 -m venv venv
    source venv/bin/activate
    ```

3. Install the required Python packages
    ```bash
    pip install -r PhysiCell-Studio/requirements.txt
    ```

# Diffusion time-step tutorial

We will walk through different examples of setting the diffusion time-step in PhysiCell.

## 1. Basic example: Substrates, diffusion, decay

In this example, we will set up a simple simulation with a single substrate that diffuses and decays in the microenvironment.

First, let's build a template PhysiCell project:

```bash
cd PhysiCell
make template
make -j
```

Next, we can start PhysiCell Studio to visualize and run the simulation:

```bash
python ../PhysiCell-Studio/bin/studio.py -c config/PhysiCell_settings.xml -e project
```

In the Studio, you can tune and run the simulation and observe the diffusion and decay of the substrate over time. We suggest you try changing these parameters:
0. In `Config Basics` tab, set `Save data (interval)` to `0.1` minutes so you can see the changes more frequently, and set `Max time` to `100` minutes.
1. In `Microenvironment` tab, set `xmin` Dirichlet boundary to `1000` and tick the box to enable it. In `Run` tab, press `Run simulation` and observe how the substrate concentration changes over time in the `Plot` tab. Don't forget to tick `substrates` box in `Plot` tab to see the substrate concentration plot.
2. In `Microenvironment` tab, set `Diffusion coefficient` to `1000` um^2/min and `Decay rate` to `0.1` 1/min and observe how the substrate concentration changes over time.
3. Set `Decay rate` to `0` 1/min and observe the changes again. Now, the substrate should not decay over time and the concentration should gradually increase until the Dirichlet boundary condition value is reached.
4. In `Microenvironment` tab, add another substrate by clicking `New`. Set its `Initial condition` to `1000`, `Decay rate` to `0.05` 1/min and some/all `Dirichlet conditions`. Run the simulation and in the `Plot` tab, swich between substrates to observe the change.

There is a predefined configuration file for this example in `diffusion/PhysiCell_settings_1.xml`. If you do not want to follow the steps above, you can copy it to your project folder to quickly set up the simulation with the parameters described above (do this before starting the Studio to have the settings loaded on startup):

```bash
cp ../diffusion/PhysiCell_settings_1.xml ./config/PhysiCell_settings.xml
```

## 2. Adding cells as sources and sinks

In this example, we will add cells that act as sources and sinks for the substrate in the microenvironment.

0. In `Config Basics` tab, tick `enable` box under `Initial conditions of cells` to allow Studio to place cells in the simulation.
1. In `Cell Types` tab, under `Secretion` sub-tab, set `secretion rate` to `100` 1/min and `target` density to `5000` for the first substrate. This will make the cells secrete the substrate into the microenvironment. Run and observe the changes in the `Plot` tab.
2. In `ICs` tab, under `Cell Initial Condition` sub-space, select the region and number of cells to place in the simulation. Click on `Plot` and `Save`. Run the simulation and observe how the substrate concentration changes over time due to cell secretion.
3. Now, set `uptake rate` to `1000` 1/min for the second substrate. This will make the cells uptake the substrate from the microenvironment. Run and observe the changes in the `Plot` tab.

There is a predefined configuration file for this example in `diffusion/PhysiCell_settings_2.xml`. If you do not want to follow the steps above, you can copy it to your project folder to quickly set up the simulation with the parameters described above (do this before starting the Studio to have the settings loaded on startup):

```bash
cp ../diffusion/PhysiCell_settings_2.xml ./config/PhysiCell_settings.xml
```

## 3. Advanced example: Bulk sources and sinks

In some situations, it is cumbersome to use cells as sources and sinks. In this example, we will show you how to set up domain regions with bulk sources and sinks for the substrate in the microenvironment.

Beware, this feature is not yet supported either in PhysiCell Studio nor in the configuration file, so you will need to change the C++ source file manually.

0. Open `main.cpp` in PhysiCell project folder.
1. On line 220, after the line `microenvironment.simulate_diffusion_decay( diffusion_dt );`, add the following line to enable bulk sources and sinks simulation:

```cpp
microenvironment.simulate_bulk_sources_and_sinks( diffusion_dt );
```

2. On line 119, after the line `setup_microenvironment(); // modify this in the custom code`, add the following code to define bulk source and sink functions:

```cpp
	microenvironment.bulk_supply_rate_function = []( Microenvironment* pMicroenvironment, int voxel_index, std::vector<double>* write_destination )
	{
		if (voxel_index % 2 == 0)
		{
			write_destination->at(0) = 100.0;
		}
		else
		{
			write_destination->at(0) = 0.0;
		}
	};

	microenvironment.bulk_supply_target_densities_function = []( Microenvironment* pMicroenvironment, int voxel_index, std::vector<double>* write_destination )
	{
		if (voxel_index % 2 == 0)
		{
			write_destination->at(0) = 5000.0;
		}
		else
		{
			write_destination->at(0) = 0.0;
		}
	};
```

Similarly as for cells, this will make secretion rates and uptake rates for selected voxels (those with even indices in this example). 

3. Build the project again:

```bash
make -j
```
4. Run the simulation. Observe how the substrate concentration changes over time due to bulk sources and sinks.

The prefedined main file for this example is in `diffusion/main_bulk_sources_and_sinks.cpp`. You can copy it to your PhysiCell project folder to quickly set up the simulation with the parameters described above:

```bash
cp ../diffusion/main_bulk_sources_and_sinks.cpp ./PhysiCell/main.cpp
```

