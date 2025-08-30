import {
  action,
  WillAppearEvent,
  streamDeck,
  DidReceiveSettingsEvent,
  JsonObject,
} from "@elgato/streamdeck";
import { Gpu } from "../types/gpu";
import * as d3 from "d3";
import * as os from "os";
import Buffer from "../utils/buffer";
import { width, height } from "../utils/constants";
import ActionWithChart, { Settings } from "../types/action";
import getMacOSMetrics from "../utils/converter";

@action({ UUID: "com.nthompson.gpu.usage" })
export class GpuUsage extends ActionWithChart<GpuUsageSettings> {
  startTimer(action: any, settings: GpuUsageSettings, gpu: Gpu): void;
  startTimer(action: any, settings: GpuUsageSettings): void;
  startTimer(action: any, settings: GpuUsageSettings, gpu?: Gpu): void {
    // Clear the timer for the action if it's replaced
    if (this.timers.has(action.id)) {
      clearInterval(this.timers.get(action.id));
    }

    this.buffers.set(action.id, new Buffer<[number, number]>(width));

    const svg = d3
      .select(this.window.document.body)
      .append("svg")
      .attr("width", width)
      .attr("height", height);

    this.timers.set(
      action.id,
      setInterval(() => {
        if (gpu === undefined && os.platform() === "win32") {
          streamDeck.logger.error("GPU not found or selected");
          return;
        }

        if (os.platform() === "darwin") {
          gpu = getMacOSMetrics();
        } else {
          streamDeck.logger.error("Unsupported platform.");
          return;
        }

        // -1 indicates an error
        if (gpu?.usage === -1) {
          action.showAlert();
          return;
        }

        action.setTitle(`${Math.round(gpu?.usage)}%`);

        if (settings.enableChart) {
          const chart = this.createChart(
            svg,
            Math.round(gpu?.usage),
            settings,
            action
          );

          action.setImage(
            `data:image/svg+xml;charset=utf8,${encodeURIComponent(chart.node())}`
          );
        } else {
          // Reset the image if the user flips back between chart or image
          action.setImage("gpu.png");
        }
      }, 1000)
    );
  }

  override onDidReceiveSettings(
    ev: DidReceiveSettingsEvent<GpuUsageSettings>
  ): Promise<void> | void {
    if (os.platform() !== "darwin") {
      const gpu = this.getGpu(ev.payload.settings.gpuId);

      this.startTimer(ev.action, ev.payload.settings, gpu!);
    } else {
      this.startTimer(ev.action, ev.payload.settings);
    }
  }

  override onWillAppear(
    ev: WillAppearEvent<GpuUsageSettings>
  ): Promise<void> | void {
    if (os.platform() !== "darwin") {
      const gpu = this.getGpu(ev.payload.settings.gpuId);

      this.startTimer(ev.action, ev.payload.settings, gpu!);
    } else {
      this.startTimer(ev.action, ev.payload.settings);
    }
  }
}

type GpuUsageSettings = {} & Settings & JsonObject;
