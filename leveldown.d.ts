import * as Abstract from 'abstract-leveldown';

declare namespace leveldown {
  export interface LevelDown<TKey, TValue>
    extends Abstract.LevelDOWN<
    TKey,
    TValue,
    LevelDownOptions,
    LevelDownPutOptions,
    LevelDownGetOptions,
    LevelDownDeleteOptions,
    LevelDownIteratorOptions<TKey, TValue>,
    LevelDownBatchOptions> {
    approximateSize(start: TKey, end: TKey, cb: (err?: any) => void): void;
    compactRange(start: TKey, end: TKey, cb: (err?: any) => void): void;
    getProperty(property: string): string;
    iterator(options?: LevelDownIteratorOptions<TKey, TValue>): LevelDownIterator<TKey>
    destroy(location: string, cb: (err?: any) => void): void;
    repair(location: string, cb: (err?: any) => void): void;
  }

  export interface LevelDownOptions {
    createIfMissing?: boolean;
    errorIfExists?: boolean;
    compression?: boolean;
    cacheSize?: number;
    writeBufferSize?: number;
    blockSize?: number;
    maxOpenFiles?: number;
    blockRestartInterval?: number;
    maxFileSize?: number;
  }

  export interface LevelDownPutOptions {
    sync?: boolean
  }

  export interface LevelDownGetOptions {
    fillCache?: boolean;
    asBuffer?: boolean;
  }

  export interface LevelDownDeleteOptions {
    sync?: boolean;
  }

  export interface LevelDownIteratorOptions<K, V> {
    gt?: K;
    gte?: K;
    lt?: K;
    lte?: K;
    reverse?: boolean;
    keys?: boolean;
    values?: boolean;
    limit?: number;
    fillCache?: boolean;
    keyAsBuffer?: boolean;
    valueAsBuffer?: boolean;
  }

  export interface LevelDownBatchOptions {
    sync?: boolean;
  }

  export interface LevelDownIterator<TKey> extends Abstract.Iterator {
    seek(key: TKey);
    binding: any;
    cache: any;
    finished: any;
    fastFuture: any;
  }
}
declare function leveldown<
  TKey=any,
  TValue=any
  >(location: string)
  : leveldown.LevelDown<TKey, TValue>;

export = leveldown;