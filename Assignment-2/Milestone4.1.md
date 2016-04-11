#Mynecraft (Final Project)#

Jeremy Wenzel
David Parker
Frank Cerasoli

### Milestone 4.1 ###

This project will be similar to the well-known game Minecraft. Mynecraft has random level generation made up of environmental cube blocks. The player can then explore this environment and see the different biomes contained within it. Future developments will hopefully include some AI animials that players can interact with.

This game is primarily focused on level generation. To generate the maps we are using Perlin Noise to randomly create the enviroments. As a player begins to explore the map, more terrain is randomly generated. As terrain is generated, biomes will also be generated. These biomes could be deserts, mountains, plains, snowy, etc. A player will never have the same experience twice.

Currently the map is dynamically allocated to generate a terrain just outside of the player's view. Terrain left behind by the player is also dynamically deallocated. The height of the terrain is generated with Perlin niose, as mentioned above, to create a continuous terrain of hills and valleys. Grass cubes are the standard terrain, but snow cubes are generated above a certain height to mimic ice caps on mountains. Biomes have also been implemented to fade into a specific terrain over a distance. Entire regions of snow, sand or other (future) materials can be easily be generated with the Biome class.