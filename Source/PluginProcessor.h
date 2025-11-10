/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>
#include "Oscillator.h"
#include "FilterSynth.h"
#include "Delay.h"
//==============================================================================
/**
*/
class DroneAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    DroneAudioProcessor();
    ~DroneAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DroneAudioProcessor)
    FilterSynth filterSynthL; //instantiate a filterSynth object with default initialization
    FilterSynth filterSynthR;
    sawOSC saw;
    squareOSC square;
    sawOSC LFO;
    sineOSC sine;
    juce::dsp::Limiter<float> limiter;
    
    float frequency = 800; // oscillator frequency
    
    float lfoRate = 1; // LFO rate
    float lfoDepth = 100; // LFO depth, modulating the cutoff freuqnecy in Hz
    
    Delay delayL;
    Delay delayR;
    
    
};
