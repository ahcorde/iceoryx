// Copyright (c) 2020 by Robert Bosch GmbH. All rights reserved.
// Copyright (c) 2021 - 2022 by Apex.AI Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef IOX_POSH_POPO_SMART_CHUNK_INL
#define IOX_POSH_POPO_SMART_CHUNK_INL

namespace iox
{
namespace popo
{
namespace internal
{
template <template <typename, typename> class TransmissionInterface, typename T, typename H>
inline SmartChunkPrivateData<TransmissionInterface, T, H>::SmartChunkPrivateData(
    cxx::unique_ptr<T>&& smartChunkUniquePtr, TransmissionInterface<T, H>& producer) noexcept
    : smartChunkUniquePtr(std::move(smartChunkUniquePtr))
    , producerRef(producer)
{
}

template <template <typename, typename> class TransmissionInterface, typename T, typename H>
inline SmartChunkPrivateData<TransmissionInterface, const T, H>::SmartChunkPrivateData(
    cxx::unique_ptr<const T>&& smartChunkUniquePtr) noexcept
    : smartChunkUniquePtr(std::move(smartChunkUniquePtr))
{
}
} // namespace internal

template <template <typename, typename> class TransmissionInterface, typename T, typename H>
template <typename S, typename>
inline SmartChunk<TransmissionInterface, T, H>::SmartChunk(cxx::unique_ptr<T>&& smartChunkUniquePtr,
                                                           TransmissionInterface<T, H>& producer) noexcept
    : m_members({std::move(smartChunkUniquePtr), producer})
{
}

template <template <typename, typename> class TransmissionInterface, typename T, typename H>
template <typename S, typename>
inline SmartChunk<TransmissionInterface, T, H>::SmartChunk(cxx::unique_ptr<T>&& smartChunkUniquePtr) noexcept
    : m_members(std::move(smartChunkUniquePtr))
{
}

template <template <typename, typename> class TransmissionInterface, typename T, typename H>
inline T* SmartChunk<TransmissionInterface, T, H>::operator->() noexcept
{
    return get();
}

template <template <typename, typename> class TransmissionInterface, typename T, typename H>
inline const T* SmartChunk<TransmissionInterface, T, H>::operator->() const noexcept
{
    return get();
}

template <template <typename, typename> class TransmissionInterface, typename T, typename H>
inline T& SmartChunk<TransmissionInterface, T, H>::operator*() noexcept
{
    return *get();
}

template <template <typename, typename> class TransmissionInterface, typename T, typename H>
inline const T& SmartChunk<TransmissionInterface, T, H>::operator*() const noexcept
{
    return *get();
}

template <template <typename, typename> class TransmissionInterface, typename T, typename H>
inline SmartChunk<TransmissionInterface, T, H>::operator bool() const noexcept
{
    return get() != nullptr;
}

template <template <typename, typename> class TransmissionInterface, typename T, typename H>
inline T* SmartChunk<TransmissionInterface, T, H>::get() noexcept
{
    return m_members.smartChunkUniquePtr.get();
}

template <template <typename, typename> class TransmissionInterface, typename T, typename H>
inline const T* SmartChunk<TransmissionInterface, T, H>::get() const noexcept
{
    return m_members.smartChunkUniquePtr.get();
}

template <template <typename, typename> class TransmissionInterface, typename T, typename H>
inline typename SmartChunk<TransmissionInterface, T, H>::ConditionalConstChunkHeader_t*
SmartChunk<TransmissionInterface, T, H>::getChunkHeader() noexcept
{
    return mepoo::ChunkHeader::fromUserPayload(m_members.smartChunkUniquePtr.get());
}

template <template <typename, typename> class TransmissionInterface, typename T, typename H>
inline const mepoo::ChunkHeader* SmartChunk<TransmissionInterface, T, H>::getChunkHeader() const noexcept
{
    return mepoo::ChunkHeader::fromUserPayload(m_members.smartChunkUniquePtr.get());
}

template <template <typename, typename> class TransmissionInterface, typename T, typename H>
template <typename R, typename>
inline R& SmartChunk<TransmissionInterface, T, H>::getUserHeader() noexcept
{
    return *static_cast<R*>(mepoo::ChunkHeader::fromUserPayload(m_members.smartChunkUniquePtr.get())->userHeader());
}

template <template <typename, typename> class TransmissionInterface, typename T, typename H>
template <typename R, typename>
inline const R& SmartChunk<TransmissionInterface, T, H>::getUserHeader() const noexcept
{
    return const_cast<SmartChunk<TransmissionInterface, T, H>*>(this)->getUserHeader();
}

template <template <typename, typename> class TransmissionInterface, typename T, typename H>
inline T* SmartChunk<TransmissionInterface, T, H>::release() noexcept
{
    return m_members.smartChunkUniquePtr.release();
}

template <template <typename, typename> class TransmissionInterface, typename T, typename H>
template <typename S, typename>
inline void SmartChunk<TransmissionInterface, T, H>::deliver() noexcept
{
    if (m_members.smartChunkUniquePtr)
    {
        m_members.producerRef.get().publish(
            std::move(*reinterpret_cast<typename TransmissionInterface<T, H>::SampleType*>(this)));
    }
    else
    {
        LogError() << "Tried to deliver empty Sample! Might be an already delivered or moved Sample!";
        errorHandler(Error::kPOSH__PUBLISHING_EMPTY_SAMPLE, nullptr, ErrorLevel::MODERATE);
    }
}


} // namespace popo
} // namespace iox

#endif // IOX_POSH_POPO_SAMPLE_INL