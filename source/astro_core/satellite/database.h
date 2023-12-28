// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Database of satellites.
//
// The database contains such informaiton about satellites as:
//
//  - The satellite name (as per TLE name).
//  - Satellite catalog number.
//  - Two line element (TLE).
//  - Transmitter information (frequencies, modes, etc).
//
// NOTE: This is an experimental API, it might get changed or even moved outside
// of the library.

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

#include "astro_core/base/exception.h"
#include "astro_core/base/linked_list.h"
#include "astro_core/satellite/tle.h"
#include "astro_core/table/paged_table.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace experimental {

class SatelliteDAO;
class ConstSatelliteDAO;
class TransmitterDAO;
class ConstTransmitterDAO;

////////////////////////////////////////////////////////////////////////////////
// Custom memory allocator.

using DatabaseMemoryAllocateFunction = void* (*)(void* data, size_t num_bytes);
using DatabaseMemoryFreeFunction = void (*)(void* data, void* ptr);

////////////////////////////////////////////////////////////////////////////////
// Internals.

namespace satellite_database_internal {

class Satellite;
class Transmitter;

////////////////////////////////////////////////////////////////////////////////
// Custom memory allocator.

// Static storage for functions which takes care of memory allocation and
// deallocation.
class AllocatorFunctions {
 public:
  static void SetAllocatorFunctions(
      void* allocator_data,
      DatabaseMemoryAllocateFunction memory_allocate_function,
      DatabaseMemoryFreeFunction memory_free_function) {
    memory_allocator_data_ = allocator_data;
    memory_allocate_function_ = memory_allocate_function;
    memory_free_function_ = memory_free_function;
  }

  static void ResetAllocatorFunctions() {
    memory_allocator_data_ = nullptr;
    memory_allocate_function_ = DefaultMemoryAllocate;
    memory_free_function_ = DefaultMemoryFree;
  }

  static auto Allocate(const size_t num_bytes) -> void* {
    return memory_allocate_function_(memory_allocator_data_, num_bytes);
  }

  static void Free(void* ptr) {
    memory_free_function_(memory_allocator_data_, ptr);
  }

 private:
  // Default implementation of the memory implementation functions.
  static auto DefaultMemoryAllocate(void* /*data*/, const size_t num_bytes)
      -> void* {
    return ::operator new(num_bytes);
  }
  static void DefaultMemoryFree(void* /*data*/, void* ptr) {
    ::operator delete(ptr);
  }

  // Currently configured memory allocation functions.
  static inline void* memory_allocator_data_ = nullptr;
  static inline DatabaseMemoryAllocateFunction memory_allocate_function_ =
      DefaultMemoryAllocate;
  static inline DatabaseMemoryFreeFunction memory_free_function_ =
      DefaultMemoryFree;
};

// C++ style allocator which uses the memory allocation functions from the
// SatelliteDatabase class.
// This allocator is passed to the low-level PagesTable.
template <class T>
class Allocator {
 public:
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using propagate_on_container_move_assignment = std::true_type;

  Allocator() = default;

  template <class U>
  constexpr Allocator(const Allocator<U>&) noexcept {}

  [[nodiscard]] auto allocate(const std::size_t n) -> T* {
    if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
      ThrowOrAbort<std::bad_array_new_length>();
    }

    if (void* ptr = AllocatorFunctions::Allocate(n * sizeof(T))) {
      return static_cast<T*>(ptr);
    }

    ThrowOrAbort<std::bad_alloc>();
  }

  void deallocate(T* ptr, const std::size_t /*n*/) noexcept {
    AllocatorFunctions::Free(static_cast<void*>(ptr));
  }

  template <class U>
  bool operator==(const Allocator<U>& /*other*/) {
    return true;
  }
  template <class U>
  bool operator!=(const Allocator<U>& /*other*/) {
    return false;
  }
};

// Storage type for the name field.
using DatabaseString =
    std::basic_string<char, std::char_traits<char>, Allocator<char>>;

////////////////////////////////////////////////////////////////////////////////
// Database row data.

// Row of transmitter table.
class Transmitter {
 public:
  DatabaseString name{};

  // Frequencies of downlink from the satellite (the frequency satellite is
  // transmitting at) and the uplink to the satellite (the frequency the
  // satellite is listening at).
  // Measured in Hertz.
  // Value of 0 means there is no communication possible.
  int64_t downlink_frequency{0};
  int64_t uplink_frequency{0};

  // Pointers for the list of transmitters.
  Transmitter* next{nullptr};
  Transmitter* prev{nullptr};
};

// Row of satellite table.
class Satellite {
 public:
  Satellite(int new_catalog_number, const std::string_view new_name)
      : catalog_number(new_catalog_number), name(new_name) {}

  int catalog_number{-1};
  DatabaseString name{};
  TLE tle{};

  // List of transmitters, organized in a list.
  // The actual storage is a dedicated table in the database.
  LinkedList<Transmitter> transmitters;
};

// A row of an index.
//
// This is a generic (key, value) row. Neither semantic of the key nor the
// semantic of the value is used here.
template <class Key, class T>
struct IndexRow {
  using key_type = Key;
  using value_type = T;

  IndexRow() = default;

  IndexRow(const Key& new_key, const T& new_value)
      : key(new_key), value(new_value) {}

  IndexRow(const IndexRow& other) = default;
  IndexRow(IndexRow&& other) noexcept = default;

  auto operator=(const IndexRow& other) -> IndexRow& = default;
  auto operator=(IndexRow&& other) -> IndexRow& = default;

  Key key;
  T value;
};

}  // namespace satellite_database_internal

////////////////////////////////////////////////////////////////////////////////
// The database itself.

class SatelliteDatabase {
  using AllocatorFunctions = satellite_database_internal::AllocatorFunctions;

  template <class T>
  using Allocator = satellite_database_internal::Allocator<T>;

 public:
  SatelliteDatabase() = default;
  ~SatelliteDatabase() = default;

  SatelliteDatabase(SatelliteDatabase&& other) noexcept = default;
  auto operator=(SatelliteDatabase&& other) -> SatelliteDatabase& = default;

  // The internal index tables are using pointers.
  // It is possible to support copy construction and assignment, but it is not
  // a priority for now.
  SatelliteDatabase(const SatelliteDatabase& other) = delete;
  auto operator=(const SatelliteDatabase& other) -> SatelliteDatabase& = delete;

  // True if there are no records in the database.
  auto IsEmpty() const -> bool { return satellites_.empty(); }

  // Entirely clear the database.
  // Invalidates all existing DAO.
  void Clear();

  // The name is clipped to the first kMaxName characters.
  //
  // This operation does not invalidate existing DAO.
  auto AddSatellite(int catalog_number, std::string_view name) -> SatelliteDAO;

  // Lookup satellite with the given catalog number in the database.
  // If such satellite does not exist an invalid DAO is returned.
  //
  // The complexity of the lookup is O(log N) where N is the number of
  // satellites in the database.
  auto LookupSatelliteByCatalogNumber(int catalog_number) -> SatelliteDAO;
  auto LookupSatelliteByCatalogNumber(int catalog_number) const
      -> ConstSatelliteDAO;

  // Remove satellite with the given catalog number from the database.
  // If such satellite does not exist the function has no effect.
  //
  // Invalidates DAOs of the corresponding satellite, but also DAO which
  // corresponds to satellite stored at the last row of the database.
  // Unfortunately, it is not possible to know which one is that.
  void RemoveSatelliteByCatalogNumber(int catalog_number);

  // Invoke the given callback with all satellite DAO objects from the database.
  // The given list of args... is passed to the callback, and they are followed
  // with the DAO.
  //
  // The order of satellites is undefined.
  template <class F, class... Args>
  void ForeachSatellite(F&& callback, Args&&... args);
  template <class F, class... Args>
  void ForeachSatellite(F&& callback, Args&&... args) const;

  // Invoke the callback on the best matches of the satellites to the query.
  //
  // The query does fuzzy-matching on the satellite name and the satellite
  // catalog number. The callback is first invoked for the best match, followed
  // with less good matches.
  //
  // The given list of args... is passed to the callback, and they are followed
  // with the DAO.
  template <class F, class... Args>
  void ForeachSearchSatellite(std::string_view query,
                              F&& callback,
                              Args&&... args);

  // Set the memory allocation functions.
  //
  // Notes:
  //
  //   - Modification of the allocation functions must happen before any data
  //     has been allocated in the database.
  //
  //   - Those are the global functions, which affects all instances of this
  //     database class.
  static void SetAllocatorFunctions(
      void* allocator_data,
      DatabaseMemoryAllocateFunction memory_allocate_function,
      DatabaseMemoryFreeFunction memory_free_function) {
    AllocatorFunctions::SetAllocatorFunctions(
        allocator_data, memory_allocate_function, memory_free_function);
  }

  // Reset allocator functions to their default values.
  static void ResetAllocatorFunctions() {
    AllocatorFunctions::ResetAllocatorFunctions();
  }

 private:
  friend class SatelliteDAO;
  friend class TransmitterDAO;

  // The number of rows per table page.
  // Used by all tables in this database.
  static constexpr int kNumRowPerPage = 32;

  // Aliases for shorter access.
  using Satellite = satellite_database_internal::Satellite;
  using Transmitter = satellite_database_internal::Transmitter;
  template <class Key, class Row>
  using IndexRow = satellite_database_internal::IndexRow<Key, Row>;

  //////////////////////////////////////////////////////////////////////////////
  // Satellite search.

  // A temporary result of satellite search.
  // Contains the information about the satellite, as well as the score how good
  // it matched the search query.
  struct SatelliteScore;

  // Result of satellite search.
  // Contains a fixed amount of the top best matches.
  struct SatelliteSearchResult;

  auto SearchSatellites(std::string_view query) -> SatelliteSearchResult;

  //////////////////////////////////////////////////////////////////////////////
  // Satellite transmitters.

  // Add empty-initialized transmitter to the satellite.
  auto AddTransmitter(Satellite& satellite) -> TransmitterDAO;

  //////////////////////////////////////////////////////////////////////////////
  // Properties.

  // Table with satellites.
  // This is the actual storage of the satellites.
  using SatelliteTable = PagedTable<Satellite, kNumRowPerPage, Allocator>;
  SatelliteTable satellites_;

  // Index on the satellite catalog number.
  //
  // The rows of the index are sorted by the satellite catalog number and
  // points to a row in the satellites table.
  using CatalogNumberIndex =
      PagedTable<IndexRow<int, Satellite*>, kNumRowPerPage, Allocator>;
  CatalogNumberIndex catalog_number_index_;

  // Table with satellite transmitters.
  using TransmitterTable = PagedTable<Transmitter, kNumRowPerPage, Allocator>;
  TransmitterTable transmitters_;
};

////////////////////////////////////////////////////////////////////////////////
// Transmitter DAO.
//
// Data access object for satellite transmitter which allows read or read/write
// access to the transmitter properties.
//
// A thin wrapper which provides read/write access to satellite transmitter
// information stored in the database.
//
// NOTE: Some operations on the database will invalidate the DAO.

// Read-only DAO.
class ConstTransmitterDAO {
  using Transmitter = satellite_database_internal::Transmitter;

 public:
  // Default constructor of an invalid DAN.
  ConstTransmitterDAO() = default;

  // Check whether it is valid DAO.
  //
  // If it is invalid then neither read nor write access to it is possible.
  // Performing such access is undefined.
  inline operator bool() const { return transmitter_; }

  // Get name of the transmitter as a null-terminated std::string-like object.
  inline auto GetName() const
      -> const satellite_database_internal::DatabaseString& {
    return transmitter_->name;
  }

  // Frequencies of downlink from the satellite (the frequency satellite is
  // transmitting at) and the uplink to the satellite (the frequency the
  // satellite is listening at).
  // Measured in Hertz.
  // Value of 0 means there is no communication possible.
  inline auto GetDownlinkFrequency() const -> int64_t {
    return transmitter_->downlink_frequency;
  }
  inline auto GetUplinkFrequency() const -> int64_t {
    return transmitter_->uplink_frequency;
  }

  inline auto Next() const -> ConstTransmitterDAO {
    return ConstTransmitterDAO(database_, transmitter_->next);
  }

 protected:
  friend class SatelliteDatabase;
  friend class SatelliteDAO;
  friend class ConstSatelliteDAO;

  ConstTransmitterDAO(const SatelliteDatabase* database,
                      const Transmitter* transmitter)
      : database_(database), transmitter_(transmitter) {}

  const SatelliteDatabase* database_{nullptr};
  const Transmitter* transmitter_{nullptr};
};

// Read-write DAO.
class TransmitterDAO : public ConstTransmitterDAO {
  using Transmitter = satellite_database_internal::Transmitter;

 public:
  TransmitterDAO() = default;

  inline void SetName(const std::string_view name) {
    GetTransmitter()->name = name;
  }

  // Frequencies of downlink from the satellite (the frequency satellite is
  // transmitting at) and the uplink to the satellite (the frequency the
  // satellite is listening at).
  // Measured in Hertz.
  // Value of 0 means there is no communication possible.
  inline void SetDownlinkFrequency(const int64_t downlink_frequency) {
    GetTransmitter()->downlink_frequency = downlink_frequency;
  }
  inline void SetUplinkFrequency(const int64_t uplink_frequency) {
    GetTransmitter()->uplink_frequency = uplink_frequency;
  }

  using ConstTransmitterDAO::Next;
  inline auto Next() -> TransmitterDAO {
    return TransmitterDAO(GetDatabase(), GetTransmitter()->next);
  }

 protected:
  friend class SatelliteDatabase;
  friend class SatelliteDAO;
  friend class ConstSatelliteDAO;

  TransmitterDAO(SatelliteDatabase* database, Transmitter* transmitter)
      : ConstTransmitterDAO(database, transmitter) {}

  inline auto GetDatabase() const -> const SatelliteDatabase* {
    return database_;
  }
  inline auto GetDatabase() -> SatelliteDatabase* {
    // It is OK to cast the const qualifier away because the object has been
    // created from mutable database and transmitter.
    return const_cast<SatelliteDatabase*>(database_);
  }

  inline auto GetTransmitter() const -> const Transmitter* {
    return transmitter_;
  }
  inline auto GetTransmitter() -> Transmitter* {
    // It is OK to cast the const qualifier away because the object has been
    // created from mutable database and transmitter.
    return const_cast<Transmitter*>(transmitter_);
  }
};

////////////////////////////////////////////////////////////////////////////////
// Satellite DAO.
//
// Data access object for satellite which allows read or read/write access to
// the satellite properties.
//
// A thin wrapper which provides read/write access to satellite information
// stored in the database.
//
// NOTE: Some operations on the database will invalidate the DAO.

// Read-only DAO.
class ConstSatelliteDAO {
  using Satellite = satellite_database_internal::Satellite;

 public:
  // Default constructor of an invalid DAN.
  ConstSatelliteDAO() = default;

  // Check whether it is valid DAO.
  //
  // If it is invalid then neither read nor write access to it is possible.
  // Performing such access is undefined.
  inline operator bool() const { return satellite_; }

  inline auto GetCatalogNumber() const -> int {
    return satellite_->catalog_number;
  }

  // Get name of the transmitter as a null-terminated std::string-like object.
  inline auto GetName() const
      -> const satellite_database_internal::DatabaseString& {
    return satellite_->name;
  }

  inline auto GetTLE() const -> TLE { return satellite_->tle; }

  // Get DAO for the first transmitter of the satellite.
  //
  // The rest of the transmitters are to be accessed by going through the Next()
  // DAO objects of the transmitters.
  //
  // The order of the transmitters is the same as in which they were added to
  // the database.
  inline auto GetFirstTransmitter() const -> ConstTransmitterDAO {
    return ConstTransmitterDAO(database_, satellite_->transmitters.GetHead());
  }

 protected:
  friend class SatelliteDatabase;

  ConstSatelliteDAO(const SatelliteDatabase* database,
                    const Satellite* satellite)
      : database_(database), satellite_(satellite) {}

  const SatelliteDatabase* database_{nullptr};
  const Satellite* satellite_{nullptr};
};

// Read-write DAO.
class SatelliteDAO : public ConstSatelliteDAO {
  using Satellite = satellite_database_internal::Satellite;

 public:
  SatelliteDAO() = default;

  inline void SetTLE(const TLE& tle) { GetSatellite()->tle = tle; }

  // Add transmitter to the back of the list of the current satellite
  // transmitters.
  // Returns DAO of the newly added transmitter. This transmitter is empty-
  // initialized.
  inline auto AddTransmitter() -> TransmitterDAO {
    return GetDatabase()->AddTransmitter(*GetSatellite());
  }

  // Get DAO for the first transmitter of the satellite.
  //
  // The rest of the transmitters are to be accessed by going through the Next()
  // DAO objects of the transmitters.
  //
  // The order of the transmitters is the same as in which they were added to
  // the database.
  using ConstSatelliteDAO::GetFirstTransmitter;
  inline auto GetFirstTransmitter() -> TransmitterDAO {
    return TransmitterDAO(GetDatabase(), satellite_->transmitters.GetHead());
  }

 protected:
  friend class SatelliteDatabase;

  SatelliteDAO(SatelliteDatabase* database, Satellite* satellite)
      : ConstSatelliteDAO(database, satellite) {}

  inline auto GetDatabase() const -> const SatelliteDatabase* {
    return database_;
  }
  inline auto GetDatabase() -> SatelliteDatabase* {
    // It is OK to cast the const qualifier away because the object has been
    // created from mutable database and satellite.
    return const_cast<SatelliteDatabase*>(database_);
  }

  inline auto GetSatellite() const -> const Satellite* { return satellite_; }
  inline auto GetSatellite() -> Satellite* {
    // It is OK to cast the const qualifier away because the object has been
    // created from mutable database and satellite.
    return const_cast<Satellite*>(satellite_);
  }
};

////////////////////////////////////////////////////////////////////////////////
// Implementation.

struct SatelliteDatabase::SatelliteScore {
  SatelliteDAO satellite_dao;
  float score;
};

struct SatelliteDatabase::SatelliteSearchResult {
  static constexpr int kMaxResults = 32;

  void Add(SatelliteDAO satellite_dao, const float score);

  StaticVector<SatelliteScore, kMaxResults> satellite_scores;
};

template <class F, class... Args>
void SatelliteDatabase::ForeachSatellite(F&& callback, Args&&... args) {
  for (Satellite& satellite : satellites_) {
    std::invoke(std::forward<F>(callback),
                std::forward<Args>(args)...,
                SatelliteDAO(this, &satellite));
  }
}

template <class F, class... Args>
void SatelliteDatabase::ForeachSatellite(F&& callback, Args&&... args) const {
  for (const Satellite& satellite : satellites_) {
    std::invoke(std::forward<F>(callback),
                std::forward<Args>(args)...,
                ConstSatelliteDAO(this, &satellite));
  }
}

template <class F, class... Args>
void SatelliteDatabase::ForeachSearchSatellite(std::string_view query,
                                               F&& callback,
                                               Args&&... args) {
  const SatelliteSearchResult result = SearchSatellites(query);
  for (const SatelliteScore& satellite_score : result.satellite_scores) {
    std::invoke(std::forward<F>(callback),
                std::forward<Args>(args)...,
                satellite_score.satellite_dao);
  }
}

}  // namespace experimental

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
