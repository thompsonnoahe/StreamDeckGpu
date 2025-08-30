import streamDeck, {
  JsonObject,
  KeyDownEvent,
  PropertyInspectorDidAppearEvent,
  SingletonAction,
  WillDisappearEvent,
} from '@elgato/streamdeck';
import { Gpu } from './gpu';
import { width, height } from '../utils/constants';
import * as d3 from 'd3';
import Buffer from '../utils/buffer';
import query from '../query';
import { Window } from 'happy-dom';
import * as os from 'os';
import getMacOSMetrics from '../utils/converter';
import { platform } from 'os';
export class Settings {
  gpuId: string = '';
  chartColor: string = '#aaaaaa';
  enableChart: boolean = false;
}

type JsonSettings = Settings & JsonObject;

let GpuMetrics: any;
if (platform() !== 'win32') {
  GpuMetrics = require('@thompsonnoahe/macos-metrics');
}

export default class ActionWithChart<
  T extends JsonSettings,
> extends SingletonAction<T> {
  buffers: Map<string, Buffer<[number, number]>> = new Map();
  timers: Map<string, NodeJS.Timeout | undefined> = new Map();
  query = os.platform() === 'win32' ? query : undefined;
  devices = os.platform() === 'win32' ? this.query.getGpus() : undefined;
  window = new Window();

  getGpu(gpuId: string): Gpu {
    return this.devices?.find((gpu: Gpu) => gpu.deviceId === gpuId);
  }

  createChart(
    svg: d3.Selection<any, any, any, any>,
    gpuMetric: number,
    settings: T,
    action: any,
    minScale: number = 0,
    maxScale: number = 100
  ) {
    // Set x axis to N samples, where width is N
    const x = d3
      .scaleTime()
      .domain([new Date(new Date().getTime() - width * 1000), new Date()]) // Domain is (past, now)
      .range([0, width]);
    // Set y axis to range from min to max, scaled to the chart height
    const y = d3.scaleLinear([minScale, maxScale], [height, 0]);

    const buff = this.buffers.get(action.id);

    // Push a data point to the buffer
    buff?.enqueue([Date.now(), gpuMetric]);

    const area = d3
      .area()
      .x(d => x(d[0]))
      .y0(height)
      .y1(d => y(d[1]))
      .curve(d3.curveCatmullRom);

    // Remove the previous SVG to refresh the drawing
    svg.selectAll('path').remove();

    svg
      .append('path')
      .attr('d', area(buff?.buffer ?? [[Date.now(), gpuMetric]]))
      .attr('fill', settings.chartColor || '#aaaaaa');

    return svg;
  }

  override onKeyDown(ev: KeyDownEvent<T>): Promise<void> | void {
    if (os.platform() === 'win32') {
      const gpu = this.getGpu(ev.payload.settings.gpuId);
      gpu?.launchAssociatedApp();
    } else {
      const gpu = getMacOSMetrics();
      gpu.launchAssociatedApp();
    }
  }

  override onWillDisappear(ev: WillDisappearEvent<T>): void {
    clearInterval(this.timers.get(ev.action.id));
  }

  override onPropertyInspectorDidAppear(
    ev: PropertyInspectorDidAppearEvent<T>
  ): Promise<void> | void {
    if (os.platform() === 'win32') {
      const gpus = this.devices.map((gpu: Gpu) => {
        return {
          title: gpu.name,
          value: gpu.deviceId,
        };
      });
      streamDeck.ui.current?.sendToPropertyInspector(gpus);
    } else if (os.platform() === 'darwin') {
      const gpus = [
        {
          title: GpuMetrics.getMetrics(0).name,
          value: GpuMetrics.getMetrics(0).id,
        },
      ];
      streamDeck.ui.current?.sendToPropertyInspector(gpus);
    }
  }
}
