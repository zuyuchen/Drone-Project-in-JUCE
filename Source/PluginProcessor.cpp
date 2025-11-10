/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_dsp/juce_dsp.h>


//==============================================================================
DroneAudioProcessor::DroneAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}
DroneAudioProcessor::~DroneAudioProcessor()
{
}

void DroneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    // LFO Modulated Subtractive Synthesis
    // set up parameters
    std::vector<FilterSynth*> ptrs2Synth = {&filterSynthL, &filterSynthR};
    for (auto* filterSynthPtr : ptrs2Synth)
    {
        filterSynthPtr -> setSampleRate(sampleRate);
       
    }
    filterSynthL.setOSC(OscType::Square, 110, 0);
    filterSynthR.setOSC(OscType::Square, 110, 0.5);
    filterSynthL.setLFO(LFOType::Saw, 0.1, 2200, 0);
    filterSynthR.setLFO(LFOType::Saw, 0.1, 2200, 0.5);
    filterSynthL.setFilter(FilterType::LowPass, 2200 + 112, 0.7);
    filterSynthR.setFilter(FilterType::LowPass, 2200 + 112, 0.7);
    // Set LFOs
    
    saw.setFrequency(0.01);   // modulating the delay feedback gain
    saw.setSampleRate(sampleRate);
    
    square.setFrequency(1);   // modulating the left and right mixing
    square.setSampleRate(sampleRate);
        
    LFO.setFrequency(0.01);   // modulating the delay time
    LFO.setSampleRate(sampleRate);
    
    // set delay buffer size
    
    std::vector<Delay*> ptr2DelayLines = {&delayL, &delayR};
    for (auto* delayPtr : ptr2DelayLines)
    {
        delayPtr -> setBufferSize(sampleRate);
    }
    
}

void DroneAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    int numChannels = buffer.getNumChannels();
    if (numChannels < 2) return; // Avoid accessing nonexistent channels
    
    int numSamples = buffer.getNumSamples();
    auto* left = buffer.getWritePointer(0);
    auto* right = buffer.getWritePointer(1);
 
    // DSP loop
    for (int i = 0; i < numSamples; i++) {

        bool exp = true; //exponential control variable for saw LFO control
        
        float sampleL = filterSynthL.process(!exp); // Left: linear saw cutoff LFO
        float sampleR = filterSynthR.process(exp); // Right: exponential ramping LFO
    
        // set delay feedback gain
        float feedbackGain = saw.process(exp);  // Modulated feedback gain!
        delayL.setFeedbackGain(feedbackGain);
        delayR.setFeedbackGain(feedbackGain);
        
       // set variational delay time
        float delayTime = 2000 *(1 + LFO.process(exp)); // delay time in samples: 0 ~ 4000
        delayL.setDelaySamples(delayTime);
        delayR.setDelaySamples(delayTime);
    
        // delay the output instead of the input as in a feedback comb filter
        // y[n] = x[n] + g*y[n - M], e.g..
        bool feedback = true; // true: feedback delay; false: feedforward delay
        
        // Dynamic stereo mixing with a 0.5 output gain applied
        float balance = square.process()*0.5 + 0.5; // 0 ~ 1
        left[i] = 0.5*balance*delayL.process(sampleL, feedback);
        right[i] = 0.5*(1 - balance) * delayR.process(sampleR, feedback);
        
    }

}


//==============================================================================
const juce::String DroneAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DroneAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DroneAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DroneAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DroneAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DroneAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DroneAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DroneAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DroneAudioProcessor::getProgramName (int index)
{
    return {};
}

void DroneAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================


void DroneAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DroneAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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


//==============================================================================
bool DroneAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DroneAudioProcessor::createEditor()
{
    return new DroneAudioProcessorEditor (*this);
}

//==============================================================================
void DroneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DroneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DroneAudioProcessor();
}
