// Copyright (c) 2013, Cloudera, inc.

#include "kudu/tablet/rowset.h"
#include "kudu/tablet/tablet_metrics.h"

#include "kudu/util/metrics.h"

// Tablet-specific metrics.
METRIC_DEFINE_counter(rows_inserted, kudu::MetricUnit::kRows,
    "Number of rows inserted into this tablet since service start");
METRIC_DEFINE_counter(rows_updated, kudu::MetricUnit::kRows,
    "Number of row update operations performed on this tablet since service start");
METRIC_DEFINE_counter(rows_deleted, kudu::MetricUnit::kRows,
    "Number of row delete operations performed on this tablet since service start");
METRIC_DEFINE_counter(insertions_failed_dup_key, kudu::MetricUnit::kRows,
                      "Number of inserts which failed because the key already existed");
METRIC_DEFINE_counter(scans_started, kudu::MetricUnit::kRequests,
                      "Number of scanners which have been started on this tablet");

METRIC_DEFINE_counter(blooms_consulted, kudu::MetricUnit::kProbes,
                      "Number of times a bloom filter was consulted");
METRIC_DEFINE_counter(keys_consulted, kudu::MetricUnit::kProbes,
                      "Number of times a key cfile was consulted");
METRIC_DEFINE_counter(deltas_consulted, kudu::MetricUnit::kProbes,
                      "Number of times a delta file was consulted");
METRIC_DEFINE_counter(mrs_consulted, kudu::MetricUnit::kProbes,
                      "Number of times a MemRowSet was consulted.");
METRIC_DEFINE_counter(bytes_flushed, kudu::MetricUnit::kBytes,
    "Number of bytes that have been flushed to disk by this tablet.");

METRIC_DEFINE_histogram(blooms_consulted_per_op, kudu::MetricUnit::kProbes,
                        "Number of times a bloom filter was consulted", 20, 2);

METRIC_DEFINE_histogram(keys_consulted_per_op, kudu::MetricUnit::kProbes,
                        "Number of times a key cfile was consulted", 20, 2);

METRIC_DEFINE_histogram(deltas_consulted_per_op, kudu::MetricUnit::kProbes,
                        "Number of times a delta file was consulted", 20, 2);

METRIC_DEFINE_histogram(write_op_duration_no_consistency,
  kudu::MetricUnit::kMicroseconds,
  "Duration of Writes to this tablet with external consistency set to NO_CONSISTENCY.",
  60000000LU, 2);

METRIC_DEFINE_histogram(write_op_duration_client_propagated_consistency,
  kudu::MetricUnit::kMicroseconds,
  "Duration of Writes to this tablet with external consistency set to CLIENT_PROPAGATED.",
  60000000LU, 2);

METRIC_DEFINE_histogram(write_op_duration_commit_wait_consistency,
  kudu::MetricUnit::kMicroseconds,
  "Duration of Writes to this tablet with external consistency set to COMMIT_WAIT.",
  60000000LU, 2);

METRIC_DEFINE_histogram(commit_wait_duration,
  kudu::MetricUnit::kMicroseconds,
  "Microseconds spent waiting for COMMIT_WAIT external consistency writes for this tablet.",
  60000000LU, 2);

METRIC_DEFINE_histogram(snapshot_scan_inflight_wait_duration,
  kudu::MetricUnit::kMicroseconds,
  "Microseconds spent waiting for in-flight writes to complete for READ_AT_SNAPSHOT scans.",
  60000000LU, 2);

METRIC_DEFINE_gauge_uint32(flush_dms_running, kudu::MetricUnit::kMaintenanceOperations,
  "Number of delta MRS flushes currently running.");

METRIC_DEFINE_gauge_uint32(flush_mrs_running, kudu::MetricUnit::kMaintenanceOperations,
  "Number of MRS flushes currently running.");

METRIC_DEFINE_gauge_uint32(compact_rs_running, kudu::MetricUnit::kMaintenanceOperations,
  "Number of RS compactions currently running.");

METRIC_DEFINE_histogram(flush_dms_duration, kudu::MetricUnit::kSeconds,
  "Seconds spent flushing delta MRS.", 60000000LU, 2);

METRIC_DEFINE_histogram(flush_mrs_duration, kudu::MetricUnit::kSeconds,
  "Seconds spent flushing MRS.", 60000000LU, 2);

METRIC_DEFINE_histogram(compact_rs_duration, kudu::MetricUnit::kSeconds,
  "Seconds spent compacting RS.", 60000000LU, 2);


namespace kudu {
namespace tablet {

#define MINIT(x) x(METRIC_##x.Instantiate(metric_ctx))
#define GINIT(x) x(AtomicGauge<uint32_t>::Instantiate(METRIC_##x, metric_ctx))
TabletMetrics::TabletMetrics(const MetricContext& metric_ctx)
  : MINIT(rows_inserted),
    MINIT(rows_updated),
    MINIT(rows_deleted),
    MINIT(insertions_failed_dup_key),
    MINIT(scans_started),
    MINIT(blooms_consulted),
    MINIT(keys_consulted),
    MINIT(deltas_consulted),
    MINIT(mrs_consulted),
    MINIT(bytes_flushed),
    MINIT(blooms_consulted_per_op),
    MINIT(keys_consulted_per_op),
    MINIT(deltas_consulted_per_op),
    MINIT(commit_wait_duration),
    MINIT(snapshot_scan_inflight_wait_duration),
    MINIT(write_op_duration_no_consistency),
    MINIT(write_op_duration_client_propagated_consistency),
    MINIT(write_op_duration_commit_wait_consistency),
    GINIT(flush_dms_running),
    GINIT(flush_mrs_running),
    GINIT(compact_rs_running),
    MINIT(flush_dms_duration),
    MINIT(flush_mrs_duration),
    MINIT(compact_rs_duration) {
}
#undef MINIT
#undef GINIT

void TabletMetrics::AddProbeStats(const ProbeStats& stats) {
  blooms_consulted->IncrementBy(stats.blooms_consulted);
  keys_consulted->IncrementBy(stats.keys_consulted);
  deltas_consulted->IncrementBy(stats.deltas_consulted);
  mrs_consulted->IncrementBy(stats.mrs_consulted);

  blooms_consulted_per_op->Increment(stats.blooms_consulted);
  keys_consulted_per_op->Increment(stats.keys_consulted);
  deltas_consulted_per_op->Increment(stats.deltas_consulted);
}

} // namespace tablet
} // namespace kudu
