/*
 * Copyright 2021 The Android Open Source Project
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

#include <binder/Parcel.h>
#include <binder/Parcelable.h>
#include <ui/Fence.h>
#include <ui/FenceResult.h>
#include <ui/GraphicBuffer.h>

namespace android::gui {

struct ScreenCaptureResults : public Parcelable {
public:
    ScreenCaptureResults() = default;
    virtual ~ScreenCaptureResults() = default;
    status_t writeToParcel(android::Parcel* parcel) const override;
    status_t readFromParcel(const android::Parcel* parcel) override;

    sp<GraphicBuffer> buffer;
    FenceResult fenceResult = Fence::NO_FENCE;
    bool capturedSecureLayers{false};
    bool capturedHdrLayers{false};
    ui::Dataspace capturedDataspace{ui::Dataspace::V0_SRGB};
    // A gainmap that can be used to "lift" the screenshot into HDR
    sp<GraphicBuffer> optionalGainMap;
    // HDR/SDR ratio value that fully applies the gainmap.
    // Note that we use 1/64 epsilon offsets to eliminate precision issues
    float hdrSdrRatio{1.0f};
};

} // namespace android::gui
