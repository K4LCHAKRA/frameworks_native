/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>

#include <utils/Errors.h>
#include <utils/Mutex.h>
#include <utils/RefBase.h>
#include <utils/String8.h>
#include <utils/Timers.h>

#include <sensor/BitTube.h>

// ----------------------------------------------------------------------------
#define WAKE_UP_SENSOR_EVENT_NEEDS_ACK (1U << 31)
struct ALooper;
struct ASensorEvent;

// Concrete types for the NDK
struct ASensorEventQueue {
    ALooper* looper;
    bool requestAdditionalInfo;
};

// ----------------------------------------------------------------------------
namespace android {
// ----------------------------------------------------------------------------

class ISensorEventConnection;
class SensorManager;
class Sensor;
class Looper;

// ----------------------------------------------------------------------------

class SensorEventQueue : public ASensorEventQueue, public RefBase
{
public:

    enum { MAX_RECEIVE_BUFFER_EVENT_COUNT = 256 };

    /**
     * Typical sensor delay (sample period) in microseconds.
     */
    // Fastest sampling, system will bound it to minDelay
    static constexpr int32_t SENSOR_DELAY_FASTEST = 0;
    // Typical sample period for game, 50Hz;
    static constexpr int32_t SENSOR_DELAY_GAME = 20000;
    // Typical sample period for UI, 15Hz
    static constexpr int32_t SENSOR_DELAY_UI = 66667;
    // Default sensor sample period
    static constexpr int32_t SENSOR_DELAY_NORMAL = 200000;

    explicit SensorEventQueue(const sp<ISensorEventConnection>& connection,
                              SensorManager& sensorManager, String8 packageName);
    virtual ~SensorEventQueue();
    virtual void onFirstRef();

    int getFd() const;

    static ssize_t write(const sp<BitTube>& tube,
            ASensorEvent const* events, size_t numEvents);

    ssize_t read(ASensorEvent* events, size_t numEvents);

    status_t waitForEvent() const;
    status_t wake() const;

    status_t enableSensor(Sensor const* sensor) const;
    status_t enableSensor(Sensor const* sensor, int32_t samplingPeriodUs) const;
    status_t disableSensor(Sensor const* sensor) const;
    status_t setEventRate(Sensor const* sensor, nsecs_t ns) const;

    // these are here only to support SensorManager.java and HIDL Frameworks SensorManager.
    status_t enableSensor(int32_t handle, int32_t samplingPeriodUs, int64_t maxBatchReportLatencyUs,
                          int reservedFlags) const;
    status_t disableSensor(int32_t handle) const;
    status_t flush() const;
    // Send an ack for every wake_up sensor event that is set to WAKE_UP_SENSOR_EVENT_NEEDS_ACK.
    void sendAck(const ASensorEvent* events, int count);

    status_t injectSensorEvent(const ASensorEvent& event);

    // Filters the given sensor events in place and returns the new number of events.
    //
    // The filtering is controlled by ASensorEventQueue.requestAdditionalInfo, and if this value is
    // false, then all SENSOR_TYPE_ADDITIONAL_INFO sensor events will be removed.
    ssize_t filterEvents(ASensorEvent* events, size_t count) const;

private:
    sp<Looper> getLooper() const;
    sp<ISensorEventConnection> mSensorEventConnection;
    sp<BitTube> mSensorChannel;
    mutable Mutex mLock;
    mutable sp<Looper> mLooper;
    ASensorEvent* mRecBuffer;
    SensorManager& mSensorManager;
    String8 mPackageName;
    size_t mAvailable;
    size_t mConsumed;
    uint32_t mNumAcksToSend;
};

// ----------------------------------------------------------------------------
}; // namespace android
