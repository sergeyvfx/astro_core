// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/database.h"

#include <algorithm>
#include <array>

#include "astro_core/base/convert.h"
#include "astro_core/base/levenshtein_distance.h"
#include "astro_core/base/static_vector.h"
#include "astro_core/base/string.h"
#include "astro_core/parse/foreach_word.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace experimental {

namespace {

// Insert (key, value) into already pre-sorted by key index table.
template <typename Index>
void IndexInsert(Index& index,
                 const typename Index::value_type::key_type& key,
                 typename Index::value_type::value_type value) {
  using Row = typename Index::value_type;
  using Key = typename Index::value_type::key_type;

  auto insert_before_it = std::upper_bound(
      index.begin(),
      index.end(),
      key,
      [](const Key& bound_key, const Row& row) { return bound_key < row.key; });

  index.emplace(insert_before_it, key, value);
}

// Lookup iterator for the given key in the index.
// The index is expected to be sorted by this key.
template <typename Index>
auto IndexLookupIterator(Index& index,
                         const typename Index::value_type::key_type& key) {
  using Row = typename Index::value_type;
  using Key = typename Index::value_type::key_type;

  auto it = std::lower_bound(
      index.begin(),
      index.end(),
      key,
      [](const Row& row, const Key& bound_key) { return row.key < bound_key; });

  if (it == index.end() || it->key != key) {
    return index.end();
  }

  return it;
}

// Lookup value for the given key in the index.
// The index is expected to be sorted by this key.
//
// If the key does not exist in the index false is returned and value is kept
// untouched. Otherwise the value is assigned to the one stored in the index and
// true is returned.
template <typename Index>
auto IndexLookup(Index& index,
                 const typename Index::value_type::key_type& key,
                 typename Index::value_type::value_type& value) -> bool {
  auto it = IndexLookupIterator(index, key);
  if (it == index.end()) {
    return false;
  }

  value = it->value;

  return true;
}

}  // namespace

void SatelliteDatabase::Clear() {
  satellites_.clear();
  catalog_number_index_.clear();
  transmitters_.clear();
}

auto SatelliteDatabase::AddSatellite(const int catalog_number_id,
                                     const std::string_view name)
    -> SatelliteDAO {
  // Trim the name to the one which fits the database.
  Satellite& satellite = satellites_.emplace_back(catalog_number_id, name);

  IndexInsert(catalog_number_index_, catalog_number_id, &satellite);

  return SatelliteDAO(this, &satellite);
}

auto SatelliteDatabase::LookupSatelliteByCatalogNumber(const int catalog_number)
    -> SatelliteDAO {
  Satellite* satellite{nullptr};
  if (!IndexLookup(catalog_number_index_, catalog_number, satellite)) {
    return SatelliteDAO(nullptr, nullptr);
  }
  return SatelliteDAO(this, satellite);
}

void SatelliteDatabase::RemoveSatelliteByCatalogNumber(
    const int catalog_number) {
  auto index_it = IndexLookupIterator(catalog_number_index_, catalog_number);
  if (index_it == catalog_number_index_.end()) {
    return;
  }

  Satellite* satellite = index_it->value;

  // TODO(sergey): Re-claim the memory used by transmitters of the satellite.
  // Either re-locate transmitters from the end of the table to the freed slots,
  // or mark the freed transmitters as such and re-use them when new items are
  // added.
  //
  // Currently the transmitters of the removed satellites are dangling pointers,
  // which don't have any other side effect as extra memory usage.

  if (satellite == &satellites_.back()) {
    satellites_.pop_back();
    catalog_number_index_.erase(index_it);
    return;
  }

  *satellite = std::move(satellites_.back());
  satellites_.pop_back();

  catalog_number_index_.erase(index_it);

  auto moved_index_it =
      IndexLookupIterator(catalog_number_index_, satellite->catalog_number);
  moved_index_it->value = satellite;
}

auto SatelliteDatabase::LookupSatelliteByCatalogNumber(
    const int catalog_number) const -> ConstSatelliteDAO {
  Satellite* satellite{nullptr};
  if (!IndexLookup(catalog_number_index_, catalog_number, satellite)) {
    return ConstSatelliteDAO(nullptr, nullptr);
  }
  return ConstSatelliteDAO(this, satellite);
}

////////////////////////////////////////////////////////////////////////////////
// Search satellite by string query.

void SatelliteDatabase::SatelliteSearchResult::Add(SatelliteDAO satellite_dao,
                                                   const float score) {
  if (satellite_scores.empty()) {
    satellite_scores.push_back(SatelliteScore{satellite_dao, score});
    return;
  }

  // Remove the least plausible element.
  if (satellite_scores.size() == satellite_scores.capacity()) {
    satellite_scores.pop_back();
  }

  auto insert_before_it = std::upper_bound(
      satellite_scores.begin(),
      satellite_scores.end(),
      score,
      [](const float score_key, const SatelliteScore& satellite_score) {
        return satellite_score.score < score_key;
      });

  satellite_scores.insert(insert_before_it,
                          SatelliteScore{satellite_dao, score});
}

namespace {

// Precalculated split of query words.
// Stores a fixed number of words: if the actual number of words is beyond of
// the limit they are ignored.
struct QueryWords {
  static constexpr int kMaxWords = 32;

  QueryWords() = default;

  explicit QueryWords(const std::string_view query) {
    for (const std::string_view word : ForeachWord(query)) {
      words.push_back(word);

      // Ignore the words past the allowed capacity.
      // It is unlikely to have such query in a real life.
      if (words.size() == words.capacity()) {
        break;
      }
    }
  }

  StaticVector<std::string_view, kMaxWords> words;
};

// Calculate score how well the word matches the query: higher the score better
// the match is.
auto WordSearchScore(const std::string_view word, const std::string_view query)
    -> float {
  // Empty str or query is no match.
  if (word.empty() || query.empty()) {
    return 0.0f;
  }

  // Word matches the query exactly.
  if (word == query) {
    return 3.0f;
  }

  // The word starts with the exact query.
  if (CaseInsensitiveStartsWith(word, query)) {
    // TODO(sergey): Give extra score if the query is almost the same length
    // than the word.
    return 2.0f;
  }

  // Substring not at word start.
  if (CaseInsensitiveFind(word, query) != std::string_view::npos) {
    return 1.0f;
  }

  const LevenshteinDistance levenshtein_distance =
      CalculateCaseInsensitiveLevenshteinDistance(word, query);
  if (levenshtein_distance.GetCost() >= query.size()) {
    // The distance is too big.
    // If it is easier to re-type the entire string then consider this as a
    // complete no-match.
    return 0.0f;
  }

  // Normalize the result to values below 1.
  const float max_cost = word.size() + query.size();
  return (max_cost - levenshtein_distance.GetCost()) / max_cost;
}

// Calculate score of the satellite: how well it matches the query. Higher the
// score better the match is.
auto SatelliteSearchScore(SatelliteDAO satellite_dao,
                          const QueryWords& query_words) -> float {
  float final_score = 0;

  std::array<bool, QueryWords::kMaxWords> used_query_words{};

  auto handle_satellite_word = [&](const std::string_view satellite_word) {
    int best_query_word_index = -1;
    float best_score = 0.0f;

    for (int i = 0; i < query_words.words.size(); ++i) {
      if (used_query_words[i]) {
        continue;
      }
      const std::string_view query_word = query_words.words[i];
      const float score = WordSearchScore(satellite_word, query_word);

      // Ignore scores of 0 (happens due to the initial value of best_score)
      // as it means the word did not match the query at all.
      if (score > best_score) {
        best_score = score;
        best_query_word_index = i;
      }
    }

    if (best_query_word_index != -1) {
      used_query_words[best_query_word_index] = true;

      // Simple accumulation, which also makes it so more words from the
      // query matched higher the satellite will be in the result.
      //
      // TODO(sergey): There could be better normalization strategies to
      // either give extra priority to results where all words matched, or
      // give extra penalty if some query words were not used.
      final_score += best_score;
    }
  };

  char catalog_number_str[32];
  IntToStringBuffer(satellite_dao.GetCatalogNumber(), catalog_number_str);

  handle_satellite_word(catalog_number_str);

  // Greedy maximization of the match between words of the name and the query.
  for (const std::string_view name_word :
       ForeachWord(satellite_dao.GetName())) {
    handle_satellite_word(name_word);
  }

  return final_score;
}

}  // namespace

auto SatelliteDatabase::SearchSatellites(const std::string_view query)
    -> SatelliteSearchResult {
  const QueryWords query_words(query);

  SatelliteSearchResult result;

  ForeachSatellite([&](SatelliteDAO satellite_dao) {
    const float score = SatelliteSearchScore(satellite_dao, query_words);

    if (score != 0) {
      result.Add(satellite_dao, score);
    }
  });

  return result;
}

////////////////////////////////////////////////////////////////////////////////
// Satellite transmitters.

auto SatelliteDatabase::AddTransmitter(Satellite& satellite) -> TransmitterDAO {
  Transmitter& transmitter = transmitters_.emplace_back();

  satellite.transmitters.Append(&transmitter);

  return TransmitterDAO(this, &transmitter);
}

}  // namespace experimental

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
