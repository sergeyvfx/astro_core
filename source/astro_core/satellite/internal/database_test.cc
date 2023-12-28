// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/database.h"

#include <filesystem>
#include <vector>

#include "astro_core/satellite/database_3le.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"
#include "tl_io/tl_io_file.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace experimental {

using testing::ElementsAre;

class SatelliteDatabaseTest : public testing::Test {
 protected:
  auto LoadActiveElementsDatabase() -> SatelliteDatabase {
    using Path = std::filesystem::path;
    using File = tiny_lib::io_file::File;

    const Path active_elements_path =
        testing::TestFileAbsolutePath(Path("celestrak") / "active.txt");

    std::string elements_3le;
    if (!File::ReadText(active_elements_path, elements_3le)) {
      ADD_FAILURE() << "Error reading " << active_elements_path;
      return {};
    }

    SatelliteDatabase db;
    Load3LE(db, elements_3le);

    return db;
  }
};

TEST_F(SatelliteDatabaseTest, AddSatellite) {
  SatelliteDatabase db;

  // Simple real case.
  {
    const SatelliteDAO satellite = db.AddSatellite(25338, "NOAA 15");

    EXPECT_EQ(satellite.GetCatalogNumber(), 25338);
    EXPECT_EQ(satellite.GetName(), "NOAA 15");
  }

  // Verify behavior with the very long name.
  {
    const SatelliteDAO satellite = db.AddSatellite(
        -1, "Lorem ipsum dolor sit amet, consectetur adipiscing elit");

    EXPECT_EQ(satellite.GetName(),
              "Lorem ipsum dolor sit amet, consectetur adipiscing elit");
  }
}

// Test whether insertion invalidates DAO.
TEST_F(SatelliteDatabaseTest, AddSatelliteKeepsDAOValid) {
  SatelliteDatabase db;

  const SatelliteDAO satellite = db.AddSatellite(28654, "NOAA 18");

  for (int i = 0; i < 10; ++i) {
    db.AddSatellite(i, "");
  }

  EXPECT_EQ(satellite.GetCatalogNumber(), 28654);
  EXPECT_EQ(satellite.GetName(), "NOAA 18");
}

TEST_F(SatelliteDatabaseTest, SatelliteTransmitter) {
  SatelliteDatabase db;

  // Make sure long names are not truncated.
  {
    SatelliteDAO satellite = db.AddSatellite(25338, "NOAA 15");
    TransmitterDAO transmitter = satellite.AddTransmitter();

    transmitter.SetName("CUCU (SpaceX COTS UHF Communication Unit)");

    EXPECT_EQ(transmitter.GetName(),
              "CUCU (SpaceX COTS UHF Communication Unit)");
  }
}

TEST_F(SatelliteDatabaseTest, LookupSatelliteByCatalogNumber) {
  SatelliteDatabase db;

  EXPECT_FALSE(db.LookupSatelliteByCatalogNumber(123));

  for (int i = 0; i < 5; ++i) {
    db.AddSatellite(i, "");
  }
  for (int i = 0; i < 5; ++i) {
    db.AddSatellite(9 - i, "");
  }

  for (int i = 0; i < 10; ++i) {
    const SatelliteDAO satellite = db.LookupSatelliteByCatalogNumber(i);
    ASSERT_TRUE(satellite) << "Can not find satellite number " << i;
    EXPECT_EQ(satellite.GetCatalogNumber(), i);
  }
}

TEST_F(SatelliteDatabaseTest, RemoveSatelliteByCatalogNumber) {
  SatelliteDatabase db;

  for (int i = 0; i < 5; ++i) {
    db.AddSatellite(i, "SAT " + std::to_string(i));
  }

  db.RemoveSatelliteByCatalogNumber(2);

  // Ensure the satellite is no longer traversed.
  std::vector<std::string> traversed_names;
  db.ForeachSatellite([&traversed_names](const SatelliteDAO& satellite) {
    traversed_names.push_back(std::string(satellite.GetName()));
  });
  EXPECT_THAT(traversed_names, ElementsAre("SAT 0", "SAT 1", "SAT 4", "SAT 3"));

  // Ensure the satellite does not exist in the index.
  EXPECT_FALSE(db.LookupSatelliteByCatalogNumber(2));

  // Ensure the rest of index is correct.
  EXPECT_EQ(db.LookupSatelliteByCatalogNumber(1).GetName(), "SAT 1");
  EXPECT_EQ(db.LookupSatelliteByCatalogNumber(0).GetName(), "SAT 0");
  EXPECT_EQ(db.LookupSatelliteByCatalogNumber(4).GetName(), "SAT 4");
  EXPECT_EQ(db.LookupSatelliteByCatalogNumber(3).GetName(), "SAT 3");
}

TEST_F(SatelliteDatabaseTest, ForeachSatellite) {
  SatelliteDatabase db;

  db.AddSatellite(1, "SAT 1");
  db.AddSatellite(2, "SAT 2");
  db.AddSatellite(3, "SAT 3");

  // Non-const iteration.
  {
    std::vector<std::string> traversed_names;
    db.ForeachSatellite([&traversed_names](const SatelliteDAO& satellite) {
      traversed_names.push_back(std::string(satellite.GetName()));
    });

    EXPECT_THAT(traversed_names, ElementsAre("SAT 1", "SAT 2", "SAT 3"));
  }

  // Const iteration.
  {
    const SatelliteDatabase& const_db = db;

    std::vector<std::string> traversed_names;
    const_db.ForeachSatellite(
        [&traversed_names](const ConstSatelliteDAO& satellite) {
          traversed_names.push_back(std::string(satellite.GetName()));
        });

    EXPECT_THAT(traversed_names, ElementsAre("SAT 1", "SAT 2", "SAT 3"));
  }
}

TEST_F(SatelliteDatabaseTest, SearchSatellites) {
  SatelliteDatabase db = LoadActiveElementsDatabase();

  {
    std::vector<std::string> traversed_names;
    db.ForeachSearchSatellite("iss", [&](const ConstSatelliteDAO& satellite) {
      traversed_names.push_back(std::string(satellite.GetName()));
    });
    ASSERT_GE(traversed_names.size(), 2);
    EXPECT_EQ(traversed_names[0], "ISS (ZARYA)");
    EXPECT_EQ(traversed_names[1], "ISS (NAUKA)");
  }

  {
    std::vector<std::string> traversed_names;
    db.ForeachSearchSatellite("zarya", [&](const ConstSatelliteDAO& satellite) {
      traversed_names.push_back(std::string(satellite.GetName()));
    });
    ASSERT_GE(traversed_names.size(), 1);
    EXPECT_EQ(traversed_names[0], "ISS (ZARYA)");
  }

  {
    std::vector<std::string> traversed_names;
    db.ForeachSearchSatellite(
        "zarya iss", [&](const ConstSatelliteDAO& satellite) {
          traversed_names.push_back(std::string(satellite.GetName()));
        });
    ASSERT_GE(traversed_names.size(), 2);
    EXPECT_EQ(traversed_names[0], "ISS (ZARYA)");
    EXPECT_EQ(traversed_names[1], "ISS (NAUKA)");
  }

  {
    std::vector<std::string> traversed_names;
    db.ForeachSearchSatellite("zaria", [&](const ConstSatelliteDAO& satellite) {
      traversed_names.push_back(std::string(satellite.GetName()));
    });
    ASSERT_GE(traversed_names.size(), 1);
    EXPECT_EQ(traversed_names[0], "ISS (ZARYA)");
  }

  {
    std::vector<std::string> traversed_names;
    db.ForeachSearchSatellite("25338", [&](const ConstSatelliteDAO& satellite) {
      traversed_names.push_back(std::string(satellite.GetName()));
    });
    ASSERT_GE(traversed_names.size(), 1);
    EXPECT_EQ(traversed_names[0], "NOAA 15");
  }
}

namespace {

struct AllocatorData {
  int num_allocation{0};
  size_t total_allocated_bytes{0};
};

auto MemoryAllocate(void* data_v, const size_t num_bytes) {
  AllocatorData* data = static_cast<AllocatorData*>(data_v);
  ++data->num_allocation;
  data->total_allocated_bytes += num_bytes;

  return ::operator new(num_bytes);
}

auto MemoryFree(void* /*data_v*/, void* ptr) { ::operator delete(ptr); }

}  // namespace

TEST_F(SatelliteDatabaseTest, Allocator) {
  AllocatorData allocator_data;
  SatelliteDatabase::SetAllocatorFunctions(
      &allocator_data, MemoryAllocate, MemoryFree);

  SatelliteDatabase db = LoadActiveElementsDatabase();

  std::cout << "Number of allocations: " << allocator_data.num_allocation
            << std::endl;
  std::cout << "Total allocated bytes: " << allocator_data.total_allocated_bytes
            << std::endl;

  SatelliteDatabase::ResetAllocatorFunctions();
}

}  // namespace experimental

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
