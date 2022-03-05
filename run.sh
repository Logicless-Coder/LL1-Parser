echo -n "Compiling..." && g++ -std=c++20 parser.cpp -o parser && echo "Done!"
echo "Running...\n" && ./parser $1
