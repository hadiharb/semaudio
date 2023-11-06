all: semaudio
semaudio: semaudio.cpp GFFT.h GFFT.cpp GSignalProcessor.h GSignalProcessor.cpp GStructs.h GFVExtractor.h GFVExtractor.cpp HHNN.h HHNN.cpp bcport.h bcport.cpp
	g++ -O3 -mfpmath=sse -msse -msse2 -mtune=corei7 semaudio.cpp GFFT.cpp GSignalProcessor.cpp GFVExtractor.cpp HHNN.cpp bcport.cpp -pthread -o semaudio
