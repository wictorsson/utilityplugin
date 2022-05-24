/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FwUtilityPluginAudioProcessor::FwUtilityPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "PARAMETERS",createParameterLayout())
#endif
{
    apvts.addParameterListener("pan", this);
    apvts.addParameterListener("gain", this);
    apvts.addParameterListener("lp", this);
    apvts.addParameterListener("hp", this);
    apvts.addParameterListener("mono", this);
}

FwUtilityPluginAudioProcessor::~FwUtilityPluginAudioProcessor()
{
    apvts.removeParameterListener("pan", this);
    apvts.removeParameterListener("gain", this);
    apvts.removeParameterListener("lp", this);
    apvts.removeParameterListener("hp", this);
    apvts.removeParameterListener("mono", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout FwUtilityPluginAudioProcessor::createParameterLayout()
{
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    auto pan = std::make_unique<juce::AudioParameterFloat>("pan", "Pan", -100, 100, 0);
    params.push_back(std::move(pan));
    
    auto gain = std::make_unique<juce::AudioParameterFloat>("gain", "Gain", -18, 18, 0);
    params.push_back(std::move(gain));
    
    auto lp = std::make_unique<juce::AudioParameterFloat>("lp", "LowPass", juce::NormalisableRange<float>(500.0f, 20000.0f, 1.0f, 0.2), 20000.0f);
    params.push_back(std::move(lp));
    
    auto hp = std::make_unique<juce::AudioParameterFloat>("hp", "HighPass", juce::NormalisableRange<float>(10.0f, 18000.0f, 1.0f, 0.2), 10.0f);
    params.push_back(std::move(hp));
    
    auto mono = std::make_unique<juce::AudioParameterBool>("mono", "Mono", false);
    params.push_back(std::move(mono));
    
    return {params.begin(), params.end()};
}

void FwUtilityPluginAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "pan")
    {
        panTransform = (newValue/200.0) + 0.5;
    }
    
    if (parameterID == "gain")
    {
        gain.setTargetValue(newValue);
    
    }
    
    if (parameterID == "lp")
    {
        lp = newValue;
    }
    
    if (parameterID == "hp")
    {
        hp = newValue;
    }
    
    if (parameterID == "mono")
    {
        monoButton = newValue;
    }
}

//==============================================================================
const juce::String FwUtilityPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FwUtilityPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FwUtilityPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FwUtilityPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FwUtilityPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FwUtilityPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FwUtilityPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FwUtilityPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FwUtilityPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void FwUtilityPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FwUtilityPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    rmsLevelLeft.reset(sampleRate, 0.7f);
    rmsLevelRight.reset(sampleRate, 0.7f);
    rmsLevelRight.setTargetValue(-100.0f);
    rmsLevelLeft.setTargetValue(-100.0f);
    
    gain.reset(sampleRate, 0.02);
    lpFilter.reset();
    hpFilter.reset();
    
    panTransform = (*apvts.getRawParameterValue("pan")/200.0) + 0.5;
    gain = juce::Decibels::decibelsToGain(static_cast<float>(*apvts.getRawParameterValue("gain")));
    lp = *apvts.getRawParameterValue("lp");
    hp = *apvts.getRawParameterValue("hp");
    monoButton = *apvts.getRawParameterValue("mono");
    //DSP module
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    lpFilter.prepare(spec);
    hpFilter.prepare(spec);
   
}

void FwUtilityPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FwUtilityPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void FwUtilityPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
   

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    lpFilter.setCutoffFrequency(lp);
    hpFilter.setCutoffFrequency(hp);
    
    auto audioBlock = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(audioBlock);
    lpFilter.process(context);
    hpFilter.process(context);
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelDataL = buffer.getWritePointer (0);
        auto* channelDataR = buffer.getWritePointer (1);
        
        // Make mono
        if(monoButton){
            // add the right (1) to the left (0)
           buffer.addFrom(0, 0, buffer, 1, 0, buffer.getNumSamples());
            // copy the combined left (0) to the right (1)
           buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());
            // apply 0.5 gain to both
            buffer.applyGain(0.5f);
        }
      
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            // Panning
            auto inputL = channelDataL[i];
            auto inputR = channelDataR[i];
            inputL = inputL * std::sqrt(1-panTransform);
            inputR = inputR * std::sqrt(panTransform);
            
            // Apply gain
            //-3db pan. Need to multiple with sqrt2 to get 0 when centered.
            // 2 ch = 0.5 gain reduction
            channelDataL[i] = inputL * std::sqrt(2)  * juce::Decibels::decibelsToGain(gain.getNextValue() * 0.5);
            channelDataR[i] = inputR * std::sqrt(2) * juce::Decibels::decibelsToGain(gain.getNextValue() * 0.5);
 
        }
        
        //Filter
        lpFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        hpFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
        // ..do something to the data...
    }
    
    rmsLevelLeft.skip(buffer.getNumSamples());
    rmsLevelRight.skip(buffer.getNumSamples());
    {
        
        auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));

        if (value < rmsLevelLeft.getCurrentValue())
            rmsLevelLeft.setTargetValue(value);
        else
            rmsLevelLeft.setCurrentAndTargetValue(value);
       
    }
    
    {
        auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));

        
        if (value < rmsLevelRight.getCurrentValue())
            rmsLevelRight.setTargetValue(value);
        else
            rmsLevelRight.setCurrentAndTargetValue(value);
       
    }
}

//==============================================================================
bool FwUtilityPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FwUtilityPluginAudioProcessor::createEditor()
{
    return new FwUtilityPluginAudioProcessorEditor (*this);
    
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void FwUtilityPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    //Save plugin parameters
    juce::MemoryOutputStream stream(destData, false);
    apvts.state.writeToStream(stream);
}

void FwUtilityPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    //Recall plugin parameters
    
    auto tree = juce::ValueTree::readFromData(data, size_t (sizeInBytes));
   
    if(tree.isValid())
    {
        apvts.state = tree;
        panTransform = (*apvts.getRawParameterValue("pan")/200.0) + 0.5;
        gain = juce::Decibels::decibelsToGain(static_cast<float>(*apvts.getRawParameterValue("gain")));
        lp = *apvts.getRawParameterValue("lp");
        hp = *apvts.getRawParameterValue("hp");
        monoButton = *apvts.getRawParameterValue("mono");
    }
}

float FwUtilityPluginAudioProcessor::getRmsValue(const int channel) const
{
    jassert(channel == 0 || channel == 1);
    if (channel == 0)
        return rmsLevelLeft.getCurrentValue();
    if (channel == 1)
        return rmsLevelRight.getCurrentValue();
    return 0.0f;
    
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FwUtilityPluginAudioProcessor();
}




