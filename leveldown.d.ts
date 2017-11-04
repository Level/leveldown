import { AbstractLevelDOWN, AbstractIteratorOptions, AbstractIterator } from 'abstract-leveldown';

type BaseType = string | Buffer

export interface LevelDown
  extends LevelDownConstructor, AbstractLevelDOWN<
  BaseType, BaseType,
  LevelDownOptions,
  LevelDownPutOptions,
  LevelDownGetOptions,
  LevelDownDeleteOptions,
  LevelDownIteratorOptions,
  LevelDownBatchOptions> {

  approximateSize(start: BaseType, end: BaseType, cb: (err: any, size: number) => void): void;
  compactRange(start: BaseType, end: BaseType, cb: (err?: any) => void): void;
  getProperty(property: string): string;
  destroy(location: string, cb: (err?: any) => void): void;
  repair(location: string, cb: (err?: any) => void): void;

  iterator(options?: LevelDownIteratorOptions & AbstractIteratorOptions<BaseType>): LevelDownIterator
}

interface LevelDownConstructor {
  new (location: string): LevelDown
  (location: string): LevelDown
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

export interface LevelDownIteratorOptions {
  fillCache?: boolean;

  keyAsBuffer?: boolean;
  valueAsBuffer?: boolean;
}

export interface LevelDownBatchOptions {
  sync?: boolean;
}

export interface LevelDownIterator extends AbstractIterator<BaseType, BaseType> {
  seek(key: BaseType): void;
  binding: any;
  cache: any;
  finished: any;
  fastFuture: any;
}

declare const LevelDown: LevelDownConstructor
export default LevelDown;
