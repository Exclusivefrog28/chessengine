# Chess engine 

This project is a chess engine written in C++ and compiled to WebAssembly. It can be played online in your browser, making it a convenient and accessible option for chess enthusiasts everywhere.

## 🎯 Features 
- Uses alpha-beta pruning as the main search algorithm 🔍
- Supports FEN notation 📝 
- Evaluates board positions using piece-square tables and material balance  📈
- Plays both as white and black ♟️ 

## 🚀 Installation 
To try it online, simply visit https://exclusivefrog28.github.io and start playing. It's that easy!

## 🛠️ Building
To build from source, you'll need to use the Emscripten CMake toolchain. Here are the steps:

1. Install the Emscripten SDK: https://emscripten.org/docs/getting_started/downloads.html
2. Set up your environment by running `source /path/to/emsdk_env.sh`
3. Clone the repository: `git clone https://github.com/exclusivefrog28/chessengine.git`
4. Create a build directory and navigate into it: `mkdir build && cd build`
5. Configure the build with CMake: `cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/emscripten.cmake ..`
6. Build the project with `make`

## 🔒 License 
The project is licensed under the MIT License. See the `LICENSE` file for more details.
