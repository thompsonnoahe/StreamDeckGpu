const require = createRequire(import.meta.url);
import { createRequire } from "node:module";
import streamDeck from "@elgato/streamdeck";
import * as os from "os";

enum LogLevel {
  Error,
  Warning,
  Info,
  Debug,
}

const gpuAddon = os.platform() === "win32" ? require("./gpu.node") : null;

const query = os.platform() === "win32" ? new gpuAddon.GpuQuery() : null;

gpuAddon?.logCallback((level: number, message: string) => {
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
