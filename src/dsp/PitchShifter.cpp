// Mollusk Voice Changer
// Copyright (C) 2026 Ito Hyakkei
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include "PitchShifter.h"
#include <cmath>

PitchShifter::PitchShifter() : fft(fftOrder)
{
    inputFifo.assign(fifoSize, 0.0f);
    outputFifo.assign(fifoSize, 0.0f);
    hannWindow.resize(fftSize);
    fftIn.resize(fftSize);
    fftOut.resize(fftSize);

    analysisMag.resize(halfSize + 1, 0.0f);
    prevAnalysisPhase.resize(halfSize + 1, 0.0f);
    trueFreqBin.resize(halfSize + 1, 0.0f);
    synthPhaseAccum.resize(halfSize + 1, 0.0f);

    for (int i = 0; i < fftSize; ++i)
        hannWindow[i] = 0.5f * (1.0f - std::cos(
            juce::MathConstants<float>::twoPi * i / (fftSize - 1)));
}

void PitchShifter::prepare(const juce::dsp::ProcessSpec&)
{
    reset();
}

void PitchShifter::reset()
{
    // inputFifo / inputWritePos are intentionally preserved:
    // process() feeds them continuously (even in Passthrough), so they always
    // hold recent audio.  Keeping them avoids zero-padded FFT windows that
    // corrupt prevAnalysisPhase and cause the pop at crossfade start.
    std::fill(outputFifo.begin(), outputFifo.end(), 0.0f);
    std::fill(prevAnalysisPhase.begin(), prevAnalysisPhase.end(), 0.0f);
    std::fill(synthPhaseAccum.begin(),   synthPhaseAccum.end(),   0.0f);

    outputReadPos   = 0;
    outputWritePos  = fftSize;
    newSamplesCount = 0;
    warmupCounter   = fftSize;
}

void PitchShifter::process(juce::AudioBuffer<float>& buffer)
{
    if (!enabled.load()) return;

    const bool wantPassthrough = std::abs(semitones.load()) < passthroughThreshold;

    if (wantPassthrough)
    {
        if (fadeState == FadeState::Active || fadeState == FadeState::FadingIn)
            fadeState = FadeState::FadingOut;
    }
    else
    {
        if (fadeState == FadeState::Passthrough)
        {
            reset();
            fadeState = FadeState::FadingIn;
            fadeGain  = 0.0f;
        }
        else if (fadeState == FadeState::FadingOut)
        {
            fadeState = FadeState::FadingIn;  // reverse fade, no reset
        }
    }

    const int numSamples  = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    float* ch0 = buffer.getWritePointer(0);

    for (int i = 0; i < numSamples; ++i)
    {
        const float dry = ch0[i];

        // Always feed inputFifo so it holds recent audio for clean FFT pre-roll
        inputFifo[inputWritePos] = dry;
        inputWritePos = (inputWritePos + 1) & fifoMask;

        if (fadeState == FadeState::Passthrough)
            continue;  // ch0[i] unchanged; inputFifo updated above

        if (++newSamplesCount >= hopSize)
        {
            newSamplesCount = 0;
            processFrame();
        }

        float out = outputFifo[outputReadPos];
        outputFifo[outputReadPos] = 0.0f;
        outputReadPos = (outputReadPos + 1) & fifoMask;

        if (fadeState == FadeState::FadingIn)
        {
            if (warmupCounter > 0)
            {
                --warmupCounter;
                out = dry;  // output dry while FFT output is not yet valid
            }
            else
            {
                fadeGain += fadeInc;
                if (fadeGain >= 1.0f) { fadeGain = 1.0f; fadeState = FadeState::Active; }
                out = dry + fadeGain * (out - dry);  // lerp(dry, fft, fadeGain)
            }
        }
        else if (fadeState == FadeState::FadingOut)
        {
            fadeGain -= fadeInc;
            if (fadeGain <= 0.0f) { fadeGain = 0.0f; fadeState = FadeState::Passthrough; }
            out = dry + fadeGain * (out - dry);  // lerp(dry, fft, fadeGain)
        }

        ch0[i] = out;
    }

    for (int ch = 1; ch < numChannels; ++ch)
        buffer.copyFrom(ch, 0, buffer, 0, 0, numSamples);
}

void PitchShifter::processFrame()
{
    const float ratio = std::pow(2.0f, semitones.load() / 12.0f);
    constexpr float twoPi      = juce::MathConstants<float>::twoPi;
    constexpr float normFactor = 1.5f;

    // Expected phase advance per bin per hop: 2π * hopSize / N
    const float phaseAdvPerBin = twoPi * (float)hopSize / (float)fftSize;

    // ---- Step 1: Windowed input → FFT ----
    for (int i = 0; i < fftSize; ++i)
    {
        const int pos = (inputWritePos - fftSize + i) & fifoMask;
        fftIn[i] = { inputFifo[pos] * hannWindow[i], 0.0f };
    }
    fft.perform(fftIn.data(), fftOut.data(), false);

    // ---- Step 2: Analysis – instantaneous frequency per bin ----
    for (int k = 0; k <= halfSize; ++k)
    {
        analysisMag[k] = std::abs(fftOut[k]);

        const float curPhase = std::arg(fftOut[k]);

        // Phase deviation from expected advance
        float delta = curPhase - prevAnalysisPhase[k] - (float)k * phaseAdvPerBin;

        // Wrap to [-π, π]
        delta -= twoPi * std::round(delta / twoPi);

        // Instantaneous frequency in bin units
        trueFreqBin[k] = (float)k + delta / phaseAdvPerBin;

        prevAnalysisPhase[k] = curPhase;
    }

    // ---- Step 3: Synthesis – shift bins with continuous phase accumulation ----
    std::fill(fftIn.begin(), fftIn.end(), std::complex<float>(0.0f, 0.0f));

    for (int k = 0; k <= halfSize; ++k)
    {
        const float srcKf = (float)k / ratio;
        const int   k0    = (int)srcKf;
        const float frac  = srcKf - (float)k0;
        const int   k1    = k0 + 1;

        if (k0 >= 0 && k1 <= halfSize)
        {
            // Interpolate magnitude and instantaneous frequency from source bins
            const float mag   = analysisMag[k0]  * (1.0f - frac) + analysisMag[k1]  * frac;
            const float tFreq = trueFreqBin[k0]  * (1.0f - frac) + trueFreqBin[k1]  * frac;

            // Scale instantaneous frequency to output bin and accumulate synthesis phase
            synthPhaseAccum[k] += tFreq * ratio * phaseAdvPerBin;

            fftIn[k] = std::polar(mag, synthPhaseAccum[k]);
        }
    }

    // ---- Step 4: Restore Hermitian symmetry for real-valued IFFT output ----
    fftIn[0]        = { fftIn[0].real(), 0.0f };
    fftIn[halfSize] = { fftIn[halfSize].real(), 0.0f };
    for (int k = 1; k < halfSize; ++k)
        fftIn[fftSize - k] = std::conj(fftIn[k]);

    // ---- Step 5: IFFT → synthesis window → overlap-add ----
    fft.perform(fftIn.data(), fftOut.data(), true);

    for (int i = 0; i < fftSize; ++i)
    {
        const int writePos = (outputWritePos + i) & fifoMask;
        outputFifo[writePos] += fftOut[i].real() * hannWindow[i] / normFactor;
    }
    outputWritePos = (outputWritePos + hopSize) & fifoMask;
}
