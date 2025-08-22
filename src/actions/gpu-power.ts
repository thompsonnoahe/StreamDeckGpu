import streamDeck, {
  action,
  DidReceiveSettingsEvent,
  JsonObject,
  WillAppearEvent,
} from '@elgato/streamdeck';
import { Gpu } from '../types/gpu';
import Vendor from '../types/vendor';
import ActionWithChart, { Settings } from '../types/action';
import { width, height } from '../utils/constants';
import Buffer from '../utils/buffer';
import * as d3 from 'd3';

@action({ UUID: 'com.nthompson.gpu.power' })
export class GpuPowerUsage extends ActionWithChart<GpuPowerUsageSettings> {
  startTimer(gpu: Gpu, action: any, settings: GpuPowerUsageSettings) {
    if (this.timers.has(action.id)) {
      clearInterval(this.timers.get(action.id));
    }

    this.buffers.set(action.id, new Buffer<[number, number]>(width));

    const svg = d3
      .select(this.window.document.body)
      .append('svg')
      .attr('width', width)
      .attr('height', height);

    this.timers.set(
      action.id,
      setInterval(() => {
        if (gpu === undefined) {
          streamDeck.logger.error('GPU not found or selected');
          return;
        }

        if (gpu.power === -1) {
          action.showAlert();
          return;
        }

        let power = gpu.power;

        if (gpu.vendor === Vendor.Nvidia) {
          // NVIDIA returns mW, convert to W
          power /= 1000;
        }

        if (settings.enableChart) {
          const chart = this.createChart(
            svg,
            power,
            settings,
            action,
            Number.parseInt(settings.minWatts || '0'),
            Number.parseInt(settings.maxWatts || '300')
          );

          action.setImage(
            `data:image/svg+xml;charset=utf8,${encodeURIComponent(chart.node())}`
          );
        } else {
          // Reset the image if the user flips back between chart or image
          action.setImage('gpu.png');
        }

        action.setTitle(`${Math.round(power)}W`);
      }, 1000)
    );
  }

  override onDidReceiveSettings(
    ev: DidReceiveSettingsEvent<GpuPowerUsageSettings>
  ): Promise<void> | void {
    const gpu = this.getGpu(ev.payload.settings.gpuId);

    this.startTimer(gpu!, ev.action, ev.payload.settings);
  }

  override onWillAppear(
    ev: WillAppearEvent<GpuPowerUsageSettings>
  ): Promise<void> | void {
    const gpu = this.getGpu(ev.payload.settings.gpuId);

    this.startTimer(gpu!, ev.action, ev.payload.settings);
  }
}

type GpuPowerUsageSettings = {
  gpuId: string;
  minWatts: string;
  maxWatts: string;
} & Settings &
  JsonObject;
