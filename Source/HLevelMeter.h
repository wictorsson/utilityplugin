/*
  ==============================================================================

    HLevelMeter.h
    Created: 24 May 2022 1:12:27pm
    Author:  Fredrik Wictorsson

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
namespace Gui {
class HMeter : public juce::Component
    {
        public :
        void paint(juce::Graphics& g) override
        {
            //BG
            auto bounds = getLocalBounds().toFloat();
            g.setColour(juce::Colours::white.withBrightness(0.4f));
            g.fillRoundedRectangle(bounds, 5.0f);
   
            // Level
            g.setGradientFill(juce::ColourGradient::vertical(juce::Colour::fromFloatRGBA(1.0f, 0.0f, 0.0f, 1.0f), 0, juce::Colour::fromFloatRGBA(0.0f, 1.0f, 0.0f, 0.6), getHeight()*0.5));
            
            const auto scaledY = juce::jmap(level, -60.0f, +12.0f, 0.0f, static_cast<float>(getHeight()));
            g.fillRoundedRectangle(bounds.removeFromBottom(scaledY), 5.0f);
            
            
            // 0 dB meter line
            const auto scaledYLine = juce::jmap(0.0f, +12.0f, -60.0f, 0.0f, static_cast<float>(getHeight()));
            g.setColour(juce::Colour::fromFloatRGBA(0.9f, 0.9f, 0.9f, 1).darker(0.2f));
            juce::Line<float> line (juce::Point<float> (0, scaledYLine),
                                        juce::Point<float> (10, scaledYLine));
            g.drawLine (line, 2.0f);
            
       
        
        
        }
        void setLevel(const float value) {level = value; }
    private:
        float level = -60.0f;
    };
}

