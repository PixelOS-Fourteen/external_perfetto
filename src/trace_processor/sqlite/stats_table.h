/*
 * Copyright (C) 2018 The Android Open Source Project
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

#ifndef SRC_TRACE_PROCESSOR_SQLITE_STATS_TABLE_H_
#define SRC_TRACE_PROCESSOR_SQLITE_STATS_TABLE_H_

#include <limits>
#include <memory>

#include "src/trace_processor/sqlite/sqlite_table.h"
#include "src/trace_processor/storage/stats.h"
#include "src/trace_processor/storage/trace_storage.h"

namespace perfetto {
namespace trace_processor {

// The stats table contains diagnostic info and errors that are either:
// - Collected at trace time (e.g., ftrace buffer overruns).
// - Generated at parsing time (e.g., clock events out-of-order).
class StatsTable final
    : public TypedSqliteTable<StatsTable, const TraceStorage*> {
 public:
  enum Column { kName = 0, kIndex, kSeverity, kSource, kValue, kDescription };
  class Cursor final : public SqliteTable::BaseCursor {
   public:
    explicit Cursor(StatsTable*);
    ~Cursor() final;

    // Implementation of SqliteTable::Cursor.
    base::Status Filter(const QueryConstraints&,
                        sqlite3_value**,
                        FilterHistory);
    base::Status Next();
    bool Eof();
    base::Status Column(sqlite3_context*, int N);

   private:
    Cursor(Cursor&) = delete;
    Cursor& operator=(const Cursor&) = delete;

    Cursor(Cursor&&) noexcept = default;
    Cursor& operator=(Cursor&&) = default;

    StatsTable* table_ = nullptr;
    const TraceStorage* storage_ = nullptr;
    size_t key_ = 0;
    TraceStorage::Stats::IndexMap::const_iterator index_{};
  };

  StatsTable(sqlite3*, const TraceStorage*);
  ~StatsTable() final;

  // Table implementation.
  util::Status Init(int, const char* const*, SqliteTable::Schema*) final;
  std::unique_ptr<SqliteTable::BaseCursor> CreateCursor() final;
  int BestIndex(const QueryConstraints&, BestIndexInfo*) final;

 private:
  const TraceStorage* const storage_;
};
}  // namespace trace_processor
}  // namespace perfetto

#endif  // SRC_TRACE_PROCESSOR_SQLITE_STATS_TABLE_H_
