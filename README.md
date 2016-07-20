# pleistocene

My end goal is to make a paleolithic tribal survival game. 

The project uses:

-SDL2 for image/input handling,

-Eigen for linear algebra.

My main concern right now though is building a simple world climate model.

(Note, I don't actually commit directly to master (as I commit this edit directly to master), it just looks like that because I only have one development branch because I'm so lone.ly)

=============================
TILE MAP
=============================
The world is split into hexagonal tiles which represent 100 km^2 of earth's surface each.
100 km^2 is a convenient ecological unit. Any smaller makes modelling a massive area unwieldly. Any bigger makes the simulation too abstract and impersonal.

To represent earth's 510 million km^2 of you would need 5 million 100 km^2 tiles, which is unweildly, so earth gets shrunk to at most ~121 rows by ~120 columns, for ~1.5 million km^2 in ~15 thousand tiles.

The simulation is run on hour units of simulated time. This allows for circadian processes to be modeled while demanding only 24 simulation frames per day.

The current implementation for holding the Tile data is a static std::vector<Tile> _tiles

Each tile has a struct Address. Address holds row, column, rendering position information, latitude/longitude, and the index "i" at which its tile may be reached (or -1 if the tile doesn't exist).

I.e. within a Tile with Address "A", another tile can be located by building Address "B": B.row=A.row+1, B.col=A.col-1. The tile at address B is accessed via _tiles[B.i].

At construction, each tile is also given a map of Neighbors: from adjacient directions {NorthWest, NorthEast, East, SouthEast, SouthWest, West} to adjacient tile addresses.

=============================
MAP GENERATION
=============================
I use Perlin noise for tile elevation generation. 

The noise function gives a value of -1 to 1 for each tile position.

Positive values are squared (or raised to a similar power) to increase the flatness of plains and the sharpness of peaks.

Negative values are adjusted via -(|value|^p), where p=~2 for -0.15<=value<0 (continental shelf), p=~1 for -0.3<=value<-0.15 (continental slope), otherwise p=~0.5 (abyssal plain).

=============================
TILE CLIMATE
=============================
The climate simulation is delegated to TileClimate, which has members: SolarRadiation, Air, Horizon, Sea, Earth.

The simulation happens in a number of steps, in which each tile completes the step before the simulation moves on.
For example, the flowchart within TileClimate::Simulate(simulationStep) is:

	switch (simulationStep) {
	case(0) :
		simulateSolarRadiation();
		simulateEvaporation();
		simulateInfraredRadiation();
		simulatePressure();
		break;
	case(2) :
		simulateAirflow();
		break;
	case(3) :
		simulateCondensation();
		simulatePrecipitation();
		break;
	case(4) :
		simulateWaterFlow();
		simulatePlants();
		break;
  	}

... before moving on to the next step.


The simulation is split into 5 major units, with each tile recieving one instance of each class:

=============================
1: SOLAR RADIATION
=============================
-Uses latitude and longitude to build a surfaceNormalVector to the tiles surface.

-Statically stores a sunRayVector determined from the simulation time: (cos t, sin t, 0) where t=time*2pi/(year length)

-Statically stores a rotationMatrix corresponding to the sideareal rotation of earth about its axis

To determine the fraction of solar irradiaion that arrives at a tiles outer atmosphere it:

-Rotates surfaceNormalVector with rotationMatrix. 

-Returns the dot product of surfaceNormalVector and sunRayVector 

=============================
2: AIR
=============================
-Holds atmospheric layers used to model weather in GaseousMixture objects (one stratosphere layer, several troposphere layers, and one boundary layer)

-Interfaces with GaseousMixture objects to perform: 

  -Filter solar radiation
  
  -Emit/filter infrared radiation
  
  -Pressure calculations (a mixture of hydrostatic assumptions and ideal gas law calculations)
  
  -Air flow between GaseousMixtures (influenced by pressure differentials, inertia, friction and the coriolis force)
  
  -Evaporation/condensation/precipitation (and the associated latent heat transfers)
  
  -Heat conduction (minimal)
  
=============================
3: SEA
=============================
-Holds layers of large bodies of water in LiquidMixture objects (water surface, aphotic layers) and a SeaIce object.

-Interfaces with GaseousMixture objects to perform: 

  -Filter solar radiation
  
  -Emit/filter infrared radiation
  
  -Evaporation
  
  -Water flow (inertia, tidal forces, air friction, coriolis force)
  
  -Heat conduction

=============================
4: HORIZON
=============================
(yet to be implemented, and may get split up. Organized together because this is the first terrestrial layer after the atmosphere and behaves very differently from EARTH)

-Holds topsoil, surface water (rivers, ponds, swamps, marshes, snowpacks, glaciers), and plants

-Implements:

  -Filter solar radiation(and photosynthesis)
  
  -Emit/filter infraredRadiation
  
  -Evaporation/Transpiration
  
  -Surface water flow
  
  -Heat conduction
  
  -Plant growth

=============================
5: EARTH
=============================
-Holds subsoil, substratum, bedrock in SolidMixture objects which in turn hold groundwater 

-Implements:

  -Ground water flow (From porousness, permeability, pressure differences)
  
  -Heat Conduction

=============================
MIXTURE
=============================
GaseousMixture, LiquidMixture, and SolidMixture inherit from Mixture.

Mixture has an optional Auxiliary_Mixture which is used to hold: 

In GaseousMixture: Droplets (clouds) and Particles (Aerosals, Snowflakes, Ice Crystals).

In LiquidMixture: Particles (Sediment), and dissoved gases.

In SolidMixture: Liquid (groundWater) and gas in void space (determined from porousness).

Mixtures are implemented as a vector of Elements and a state. 

"Element" doesnt really mean a periodic element, but rather...

	enum ElementType {
		DRY_AIR,
		WATER_VAPOR,
		CLOUD,
		WATER,
		ICE,
		SNOW,
		SAND,
		SILT,
		CLAY,
		ROCK,		//(May split into consolidated rock and unconsolidated rock)
		BEDROCK		//(Think unfractured basalt)
	};
	
...

The Element class has a set of static maps for element properties (Density, Heat Capacity, Albedo...)

An Element object consists of an ElementType, Mass, and either Mols (for gas), Volume (for solid), or both (solid and liquid states of water).

Elements/Mixtures are handled as m^2 columns and scaled up to full tile area size when needed. This makes numeric readouts much simpler to implement, read and thus debug.

=============================
TO DO
=============================

=============================
MATERIAL SURFACES and FLOW
=============================
The next main hurdle is to work out mixture flow.

This concerns Air Flow, Ground Water Flow, and Ocean Current Flow. (River flow is different).

Its already easy to call a transferMaterial(Material *recievingMaterial, Material *givingMaterial, double proportion), which transfers proportion of givingMaterial to recieving material, but I havent written the code to implement the transfers along every relevant material surface yet. 

My plan is for each material to have 8 surface objects. One corresponding to each of the directions: {Up, Down, NorthEast, East, SouthEast, SouthWest, West, NorthWest}.

Overlapping surfaces (not sure how to implement that yet) can form surface pairs, with a corresponding area and references to the materials that make up the pair.

Flow across the surface can be calculated from the dot product of the "inertial flow vector" (average of paired materials inertial flow) and the "surface normal vector" (of which there are just 8, or just 3 if you dont count opposites), then multiplied by the paired surface area.

This gives a surface flux and a corresponding direction vector 

To allow for divergent flow, we then calculate pressure differences between the materials at appropriate (the center?) places on the surface. This adds to the flow in the direction of decresing pressure (i.e. the normal vector pointing to the material with less pressure).

Altogether we get a flux and a vector direction for each surface pair. All of the fluxes should be calculated before moving on. 

givingMaterial tranfers a proportion of its material to recievingMaterial in the transferMaterial method (which handles temperature balancing)

The recieving material also has the flux momentum added to its inertial flow vector. (pushing)

The giving material has the pressure differential vector added to its inertial flow vector (pulling)

Once all of the new flow vectors are computed, we can coriolis deflect them and apply some friction.
