/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "./LookAndFeel/DialLAF.h"

//==============================================================================
/**
*/
class FwUtilityPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    FwUtilityPluginAudioProcessorEditor (FwUtilityPluginAudioProcessor&);
    ~FwUtilityPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FwUtilityPluginAudioProcessor& audioProcessor;
    
    juce::Slider gainDial;
    juce::Label gainLabel;
    juce::Slider panDial;
    juce::Label panLabel;
    juce::Slider lpDial;
    juce::Label lpLabel;
    juce::Slider hpDial;
    juce::Label hpLabel;
    
    juce::TextButton monoButton;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainDialAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> panDialAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lpDialAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hpDialAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> monoButtonAttach;
    
    
    CustomDial customDialLAF;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FwUtilityPluginAudioProcessorEditor)
};
