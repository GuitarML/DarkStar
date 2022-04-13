#add_subdirectory(headless)

target_sources(DarkStar PRIVATE
	PluginProcessor.cpp
	PluginProcessor.h
	RTNeuralLSTM.cpp
	RTNeuralLSTM.h
)

#target_precompile_headers(DarkStar PRIVATE pch.h)
