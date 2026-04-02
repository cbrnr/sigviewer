// © SigViewer developers
//
// License: GPL-3.0

#include "channel_manager_proxy.h"

namespace sigviewer {

ChannelManagerProxy::ChannelManagerProxy(ChannelManager* target) : target_(target) {
    setXAxisUnitLabel(target_->getXAxisUnitLabel());
}

void ChannelManagerProxy::setTarget(ChannelManager* target) {
    target_ = target;
    setXAxisUnitLabel(target_->getXAxisUnitLabel());
    invalidateMinMaxCache();
}

std::set<ChannelID> ChannelManagerProxy::getChannels() const {
    return target_->getChannels();
}

uint32 ChannelManagerProxy::getNumberChannels() const {
    return target_->getNumberChannels();
}

QString ChannelManagerProxy::getChannelLabel(ChannelID id) const {
    return target_->getChannelLabel(id);
}

QString ChannelManagerProxy::getChannelLabel(ChannelID id, int streamNumber) const {
    return target_->getChannelLabel(id, streamNumber);
}

QString ChannelManagerProxy::getChannelYUnitString(ChannelID id) const {
    return target_->getChannelYUnitString(id);
}

QSharedPointer<DataBlock const> ChannelManagerProxy::getData(ChannelID id,
    unsigned start_pos,
    unsigned length) const {
    return target_->getData(id, start_pos, length);
}

float64 ChannelManagerProxy::getDurationInSec() const {
    return target_->getDurationInSec();
}

size_t ChannelManagerProxy::getNumberSamples() const {
    return target_->getNumberSamples();
}

float64 ChannelManagerProxy::getSampleRate() const { return target_->getSampleRate(); }

float64 ChannelManagerProxy::getMinValue(ChannelID id) const {
    return target_->getMinValue(id);
}

float64 ChannelManagerProxy::getMaxValue(ChannelID id) const {
    return target_->getMaxValue(id);
}

float64 ChannelManagerProxy::getMinValue(std::set<ChannelID> const& channels) const {
    return target_->getMinValue(channels);
}

float64 ChannelManagerProxy::getMaxValue(std::set<ChannelID> const& channels) const {
    return target_->getMaxValue(channels);
}

void ChannelManagerProxy::addDownsampledMinMaxVersion(ChannelID id,
    QSharedPointer<DataBlock const> min,
    QSharedPointer<DataBlock const> max,
    unsigned factor) {
    target_->addDownsampledMinMaxVersion(id, min, max, factor);
}

unsigned ChannelManagerProxy::getNearestDownsamplingFactor(ChannelID id, unsigned factor) const {
    return target_->getNearestDownsamplingFactor(id, factor);
}

QSharedPointer<DataBlock const> ChannelManagerProxy::getDownsampledMin(ChannelID id,
    unsigned factor) const {
    return target_->getDownsampledMin(id, factor);
}

QSharedPointer<DataBlock const> ChannelManagerProxy::getDownsampledMax(ChannelID id,
    unsigned factor) const {
    return target_->getDownsampledMax(id, factor);
}

}  // namespace sigviewer
