#Mynecraft (Final Project)#

Jeremy Wenzel
David Parker
Frank Cerasoli

## Milestone 4.2
We are definitely on track for the final submission. Features of our project include:

### How to Build
* Refer to "HOW_TO_BUILD" file!

### How to play
* To play alone, select single player
* WSAD will move the character
* Space to jump
* Use mouse to look around
* Keys 1-9 will select an item from the inventory, the mouse scroll wheel will also select the next item
* To delete blocks, equip the pickaxe and use either mouse click while looking at a block
* To create blocks, select a block (or torch) that you want to place and use either mouse click.
* Press ESC to pause and click quit to quit

### Progress

#### Summary
* We created an infinite, fully randomly generated Minecraft world with the ability to remove and place blocks.

#### Implemented
* Random terain based on 2D perlin noise.
* Biomes, randomly generated in areas that blend in from the environment (Desert, Tundra and Rock biomes available)
* Random tree generation, many different types of trees (oak, fir and cactus)
* Day/Night cycle, time of day moves along with the sun on a sinusoidal position function, sun will set and rise.
* Randomly generated clouds
* Localized lighting, holding or placing a torch will create local lighting
* Inventory, players can hold and select an item to use (supply is unlimited)
* World saving, exiting the game will save it in a file called save.txt where it is reloaded on game start up, delete this file to generate a new random world
* Block removal, there is no pre-generated depth, blocks get created when one digs
* Block placement, put new blocks anywhere
* Heavily optimized, static geometry creation and destruction based on position, localized bullet simulation, etc.
* Background game music

#### Issues
* Inifnite block growth (cancer blocks), when a block gets incorrectly generated, it thinks its neihbors are null and starts to generate blocks everywhere when deleted. This bug is rarare and usually only occurs when something else was setup incorrectly.
* Clouds, when brought back in from level save, are of unit size and look like dots in the sky
* Somtimes it is possible to get pushed out of the level and fall infinitely into the ground

### Todo
* Cave generation, use perlin + fractal noise to simualte "perlin worms" which generate nice, connected caves
* Multiplayer, online playing with friends would be cool
* Enhanced GUI, able to select create world or load a different save based on file name
* More sounds

### Resources Used
Many image and sound files were found on the internet
* Pickaxe model downloaded from http://www.blendswap.com/blends/view/50497
* Sword model downloaded from http://www.blendswap.com/blends/view/76641
* Minecraft grass image: https://s-media-cache-ak0.pinimg.com/736x/7f/1b/49/7f1b49d6f3447ece2f9ff827e1d54456.jpg
* Minecraft dirt image: http://vignette2.wikia.nocookie.net/teamcrafted/images/b/bd/Minecraft_dirt.jpg/revision/latest?cb=20130729070015
* Minecraft sand image: http://4.bp.blogspot.com/-5wBEWB9bols/T6AuPeLp8SI/AAAAAAAABOw/OcDeiS3WRt8/s1600/minecraft_sand.jpg
* Minecraft snow image: http://4.bp.blogspot.com/-xfK7QZY_KPQ/T6AuJeW1onI/AAAAAAAABN8/_8ZiHTUCBUg/s1600/Minecraft_snow.jpg
* Minecraft cactus image: http://lh3.ggpht.com/4zAukzb2KLkeIdyQraQKWswfnKQWzHueqkXHZmUYzrT_ggUTcp-sFP94zUaG7rV3WNe2ahGJHWj_bippDa5_HQ
* Minecraft leaves image: http://imgur.com/KYF9ie9
* Minecraft wood image: http://4.bp.blogspot.com/-R_1PjGEOeC0/T6AuUnFsHaI/AAAAAAAABPQ/J9IdcdWX5aU/s1600/minecraft_tree_wood.jpg
* Minecraft rock image: http://i.imgur.com/lZIjk.png
* Minecraft torch image: https://t5.rbxcdn.com/74bb9f32a7560018ac55b4c68b030a93
* Minecraft plank image: http://www.pixelpapercraft.com/files/1335684299762.jpg
* Perlin noise pseudocode: https://en.wikipedia.org/wiki/Perlin_noise
* srand/rand code http://linux.die.net/man/3/srand
* NetManager.h and NetManager.cpp taken from Wade Burch, supplied by Sarah
* OGRE Basic tutorial 7 for CEGUI
* Minecraft music downloaded from: http://downloads.khinsider.com/game-soundtracks/album/minecraft


## Milestone 4.1

This project will be similar to the well-known game Minecraft. Mynecraft has random level generation made up of environmental cube blocks. The player can then explore this environment and see the different biomes contained within it. Future developments will hopefully include some AI animials that players can interact with.

This game is primarily focused on level generation. To generate the maps we are using Perlin Noise to randomly create the enviroments. As a player begins to explore the map, more terrain is randomly generated. As terrain is generated, biomes will also be generated. These biomes could be deserts, mountains, plains, snowy, etc. A player will never have the same experience twice.

Currently the map is dynamically allocated to generate a terrain just outside of the player's view. Terrain left behind by the player is also dynamically deallocated. The height of the terrain is generated with Perlin niose, as mentioned above, to create a continuous terrain of hills and valleys. Grass cubes are the standard terrain, but snow cubes are generated above a certain height to mimic ice caps on mountains. Biomes have also been implemented to fade into a specific terrain over a distance. Entire regions of snow, sand or other (future) materials can be easily be generated with the Biome class.