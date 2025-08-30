import {
  action,
  WillAppearEvent,
  streamDeck,
  DidReceiveSettingsEvent,
  JsonObject,
} from "@elgato/streamdeck";
import { Gpu } from "../types/gpu";
import ActionWithChart, { Settings } from "../types/action";
import { height, width } from "../utils/constants";
import Buffer from "../utils/buffer";
import * as d3 from "d3";
import * as os from "os";
import getMacOSMetrics from "../utils/converter";

@action({ UUID: "com.nthompson.gpu.temp" })
export class GpuTemp extends ActionWithChart<GpuTempSettings> {
  startTimer(
    gpu: Gpu,
    action: any,
    celsius: boolean = true,
    settings: GpuTempSettings
  ): void {
    if (this.timers.has(action.id)) {
      clearInterval(this.timers.get(action.id)!);
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
        if (gpu === undefined) {
          streamDeck.logger.error("GPU not found or selected");
          return;
        }

        let temp = gpu?.temperature;

        if (temp === -1) {
          action.showAlert();
          return;
        }

        if (!celsius) {
          temp = (temp * 9) / 5 + 32;
        }

        action.setTitle(`${Math.round(temp)}°${celsius ? "C" : "F"}`);

        if (settings.enableChart) {
          const chart = this.createChart(
            svg,
            Math.round(temp),
            settings,
            action,
            Number.parseInt(settings.minTemp || "0"),
            Number.parseInt(settings.maxTemp || "100")
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
    ev: DidReceiveSettingsEvent<GpuTempSettings>
  ): Promise<void> | void {
    let gpu: Gpu;
    if (os.platform() !== "darwin") {
      gpu = this.getGpu(ev.payload.settings.gpuId);
    } else {
      gpu = getMacOSMetrics();
    }
    let celsius = true;

    if (ev.payload.settings.temp === "fahrenheit") {
      celsius = false;
    }

    this.startTimer(gpu!, ev.action, celsius, ev.payload.settings);
  }

  override onWillAppear(
    ev: WillAppearEvent<GpuTempSettings>
  ): Promise<void> | void {
    let gpu: Gpu;
    if (os.platform() !== "darwin") {
      gpu = this.getGpu(ev.payload.settings.gpuId);
    } else {
      gpu = getMacOSMetrics();
    }

    let celsius = true;

    if (ev.payload.settings.temp === "fahrenheit") {
      celsius = false;
    }

    this.startTimer(gpu!, ev.action, celsius, ev.payload.settings);
  }
}

type GpuTempSettings = {
  temp: string;
  minTemp: string;
  maxTemp: string;
} & Settings &
  JsonObject;
