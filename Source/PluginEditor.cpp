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
    const float ratio = 3.0/ 4.0;
    setResizeLimits (280,  juce::roundToInt (280.0 / ratio),
                         500, juce::roundToInt (500.0 / ratio));
    getConstrainer()->setFixedAspectRatio (ratio);
    setSize (300, 300/ratio);
    
    addAndMakeVisible(hMeterLeft);
    addAndMakeVisible(hMeterRight);
    startTimerHz(24);
    
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

    title.setText ("F.W Utility v1.0", juce::dontSendNotification);
    title.setJustificationType(juce::Justification::left);
    title.setColour(0x1000281, juce::Colours::whitesmoke);
    title.setFont (juce::Font (10.0f));
    title.setInterceptsMouseClicks(false, false);
    addAndMakeVisible (title);
    
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
    g.setGradientFill(juce::ColourGradient::vertical(juce::Colour::fromFloatRGBA(0.6f, 0.6f, 0.6f, 1.0), 0, juce::Colour::fromFloatRGBA(0.15f, 0.15f, 0.15f, 1.0), background.getBottom()));
    
    g.fillRect(background);
   
}

void FwUtilityPluginAudioProcessorEditor::resized()
{
    auto leftMargin = getWidth() * 0.1;
    auto topMargin = getHeight() * 0.11;
    //auto bottomMargin = getHeight() * 0.25;
    auto dialSize = getWidth() * 0.4;
    
    auto buttonWidth = getWidth()*0.18;
    auto buttonHeight = getHeight()*0.08;
    auto meterHeight = getHeight()*0.3;
    auto meterWidth = getWidth()*0.015;
    
    title.setBounds(0,0,40,40);
    
    gainDial.setBounds(leftMargin, topMargin, dialSize, dialSize);
    
    hMeterLeft.setBounds(gainDial.getX() + gainDial.getWidth(), topMargin, meterWidth, meterHeight);
    hMeterRight.setBounds(hMeterLeft.getX() + meterWidth * 1.8f , topMargin, meterWidth, meterHeight);
    
    panDial.setBounds(gainDial.getX() + gainDial.getWidth() + meterWidth, topMargin , dialSize, dialSize);
    
        monoButton.setBounds(hMeterLeft.getX() + hMeterLeft.getWidth()  - buttonWidth * 0.5f, topMargin * 0.2f + hMeterLeft.getY() + hMeterLeft.getHeight(), buttonWidth , buttonHeight);
    
    lpDial.setBounds(leftMargin, topMargin + monoButton.getY() + monoButton.getHeight()*1.2f, dialSize, dialSize);
    hpDial.setBounds(lpDial.getX() + lpDial.getWidth() , topMargin + monoButton.getY() + monoButton.getHeight()*1.2f, dialSize, dialSize);

    //saveWindowSize();
}

void FwUtilityPluginAudioProcessorEditor::timerCallback()
{
    hMeterLeft.setLevel(audioProcessor.getRmsValue(0));
    hMeterRight.setLevel(audioProcessor.getRmsValue(1));
    hMeterLeft.repaint();
    hMeterRight.repaint();
}
