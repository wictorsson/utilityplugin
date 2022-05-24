/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
FwUtilityPluginAudioProcessorEditor::FwUtilityPluginAudioProcessorEditor (FwUtilityPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    setResizable (true, true);
    const float ratio = 16.0/ 9.0;
    setResizeLimits (600,  juce::roundToInt (600.0 / ratio),
                         900, juce::roundToInt (900.0 / ratio));
    getConstrainer()->setFixedAspectRatio (ratio);
    setSize (600, 600/ratio);
    
    addAndMakeVisible(gainDial);
    gainLabel.setText("GAIN", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainDial, false);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    gainDial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 36);
    gainDial.setRange(0.0,24.0,0.1);
    gainDial.setLookAndFeel(&customDialLAF);
    gainDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "gain", gainDial);
    
    addAndMakeVisible(panDial);
    panLabel.setText("BALANCE", juce::dontSendNotification);
    panLabel.attachToComponent(&panDial, false);
    panLabel.setJustificationType(juce::Justification::centred);
    panDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    panDial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 36);
    panDial.setRange(0.0,24.0,1.0);
    panDial.setLookAndFeel(&customDialLAF);
    panDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "pan", panDial);
    
    addAndMakeVisible(lpDial);
    lpLabel.setText("LOW PASS", juce::dontSendNotification);
    lpLabel.attachToComponent(&lpDial, false);
    lpLabel.setJustificationType(juce::Justification::centred);
    lpDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    lpDial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 36);
    lpDial.setRange(500.0,20000.0,1.0);
    lpDial.setSkewFactorFromMidPoint(2000.0);
    lpDial.setLookAndFeel(&customDialLAF);
    lpDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "lp", lpDial);
    
    addAndMakeVisible(hpDial);
    hpLabel.setText("HIGH PASS", juce::dontSendNotification);
    hpLabel.attachToComponent(&hpDial, false);
    hpLabel.setJustificationType(juce::Justification::centred);
    hpDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    hpDial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 36);
    hpDial.setRange(500.0,20000.0,1.0);
    hpDial.setSkewFactorFromMidPoint(2000.0);
    hpDial.setLookAndFeel(&customDialLAF);
    hpDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "hp", hpDial);
    
    addAndMakeVisible(monoButton);
    monoButtonAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "mono", monoButton);
    
    monoButton.setButtonText("MONO");
    monoButton.setClickingTogglesState(true);
    monoButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour::fromFloatRGBA(0.9f, 0.9f, 0.9f, 1).darker(0.5f));
    monoButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::dimgrey);
    
    monoButton.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colour::fromFloatRGBA(0.9f, 0.9f, 0.9f, 0.5f));
//    monoButton.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::black);
    
    
}

FwUtilityPluginAudioProcessorEditor::~FwUtilityPluginAudioProcessorEditor()
{
    // avoid memory leak
    gainDial.setLookAndFeel(nullptr);
}

//==============================================================================
void FwUtilityPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
   
    juce::Rectangle<int> background = getLocalBounds();
    g.setGradientFill(juce::ColourGradient::vertical(juce::Colour::fromFloatRGBA(0.65f, 0.65f, 0.65f, 1.0), 0, juce::Colour::fromFloatRGBA(0.3f, 0.3f, 0.3f, 1.0), background.getBottom()));
    
    g.fillRect(background);
   
}

void FwUtilityPluginAudioProcessorEditor::resized()
{
    auto leftMargin = getWidth() * 0.1;
    auto topMargin = getHeight() * 0.25;
    auto dialSize = getWidth() * 0.2;
    auto buttonWidth = getWidth()*0.1;
    auto buttonHeight = getHeight()*0.1;
    
    gainDial.setBounds(leftMargin, topMargin, dialSize, dialSize);
    panDial.setBounds(gainDial.getX() + gainDial.getWidth() , topMargin, dialSize, dialSize);
    lpDial.setBounds(panDial.getX() + panDial.getWidth() , topMargin, dialSize, dialSize);
    hpDial.setBounds(lpDial.getX() + lpDial.getWidth() , topMargin, dialSize, dialSize);
    monoButton.setBounds(gainDial.getX() + gainDial.getWidth() + dialSize*0.5f - buttonWidth*0.5f, topMargin + dialSize, buttonWidth , buttonHeight);
    //saveWindowSize();
}
