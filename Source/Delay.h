/*
  ==============================================================================

    Delay.h
    Class for feedback delay and feedforward delay with variable delay time and feedback gain
    Created: 5 Mar 2025 9:30:24am
    Author:  chenzuyu

  ==============================================================================
*/

#pragma once

class Delay
{
    
public:
    
    // Setters
    void setDelaySamples(float newDelay)
    {
        delaySamples = newDelay;
        
        // constrain delay time to size of the delay line
        if ( delaySamples >= size)
            delaySamples = size - 1;
        
        // set readPos relative to delay time
        readPos = writePos - delaySamples;
        
        // constrain the read head
        if (readPos < 0)
            readPos = 0;
            
    }
    
    void setBufferSize(int newSize)
    {
        size = newSize;
        buffer.resize(size);
    }
    
    void setDryWet(float dw)
    {
        dryWet = dw;
    }
    
    float readSample()
    {
        float output = buffer[readPos];
        
        // increment read head (update)
        readPos ++;
        
        // wrap read head
//        readPos %= size;
        if (readPos >= size)
            readPos -= size;
        
        return output;
    }
    
    
  // when the readPos is not an integer, use interpolation
    float linearInterp()
    {
        // bound the read head
        if (readPos >= size)
            readPos -= size;
        
        int lowerPos = floor(readPos);
        int higherPos = lowerPos + 1;
        
        float frac = readPos - lowerPos;
    
        float output = (1 - frac) * buffer[lowerPos] + frac * buffer[higherPos];
        
        return output;
    }
    
    void writeSample(float inputSample)
    {
        buffer[writePos] = inputSample;
        
        // increment writePos
        writePos ++;
        
        // wrap
        if (writePos >= size)
            writePos -= size;
    }
    
    void setFeedbackGain(float _fb)
    {
        jassert (_fb < 1);
        
        feedbackGain = _fb;
        if (feedbackGain > 1)
            feedbackGain = 1;
        
    }
    float process(float inputSample, bool feedBack)
    {
        float output;
        if (feedBack) // feedback comb filter
        {
            // read the sample with linear interpolation: g * y[n - M]
            float feedbackSample = feedbackGain * linearInterp();
            // add scaled feedback sample to the input:
            // y[n] = x[n] + g*y[n - M]
            output = inputSample + feedbackSample;
            writeSample(output);
        }
        else // feedforward comb filter
        {
            // y[n] = x[n] + g*x[n - M]
            output = inputSample + feedbackGain * linearInterp();
            writeSample(inputSample);
        }
        // mix the dry intput signal with the processed output signal:
        return inputSample * (1 - dryWet) + output * dryWet;
    }
    
    private:
    // member variables
    float delaySamples;
    
    std::vector<float> buffer; //delay line
    int size; // buffer size
    float readPos = 0;
    float writePos = 0;
    float dryWet = 1; // 0 ~ 1
    float feedbackGain = 0.9; // 0 ~ 1, acts as a loss factor
};
