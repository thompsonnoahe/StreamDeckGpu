import streamDeck from '@elgato/streamdeck';

export default class Buffer<T> {
  buffer: T[];
  size: number;
  isFull: boolean;

  constructor(maxSize: number) {
    this.buffer = new Array();
    this.size = maxSize;
    this.isFull = false;
  }

  enqueue(item: any) {
    if (this.buffer.length > this.size) {
      this.isFull = true;
      this.buffer.shift();
    }

    return this.buffer.push(item);
  }
  dequeue() {
    this.isFull = this.buffer.length - 1 > this.size;
    return this.buffer.pop();
  }
  isEmpty() {
    return !this.isFull;
  }
  printBuffer() {
    streamDeck.logger.debug(this.buffer);
  }
}
