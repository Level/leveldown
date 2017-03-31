{'targets': [{
    'target_name': 'leveldb'
  , 'variables': {
        'ldbversion': 'rocksdb'
    }
  , 'type': 'static_library'
		# Overcomes an issue with the linker and thin .a files on SmartOS
  , 'standalone_static_library': 1
  , 'dependencies': [
        '../snappy/snappy.gyp:snappy'
    ]
  , 'direct_dependent_settings': {
        'include_dirs': [
            'leveldb-<(ldbversion)/include/'
          # , 'leveldb-<(ldbversion)/port/'
          # , 'leveldb-<(ldbversion)/util'
          # , 'leveldb-<(ldbversion)/'
        ]
    }
  , 'defines': [
        'SNAPPY=1'
    ]
  , 'include_dirs': [
        'leveldb-<(ldbversion)/'
      , 'leveldb-<(ldbversion)/include/'
    ]
  , 'conditions': [
        ['OS == "win"', {
            'conditions': [
                ['MSVS_VERSION != "2015" and MSVS_VERSION != "2013"', {
                     'include_dirs': [ 'leveldb-<(ldbversion)/port/win' ]
                }]
            ],
            'include_dirs': [
                'port-libuv/'
            ]
          , 'defines': [
                'LEVELDB_PLATFORM_UV=1'
              , 'NOMINMAX=1'
              , '_HAS_EXCEPTIONS=0'
            ]
          , 'sources': [
                'port-libuv/port_uv.cc'
              , 'port-libuv/env_win.cc'
              , 'port-libuv/win_logger.cc'
            ]
          , 'msvs_settings': {
                'VCCLCompilerTool': {
                    'RuntimeTypeInfo': 'false'
                  , 'EnableFunctionLevelLinking': 'true'
                  , 'ExceptionHandling': '2'
                  , 'DisableSpecificWarnings': [ '4355', '4530' ,'4267', '4244' ]
                }
            }
        }, { # OS != "win"
            'sources': [
                'leveldb-<(ldbversion)/port/port_posix.cc'
              , 'leveldb-<(ldbversion)/port/port_posix.h'
              , 'leveldb-<(ldbversion)/util/env_posix.cc'
            ]
          , 'defines': [
                'ROCKSDB_PLATFORM_POSIX=1'
            ]
          , 'ccflags': [
                '-fno-builtin-memcmp'
              , '-fPIC'
            ]
          , 'cflags': [ '-std=c++0x' ]
          , 'cflags!': [ '-fno-tree-vrp', '-fno-rtti' ]
          , 'cflags_cc!': [ '-fno-rtti' ]
          # , 'cflags_cc+': [ '-frtti' ]
        }]
      , ['OS != "win"' and 'OS != "freebsd"', {
            'cflags': [
                '-Wno-sign-compare'
              , '-Wno-unused-but-set-variable'
            ]
        }]
      , ['OS == "linux"', {
            'defines': [
                'OS_LINUX=1'
            ]
          , 'libraries': [
                '-lpthread'
            ]
          , 'ccflags': [
                '-pthread'
            ]
        }]
      , ['OS == "freebsd"', {
            'defines': [
                'OS_FREEBSD=1'
              , '_REENTRANT=1'
            ]
          , 'libraries': [
                '-lpthread'
            ]
          , 'ccflags': [
                '-pthread'
            ]
          , 'cflags': [
                '-Wno-sign-compare'
            ]
        }]
      , ['OS == "solaris"', {
            'defines': [
                'OS_SOLARIS=1'
              , '_REENTRANT=1'
            ]
          , 'libraries': [
                '-lrt'
              , '-lpthread'
            ]
          , 'ccflags': [
                '-pthread'
            ]
        }]
      , ['OS == "mac"', {
            'defines': [
                'OS_MACOSX=1',
                'ROCKSDB_LIB_IO_POSIX=1',
                'ROCKSDB_BACKTRACE=1'
            ]
          , 'libraries': []
          , 'ccflags': []
          , 'xcode_settings': {
                'WARNING_CFLAGS': [
                    '-Wno-sign-compare'
                  , '-Wno-unused-variable'
                  , '-Wno-unused-function'
                ]
                , 'OTHER_CPLUSPLUSFLAGS': [
                    '-mmacosx-version-min=10.8'
                  , '-std=c++11'
                  , '-stdlib=libc++'
                  , '-march=native'
                  , '-fno-omit-frame-pointer'
                  , '-momit-leaf-frame-pointer'
                ]
# , 'OTHER_LDFLAGS': ['-stdlib=libc++']
                , 'GCC_ENABLE_CPP_RTTI': 'YES'
                , 'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
                , 'MACOSX_DEPLOYMENT_TARGET': '10.8'
            }
        }]
    ]
  , 'sources': [
        'leveldb-<(ldbversion)/db/auto_roll_logger.cc'
      , 'leveldb-<(ldbversion)/db/builder.cc'
      , 'leveldb-<(ldbversion)/db/c.cc'
      , 'leveldb-<(ldbversion)/db/column_family.cc'
      , 'leveldb-<(ldbversion)/db/compacted_db_impl.cc'
      , 'leveldb-<(ldbversion)/db/compaction.cc'
      , 'leveldb-<(ldbversion)/db/compaction_iterator.cc'
      , 'leveldb-<(ldbversion)/db/compaction_job.cc'
      , 'leveldb-<(ldbversion)/db/compaction_picker.cc'
      , 'leveldb-<(ldbversion)/db/convenience.cc'
      , 'leveldb-<(ldbversion)/db/range_del_aggregator.cc'
      , 'leveldb-<(ldbversion)/db/db_filesnapshot.cc'
      , 'leveldb-<(ldbversion)/db/dbformat.cc'
      , 'leveldb-<(ldbversion)/db/db_impl.cc'
      , 'leveldb-<(ldbversion)/db/db_impl_debug.cc'
      , 'leveldb-<(ldbversion)/db/db_impl_readonly.cc'
      , 'leveldb-<(ldbversion)/db/db_impl_experimental.cc'
      , 'leveldb-<(ldbversion)/db/db_info_dumper.cc'
      , 'leveldb-<(ldbversion)/db/db_iter.cc'
      , 'leveldb-<(ldbversion)/db/external_sst_file_ingestion_job.cc'
      , 'leveldb-<(ldbversion)/db/experimental.cc'
      , 'leveldb-<(ldbversion)/db/event_helpers.cc'
      , 'leveldb-<(ldbversion)/db/file_indexer.cc'
      , 'leveldb-<(ldbversion)/db/filename.cc'
      , 'leveldb-<(ldbversion)/db/flush_job.cc'
      , 'leveldb-<(ldbversion)/db/flush_scheduler.cc'
      , 'leveldb-<(ldbversion)/db/forward_iterator.cc'
      , 'leveldb-<(ldbversion)/db/internal_stats.cc'
      , 'leveldb-<(ldbversion)/db/log_reader.cc'
      , 'leveldb-<(ldbversion)/db/log_writer.cc'
      , 'leveldb-<(ldbversion)/db/managed_iterator.cc'
      , 'leveldb-<(ldbversion)/db/memtable_allocator.cc'
      , 'leveldb-<(ldbversion)/db/memtable.cc'
      , 'leveldb-<(ldbversion)/db/memtable_list.cc'
      , 'leveldb-<(ldbversion)/db/merge_helper.cc'
      , 'leveldb-<(ldbversion)/db/merge_operator.cc'
      , 'leveldb-<(ldbversion)/db/repair.cc'
      , 'leveldb-<(ldbversion)/db/snapshot_impl.cc'
      , 'leveldb-<(ldbversion)/db/table_cache.cc'
      , 'leveldb-<(ldbversion)/db/table_properties_collector.cc'
      , 'leveldb-<(ldbversion)/db/transaction_log_impl.cc'
      , 'leveldb-<(ldbversion)/db/version_builder.cc'
      , 'leveldb-<(ldbversion)/db/version_edit.cc'
      , 'leveldb-<(ldbversion)/db/version_set.cc'
      , 'leveldb-<(ldbversion)/db/wal_manager.cc'
      , 'leveldb-<(ldbversion)/db/write_batch.cc'
      , 'leveldb-<(ldbversion)/db/write_batch_base.cc'
      , 'leveldb-<(ldbversion)/db/write_controller.cc'
      , 'leveldb-<(ldbversion)/db/write_thread.cc'
      , 'leveldb-<(ldbversion)/memtable/hash_cuckoo_rep.cc'
      , 'leveldb-<(ldbversion)/memtable/hash_linklist_rep.cc'
      , 'leveldb-<(ldbversion)/memtable/hash_skiplist_rep.cc'
      , 'leveldb-<(ldbversion)/memtable/skiplistrep.cc'
      , 'leveldb-<(ldbversion)/memtable/vectorrep.cc'
      , 'leveldb-<(ldbversion)/port/port_posix.cc'
      , 'leveldb-<(ldbversion)/port/port.h'
      , 'leveldb-<(ldbversion)/port/stack_trace.cc'
      , 'leveldb-<(ldbversion)/table/adaptive_table_factory.cc'
      , 'leveldb-<(ldbversion)/table/block_based_filter_block.cc'
      , 'leveldb-<(ldbversion)/table/block_based_table_builder.cc'
      , 'leveldb-<(ldbversion)/table/block_based_table_factory.cc'
      , 'leveldb-<(ldbversion)/table/block_based_table_reader.cc'
      , 'leveldb-<(ldbversion)/table/block_builder.cc'
      , 'leveldb-<(ldbversion)/table/block.cc'
      , 'leveldb-<(ldbversion)/table/block_prefix_index.cc'
      , 'leveldb-<(ldbversion)/table/bloom_block.cc'
      , 'leveldb-<(ldbversion)/table/cuckoo_table_builder.cc'
      , 'leveldb-<(ldbversion)/table/cuckoo_table_factory.cc'
      , 'leveldb-<(ldbversion)/table/cuckoo_table_reader.cc'
      , 'leveldb-<(ldbversion)/table/flush_block_policy.cc'
      , 'leveldb-<(ldbversion)/table/format.cc'
      , 'leveldb-<(ldbversion)/table/full_filter_block.cc'
      , 'leveldb-<(ldbversion)/table/get_context.cc'
      , 'leveldb-<(ldbversion)/table/index_builder.cc'
      , 'leveldb-<(ldbversion)/table/iterator.cc'
      , 'leveldb-<(ldbversion)/table/merging_iterator.cc'
      , 'leveldb-<(ldbversion)/table/meta_blocks.cc'
      , 'leveldb-<(ldbversion)/table/sst_file_writer.cc'
      , 'leveldb-<(ldbversion)/table/partitioned_filter_block.cc'
      , 'leveldb-<(ldbversion)/table/plain_table_builder.cc'
      , 'leveldb-<(ldbversion)/table/plain_table_factory.cc'
      , 'leveldb-<(ldbversion)/table/plain_table_index.cc'
      , 'leveldb-<(ldbversion)/table/plain_table_key_coding.cc'
      , 'leveldb-<(ldbversion)/table/plain_table_reader.cc'
      , 'leveldb-<(ldbversion)/table/persistent_cache_helper.cc'
      , 'leveldb-<(ldbversion)/table/table_properties.cc'
      , 'leveldb-<(ldbversion)/table/two_level_iterator.cc'
      , 'leveldb-<(ldbversion)/tools/dump/db_dump_tool.cc'
      , 'leveldb-<(ldbversion)/util/arena.cc'
      , 'leveldb-<(ldbversion)/util/bloom.cc'
, 'leveldb-<(ldbversion)/util/build_version.cc'
      , 'leveldb-<(ldbversion)/util/cf_options.cc'
      , 'leveldb-<(ldbversion)/util/clock_cache.cc'
      , 'leveldb-<(ldbversion)/util/coding.cc'
      , 'leveldb-<(ldbversion)/util/comparator.cc'
      , 'leveldb-<(ldbversion)/util/compaction_job_stats_impl.cc'
      , 'leveldb-<(ldbversion)/util/concurrent_arena.cc'
      , 'leveldb-<(ldbversion)/util/crc32c.cc'
      , 'leveldb-<(ldbversion)/util/db_options.cc'
      , 'leveldb-<(ldbversion)/util/delete_scheduler.cc'
      , 'leveldb-<(ldbversion)/util/dynamic_bloom.cc'
      , 'leveldb-<(ldbversion)/util/env.cc'
      , 'leveldb-<(ldbversion)/util/env_chroot.cc'
      , 'leveldb-<(ldbversion)/util/env_hdfs.cc'
      , 'leveldb-<(ldbversion)/util/env_posix.cc'
      , 'leveldb-<(ldbversion)/util/event_logger.cc'
      , 'leveldb-<(ldbversion)/util/file_util.cc'
      , 'leveldb-<(ldbversion)/util/file_reader_writer.cc'
      , 'leveldb-<(ldbversion)/util/filter_policy.cc'
      , 'leveldb-<(ldbversion)/util/hash.cc'
      , 'leveldb-<(ldbversion)/util/histogram.cc'
      , 'leveldb-<(ldbversion)/util/histogram_windowing.cc'
      , 'leveldb-<(ldbversion)/util/instrumented_mutex.cc'
      , 'leveldb-<(ldbversion)/util/iostats_context.cc'
      , 'leveldb-<(ldbversion)/util/io_posix.cc'
      , 'leveldb-<(ldbversion)/util/log_buffer.cc'
      , 'leveldb-<(ldbversion)/util/logging.cc'
      , 'leveldb-<(ldbversion)/util/lru_cache.cc'
      , 'leveldb-<(ldbversion)/util/memenv.cc'
      , 'leveldb-<(ldbversion)/util/murmurhash.cc'
      , 'leveldb-<(ldbversion)/util/options.cc'
      , 'leveldb-<(ldbversion)/util/options_helper.cc'
      , 'leveldb-<(ldbversion)/util/options_parser.cc'
      , 'leveldb-<(ldbversion)/util/options_sanity_check.cc'
      , 'leveldb-<(ldbversion)/util/perf_context.cc'
      , 'leveldb-<(ldbversion)/util/perf_level.cc'
      , 'leveldb-<(ldbversion)/util/random.cc'
      , 'leveldb-<(ldbversion)/util/rate_limiter.cc'
      , 'leveldb-<(ldbversion)/util/sharded_cache.cc'
      , 'leveldb-<(ldbversion)/util/slice.cc'
      , 'leveldb-<(ldbversion)/util/sst_file_manager_impl.cc'
      , 'leveldb-<(ldbversion)/util/statistics.cc'
      , 'leveldb-<(ldbversion)/util/status.cc'
      , 'leveldb-<(ldbversion)/util/status_message.cc'
      , 'leveldb-<(ldbversion)/util/string_util.cc'
      , 'leveldb-<(ldbversion)/util/sync_point.cc'
      , 'leveldb-<(ldbversion)/util/thread_local.cc'
      , 'leveldb-<(ldbversion)/util/thread_status_impl.cc'
      , 'leveldb-<(ldbversion)/util/thread_status_updater.cc'
      , 'leveldb-<(ldbversion)/util/thread_status_updater_debug.cc'
      , 'leveldb-<(ldbversion)/util/thread_status_util.cc'
      , 'leveldb-<(ldbversion)/util/thread_status_util_debug.cc'
      , 'leveldb-<(ldbversion)/util/threadpool_imp.cc'
      , 'leveldb-<(ldbversion)/util/transaction_test_util.cc'
      , 'leveldb-<(ldbversion)/util/xxhash.cc'
      , 'leveldb-<(ldbversion)/utilities/backupable/backupable_db.cc'
      , 'leveldb-<(ldbversion)/utilities/blob_db/blob_db.cc'
      , 'leveldb-<(ldbversion)/utilities/convenience/info_log_finder.cc'
      , 'leveldb-<(ldbversion)/utilities/checkpoint/checkpoint.cc'
      , 'leveldb-<(ldbversion)/utilities/compaction_filters/remove_emptyvalue_compactionfilter.cc'
      , 'leveldb-<(ldbversion)/utilities/document/document_db.cc'
      , 'leveldb-<(ldbversion)/utilities/document/json_document_builder.cc'
      , 'leveldb-<(ldbversion)/utilities/document/json_document.cc'
      , 'leveldb-<(ldbversion)/utilities/env_mirror.cc'
      , 'leveldb-<(ldbversion)/utilities/geodb/geodb_impl.cc'
      , 'leveldb-<(ldbversion)/utilities/leveldb_options/leveldb_options.cc'
      , 'leveldb-<(ldbversion)/utilities/lua/rocks_lua_compaction_filter.cc'
      , 'leveldb-<(ldbversion)/utilities/memory/memory_util.cc'
      , 'leveldb-<(ldbversion)/utilities/merge_operators/put.cc'
      , 'leveldb-<(ldbversion)/utilities/merge_operators/max.cc'
      , 'leveldb-<(ldbversion)/utilities/merge_operators/string_append/stringappend2.cc'
      , 'leveldb-<(ldbversion)/utilities/merge_operators/string_append/stringappend.cc'
      , 'leveldb-<(ldbversion)/utilities/merge_operators/uint64add.cc'
      , 'leveldb-<(ldbversion)/utilities/option_change_migration/option_change_migration.cc'
      , 'leveldb-<(ldbversion)/utilities/options/options_util.cc'
      , 'leveldb-<(ldbversion)/utilities/persistent_cache/persistent_cache_tier.cc'
      , 'leveldb-<(ldbversion)/utilities/persistent_cache/volatile_tier_impl.cc'
      , 'leveldb-<(ldbversion)/utilities/persistent_cache/block_cache_tier_file.cc'
      , 'leveldb-<(ldbversion)/utilities/persistent_cache/block_cache_tier_metadata.cc'
      , 'leveldb-<(ldbversion)/utilities/persistent_cache/block_cache_tier.cc'
      , 'leveldb-<(ldbversion)/utilities/redis/redis_lists.cc'
      , 'leveldb-<(ldbversion)/utilities/simulator_cache/sim_cache.cc'
      , 'leveldb-<(ldbversion)/utilities/spatialdb/spatial_db.cc'
      , 'leveldb-<(ldbversion)/utilities/table_properties_collectors/compact_on_deletion_collector.cc'
      , 'leveldb-<(ldbversion)/utilities/transactions/optimistic_transaction_impl.cc'
      , 'leveldb-<(ldbversion)/utilities/transactions/optimistic_transaction_db_impl.cc'
      , 'leveldb-<(ldbversion)/utilities/transactions/transaction_base.cc'
      , 'leveldb-<(ldbversion)/utilities/transactions/transaction_db_impl.cc'
      , 'leveldb-<(ldbversion)/utilities/transactions/transaction_db_mutex_impl.cc'
      , 'leveldb-<(ldbversion)/utilities/transactions/transaction_lock_mgr.cc'
      , 'leveldb-<(ldbversion)/utilities/transactions/transaction_impl.cc'
      , 'leveldb-<(ldbversion)/utilities/transactions/transaction_util.cc'
      , 'leveldb-<(ldbversion)/utilities/ttl/db_ttl_impl.cc'
      , 'leveldb-<(ldbversion)/utilities/date_tiered/date_tiered_db_impl.cc'
      , 'leveldb-<(ldbversion)/utilities/write_batch_with_index/write_batch_with_index.cc'
      , 'leveldb-<(ldbversion)/utilities/write_batch_with_index/write_batch_with_index_internal.cc'
    ]
}]}
