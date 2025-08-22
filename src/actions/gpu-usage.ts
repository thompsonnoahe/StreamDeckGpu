import {
  action,
  WillAppearEvent,
  streamDeck,
  DidReceiveSettingsEvent,
  JsonObject,
} from '@elgato/streamdeck';
import { Gpu } from '../types/gpu';
import * as d3 from 'd3';
import Buffer from '../utils/buffer';
import { width, height } from '../utils/constants';
import ActionWithChart, { Settings } from '../types/action';

@action({ UUID: 'com.nthompson.gpu.usage' })
export class GpuUsage extends ActionWithChart<GpuUsageSettings> {
  startTimer(gpu: Gpu, action: any, settings: GpuUsageSettings): void {
    // Clear the timer for the action if it's replaced
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

        // -1 indicates an error
        if (gpu?.usage === -1) {
          action.showAlert();
          return;
        }

        action.setTitle(`${gpu?.usage}%`);

        if (settings.enableChart) {
          const chart = this.createChart(svg, gpu?.usage, settings, action);

          action.setImage(
            `data:image/svg+xml;charset=utf8,${encodeURIComponent(chart.node())}`
          );
        } else {
          // Reset the image if the user flips back between chart or image
          action.setImage('gpu.png');
        }
      }, 1000)
    );
  }

  override onDidReceiveSettings(
    ev: DidReceiveSettingsEvent<GpuUsageSettings>
  ): Promise<void> | void {
    const gpu = this.getGpu(ev.payload.settings.gpuId);

    this.startTimer(gpu!, ev.action, ev.payload.settings);
  }

  override onWillAppear(
    ev: WillAppearEvent<GpuUsageSettings>
  ): Promise<void> | void {
    const gpu = this.getGpu(ev.payload.settings.gpuId);
    this.startTimer(gpu!, ev.action, ev.payload.settings);
  }
}

type GpuUsageSettings = {} & Settings & JsonObject;
