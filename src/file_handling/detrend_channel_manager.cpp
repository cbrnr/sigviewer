// © SigViewer developers
//
// License: GPL-3.0

#include "detrend_channel_manager.h"

#include <algorithm>
#include <cmath>
#include <numeric>

#include "base/fixed_data_block.h"
#include "signal_processing/FFTReal.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace sigviewer {

//-----------------------------------------------------------------------------
DetrendChannelManager::DetrendChannelManager(ChannelManager* source, double hp_cutoff)
    : source_(source), hp_cutoff_(hp_cutoff) {
    setXAxisUnitLabel(source_->getXAxisUnitLabel());
}

//-----------------------------------------------------------------------------
std::set<ChannelID> DetrendChannelManager::getChannels() const {
    return source_->getChannels();
}

//-----------------------------------------------------------------------------
uint32 DetrendChannelManager::getNumberChannels() const {
    return source_->getNumberChannels();
}

//-----------------------------------------------------------------------------
QString DetrendChannelManager::getChannelLabel(ChannelID id) const {
    return source_->getChannelLabel(id);
}

//-----------------------------------------------------------------------------
QString DetrendChannelManager::getChannelLabel(ChannelID id, int streamNumber) const {
    return source_->getChannelLabel(id, streamNumber);
}

//-----------------------------------------------------------------------------
QString DetrendChannelManager::getChannelYUnitString(ChannelID id) const {
    return source_->getChannelYUnitString(id);
}

//-----------------------------------------------------------------------------
QSharedPointer<DataBlock const> DetrendChannelManager::getData(ChannelID id,
    unsigned start_pos,
    unsigned length) const {
    QSharedPointer<QVector<float32>> full = processedChannel(id);
    if (full.isNull()) return QSharedPointer<DataBlock const>();

    size_t n = static_cast<size_t>(full->size());
    if (static_cast<size_t>(start_pos) >= n || length == 0)
        return QSharedPointer<DataBlock const>();

    size_t end = static_cast<size_t>(start_pos) + length;
    if (end > n) return QSharedPointer<DataBlock const>();

    // Create a sub-block that shares the underlying data via FixedDataBlock.
    QSharedPointer<FixedDataBlock> full_block(new FixedDataBlock(full, source_->getSampleRate()));
    return full_block->createSubBlock(start_pos, length);
}

//-----------------------------------------------------------------------------
float64 DetrendChannelManager::getDurationInSec() const {
    return source_->getDurationInSec();
}

//-----------------------------------------------------------------------------
size_t DetrendChannelManager::getNumberSamples() const {
    return source_->getNumberSamples();
}

//-----------------------------------------------------------------------------
float64 DetrendChannelManager::getSampleRate() const {
    return source_->getSampleRate();
}

//-----------------------------------------------------------------------------
// Downsampled min/max data was built from the *raw* signal at file-open time.
// Re-using it after filtering would show the wrong envelope and break the
// auto-scale range.  Report that no pre-computed data is available so that
// the renderer always falls back to the on-the-fly path that reads from our
// filtered data cache.
void DetrendChannelManager::addDownsampledMinMaxVersion(ChannelID /*id*/,
    QSharedPointer<DataBlock const> /*min*/,
    QSharedPointer<DataBlock const> /*max*/,
    unsigned /*factor*/) {
    // intentionally ignored – we do not store downsampled versions
}

//-----------------------------------------------------------------------------
unsigned DetrendChannelManager::getNearestDownsamplingFactor(ChannelID /*id*/,
    unsigned /*factor*/) const {
    return 1;  // signal "no pre-computed data" to the renderer
}

//-----------------------------------------------------------------------------
QSharedPointer<DataBlock const> DetrendChannelManager::getDownsampledMin(ChannelID /*id*/,
    unsigned /*factor*/) const {
    return QSharedPointer<DataBlock const>();
}

//-----------------------------------------------------------------------------
QSharedPointer<DataBlock const> DetrendChannelManager::getDownsampledMax(ChannelID /*id*/,
    unsigned /*factor*/) const {
    return QSharedPointer<DataBlock const>();
}

//-----------------------------------------------------------------------------
float64 DetrendChannelManager::getMinValue(ChannelID id) const {
    processedChannel(id);  // populates channel_min_cache_ if not done yet
    QMutexLocker lock(&cache_mutex_);
    return channel_min_cache_.value(id, 0.0);
}

//-----------------------------------------------------------------------------
float64 DetrendChannelManager::getMaxValue(ChannelID id) const {
    processedChannel(id);
    QMutexLocker lock(&cache_mutex_);
    return channel_max_cache_.value(id, 0.0);
}

//-----------------------------------------------------------------------------
// Private helpers
//-----------------------------------------------------------------------------

void DetrendChannelManager::precomputeFromRawData(ChannelID id,
    QSharedPointer<DataBlock const> raw) const {
    // Fast path: already cached (double-checked under lock below).
    {
        QMutexLocker lock(&cache_mutex_);
        if (cache_.contains(id)) return;
    }
    if (raw.isNull()) return;

    size_t total = static_cast<size_t>(raw->size());
    if (total == 0) return;

    // Copy into a double-precision work buffer.
    std::vector<double> work(total);
    for (size_t i = 0; i < total; ++i) work[i] = static_cast<double>((*raw)[i]);

    // Remove mean (DC offset).
    double mean = std::accumulate(work.begin(), work.end(), 0.0) / static_cast<double>(total);
    for (auto& v : work) v -= mean;

    // Optional zero-phase FIR high-pass filter.
    std::vector<double> kernel = buildFirKernel();
    if (!kernel.empty()) applyZeroPhaseFFT(kernel, work);

    // Pack back into float32 and track per-channel min/max in the same pass.
    QSharedPointer<QVector<float32>> result(new QVector<float32>(static_cast<int>(total)));
    float32 mn = std::numeric_limits<float32>::max();
    float32 mx = -std::numeric_limits<float32>::max();
    for (size_t i = 0; i < total; ++i) {
        float32 v = static_cast<float32>(work[i]);
        (*result)[static_cast<int>(i)] = v;
        if (!std::isnan(v)) {
            if (v < mn) mn = v;
            if (v > mx) mx = v;
        }
    }

    QMutexLocker lock(&cache_mutex_);
    if (!cache_.contains(id))  // guard against a concurrent call winning the race
    {
        cache_[id] = result;
        channel_min_cache_[id] = static_cast<float64>(mn);
        channel_max_cache_[id] = static_cast<float64>(mx);
    }
}

QSharedPointer<QVector<float32>> DetrendChannelManager::processedChannel(ChannelID id) const {
    {
        QMutexLocker lock(&cache_mutex_);
        if (cache_.contains(id)) return cache_[id];
    }

    size_t total = source_->getNumberSamples();
    QSharedPointer<DataBlock const> raw = source_->getData(id, 0, static_cast<unsigned>(total));
    precomputeFromRawData(id, raw);

    QMutexLocker lock(&cache_mutex_);
    return cache_.value(id);
}

//-----------------------------------------------------------------------------
std::vector<double> DetrendChannelManager::buildFirKernel() const {
    if (hp_cutoff_ <= 0.0) return {};

    double fs = source_->getSampleRate();
    double fc = hp_cutoff_ / fs;  // normalised cutoff (0–0.5)

    // Kernel length: odd.  One second of taps gives a transition-band
    // width of ~8/M normalised ≈ 8*fs Hz, which is plenty for dc removal.
    // Since we apply |H|^2 (two-pass equivalent), we get 12 dB more
    // stopband rejection than a single pass, so a moderate M is fine.
    int fs_int = static_cast<int>(std::round(source_->getSampleRate()));
    int M = static_cast<int>(4.0 / fc);  // ~4/fc for nominal stopband
    M = std::min(M, fs_int);             // cap at 1 s of taps
    M = std::max(M, 21);                 // always enough for a reasonable kernel
    if (M % 2 == 0) ++M;                 // ensure odd length for a Type-I filter

    // Limit to at most a quarter of the signal so FFT padding stays sane.
    size_t total = source_->getNumberSamples();
    int max_M = static_cast<int>(total / 4) - 1;
    if (max_M < 21) return {};  // signal too short to filter
    if (M > max_M) M = (max_M % 2 == 0) ? max_M - 1 : max_M;

    int half = M / 2;
    std::vector<double> kernel(M);

    // Blackman-windowed sinc low-pass kernel, then spectral inversion → HP.
    for (int n = 0; n < M; ++n) {
        int k = n - half;
        double sinc = (k == 0) ? 2.0 * fc : std::sin(2.0 * M_PI * fc * k) / (M_PI * k);
        // Blackman window
        double w = 0.42 - 0.5 * std::cos(2.0 * M_PI * n / (M - 1))
                   + 0.08 * std::cos(4.0 * M_PI * n / (M - 1));
        kernel[n] = sinc * w;
    }

    // Normalise low-pass to unit DC gain.
    double sum = std::accumulate(kernel.begin(), kernel.end(), 0.0);
    for (auto& v : kernel) v /= sum;

    // Spectral inversion to turn low-pass → high-pass.
    for (auto& v : kernel) v = -v;
    kernel[half] += 1.0;

    return kernel;
}

//-----------------------------------------------------------------------------
// Zero-phase FIR filtering via FFT overlap-save.
//
// The kernel is a linear-phase (Type-I, odd-length, symmetric) FIR built by
// buildFirKernel().  Its group delay is exactly (M-1)/2 samples.
// We compute Y = X * H (single complex multiply), IFFT to get the linearly-
// convolved output, then extract y[(M-1)/2 .. (M-1)/2+N-1] to compensate the
// group delay and get a zero-phase result — without any time-domain shift.
//
// FFTReal half-complex layout (length N_fft = power of 2):
//   f[0]            = Re[DC]
//   f[1 .. N/2-1]   = Re[bins 1 .. N/2-1]
//   f[N/2]          = Re[Nyquist]
//   f[N/2+1 .. N-1] = Im[bins 1 .. N/2-1]
//
void DetrendChannelManager::applyZeroPhaseFFT(const std::vector<double>& kernel,
    std::vector<double>& data) {
    size_t N = data.size();
    size_t M = kernel.size();
    size_t delay = (M - 1) / 2;  // exact group delay of the linear-phase FIR

    // Odd-reflection mirror-pad by `delay` samples at each end.  This extends
    // the signal smoothly so that the FIR taps never see a zero boundary,
    // eliminating the transient edge artefacts that zero-padding produces.
    //
    // Left:  2·x[0]   − x[delay], x[delay−1], …, x[1]
    // Right: 2·x[N−1] − x[N−2],   x[N−3],    …, x[N−1−delay]
    size_t N_pad = N + 2 * delay;
    std::vector<double> padded(N_pad);
    for (size_t i = 0; i < delay; ++i) padded[i] = 2.0 * data[0] - data[delay - i];
    for (size_t i = 0; i < N; ++i) padded[delay + i] = data[i];
    for (size_t i = 0; i < delay; ++i)
        padded[delay + N + i] = 2.0 * data[N - 1] - data[N - 2 - i];

    // Next power of 2 ≥ N_pad + M − 1 for linear (non-circular) convolution.
    size_t N_fft = 1;
    while (N_fft < N_pad + M - 1) N_fft <<= 1;

    std::vector<FFTReal::flt_t> x_buf(N_fft, 0.0f);
    std::vector<FFTReal::flt_t> h_buf(N_fft, 0.0f);
    for (size_t i = 0; i < N_pad; ++i)
        x_buf[i] = static_cast<FFTReal::flt_t>(padded[i]);
    for (size_t i = 0; i < M; ++i) h_buf[i] = static_cast<FFTReal::flt_t>(kernel[i]);

    FFTReal fft(static_cast<long>(N_fft));
    std::vector<FFTReal::flt_t> X(N_fft), H(N_fft), Y(N_fft), y(N_fft);
    fft.do_fft(X.data(), x_buf.data());
    fft.do_fft(H.data(), h_buf.data());

    // Complex multiply Y = X * H.
    size_t half = N_fft / 2;
    Y[0] = X[0] * H[0];           // DC    — both real-only for real input
    Y[half] = X[half] * H[half];  // Nyquist
    for (size_t k = 1; k < half; ++k) {
        FFTReal::flt_t xr = X[k], xi = X[k + half];
        FFTReal::flt_t hr = H[k], hi = H[k + half];
        Y[k] = xr * hr - xi * hi;         // Re(X·H)
        Y[k + half] = xr * hi + xi * hr;  // Im(X·H)
    }

    // IFFT and rescale (FFTReal's do_ifft does NOT normalise).
    fft.do_ifft(Y.data(), y.data());
    fft.rescale(y.data());

    // The padded signal starts `delay` samples before the original signal.
    // After compensating the FIR group delay (another `delay` samples), the
    // original N samples are at y[2*delay .. 2*delay+N-1].
    // Since M is always odd: 2*delay = M-1.
    size_t start = M - 1;  // = 2 * delay
    for (size_t i = 0; i < N; ++i) data[i] = static_cast<double>(y[start + i]);
}

}  // namespace sigviewer
