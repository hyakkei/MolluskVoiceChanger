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

#include "FormantShifter.h"
#include <cmath>

FormantShifter::FormantShifter() : fft(fftOrder)
{
    inputFifo.assign(fifoSize, 0.0f);
    outputFifo.assign(fifoSize, 0.0f);
    hannWindow.resize(fftSize);
    fftBuf.resize(fftSize);
    specBuf.resize(fftSize);
    cepBuf.resize(fftSize);
    envelope.resize(fftSize / 2 + 1, 0.0f);
    shiftedEnvelope.resize(fftSize / 2 + 1, 0.0f);

    for (int i = 0; i < fftSize; ++i)
        hannWindow[i] = 0.5f * (1.0f - std::cos(
            juce::MathConstants<float>::twoPi * i / (fftSize - 1)));
}

void FormantShifter::prepare(const juce::dsp::ProcessSpec&)
{
    reset();
}

void FormantShifter::reset()
{
    std::fill(inputFifo.begin(),  inputFifo.end(),  0.0f);
    std::fill(outputFifo.begin(), outputFifo.end(), 0.0f);
    std::fill(fftBuf.begin(),     fftBuf.end(),     std::complex<float>(0, 0));
    std::fill(specBuf.begin(),    specBuf.end(),     std::complex<float>(0, 0));
    std::fill(cepBuf.begin(),     cepBuf.end(),      std::complex<float>(0, 0));

    inputWritePos   = 0;
    outputReadPos   = 0;
    outputWritePos  = fftSize;  // initial latency = fftSize samples
    newSamplesCount = 0;
}

void FormantShifter::process(juce::AudioBuffer<float>& buffer)
{
    if (!enabled.load()) return;

    const int numSamples  = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    float* ch0 = buffer.getWritePointer(0);

    for (int i = 0; i < numSamples; ++i)
    {
        inputFifo[inputWritePos] = ch0[i];
        inputWritePos = (inputWritePos + 1) & fifoMask;

        if (++newSamplesCount >= hopSize)
        {
            newSamplesCount = 0;
            processFrame();
        }

        ch0[i] = outputFifo[outputReadPos];
        outputFifo[outputReadPos] = 0.0f;
        outputReadPos = (outputReadPos + 1) & fifoMask;
    }

    for (int ch = 1; ch < numChannels; ++ch)
        buffer.copyFrom(ch, 0, buffer, 0, 0, numSamples);
}

void FormantShifter::processFrame()
{
    const float ratio = std::pow(2.0f, semitones.load() / 12.0f);
    constexpr float eps        = 1e-8f;
    constexpr float normFactor = 1.5f;  // Hann^2 with 4x overlap sums to 1.5

    // ---- Step 1: windowed input → fftBuf ----
    for (int i = 0; i < fftSize; ++i)
    {
        const int pos = (inputWritePos - fftSize + i) & fifoMask;
        fftBuf[i] = { inputFifo[pos] * hannWindow[i], 0.0f };
    }

    // ---- Step 2: forward FFT → spectrum X[k] in specBuf ----
    fft.perform(fftBuf.data(), specBuf.data(), false);

    // ---- Step 3: log-magnitude into fftBuf ----
    for (int k = 0; k < fftSize; ++k)
        fftBuf[k] = { std::log(std::abs(specBuf[k]) + eps), 0.0f };

    // ---- Step 4: IFFT of log-magnitude → cepstrum in cepBuf ----
    // (JUCE IFFT normalises by 1/N; FFT(IFFT(L)) = L so no extra scaling needed)
    fft.perform(fftBuf.data(), cepBuf.data(), true);

    // ---- Step 5: lifter – zero out high quefrency components ----
    for (int n = lifterOrder; n <= fftSize - lifterOrder; ++n)
        cepBuf[n] = { 0.0f, 0.0f };

    // ---- Step 6: forward FFT of liftered cepstrum → smooth log envelope ----
    fft.perform(cepBuf.data(), fftBuf.data(), false);

    // ---- Step 7: exponentiate → envelope magnitudes (positive bins only) ----
    for (int k = 0; k <= fftSize / 2; ++k)
        envelope[k] = std::exp(fftBuf[k].real());

    // ---- Step 8: shift envelope along frequency axis ----
    std::fill(shiftedEnvelope.begin(), shiftedEnvelope.end(), 0.0f);
    for (int k = 0; k <= fftSize / 2; ++k)
    {
        const float srcKf = (float)k / ratio;
        const int   k0    = (int)srcKf;
        const float frac  = srcKf - (float)k0;
        const int   k1    = k0 + 1;
        if (k0 >= 0 && k1 <= fftSize / 2)
            shiftedEnvelope[k] = envelope[k0] * (1.0f - frac) + envelope[k1] * frac;
    }

    // ---- Step 9: apply shifted envelope to original spectrum ----
    // fftBuf[k] = specBuf[k] * (E_shifted[k] / E[k])  →  phase preserved, only magnitude envelope changes
    for (int k = 0; k <= fftSize / 2; ++k)
    {
        const float orig       = envelope[k];
        const float envRatio   = (orig > eps) ? shiftedEnvelope[k] / orig : 0.0f;
        fftBuf[k] = specBuf[k] * envRatio;
    }
    // Restore Hermitian symmetry for real-valued output
    fftBuf[0]           = { fftBuf[0].real(),           0.0f };
    fftBuf[fftSize / 2] = { fftBuf[fftSize / 2].real(), 0.0f };
    for (int k = 1; k < fftSize / 2; ++k)
        fftBuf[fftSize - k] = std::conj(fftBuf[k]);

    // ---- Step 10: IFFT → time domain (JUCE normalises by 1/N) ----
    fft.perform(fftBuf.data(), specBuf.data(), true);

    // ---- Step 11: synthesis Hann window + overlap-add ----
    for (int i = 0; i < fftSize; ++i)
    {
        const int writePos = (outputWritePos + i) & fifoMask;
        outputFifo[writePos] += specBuf[i].real() * hannWindow[i] / normFactor;
    }
    outputWritePos = (outputWritePos + hopSize) & fifoMask;
}
