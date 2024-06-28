# Gl_Craft
Minecraft clone in C++ with OpenGL.

# Preview
**Grass Chunk**

![image](https://github.com/Talkative-Banana/Gl_Craft/blob/master/preview/GrassBlock.png)

**Sand Chunk**

![image](https://github.com/Talkative-Banana/Gl_Craft/blob/master/preview/SandBlock.png)

**Dirt Chunk**

![image](https://github.com/Talkative-Banana/Gl_Craft/blob/master/preview/DirtBlock.png)

## Quick Start
Visit https://github.com/Mojang/bedrock-samples/releases and get the souce file for textures in a texture directory.

![image](https://github.com/Talkative-Banana/Gl_Craft/assets/100795948/5ca83734-75fc-4040-adb7-dd5a806b983e)

Extract the .zip or .tar file

* #### Top level directory layout:
        ./
        ├── depends
        ├── shaders
        ├── src
        ├── stb
        ├── textures
            ├── bedrock-samples-1.21.0.3/
            ├── bedrock-samples-1.21.0.3.zip

### GNU/Linux
``` bash
chmod +x build.sh clean.sh
./build.sh
```
### Windows
``` bash
build
```

## Dependencies
- [CMake](https://cmake.org/)
- [OpenGL](https://www.opengl.org/)
