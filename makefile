srcFile = src
binFile = bin
stdVersion = -std=c++14

wordGenerator:
	g++ $(stdVersion) $(srcFile)/wordGenerator.cpp -o $(binFile)/wordGenerator

countWords:
	g++ $(stdVersion) $(srcFile)/countWords.cpp -o $(binFile)/countWords -pthread

main:
	g++ $(stdVersion) $(srcFile)/main.cpp -o $(binFile)/runMain -pthread