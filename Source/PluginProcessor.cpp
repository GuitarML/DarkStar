/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

#include <iostream>
#include <fstream>

//==============================================================================
DarkStarAudioProcessor::DarkStarAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", AudioChannelSet::mono(), true)
#endif
        .withOutput("Output", AudioChannelSet::mono(), true)
#endif
    )  
 
#endif
{
    // initialize parameters:
    addParameter(gainParam = new AudioParameterFloat(GAIN_ID, GAIN_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    addParameter(masterParam = new AudioParameterFloat(MASTER_ID, MASTER_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
}


DarkStarAudioProcessor::~DarkStarAudioProcessor()
{
}

//==============================================================================
const String DarkStarAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DarkStarAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DarkStarAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DarkStarAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DarkStarAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DarkStarAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DarkStarAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DarkStarAudioProcessor::setCurrentProgram (int index)
{
}

const String DarkStarAudioProcessor::getProgramName (int index)
{
    return {};
}

void DarkStarAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void DarkStarAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    LSTM.reset();

    // load 48 kHz sample rate model
    MemoryInputStream jsonInputStream(BinaryData::model_HT40_48k_cond_json, BinaryData::model_HT40_48k_cond_jsonSize, false);
    nlohmann::json weights_json = nlohmann::json::parse(jsonInputStream.readEntireStreamAsString().toStdString());

    LSTM.load_json3(weights_json);

    // set up DC blocker
    dcBlocker.coefficients = dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 35.0f);
    dsp::ProcessSpec spec{ sampleRate, static_cast<uint32> (samplesPerBlock), 2 };
    dcBlocker.prepare(spec);
}

void DarkStarAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DarkStarAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const AudioChannelSet& mainInput = layouts.getMainInputChannelSet();
    const AudioChannelSet& mainOutput = layouts.getMainOutputChannelSet();

    return mainInput.size() == 1 && mainOutput.size() == 1;
}
#endif
/*
bool DarkStarAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif
*/

void DarkStarAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    // Setup Audio Data
    const int numSamples = buffer.getNumSamples();


    // Amp =============================================================================
    if (fw_state == 1) {
        auto gain = static_cast<float> (gainParam->get());
        auto master = static_cast<float> (masterParam->get());

        auto block = dsp::AudioBlock<float>(buffer.getArrayOfWritePointers(), 1, numSamples);

        // Apply LSTM model
        LSTM.process(block.getChannelPointer(0), gain, block.getChannelPointer(0), (int) block.getNumSamples());

        // Master Volume 
        // Apply ramped changes for gain smoothing
        if (master == previousMasterValue)
        {
            buffer.applyGain(master);
        }
        else {
            buffer.applyGainRamp(0, (int) buffer.getNumSamples(), previousMasterValue , master);
            previousMasterValue = master;
        }

        // process DC blocker
        auto monoBlock = dsp::AudioBlock<float>(buffer).getSingleChannelBlock(0);
        dcBlocker.process(dsp::ProcessContextReplacing<float>(monoBlock));
    }
    
    for (int ch = 1; ch < buffer.getNumChannels(); ++ch)
        buffer.copyFrom(ch, 0, buffer, 0, 0, buffer.getNumSamples());
}


//==============================================================================
void DarkStarAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    MemoryOutputStream stream(destData, true);

    stream.writeFloat(*gainParam);
    stream.writeFloat(*masterParam);
}

void DarkStarAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    MemoryInputStream stream(data, static_cast<size_t> (sizeInBytes), false);

    gainParam->setValueNotifyingHost(stream.readFloat());
    masterParam->setValueNotifyingHost(stream.readFloat());
}



//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DarkStarAudioProcessor();
}
