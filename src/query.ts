const require = createRequire(import.meta.url);
import { createRequire } from 'node:module';
import streamDeck from '@elgato/streamdeck';

enum LogLevel {
  Error,
  Warning,
  Info,
  Debug,
}

const gpuAddon = require('./gpu-metrics.node');

const query = new gpuAddon.GpuQuery();

gpuAddon.logCallback((level: number, message: string) => {
  switch (level) {
    case LogLevel.Error:
      streamDeck.logger.error(message);
      break;
    case LogLevel.Warning:
      streamDeck.logger.warn(message);
      break;
    case LogLevel.Info:
      streamDeck.logger.info(message);
      break;
    case LogLevel.Debug:
      streamDeck.logger.debug(message);
      break;
    default:
      streamDeck.logger.info(message);
  }
});

export default query;
